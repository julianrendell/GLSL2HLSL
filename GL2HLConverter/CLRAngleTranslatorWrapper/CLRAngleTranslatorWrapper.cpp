// This is the main DLL file.


#include "stdafx.h"

// marshall.h must be included first as it blindly redefines things... leading to compiler errors if it's not included first.
#include <msclr/marshal.h> 
using namespace msclr::interop; 

#include "CLRAngleTranslatorWrapper.h"
using namespace CLRAngleTranslatorWrapper; 

CLRAngleTranslator::CLRAngleTranslator(void) 
{ 
} 
 
CLRAngleTranslator::~CLRAngleTranslator(void) 
{ 
} 
 
CLRAngleTranslator::!CLRAngleTranslator(void) 
{ 
} 

String^ CLRAngleTranslator::DoIt(String^ input) {
	String^ netString = gcnew String("Hello World");

	return netString;
}