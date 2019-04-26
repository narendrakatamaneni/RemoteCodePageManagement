/////////////////////////////////////////////////////////////////////////////
// Shim.cpp       : provides definitions and test stub for Shim.h          //
// ver 1.0                                                                 //
// Author         :Narendra Katamaneni,CSE687, Syracuse University		   //
/////////////////////////////////////////////////////////////////////////////

/*  Package Description :
*======================
*This package defines Shim class which can be used by.NET applications.
*  In this demo, Shim is used in WPF project to allow making calls from
*  WPF C# code to Publisher native C++ code.The Shim is a mangaged(.NET)
*  wrapper around IPubliseher interface which can make calls to native C++
*  classes and, in the same time, can be called by any.NET code(C# here).
*
*  Required Files :
*================ =
*	Shim.h 
*
*  Build Command :
*================
*msbuild Shim.vcxproj
*
*  Maintainence History :
*====================== =
*ver 1.0 - March 28th 2019
* -first release
*
*/
#include "Shim.h"
#include "string.h"
#include <vector>

// -----< Shim constructor - instantiates ICppClass >------------------
Shim::Shim() {
	Console::Write("\n  Shim created ");
	ObjectFactory factory;
	// use factory to get instance of Publisher
	client = factory.createClient();
	Console::Write("\n  Shim instantiated Publisher as IPublisher* ");
}


// -----< Shim destructor - deletes ICppClass >------------------------
Shim::~Shim() {
	delete client;
	Console::Write("\n  Shim destroyed\n\n");
}
void Shim::SetCmd(List<String^>^ argv) {

	int argc = argv->Count;
	char ** arg = new char*[argc];
	for (int i = 0;i < argc;i++) {
		std::string ar = sysStrToStdStr(argv[i]);
		arg[i] = new char[ar.size()+1];
		strcpy_s(arg[i], ar.size() + 1, ar.c_str());
	}
	client->SetCmd(argc, arg);
	delete[] arg;
}

List<String^>^ Shim::GetConvertedFiles() {
	std::vector<std::string> strvec = client->GetConvertFiles();
	
	List<String^>^ list = gcnew List<String^>();

	for (size_t i = 0;i < strvec.size();i++) {
		String^ str = stdStrToSysStr(strvec[i]);
		list->Add(str);
	}
	return list;
}

//----< convert std::string to System.String >-------------------------
String^ Shim::stdStrToSysStr(const std::string& str) {
	return gcnew String(str.c_str());
}

//----< convert System.String to std::string >-------------------------
std::string Shim::sysStrToStdStr(String^ str) {
	std::string temp;
	for (int i = 0; i < str->Length; ++i)
		temp += char(str[i]);
	return temp;
}


// -----< Shim Test Stub >---------------------------------------------
#ifdef TEST_SHIM

int main(array<System::String^> ^args) {
	Shim localShim;  // C++ style construction
	// Shim^ shim = gcnew Shim(); // managed heap allocation and construction
	localShim.Start();

	Console::Write("\n\n  Shim posting 10 messages to CppClass:");
	Console::Write("\n =======================================\n");
	for (int i = 0; i < 10; i++)
		localShim.PostMessage("Message from C++/CLI Shim Test Stub");

	// sleep for 100ms to allow better formatted output
	System::Threading::Thread::Sleep(100);

	Console::Write("\n\n  Shim getting 10 messages from CppClass:");
	Console::Write("\n =========================================\n");
	for (int i = 0; i < 10; i++)
		Console::Write("\n  Shim got message from CppClass: {0}", localShim.GetMessage());

	Console::Write("\n\n  Finished demonstration.");
	Console::Write("\n  Shim dtor is automatically called after 'main' goes out of scope.\n");

	return 0;
} // NOTE: Shim destructor will automatically be called here just like in native C++

#endif