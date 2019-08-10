/////////////////////////////////////////////////////////////////////
// DirExplorerN.cpp - Naive directory explorer                     //
// ver 1.2                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018 //
/////////////////////////////////////////////////////////////////////

/*
 * Package Operations:
 * -------------------
 * DirExplorerN provides a class, of the same name, that executes a
 * depth first search of a directory tree rooted at a specified path.
 * Each time it enters a directory, it invokes its member function
 * doDir, and for every file in that directory, it invokes doFile.
 *
 * We call this a "Naive Directory Explorer" because, while it carries
 * out its assigned responsibilities well, there is no way to change
 * what its doDir and doFile functions do, without changing the class
 * itself.  It would be much better to provide a mechanism to allow
 * a using application to supply this processing without changing
 * the Directory Navigator.
 *
 * Other projects in this solution do just that, in different ways.

 * - DirExplorer-Naive:
 *     Implements basic processing well, but applications have to
 *     change its code to affect how files and directories are
 *     handled.
 * - DirExplorer-Template:
 *     Applications provide template class parameters to define file
 *      and directory processing.
 * - DirExplorer-Inheritance:
 *     Applications subclass DirExplorerI, overriding virtual functions
 *     doFile, doDir, and doQuit to define how files and directories
 *     are handled, and whether processing should terminate before
 *     visiting all directories.
 * - DirExplorer-Events:
 *     Applications use Event Interfaces, published by DirExplorerE,
 *     by subscribing event-handler functions, and control terminate
 *      processing using a plug-in interface.
 * - DirExplorer-Provider:
 *     Applications implement a file system provider that implements
 *     a Provider interface published by DirExplorerP.
 *
 * We'll be using this solution to illustrate techniques for building
 * flexible software.
 *
 * Required Files:
 * ---------------
 * Logger.h , Loader.h
 *
 * Maintenance History:
 * --------------------
 * ver 1.4 : 11 Feb 2019
 * - Added file containers
 *   Added regex matching
 * ver 1.3 : 19 Aug 2018
 * - Removed some options to make this version simple.  Those are
 *   implemented in the more advanced navigators, presented here.
 * ver 1.2 : 17 Aug 2018
 * - Moved method definitions to inlines, below the class declaration.
 * ver 1.1 : 16 Aug 2018
 * - Made no recursion default, added option /s for recursion.
 * ver 1.0 : 11 Aug 2018
 * - first release
 *
*/

#include <regex>
#include "Loader.h"
#include "../Logger/Logger.h"

using namespace Logging;
using namespace FileSystem;

//----< construct DirExplorerN instance with default pattern >-----

Loader::Loader(const std::string& path) : path_(path)
{
	patterns_.push_back("*.*");
}
//----< add specified patterns for selecting file names >----------

void Loader::addPattern(const std::string& patt)
{
	if (patterns_.size() == 1 && patterns_[0] == "*.*")
		patterns_.pop_back();
	patterns_.push_back(patt);
}
//----< set option to recusively walk dir tree >-------------------

void Loader::recurse(bool doRecurse)
{
	recurse_ = doRecurse;
}
//----< start Depth First Search at path held in path_ >-----------

bool Loader::search()
{
  LoggerDemo::write("\n  Files found matching patterns:\n");

	find(path_);
  // found files matching set patterns, return true
  if (files_.size() > 0) return true;
  // no files found, return false
  return false;
}
//----< search for directories and their files >-------------------
/*
  Recursively finds all the dirs and files on the specified path,
  executing doDir when entering a directory and doFile when finding a file
*/
void Loader::find(const std::string& path)
{
	// show current directory

	std::string fpath = Path::getFullFileSpec(path);
	doDir(fpath);

	for (auto patt : patterns_)
	{
		std::vector<std::string> files = Directory::getFiles(fpath, patt);
		for (auto f : files)
		{
			doFile(fpath, f);  // show each file in current directory
		}
	}

	std::vector<std::string> dirs = Directory::getDirectories(fpath);
	for (auto d : dirs)
	{
		if (d == "." || d == "..")
			continue;
		std::string dpath = fpath + "\\" + d;
		if (recurse_)
		{
			find(dpath);   // recurse into subdirectories
		}
		else
		{
			doDir(dpath);  // show subdirectories
		}
	}
}

bool Loader::match_regexes(const std::vector<std::string>& regexes)
{
  LoggerDebug::write("\n\n  Matching files to regexes - Available regexes: ");
  LoggerDemo::write("\n\n  Matching files to regexes - Available regexes: ");
  for (auto regex : regexes) {
    LoggerDebug::write(regex + " ");
    LoggerDemo::write(regex + " ");
  }

  LoggerDebug::write("\n  Files found matching regexes: ");
  LoggerDemo::write("\n   Files found matching regexes: ");
  
  std::vector<std::string> filteredFiles; // to store files matching regex

  // match regex to files
  for (auto file : files_) {
    std::string filename = Path::getName(file);
    for (auto regex : regexes) {
      std::regex r(regex);
      if (std::regex_match(filename, r)) {
        filteredFiles.push_back(file);
        LoggerDebug::write("\n   -- " + file);
        LoggerDemo::write("\n   -- " + file);
        break; // break to avoid adding same file twice to the  
               // list in case it matches multiple regexes
      }
    }
  }

  LoggerDebug::write("\n");
  LoggerDemo::write("\n");

  // found files matching regex?
  if (filteredFiles.size() > 0) {
    files_ = filteredFiles;
    return true;
  }

  LoggerDebug::write("   -- no files found");
  LoggerDemo::write("   -- no files found");
  // no matches found
  return false;
}

//----< an application changes to enable specific file ops >-------
void Loader::doFile(const std::string& dir, const std::string& filename)
{
	++fileCount_;
  std::string filepath = dir + "\\" + filename;
  files_.push_back(filepath);
  LoggerDebug::write("\n  --   " + filename);
  LoggerDemo::write("\n  -- " + filepath);
}
//----< an application changes to enable specific dir ops >--------

void Loader::doDir(const std::string& dirname)
{
  ++dirCount_;
  LoggerDebug::write("\n  ++ " + dirname);
}
//----< return number of files processed >-------------------------

const Loader::files& Loader::filesList() const
{
  return files_;
}

size_t Loader::fileCount()
{
	return fileCount_;
}
//----< return number of directories processed >-------------------

size_t Loader::dirCount()
{
	return dirCount_;
}
//----< show final counts for files and dirs >---------------------

void Loader::showStats()
{
  std::ostringstream msg;
  msg << "\n\n  processed " << fileCount_ << " files in " << dirCount_ <<" directories";
  LoggerDebug::write(msg.str());
  LoggerDemo::write(msg.str());
}



#ifdef TEST_DIREXPLORERN  // only compile the following when defined

#include "../Utilities/StringUtilities/StringUtilities.h"
#include "../Utilities/CodeUtilities/CodeUtilities.h"

using namespace Utilities;

ProcessCmdLine::Usage customUsage()
{
  std::string usage;
  usage += "\n  Command Line: path [/option]* [/pattern]*";
  usage += "\n    path is relative or absolute path where processing begins";
  usage += "\n    [/option]* are one or more options of the form:";
  usage += "\n      /s - walk directory recursively";
  usage += "\n      [/demo | /debug] - display mode: either demo OR debug";
  //usage += "\n      /h - hide empty directories";
  //usage += "\n      /a - on stopping, show all files in current directory";
  usage += "\n    [pattern]* are one or more pattern strings of the form:";
  usage += "\n      *.h *.cpp *.cs *.txt or *.*";
  usage += "\n";
  return usage;
}

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

void stopLoggers() {
  LoggerDemo::stop();
  LoggerDebug::stop();
}

int main(int argc, char *argv[])
{
  Title("Demonstrate DirExplorer-Naive, " + Loader::version());

  ProcessCmdLine pcl(argc, argv);
  pcl.usage(customUsage());

  preface("Command Line: ");
  pcl.showCmdLine();
  //putline();

  if (pcl.parseError())
  {
    pcl.usage();
    std::cout << "\n\n";
    return 1;
  }

  initializeLoggers(pcl.displayMode());

  Loader de(pcl.path());
  for (auto patt : pcl.patterns())
  {
    de.addPattern(patt);
  }

  if (pcl.hasOption("s"))
  {
    de.recurse();
  }

  bool res = de.search() && de.match_regexes(pcl.regexes());
  de.showStats();

  if (!res) {
    LoggerDemo::write("\n\n  Failed to find any files matching patterns and regexes. Terminating..\n\n");
    return 1; 
  }

  stopLoggers();
  //std::cout << "\n\n";
  return 0;
}

#endif