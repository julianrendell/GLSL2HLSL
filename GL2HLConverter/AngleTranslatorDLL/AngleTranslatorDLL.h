// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ANGLETRANSLATORDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ANGLETRANSLATORDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ANGLETRANSLATORDLL_EXPORTS
#define ANGLETRANSLATORDLL_API __declspec(dllexport)
#else
#define ANGLETRANSLATORDLL_API __declspec(dllimport)
#endif

#include "GLSLANG\ShaderLang.h"

ANGLETRANSLATORDLL_API int Initialize();

//ANGLETRANSLATORDLL_API ShHandle ConstructCompiler();
//
//ANGLETRANSLATORDLL_API int Compile();
//
//ANGLETRANSLATORDLL_API void Destruct();
//
//ANGLETRANSLATORDLL_API int Finalize();