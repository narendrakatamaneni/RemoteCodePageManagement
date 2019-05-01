/////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 1.0                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////////

#include "ServerPrototype.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include <chrono>
#include "../Executive/Publisher.h"
#include <regex>

namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

Files Server::getFiles(const Repository::SearchPath& path)
{
  return Directory::getFiles(path);
}

Dirs Server::getDirs(const Repository::SearchPath& path)
{
  return Directory::getDirectories(path);
}

template<typename T>
void show(const T& t, const std::string& msg)
{
  std::cout << "\n  " << msg.c_str();
  for (auto item : t)
  {
    std::cout << "\n    " << item.c_str();
  }
}

std::function<Msg(Msg)> echo = [](Msg msg) {
  Msg reply = msg;
  reply.to(msg.from());
  reply.from(msg.to());
  return reply;
};

std::function<Msg(Msg)> getFiles = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getFiles");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files files = Server::getFiles(searchPath);

	/*std::vector<std::string> filteredFiles; // to store files matching regex
	std::vector<std::string> regexes;
	regexes.push_back("*.h");
	regexes.push_back("*.cpp");

  // match regex to files
	for (auto file : files) {
		std::string filename = Path::getName(file);
		for (auto regex : regexes) {
			std::regex r(regex);
			if (std::regex_match(filename, r)) {
				filteredFiles.push_back(file);
				break; // break to avoid adding same file twice to the  
					   // list in case it matches multiple regexes
			}
		}
	}
	// found files matching regex?
	if (filteredFiles.size() > 0) {
		files = filteredFiles;
	}*/
    size_t count = 0;
    for (auto item : files)
    {
      std::string countStr = Utilities::Converter<size_t>::toString(++count);
      reply.attribute("file" + countStr, item);
    }
  }
  else
  {
    std::cout << "\n  getFiles message did not define a path attribute";
  }
  return reply;
};

std::function<Msg(Msg)> getDirs = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getDirs");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files dirs = Server::getDirs(searchPath);
    size_t count = 0;
    for (auto item : dirs)
    {
      if (item != ".." && item != ".")
      {
        std::string countStr = Utilities::Converter<size_t>::toString(++count);
        reply.attribute("dir" + countStr, item);
      }
    }
  }
  else
  {
    std::cout << "\n  getDirs message did not define a path attribute";
  }
  return reply;
};

std::function<Msg(Msg)>getFile = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("file");
  reply.attribute("browse", msg.attributes()["convertedFile"]);
  reply.file(msg.attributes()["convertedFile"]);
  return reply;

  //add another msg to notify client that file is received
};

//converts the files requested by the client
std::function<Msg(Msg)>convert = [](Msg msg){
	Msg reply;
	std::string path = msg.attributes()["path"];
	std::string state = msg.attributes()["state"];
	std::string appName = msg.attributes()["appName"];
	std::string recursive = msg.attributes()["recursive"];
	std::string patterns = msg.attributes()["patterns"];
	std::string regex = msg.attributes()["regex"];
	std::list<std::string> cmdLineArguments;

	cmdLineArguments.push_back(appName);
	cmdLineArguments.push_back("../"+path);
	cmdLineArguments.push_back(recursive);
	cmdLineArguments.push_back(state);
	std::istringstream iss(patterns);
	for (std::string s; iss >> s; )
		cmdLineArguments.push_back(s);
	cmdLineArguments.push_back(regex);

	Publisher publisher;
	int argc = cmdLineArguments.size();
	char **argv = new char *[argc];
	int i = 0;
	for (auto it = cmdLineArguments.begin();it != cmdLineArguments.end();++it) {
		std::string arg = *it;
		argv[i] = new char[arg.length() + 1];
		strcpy_s(argv[i], arg.length()+1, arg.c_str());
		i++;
	}
	std::vector<std::string> convertedFiles=publisher.SetCmd(argc,argv);
	size_t count = 0;
	for (auto item : convertedFiles) {
		std::string countstr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("file" + countstr, item);
	}
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("convert");
	
  return reply;
};

int main()
{
  std::cout << "\n  Testing Server Prototype";
  std::cout << "\n ==========================";
  std::cout << "\n";

  //StaticLogger<1>::attach(&std::cout);
  //StaticLogger<1>::start();

  Server server(serverEndPoint, "ServerPrototype");
  server.start();

  /*std::cout << "\n  testing getFiles and getDirs methods";
  std::cout << "\n --------------------------------------";
  Files files = server.getFiles();
  show(files, "Files:");
  Dirs dirs = server.getDirs();
  show(dirs, "Dirs:");
  std::cout << "\n";*/

  std::cout << "\n  testing message processing";
  std::cout << "\n ----------------------------";
  server.addMsgProc("echo", echo);
  server.addMsgProc("getFiles", getFiles);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("getFile", getFile);
  server.addMsgProc("convert", convert);
  server.addMsgProc("serverQuit", echo);
  server.processMessages();
  
  Msg msg(serverEndPoint, serverEndPoint);  // send to self
  msg.name("msgToSelf");
  msg.command("echo");
  msg.attribute("verbose", "show me");
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  msg.command("getFiles");
  msg.remove("verbose");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  msg.command("getDirs");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  //-------------------Ammar's code----------------------------------//
  /*MsgPassingCommunication::EndPoint clientEp;
  clientEp.address = "localhost";
  clientEp.port = 8082;
  Msg filemsg(clientEp, serverEndPoint);
  filemsg.command("file");
  filemsg.file("CommLibWrapper.cpp");
  server.postMessage(filemsg);
  filemsg.file("CommLibWrapper.h");

  server.postMessage(filemsg);*/

  //msg.command("getFile");
  //msg.remove("verbose");
  //msg.attributes()["path"] = storageRoot;
  //server.postMessage(msg);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));


  std::cout << "\n  press enter to exit";
  std::cin.get();
  std::cout << "\n";

  msg.command("serverQuit");
  server.postMessage(msg);
  server.stop();
  return 0;
}