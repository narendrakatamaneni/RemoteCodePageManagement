/////////////////////////////////////////////////////////////////////////////
// Shim.h  : defines a warpper for ICppClass which works with .NET code    //
// ver 1.0                                                                 //
// Author  : Narendra Katamaneni, CSE687, Syracuse University			   //
/////////////////////////////////////////////////////////////////////////////
/*
*  Package Description:
* ======================
*  This package defines Shim class which can be used by .NET applications.
*  In this demo, Shim is used in WPF project to allow making calls from
*  WPF C# code to Publisher native C++ code. The Shim is a mangaged (.NET)
*  wrapper around IPublisher interface which can make calls to native C++
*  classes and, in the same time, can be called by any .NET code (C# here).
*
*  Required Files:
* =================
*  IPublisher.h
*
*  Build Command:
* ================
*  msbuild Shim.vcxproj
*
*  Maintainence History:
* =======================
*  ver 1.0 - March 28th 2019
*    - first release
*
*/

#pragma once

#include <string>
#include "../Executive/IPublisher.h"

using namespace System;
using namespace System::Collections::Generic;

// 'public ref' identifier tells the compiler this is
// a managed class, not native C++ class. this class
// has to be managed for the C# code in WPF to be able
// to use it. You CAN define native classes here but in
// this demo there is no point to do so. 
public ref class Shim {
public:
	Shim();
	~Shim();
	virtual void SetCmd(List<String^>^ argv);
	virtual List<String^>^ GetConvertedFiles();

private:
	// convert System::String to std::string
	std::string sysStrToStdStr(String^ str);
	// convert std::string to System::String
	String^ stdStrToSysStr(const std::string& str);
	// this will point to Publisher when instantiated through the ObjectFactory
	IPublisher* client;
};
