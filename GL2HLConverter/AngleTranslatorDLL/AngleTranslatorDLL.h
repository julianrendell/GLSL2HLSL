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

// This class is exported from the AngleTranslatorDLL.dll
class ANGLETRANSLATORDLL_API CAngleTranslatorDLL {
public:
	CAngleTranslatorDLL(void);
	// TODO: add your methods here.

	int Initialize();

	ShHandle ConstructCompiler();

	int Compile();

	void Destruct();

	int Finalize();
};

extern ANGLETRANSLATORDLL_API int nAngleTranslatorDLL;

ANGLETRANSLATORDLL_API int fnAngleTranslatorDLL(void);
