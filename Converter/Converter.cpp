///////////////////////////////////////////////////////////////////////////
// Converter.cpp : implements and provides test stub for Converter.h     //
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
* Required Files:
*-----------------
* Converter.h,Logger.h,FileSystem.h,ConfigureParser.h,Parser.h,ActionsAndRules.h,
* AbstrSynTree.h,Display.h
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

#include "Converter.h"
#include "../Logger/Logger.h"
#include "../FileSystem/FileSystem.h"
#include "../Parser/ConfigureParser.h"
#include "../Parser/Parser.h"
#include "../Parser/ActionsAndRules.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Display/Display.h"
#include <string>
#include <vector>
#include <unordered_map>

using namespace FileSystem;
using namespace Logging;
using namespace CodeAnalysis;

// -----< default ctor >--------------------------------------------------
CodeConverter::CodeConverter()
{
}

// -----< ctor to set dep table >-----------------------------------------
CodeConverter::CodeConverter(const DependencyTable & dt) : dt_(dt)
{
  LoggerDebug::write("\n  CodeConverter initialized with DependencyTable\n");
}

// -----< set dependency table function >---------------------------------
void CodeConverter::setDepTable(const DependencyTable & dt)
{
  dt_ = dt;
}

// -----< convert pre-set dependency table >------------------------------
std::vector<std::string> CodeConverter::convert()
{
  if (!createOutpurDir()) {
    LoggerDebug::write("\n\n  Failed to start conversion due to inability to create output directory");
    LoggerDebug::write("\n  -- Output Directory: " + Path::getFullFileSpec(outputDir_));
    return std::vector<std::string>();
  }

  LoggerDemo::write("\n\n  Converting files to webpages");
  LoggerDebug::write("\n\n  Converting files to webpages");

  LoggerDebug::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));
  LoggerDemo::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));

  for (auto entry : dt_) {
    if (convertFile(entry.first)) {
      LoggerDemo::write("\n  -- Converted: ");
      LoggerDebug::write("\n  -- Converted: ");
    }
    else {
      LoggerDemo::write("\n  -- Failed:    ");
      LoggerDebug::write("\n  -- Failed:    ");
    }

    std::string filename = Path::getName(entry.first);
    LoggerDebug::write(filename);
    LoggerDemo::write(filename);
  }
  LoggerDebug::write("\n");
  LoggerDemo::write("\n");
  return convertedFiles_;
}

// -----< convert single file given path >----------------------------------
std::string CodeConverter::convert(const std::string & filepath)
{
  if (!createOutpurDir()) {
    LoggerDebug::write("\n\n  Failed to start conversion due to inability to create output directory");
    LoggerDebug::write("\n  -- Output Directory: " + Path::getFullFileSpec(outputDir_));
    return "";
  }

  LoggerDemo::write("\n\n  Converting files to webpages");
  LoggerDebug::write("\n\n  Converting files to webpages");

  LoggerDebug::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));
  LoggerDemo::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));

  if (convertFile(filepath)) {
    LoggerDemo::write("\n  -- Converted: ");
    LoggerDebug::write("\n  -- Converted: ");
  }
  else {
    LoggerDemo::write("\n  -- Failed:    ");
    LoggerDebug::write("\n  -- Failed:    ");
  }

  std::string filename = Path::getName(filepath);
  LoggerDebug::write(filename);
  LoggerDemo::write(filename);

  return filename + ".html";
}

// -----< Add dependency to dependency tables and convert all the files >----------------------------------------
std::vector<std::string> CodeConverter::convert(const std::vector<std::string>& files)
{
  clear();
  dt_ = DependencyTable(files);
  return convert();
}

std::vector<std::string> CodeConverter::convertFile(std::vector<std::string> files)
{
	clear();
	dt_ = DependencyTable(files);
	return convert();
}


// -----< get set output directory >----------------------------------------
const std::string & CodeConverter::outputDir() const
{
  return outputDir_;
}

// -----< set output directory >--------------------------------------------
void CodeConverter::outputDir(const std::string & dir)
{
  outputDir_ = dir;
}

// -----< create output directory >-----------------------------------------
/* returns success of creation. it could fail if dir is sys protected */
bool CodeConverter::createOutpurDir()
{
  if (!Directory::exists(outputDir_)) {
    LoggerDebug::write("\n  Creating output directory. Path: " + Path::getFullFileSpec(outputDir_));
    return Directory::create(outputDir_);
  }
  LoggerDebug::write("\n  Output directory already exists. Proceeding...");
  return true;
}

// -----< get list of converted files >-------------------------------------
const std::vector<std::string> CodeConverter::convertedFiles() const
{
  return convertedFiles_;
}

void CodeConverter::clear()
{
  convertedFiles_.clear();
  dt_.clear();
}

// -----< private - read file and create webpage >--------------------------
bool CodeConverter::convertFile(std::string file)
{
  std::ifstream in(file);
  if (!in.is_open() || !in.good()) {
    LoggerDebug::write("Error -- unable to read file, path may be invalid.");
    in.close();
    return false;
  }

  ConfigParseForCodeAnal configure;
  Parser* pParser = configure.Build();
 
  std::string name;
  if (pParser)
  {
	  name = FileSystem::Path::getName(file);
	  if (!configure.Attach(file))
	  {
		  std::cout << "\n  could not open file " << name << std::endl;
	  }
  }
  else
  {
	  std::cout << "\n\n  Parser not built\n\n";
	  return 1;
  }

  Repository* pRepo = Repository::getInstance();
  pRepo->package() = name;

  while (pParser->next())
  {
	  pParser->parse();
  }
  std::cout << "\n";

  ASTNode* pGlobalScope = pRepo->getGlobalScope();
  
  collectDependencies(pGlobalScope, file);
  std::cout << "Dependent files is/are : \n" << std::endl;

  for (auto dep : dt_[file]) {
	  std::cout << Path::getValueInQuotes(dep) << std::endl;
  }

  for (auto dep : dt_[file]) {
	  processDependencyFiles(Path::getFullFileSpec(Path::getPath(file) + Path::getDependencyPath(dep)));
	}

  complexityEval(pGlobalScope);

  TreeWalk(pGlobalScope);
 

  std::string filename = Path::getName(file);
  std::string outputPath = outputDir_ + filename + ".html";
  out_.open(outputPath, std::ofstream::out);
  if (!out_.is_open() || !out_.good()) {
    LoggerDebug::write("Error -- unable to open output file for writing.");
    in.close();
    return false;
  }

  htmlOpeartions(filename, file, pGlobalScope, in);

  out_.close();

  convertedFiles_.push_back(outputPath);
  return true;
}

// -----< private -processes the files which have dependency on other files >--------------------------
bool CodeConverter::processDependencyFiles(std::string file) {

	std::cout << "Converting dependency file... " << std::endl;

	std::ifstream in(file);
	if (!in.is_open() || !in.good()) {
		LoggerDebug::write("Error -- unable to read file, path may be invalid.");
		in.close();
		return false;
	}

	ConfigParseForCodeAnal configure;
	Parser* pParser = configure.Build();


	std::string name;
	if (pParser)
	{
		name = FileSystem::Path::getName(file);
		if (!configure.Attach(file))
		{
			std::cout << "\n  could not open file " << name << std::endl;
		}
	}
	else
	{
		std::cout << "\n\n  Parser not built\n\n";
		return 1;
	}

	Repository* pRepo = Repository::getInstance();
	pRepo->package() = file;


	while (pParser->next())
	{
		pParser->parse();
	}
	std::cout << "\n";

	ASTNode* pGlobalScope = pRepo->getGlobalScope();

	complexityEval(pGlobalScope);

	TreeWalk(pGlobalScope);


	std::string filename = Path::getName(file);
	std::string outputPath = outputDir_ + filename + ".html";
	out_.open(outputPath, std::ofstream::out);
	if (!out_.is_open() || !out_.good()) {
		LoggerDebug::write("Error -- unable to open output file for writing.");
		in.close();
		return false;
	}

	htmlOpeartions(filename, file, pGlobalScope, in);
	out_.close();

	return true;

}


void CodeConverter::htmlOpeartions(const std::string& filename, std::string file, CodeAnalysis::ASTNode* pGlobalScope, std::ifstream & in) {

	// write everything to file 
	addPreCodeHTML(filename);
	addDependencyLinks(file);
	addPreTag();
	writeLine(pGlobalScope, in);
	addClosingTags();
}



// -----< collect dependencies in a file >--------------------------
void CodeConverter::collectDependencies(CodeAnalysis::ASTNode* pGlobalScope, std::string file) {

	// Add all file dependencies to Dependency Table
	auto iter2 = pGlobalScope->statements_.begin();
	while (iter2 != pGlobalScope->statements_.end()) {
		std::string dependency = (*iter2)->ToString();
		if (std::string::npos == dependency.find('<') && std::string::npos == dependency.find("ifdef") &&
			std::string::npos == dependency.find("endif") && std::string::npos == dependency.find("pragma") && std::string::npos == dependency.find("ifndef") &&
			std::string::npos == dependency.find("define")) {
			dt_.addDependency(file, (*iter2)->ToString());
		}
		++iter2;
	}

}

// -----< writes a line into output file >--------------------------
void CodeConverter::writeLine(CodeAnalysis::ASTNode* pGlobalScope, std::ifstream & in) {

	Display d;

	std::unordered_map < std::size_t, TypeInfo> result;

	std::unordered_map < std::size_t, TypeInfo> resultMap = d.collectFullDetails(pGlobalScope, result);
	int count = 1;

	while (in.good()) {
		std::string line;
		while (std::getline(in, line)) {
			skipSpecialChars(line);
			d.handlecomments(line);
			d.handleMultipleComments(line);

			if (resultMap.find(count) == resultMap.end()) {
				out_ << line << std::endl;
				count++;
				continue;
			}
			else {

				TypeInfo value = resultMap[count];

				if (value == TypeInfo::function_begin) {
					d.handlefunctions(line);
				}
				else if (value == TypeInfo::class_begin) {
					d.handleclass(line);
				}
				else if (value == TypeInfo::end_scope) {
					d.handleendscope(line);
				}

				out_ << line << std::endl;
				count++;
			}
		}
	}
}

// -----< private - add generic HTML preliminary markup >-------------------
void CodeConverter::addPreCodeHTML(const std::string& title)
{
  out_ << "<DOCTYPE !HTML>" << std::endl;
  out_ << "<html>" << std::endl;
  out_ << "  <head>" << std::endl;
  out_ << "    <Title>" << title << "</Title>" << std::endl;
  out_ << "    <style>" << std::endl;
  out_ << "      body {" << std::endl;
  out_ << "        padding:15px 40px;" << std::endl;
  out_ << "        font-family: Consolas;" << std::endl;
  out_ << "        font-size: 1.25em;" << std::endl;
  out_ << "        font-weight: normal;" << std::endl;
  out_ << "      }" << std::endl;
  out_ << "      </style>" << std::endl;
  out_ << "  <link rel = \"stylesheet\" type = \"text/css\" href = \"style.css\" >" << std::endl;
  out_ << "  <script src=\"script.js\"> </script> " << std::endl;
  out_ << "  </head>" << std::endl << std::endl;
  out_ << "  <body>" << std::endl;
  out_ << " <input type =\"submit\" value= \"Comments\" id =\"cmt\" onclick=\"togglevisibility('comments')\" ></input>" << std::endl;
  out_ << " <input type =\"submit\" value= \"Class\"  id =\"cls\" onclick=\"togglevisibility('clsClass')\" ></input>" << std::endl;
  out_ << " <input type =\"submit\" value= \"Function\" id =\"fun\" onclick=\"togglevisibility('funClass')\" ></input>" << std::endl;
}

// -----< private - add pre tag >------------------------------------------
/* seperated into seperate function to allow for dependencies addition
*  before the actual code of the file */
void CodeConverter::addPreTag()
{
  out_ << "    <pre>" << std::endl;
}

// -----< private - add depedency links markup code >----------------------
void CodeConverter::addDependencyLinks(std::string file)
{
  std::string filename = Path::getName(file);
  if (!dt_.has(file)) { // in case of single file conversion
    LoggerDebug::write("\n    No entry found in DependencyTable for [" + filename + "]. Skipping dependency links..");
    return;
  }

  if (dt_[file].size() == 0) { // in case the file has no dependencies
    LoggerDebug::write("\n    No dependencies found for [" + filename + "]. Skipping dependency links..");
    return;
  }

  out_ << "    <h3>Dependencies: " << std::endl;
  for (auto dep : dt_[file]) {
		
		out_ << "      <a href=\"" << Path::getValueInQuotes(dep) << ".html\">" << Path::getValueInQuotes(dep) << "</a>" << "<br/>" << std::endl;
  }
  out_ << "    </h3>";
}

// -----< private - add generic HTML markup closing tags >-----------------
void CodeConverter::addClosingTags()
{
  out_ << "    </pre>" << std::endl;
  out_ << "  </body>" << std::endl;
  out_ << "</html>" << std::endl;
}

// -----< private - replace HTML special chars >---------------------------
/* note: the function uses while loop to make sure ALL special characters
*  are replaced instead of just the first encounter. */
void CodeConverter::skipSpecialChars(std::string & line)
{
  size_t pos = line.npos;
  while((pos = line.find('<')) != line.npos) 
    line.replace(pos, 1, "&lt;");

  while ((pos = line.find('>')) != line.npos)
    line.replace(pos, 1, "&gt;");
}



#ifdef TEST_CONVERTER

// -----< test stub for converter class >-----------------------------------
int main() {
  LoggerDebug::attach(&std::cout);
  LoggerDebug::start();

  LoggerDebug::title("Testing CodeConverter functions");

  LoggerDebug::stop(); // do not log DependencyTable logs
  
  DependencyTable dt;
  dt.addDependency("Converter.h", "Converter.cpp");
  dt.addDependency("Converter.cpp", "Converter.h");
  dt.addDependency("A.h", "A.h");
  
  LoggerDebug::start(); // log from now on

  CodeConverter cc(dt);
  dt.display();
  LoggerDebug::write("\n");
  //cc.outputDir("C:\\windows\\system32\\"); // will trigger errors which are handled
  cc.convert();

  LoggerDebug::stop();
  return 0;
}

#endif