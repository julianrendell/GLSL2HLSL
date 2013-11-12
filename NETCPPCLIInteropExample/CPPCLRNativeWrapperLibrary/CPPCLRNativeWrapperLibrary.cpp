// This is the main DLL file.


#include "stdafx.h"

// marshall.h must be included first as it blindly redefines things... leading to compiler errors if it's not included first.
#include <msclr/marshal.h> 
using namespace msclr::interop; 

#include "CPPCLRNativeWrapperLibrary.h"
using namespace CPPCLRNativeWrapperLibrary; 

CPPCLRWrapper::CPPCLRWrapper(void) 
{ 
    m_nativeCppLibInstance = new CNativeCPPLibrary(); 
} 
 
CPPCLRWrapper::~CPPCLRWrapper(void) 
{ 
    if (m_nativeCppLibInstance) 
    { 
        delete m_nativeCppLibInstance; 
        m_nativeCppLibInstance = NULL; 
    } 
} 
 
CPPCLRWrapper::!CPPCLRWrapper(void) 
{ 
    if (m_nativeCppLibInstance) 
    { 
        delete m_nativeCppLibInstance; 
        m_nativeCppLibInstance = NULL; 
    } 
} 

String^ CPPCLRWrapper::DoIt(String^ input) {
	// convert managed string to native
	using namespace Runtime::InteropServices;
	const char* chars = 
      (const char*)(Marshal::StringToHGlobalAnsi(input)).ToPointer();
	string nativeInput = chars;

	// Seems to me that doing a free here could invalidate the nativeInput variable.
	// Marshal::FreeHGlobal(IntPtr((void*)chars));

	string nativeOutput = m_nativeCppLibInstance->DoIt(nativeInput);
	Marshal::FreeHGlobal(IntPtr((void*)chars));

	String^ netString = gcnew String(nativeOutput.c_str());

	return netString;
}