#pragma once
///////////////////////////////////////////////////////////////////////////
// Display.h     : defines webpage display using browser functions       //
// ver 1.0                                                               //
// Author        :Narendra Katamaneni, Syracuse University               //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines Display class which accepts a list of files as a
*  vector<string> and uses the default internet browser to display them
*  one by one. Please note that the functionality has limiations:
*   1) Opera/MS Edge: will pop-up all tabs instantly. 
*   2) Chrome/Firefox: will pop-up windows separately only if no
*      already existing Chrome/Firefox window is opened (all must be
*      closed before running this). 
*
*  Required Files:
* =======================
*  AbstrSynTree.h
*
*  Maintainence History:
* =======================
*  ver 1.0 - 14 Feb 2019
*  - first release
*/


#include <vector>
#include <string>
#include <unordered_map>
#include "../AbstractSyntaxTree/AbstrSynTree.h"

enum TypeInfo { class_begin, function_begin, end_scope };

class Display
{
public:
  Display();
  
  void display(const std::string& file);
  void display(const std::vector<std::string>& files);
  void handlecomments(std::string & line);
  void handleMultipleComments(std::string & line);
  void handlefunctions(std::string &line);
  void handleclass(std::string &line);
  void handleendscope(std::string & line);
  std::unordered_map<std::size_t, TypeInfo> collectFullDetails(CodeAnalysis::ASTNode* pGlobalScope, std::unordered_map<std::size_t, TypeInfo>& inUnorderedMap);

};

