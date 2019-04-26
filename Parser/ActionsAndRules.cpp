/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.cpp - implements new parsing rules and actions //
//  ver 3.4                                                        //
//  Author:        Narendra Katamaneni, CST 4-187, Syracuse University 
/////////////////////////////////////////////////////////////////////

#include "ActionsAndRules.h"
#include "../Utilities/Utilities.h"

using namespace CodeAnalysis;

Repository* Repository::instance;

#ifdef TEST_ACTIONSANDRULES

#include <iostream>
#include "ActionsAndRules.h"

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing ActionsAndRules class\n "
            << std::string(30,'=') << std::endl;

  try
  {
    Utilities::StringHelpers::Title(
      "Actions and Rules - these construction tests are no longer valid"
    );
    //PreprocStatement pps;
    //pps.addAction(&ppq);

    //FunctionDefinition fnd;
    //PrettyPrintToQ pprtQ(resultsQ);
    //fnd.addAction(&pprtQ);

    //Toker toker("../ActionsAndRules.h");
    //Semi se(&toker);
    //Parser parser(&se);
    //parser.addRule(&pps);
    //parser.addRule(&fnd);
    //while(se.get())
    //  parser.parse();
    //size_t len = resultsQ.size();
    //for(size_t i=0; i<len; ++i)
    //{
    //  std::cout << "\n  " << resultsQ.front().c_str();
    //  resultsQ.pop();
    //}
    std::cout << "\n\n";
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
  }
}
#endif
