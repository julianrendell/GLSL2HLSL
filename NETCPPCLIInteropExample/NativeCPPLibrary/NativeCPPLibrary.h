// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the NATIVECPPLIBRARY_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// NATIVECPPLIBRARY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef NATIVECPPLIBRARY_EXPORTS
#define NATIVECPPLIBRARY_API __declspec(dllexport)
#else
#define NATIVECPPLIBRARY_API __declspec(dllimport)
#endif

#include <iostream>
#include <string>

using namespace std;

// This class is exported from the NativeCPPLibrary.dll
class NATIVECPPLIBRARY_API CNativeCPPLibrary {
public:
	CNativeCPPLibrary(void);
	// TODO: add your methods here.

	string DoIt(string input);
};

//extern NATIVECPPLIBRARY_API int nNativeCPPLibrary;

//NATIVECPPLIBRARY_API int fnNativeCPPLibrary(void);
