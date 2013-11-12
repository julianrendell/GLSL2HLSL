// NativeCPPLibrary.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "NativeCPPLibrary.h"


// This is an example of an exported variable
// NATIVECPPLIBRARY_API int nNativeCPPLibrary=0;

// This is an example of an exported function.
//NATIVECPPLIBRARY_API int fnNativeCPPLibrary(void)
//{
//	return 42;
//}

// This is the constructor of a class that has been exported.
// see NativeCPPLibrary.h for the class definition
CNativeCPPLibrary::CNativeCPPLibrary()
{
	return;
}

string CNativeCPPLibrary::DoIt(string input) {
	string a= input + string(" + extra text added by library");
	return a;
}