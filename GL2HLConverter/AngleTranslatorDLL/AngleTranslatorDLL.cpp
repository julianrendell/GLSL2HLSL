// AngleTranslatorDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "AngleTranslatorDLL.h"

ANGLETRANSLATORDLL_API int Initialize()
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