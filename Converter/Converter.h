#pragma once
///////////////////////////////////////////////////////////////////////////
// Converter.h   : defines source code conversion to webpage functions   //
// ver 1.0                                                               //
// Author        :Jim Fawcett, Syracuse University               //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines CodeConverter class which accepts DependencyTable
*  and uses its relationships to create linked webpages that point to each
*  other based on dependencies. It can also convert single files if needed.
*  The conversion process filters HTML special characters before printing
*  them into output files. The resulting output of this converter is a list
*  of files (vector<string>) of the created webpages. 
*
*  Required Files:
* =======================
*   DependencyTable.h , AbstrSynTree.h
*
*  Maintainence History:
* =======================
*  ver 1.0 - 11 Feb 2019
*  - first release
*/

#include "../Dependencies/DependencyTable.h"
#include <vector>
#include <string>
#include <fstream>
#include "../AbstractSyntaxTree/AbstrSynTree.h"



class CodeConverter
{
public:
  CodeConverter();
  CodeConverter(const DependencyTable& dt);

  const std::string& outputDir() const;
  void outputDir(const std::string& dir);
  bool createOutpurDir();

  void setDepTable(const DependencyTable& dt);

  std::vector<std::string> convert();
  std::string convert(const std::string& filepath);
  std::vector<std::string> convert(const std::vector<std::string>& files);
  std::vector<std::string> convertFile(std::vector<std::string> files);
  
  const std::vector<std::string> convertedFiles() const;

  void clear();

private:
  bool convertFile(std::string file);
  void addPreCodeHTML(const std::string& title);
  void addPreTag();
  void addDependencyLinks(std::string file);
  void addClosingTags();
  void skipSpecialChars(std::string& line);
  void writeLine(CodeAnalysis::ASTNode* pGlobalScope, std::ifstream& in);
  void collectDependencies(CodeAnalysis::ASTNode* pGlobalScope, std::string file);
  bool processDependencyFiles(std::string file);
  void htmlOpeartions(const std::string& filename, std::string file, CodeAnalysis::ASTNode* pGlobalScope, std::ifstream & in);

private:
  DependencyTable dt_;
  std::string outputDir_ = "../ConvertedWebpages/";
  std::vector<std::string> convertedFiles_;
  std::ofstream out_;
  static std::vector<std::string>& static_vector;
};

