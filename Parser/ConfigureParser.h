#ifndef CONFIGUREPARSER_H
#define CONFIGUREPARSER_H
/////////////////////////////////////////////////////////////////////
//  ConfigureParser.h - builds and configures parsers              //
//  ver 1.0                                                        //
//  Author:        Narendra Katamaneni,Syracuse University		   //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module builds and configures parsers.  It builds the parser
  parts and configures them with application specific rules and actions.

  Public Interface:
  =================
  ConfigParseForCodeAnal config;
  config.Build();
  config.Attach(someFileName);

  Build Process:
  ==============
  Required files
    -  Parser.h,
      ActionsAndRules.h,
      Semi.h, Toker.h
  Build commands (either one)
    - devenv Project1HelpS06.sln
    - cl /EHsc /DTEST_PARSER ConfigureParser.cpp parser.cpp \
         ActionsAndRules.cpp \
         Semiression.cpp tokenizer.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.0 : 28 Feb 2019
  - first release
*/

#include <fstream>
#include "Parser.h"
#include "ActionsAndRules.h"
#include "../SemiExpression/Semi.h"
#include "../Tokenizer/Toker.h"

namespace CodeAnalysis
{
  ///////////////////////////////////////////////////////////////
  // build parser that writes its output to console

  class ConfigParseForCodeAnal : IBuilder
  {
  public:
    ConfigParseForCodeAnal() /*: pIn(nullptr)*/ {
	};
    ~ConfigParseForCodeAnal();
    bool Attach(const std::string& name, bool isFile = true);
    Parser* Build();

  private:
    // Builder must hold onto all the pieces

    //std::ifstream* pIn = nullptr;
    Lexer::Toker* pToker;
    Lexer::Semi* pSemi;
    Parser* pParser;
    Repository* pRepo;

    // add Rules and Actions

    BeginScope* pBeginScope = nullptr;
    HandleBeginScope* pHandleBeginScope = nullptr;

    EndScope* pEndScope = nullptr;
    HandleEndScope* pHandleEndScope = nullptr;

    PreprocStatement* pPreprocStatement = nullptr;
    HandlePreprocStatement* pHandlePreprocStatement = nullptr;

    NamespaceDefinition* pNamespaceDefinition = nullptr;
    HandleNamespaceDefinition* pHandleNamespaceDefinition = nullptr;

    ClassDefinition* pClassDefinition = nullptr;
    HandleClassDefinition* pHandleClassDefinition = nullptr;

    StructDefinition* pStructDefinition = nullptr;
    HandleStructDefinition* pHandleStructDefinition = nullptr;

    CppFunctionDefinition* pCppFunctionDefinition = nullptr;
    HandleCppFunctionDefinition* pHandleCppFunctionDefinition = nullptr;
    //PrintFunction* pPrintFunction;

    CSharpFunctionDefinition* pCSharpFunctionDefinition = nullptr;
    HandleCSharpFunctionDefinition* pHandleCSharpFunctionDefinition = nullptr;

    ControlDefinition* pControlDefinition = nullptr;
    HandleControlDefinition* pHandleControlDefinition = nullptr;

    CppDeclaration* pCppDeclaration = nullptr;
    HandleCppDeclaration* pHandleCppDeclaration = nullptr;

    CSharpDeclaration* pCSharpDeclaration = nullptr;
    HandleCSharpDeclaration* pHandleCSharpDeclaration = nullptr;

    CppExecutable* pCppExecutable = nullptr;
    HandleCppExecutable* pHandleCppExecutable = nullptr;

    CSharpExecutable* pCSharpExecutable = nullptr;
    HandleCSharpExecutable* pHandleCSharpExecutable = nullptr;

    Default* pDefault = nullptr;
    HandleDefault* pHandleDefault = nullptr;

    // prohibit copies and assignments

    ConfigParseForCodeAnal(const ConfigParseForCodeAnal&) = delete;
    ConfigParseForCodeAnal& operator=(const ConfigParseForCodeAnal&) = delete;
  };
}
#endif
