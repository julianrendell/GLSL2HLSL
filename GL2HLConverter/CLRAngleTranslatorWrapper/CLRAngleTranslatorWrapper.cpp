// This is the main DLL file.


#include "stdafx.h"

// marshall.h must be included first as it blindly redefines things... leading to compiler errors if it's not included first.
#include <msclr/marshal.h> 
using namespace msclr::interop; 

#include "CLRAngleTranslatorWrapper.h"
using namespace CLRAngleTranslatorWrapper; 

CLRAngleTranslator ^ CLRAngleTranslator::Instance::get()
{
	if (instance == nullptr) {
			instance = gcnew CLRAngleTranslator();
		}
		return instance; 
}


CLRAngleTranslator::CLRAngleTranslator()
{
	/*int ret = ShInitialize();

	if (ret != 0)
	{
		throw gcnew ApplicationException("Could not initialize Angle translate library");
	}*/
}

CLRAngleTranslator::CLRAngleTranslator(const CLRAngleTranslator%)
{ 
	throw gcnew System::InvalidOperationException("singleton cannot be copy-constructed");
}


CLRAngleTranslator::~CLRAngleTranslator()
{
	this->!CLRAngleTranslator();
}

CLRAngleTranslator::!CLRAngleTranslator()
{
	//ShFinalize();
}

String^ CLRAngleTranslator::DoIt(String^ input) {
	String^ netString = gcnew String("Hello World");

	int ret = Initialize();

	return netString;
}