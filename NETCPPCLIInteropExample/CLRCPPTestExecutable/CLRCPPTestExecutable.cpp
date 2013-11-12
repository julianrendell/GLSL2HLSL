// CLRCPPTestExecutable.cpp : main project file.

#include "stdafx.h"

using namespace System;

using namespace CPPCLRNativeWrapperLibrary; 

int main(array<System::String ^> ^args)
{
	Console::WriteLine(L"Hello there!");

	CPPCLRWrapper^ cppClrWrapper = gcnew CPPCLRWrapper();

	String^ output = cppClrWrapper->DoIt(gcnew String(L"String from C++ CLR excecutable"));
		
	Console::WriteLine(L"Output from calling CLR wrapper library code:");
	Console::WriteLine(output);

	Console::WriteLine("Press return to continue");
	Console::ReadLine();

	return 0;
}
