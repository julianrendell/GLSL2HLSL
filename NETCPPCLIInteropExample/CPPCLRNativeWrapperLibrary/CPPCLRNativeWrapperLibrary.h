// CPPCLRNativeWrapperLibrary.h

#pragma once

#include "NativeCPPLibrary.h"
using namespace System;
using namespace System::Runtime::InteropServices; 

namespace CPPCLRNativeWrapperLibrary {

	public ref class CPPCLRWrapper
	{
		// TODO: Add your methods for this class here.
	public:
		CPPCLRWrapper();
        virtual ~CPPCLRWrapper(void); 

		String ^ DoIt( String^ input);

	protected: 
        !CPPCLRWrapper(void); 

	private:
		CNativeCPPLibrary * m_nativeCppLibInstance;
		
	};
}

