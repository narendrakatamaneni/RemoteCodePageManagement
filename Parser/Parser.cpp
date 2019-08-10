/////////////////////////////////////////////////////////////////////
//  Parser.cpp - Analyzes C++ and C# language constructs           //
//  ver 1.0                                                        //
//  Author:        Jim Fawcett,  Syracuse University				//
/////////////////////////////////////////////////////////////////////

/*
  Module Operations:
  ==================
  This module defines a Parser class.  Its instances collect
  semi-expressions from one or more files for analysis.
  Analysis consists of applying a set of rules to the semi-expression,
  and for each rule that matches, invoking a set of one or more actions.

  Build Process:
  ==============
  Required files
	- Utilities.h,Toker.h,Semi.h,Parser.h,ActionsAndRules.h,ConfigureParser.h

  Maintenance History:
  ===================
  ver 1.0 : 11 Feb 2019
  - first release
  - This is a modification of the parser used in CodeAnalyzer.
  - The most important change is replacing its tokenizer with a
	State Pattern based design

*/

#include <iostream>
#include <string>
#include "../Utilities/Utilities.h"
#include "../Tokenizer/Toker.h"
#include "../SemiExpression/Semi.h"
#include "Parser.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"

//using namespace CodeAnalysis;
using namespace Lexer;
using namespace Utilities;
using Demo = Logging::StaticLogger<1>;

namespace CodeAnalysis {

  //----< register parsing rule >--------------------------------

  void Parser::addRule(IRule* pRule)
  {
    rules.push_back(pRule);
  }
  //----< get next ITokenCollection >------------------------------

  bool Parser::next()
  {
    bool succeeded = pTokColl->get();
    if (!succeeded)
    {
      return false;
    }

    pTokColl->trim();

    return true;
  }

  //----< parse the Semi by applying all rules to it >--------

  bool Parser::parse()
  {
    for (size_t i = 0; i < rules.size(); ++i)
    {
      bool doWhat = rules[i]->doTest(pTokColl);
      if (doWhat == IRule::Stop)
        break;
    }
    return true;
  }
  //----< register action with a rule >--------------------------

  void IRule::addAction(IAction *pAction)
  {
    actions.push_back(pAction);
  }
  //----< invoke all actions associated with a rule >------------

  void IRule::doActions(const Lexer::ITokenCollection* pTokColl)
  {
    if (actions.size() > 0)
      for (size_t i = 0; i < actions.size(); ++i)
        actions[i]->doAction(pTokColl);
  }

  //----< test stub >--------------------------------------------
}

#ifdef TEST_PARSER

#include "../FileSystem/FileSystem.h"
#include <queue>
#include <string>
#define Util StringHelper

using namespace CodeAnalysis;

int main(int argc, char* argv[])
{
  Util::Title("Testing Parser Class");
  putline();

  // Analyzing files, named on the command line

  if(argc < 2)
  {
    std::cout 
      << "\n  please enter name of file to process on command line\n\n";
    return 1;
  }

  std::string fileSpec;

  for(int i=1; i<argc; ++i)  // iterate over files
  {
    fileSpec = FileSystem::Path::getFullFileSpec(argv[i]);
    std::string msg = "Processing file" + fileSpec;
    Util::title(msg);


	// configure the parser everytime you want to use parser
    ConfigParseForCodeAnal configure;
    Parser* pParser = configure.Build();

    std::string name;

    try
    {
		//if parser is valid or not
      if(pParser)
      {
        name = FileSystem::Path::getName(argv[i]);
        if(!configure.Attach(fileSpec))
        {
          std::cout << "\n  could not open file " << name << std::endl;
          continue;
        }
      }
      else
      {
        std::cout << "\n\n  Parser not built\n\n";
        return 1;
      }

      // save current package name

      Repository* pRepo = Repository::getInstance();
      pRepo->package() = name;

      // parse the package

      while (pParser->next())
      {
        pParser->parse();
      }
      std::cout << "\n";

      // final AST operations
      ASTNode* pGlobalScope = pRepo->getGlobalScope();

	  //Remove the below code
	  auto iter2 = pGlobalScope->statements_.begin();
	  while (iter2 != pGlobalScope->statements_.end()) {
		  std::cout << "\n " << std::string(2 * 4, ' ') << (*iter2)->ToString();
		  ++iter2;
	  }


      // walk AST, computing complexity for each node
      // and record in AST node's element

      complexityEval(pGlobalScope);
      
      // Walk AST, displaying each element, indented
      // by generation

      TreeWalk(pGlobalScope);
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
      std::cout << "\n  exception caught at line " << __LINE__ << " ";
      std::cout << "\n  in package \"" << name << "\"";
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}

#endif
