#pragma once
/////////////////////////////////////////////////////////////////////////////
// IPublisher.h  : defines an interface to a worker C++ class              //
// ver 1.0                                                                 //
// Author  : Narendra Katamaneni, CSE687, Syracuse University			   //
/////////////////////////////////////////////////////////////////////////////
/*
*  Package Description:
* ======================
*  This package defines a C++ class that uses <thread>, Cpp11-BlockingQueue
*  which uses <condition variable> and <mutex>. These concurrency classes
*  are not supported in CLR mode and cannot compile under /CLR flag.
*  Therefore, the implementation of the class was moved to the .cpp file
*  leaving the .h file free of any CLR-incompatible types. However, we
*  still need to use the C++ class in C++/CLI environment and under /CLR
*  code. To do so, we created an interface (ICppClass shown below) that
*  declares the methods we want the C++/CLI Shim to use. The concept of
*  inheritance allows the use of an interface pointer (IPublisher*) to
*  point into any derived (or implementation) class objects (Publisher).
*  Therefore, we declared an object factory that returned IPublisher*
*  and implemented the function in the .cpp file to return IPublisher*.
*  This way, the C++/CLI Shim will be able to use CppClass through
*  its interface without any knowledge about it and hence IPublisher'
*  CLR incompatible types are of no issue anymore.
*
*  The ObjectFactory (shown below) can be used without the 'extern "C"'
*  marker but it is not recommended to do so. The marker will prevent
*  the compiler from doing any namemangeling on the function allowing
*  the use of PInvoke if needed. For this application it doesn't make
*  a difference so you can use either.
*
*  Publisher is a simple demonstration class that contains two blocking-
*  queues and two threads in the following manner:
*    msgCreatorThread puts a message, every 800ms, in createdMessages queue
*    printingThread keeps listening on receivedMessages and prints any message
*
*  The created messages are meant to be shown in the WPF application.
*  the received messages are sent by the WPF application and displayed
*  on the console to demonstrate that native C++ code and C# code are
*  executing in the same program.
*
*  IMPORTANT NOTE: you cannot make an abstract declaration of Publisher like:
*                    class Publisher;
*                  This will make make Shim fail to build since it will not
*                  be able to find the real declaration and definitions of
*                  Publisher. The only way for Shim to use Publisher is through
*                  IPublisher pointer.
*
*  Required Files:
* =================
*  
*
*  Build Command:
* ================
* 
*
*  Maintainence History:
* =======================
*  ver 1.0 - March 23rd 2019
*    - first release
*
*/
#pragma once

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif

#include <string>
#include <iostream>
#include <vector>

// interface that allows Shim to use Publisher which contains
// CLR incompatible types (thread, condition variable and mutex)
// ONLY declare functions you WANT to use in the Shim and WPF app
class IPublisher {
public:
	virtual int SetCmd(int argc,char** argv) = 0;
	virtual std::vector<std::string> GetConvertFiles() = 0;
	virtual ~IPublisher() {};
};


// you can remove the | extern "C" | wrapper if you wish
	// Instantiates CppClass* as ICppClass*
extern "C" {
	struct ObjectFactory {
		// See Executive.cpp for implementation
		DLL_DECL IPublisher* createClient();
	};
}
