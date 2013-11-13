// AngleTranslatorDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "AngleTranslatorDLL.h"

// This is an example of an exported variable
ANGLETRANSLATORDLL_API int nAngleTranslatorDLL=0;

// This is an example of an exported function.
ANGLETRANSLATORDLL_API int fnAngleTranslatorDLL(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see AngleTranslatorDLL.h for the class definition
CAngleTranslatorDLL::CAngleTranslatorDLL()
{
	return;
}

int Initialize()
{
	return ShInitialize();
}

//ShHandle ConstructCompiler()
//{
//	//ShConstructCompiler();
//
//	return NULL;
//}
//
//int Compile()
//{
//	//ShCompile();
//
//	return 0;
//}
//
//void Destruct()
//{
//	//ShDestruct();
//}
//
//int Finalize()
//{
//	return ShFinalize();
//}