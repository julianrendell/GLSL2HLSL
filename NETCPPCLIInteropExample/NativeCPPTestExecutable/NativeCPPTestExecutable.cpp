// NativeCPPTestExecutable.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include "NativeCPPLibrary.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Hello there!\n";

	CNativeCPPLibrary* nativeLibObject = new CNativeCPPLibrary();
	string output = nativeLibObject->DoIt("string from the main native C++ application");

	cout << "Output from calling library code:" <<endl;
	cout << output << endl;

	cout << endl << "Press return to continue" << endl;
	cout << endl ;

	string c;
	getline(cin, c);

	return 0;
}

