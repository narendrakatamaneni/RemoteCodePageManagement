///////////////////////////////////////////////////////////////////////////
// Display.cpp   : defines webpage display using browser functions       //
// ver 1.0                                                               //
// Author        :Narendra Katamaneni, Syracuse University               //
///////////////////////////////////////////////////////////////////////////

/*
* Package Operations:
* -------------------
* This package supports logging for multiple concurrent clients to a
* single std::ostream.  It does this be enqueuing messages in a
* blocking queue and dequeuing with a single thread that writes to
* the std::ostream.
*
* Build Process:
* --------------
* Required Files: Process.h,Logger.h,FileSystem.h,Display.h
*
* Build Command: devenv logger.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 11 Feb 2019
* - added multiple stream capability
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/

#include "Display.h"
#include "../Process/Process.h"
#include "../Logger/Logger.h"
#include "../FileSystem/FileSystem.h"

using namespace Logging;

// -----< default ctor >--------------------------------------------------
Display::Display() { }

// -----< display single file >-------------------------------------------
void Display::display(const std::string & file)
{
  LoggerDebug::write("\n  Displaying file: " + FileSystem::Path::getName(file) + " in browser");
  LoggerDemo::write("\n  Displaying file: " + FileSystem::Path::getName(file) + " in browser");

  
  Process p;
  p.application("C:/Windows/System32/cmd.exe");
  
  std::string WebBrowserPath = "chrome.exe";
  std::string cmd = "/c start /wait \"\" \""+WebBrowserPath+"\" /new-window " + FileSystem::Path::getFullFileSpec(file);

  p.commandLine(cmd);
  p.create();
  CBP callback = []() {
	  LoggerDebug::write("\n  --- child browser exited with this message ---");
	  LoggerDemo::write("\n  --- child browser exited with this message ---");
  };
  p.setCallBackProcessing(callback);
  p.registerCallback();
  WaitForSingleObject(p.getProcessHandle(), INFINITE);
}

// -----< display multiple files  >---------------------------------------
void Display::display(const std::vector<std::string>& files)
{
  for (auto file : files) 
    display(file);
}

// -----< private - hanles html comments >---------------------------
void Display::handlecomments(std::string & line)
{
	size_t pos = line.find('/');
	if (pos == line.npos) return;
	if (pos == line.size() - 1) return;
	if (line[pos + 1] == '/') {
		line.replace(pos, 1, "<span class=\"comments\">/");
		line += "</span>";
	}
	if (line[pos + 1] == '*') {
		line.replace(pos, 1, "<span class=\"comments\">/");
	}

}

// <------- handles multiple comments in a file --------------->
void Display::handleMultipleComments(std::string & line)
{
	size_t position = line.find('*');
	if (position == line.npos) return;
	if (position == line.size() - 1) return;
	if (line[position + 1] == '/') {
		line += "</span>";
	}

}

// -----< private - collects type,name,startlinecount and endlinecount for each children of parser >--------------------------
std::unordered_map<std::size_t, TypeInfo> Display::collectFullDetails(CodeAnalysis::ASTNode* pGlobalScope, std::unordered_map<std::size_t, TypeInfo>& inUnOrderedMap) {

	std::ostringstream out;

	if (pGlobalScope->type_ == "class") {
		inUnOrderedMap[pGlobalScope->startLineCount_] = TypeInfo::class_begin;
		inUnOrderedMap[pGlobalScope->endLineCount_] = TypeInfo::end_scope;
	}

	else if (pGlobalScope->type_ == "function") {
		inUnOrderedMap[pGlobalScope->startLineCount_] = TypeInfo::function_begin;
		inUnOrderedMap[pGlobalScope->endLineCount_] = TypeInfo::end_scope;
	}

	auto iter = pGlobalScope->children_.begin();

	while (iter != pGlobalScope->children_.end())

	{
		collectFullDetails(*iter, inUnOrderedMap);

		++iter;
	}

	return inUnOrderedMap;
}

// <------- handles function starting scope in a file --------------->
void Display::handlefunctions(std::string &line) {
	size_t pos = line.find('{');
	if (pos == line.npos) return;

	line.replace(pos, 1, "<span class=\"funClass\">{");

}

// <------- handles class  starting scope in a file --------------->
void Display::handleclass(std::string &line) {
	size_t pos = line.find('{');
	if (pos == line.npos) return;

	line.replace(pos, 1, "<span class=\"clsClass\">{");

}

// <------- handles end scope for classes and functions in a file --------------->
void Display::handleendscope(std::string & line)
{
	line += "</span>";
}

#ifdef TEST_DISPLAY

#include <iostream>

int main() {
  LoggerDebug::attach(&std::cout);
  LoggerDebug::start();

  std::vector<std::string> files;
  files.push_back("..\\ConvertedWebpages\\Converter.h.html");
  files.push_back("..\\ConvertedWebpages\\Converter.cpp.html");

  Display d();
  d.display(files);

  LoggerDebug::stop();
}

#endif