///////////////////////////////////////////////////////////////////////////
// CodePublisher.cpp : defines facade/executive for OOD Pr1 S19          //
// ver 1.0                                                               //
// Author        :Jim Fawcett, Syracuse University               //
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
* Required Files: Logger.h,Publisher.h,IPublisher.h
*
* Build Command: devenv logger.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 2.0 : 04 Apr 2019
* - added functionality for shim
* ver 1.0 : 11 Feb 2019
* - added multiple stream capability
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/

#include "Publisher.h"
#include "../Logger/Logger.h"
#include <conio.h>
#include <exception>
#include "IPublisher.h"

using namespace CodeUtilities;
using namespace Logging; 
using namespace FileSystem;

ProcessCmdLine::Usage customUsage();

std::vector<std::string> convertedfiles_;

Publisher::Publisher()
{
}

// -----< process cmd to get info >------------------------------------
bool Publisher::processCommandLineArgs(int argc, char ** argv)
{
  pcl_ = new ProcessCmdLine(argc, argv); 
  pcl_->usage(customUsage());

  preface("Command Line: ");
  pcl_->showCmdLine();

  if (pcl_->parseError())
  {
    pcl_->usage();
    std::cout << "\n\n";
    return false;
  }
  dirIn_ = pcl_->path();
  return true;
}

// -----< return input directory - from PCL >----------------------------
const std::string & Publisher::workingDirectory() const
{
  return dirIn_;
}

// -----< set output directory >-----------------------------------------
void Publisher::outputDirectory(const std::string & dir)
{
  dirOut_ = dir; 
  cconv_.outputDir(dir);
}

// -----< return output directory >--------------------------------------
const std::string & Publisher::outputDirectory() const
{
  return dirOut_;
}

// -----< extract files - after processing cmd >-------------------------
bool Publisher::extractFiles()
{
	Loader de(pcl_->path());

  for (auto patt : pcl_->patterns())
  {
    de.addPattern(patt);
  }

  if (pcl_->hasOption("s"))
  {
    de.recurse();
  }

  bool res = de.search() && de.match_regexes(pcl_->regexes());
  de.showStats();

  files_ = de.filesList();

  return res;
}


// -----< publish - files extracted from directory explorer >---------------
void Publisher::publish()
{
  display_.display(cconv_.convert(files_));
}

// -----< publish - single file >-------------------------------------------
void Publisher::publish(const std::string & file)
{
  display_.display(cconv_.convert(file));
}

// -----< publish - must provide list >-------------------------------------
void Publisher::publish(const std::vector<std::string>& files)
{
  display_.display(cconv_.convert(files));
}

// -----< gets display mode as set by PCL >---------------------------------
CodeUtilities::DisplayMode Publisher::displayMode() const
{
  return pcl_->displayMode();
}


// -----< command line usage >----------------------------------------------
ProcessCmdLine::Usage customUsage()
{
  std::string usage;
  usage += "\n  Command Line: path [/option]* [/pattern]* [/regex]*";
  usage += "\n    path is relative or absolute path where processing begins";
  usage += "\n    [/option]* are one or more options of the form:";
  usage += "\n      /s     - walk directory recursively";
  usage += "\n      /demo  - run in demonstration mode (cannot coexist with /debug)";
  usage += "\n      /debug - run in debug mode (cannot coexist with /demo)";
  //usage += "\n      /h - hide empty directories";
  //usage += "\n      /a - on stopping, show all files in current directory";
  usage += "\n    [pattern]* are one or more pattern strings of the form:";
  usage += "\n      *.h *.cpp *.cs *.txt or *.*";
  usage += "\n    [regex] regular expression(s), i.e. [A-B](.*)";
  usage += "\n";
  return usage;
}

// -----< initialize loggers in proper display mode >------------------------
void initializeLoggers(DisplayMode dm) {
  if (dm == DisplayMode::Debug) {
    LoggerDebug::attach(&std::cout);
    LoggerDebug::start();
  }

  else if (dm == DisplayMode::Demo) {
    LoggerDemo::attach(&std::cout);
    LoggerDemo::start();
  }
}

// -----< stop loggers >-----------------------------------------------------
void stopLoggers() {
  LoggerDemo::stop();
  LoggerDebug::stop();
}

/*IPublisher* ObjectFactory::createClient() {
	return new Publisher;
}*/

//#ifdef DEMO_CP

#include <iostream>

// -----< demonstrate requirement 3 and 4 >----------------------------------------------
void demoReq34() {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirement #3 & #4 - Packages");
  LoggerDemo::write("\n Contains Executive package that accepts, from its command line, a path containing files to be converted and also contains Loader, Converter, Dependencies, and Display packages");
}


// -----< demonstrate requirements 5 >---------------------------------------
int demoReq5(Publisher& cp, int argc, char ** argv) {

  if (!cp.processCommandLineArgs(argc, argv)) return 1; // just in case
  if (!cp.extractFiles()) { // if this fails, then there are no files to process at all
    return 2; // the demo will end after this
  }

  return 0;
}

// -----< demonstrate requirements 6,7 and 8 >------------------------------
std::vector<std::string> demoReq678(Publisher& cp) {
  LoggerDemo::write("\n");
  LoggerDemo::title("Demonstrating Requirements #6, #7 & #8 - Convert , Dependencies and Display");
  return cp.CollectConvertedFiles();

}


std::vector<std::string> Publisher::SetCmd(int argc,char** argv) {
	std::vector<std::string>files;

	try {		
		initializeLoggers(CodeUtilities::DisplayMode::Debug); // by default go debug
		for (int i = 0; i < argc; i++) {
			std::string arg = argv[i];
			//if (arg == "/demo") { // only go demo once told 
				stopLoggers();
				initializeLoggers(CodeUtilities::DisplayMode::Demo);
				break;
			//}
		}

		demoReq34();

		Publisher cp;

		int err = demoReq5(cp, argc, argv);
		files= demoReq678(cp);
		stopLoggers();
	}
	catch (std::exception& e) {
		std::cout << "Oops an Exception occured :  " << e.what() << '\n';
	}
	return files;
}

std::vector<std::string> Publisher::GetConvertFiles() {
	return convertedfiles_;
}

std::vector<std::string> Publisher::CollectConvertedFiles() {

	convertedfiles_ = cconv_.convert(files_);
	return convertedfiles_;
}

bool Publisher::convertFiles(std::string file) {
	std::vector<std::string> files;
	files.push_back(file);
	std::vector<std::string> convertedFiles = cconv_.convertFile(files);
	if (convertedFiles.size()<=0) {
		return false;
	}
	return true;
}


//#endif

#ifdef USE_CP

#include <iostream>

// -----< NORMAL USE MAIN > ------------------------------------------
int main(int argc, char ** argv) {
	Publisher cp;
  if (!cp.SetCmd(argc, argv)) {
    std::cout << "\n  Failed to process command line arguments. Terminating\n\n";
    return 1;
  }

  initializeLoggers(cp.displayMode());

  if (!cp.extractFiles()) {
    stopLoggers();
    std::cout << "\n  Found no files to process. Terminating.\n\n";
    return 0; 
  }

  cp.publish();
  stopLoggers();
  return 0;
}

#endif