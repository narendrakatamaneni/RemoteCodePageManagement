#pragma once
/////////////////////////////////////////////////////////////////////
// IComm.h - interface for message-passing communication facility  //
// ver 1.0                                                         //
// Jim Fawcett, CSE687-OnLine Object Oriented Design, Fall 2017    //
/////////////////////////////////////////////////////////////////////

/*
*  Package Operations:
*  -------------------
*  This package defines Sender and Receiver classes.
*  - Sender uses a SocketConnecter and supports connecting to multiple
*    sequential endpoints and posting messages.
*  - Receiver uses a SocketListener which returns a Socket on connection.
*  It also defines a Comm class
*  - Comm simply composes a Sender and a Receiver, exposing methods:
*    postMessage(Message) and getMessage()
*
*  Required Files:
*  ---------------
*  Message.h
*
*  Maintenance History:
*  --------------------
*  ver 2.2 : 27 Mar 2018
*  - added interface IComm and object factory (static method in IComm)
*  - Comm now implements the IComm interface
*  - No change to code other that derive from interface and implement its create method
*  ver 2.1 : 25 Mar 2018
*  - added declaration of SocketSystem in Comm class
*  ver 2.0 : 07 Oct 2017
*  - added sendFile(...) as private member of Sender
*  - added receiveFile() as member of ClientHandler
*  ver 1.0 : 03 Oct 2017
*  - first release
*/

#include <string>
#include "../Message/Message.h"

namespace MsgPassingCommunication
{
  class IComm
  {
  public:
    static IComm* create(const std::string& machineAddress, size_t port);
    virtual void start(const std::string& rcvpath= "../SaveFiles", const std::string& sndPath= "../SendFiles") = 0;
    virtual void stop() = 0;
    virtual void postMessage(Message msg) = 0;
    virtual Message getMessage() = 0;
    virtual std::string name() = 0;
    virtual ~IComm() {}
  };
}