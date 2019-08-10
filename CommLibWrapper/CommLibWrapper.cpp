/////////////////////////////////////////////////////////////////////
// CommLibWrapper.cpp - Comm object factory                        //
// ver 1.0                                                         //
// Jim Fawcett, CSE687-OnLine Object Oriented Design, Fall 2017    //
/////////////////////////////////////////////////////////////////////

/*
*  Provides export and import declarations for Comm
*
*  Required Files:
* -----------------
*  CommLibWrapper.h, Comm.h
*
*  Maintenance History:
* ----------------------
*  ver 1.0 : 27 Mar 2018
*  - first release
*/

#define IN_DLL

#include "CommLibWrapper.h"
#include "../CppCommWithFileXfer/MsgPassingComm/Comm.h"  // definition of create

using namespace MsgPassingCommunication;

DLL_DECL IComm* CommFactory::create(const std::string& machineAddress, size_t port)
{
  std::cout << "\n  using CommFactory to invoke IComm::create";
  return IComm::create(machineAddress, port);
}


