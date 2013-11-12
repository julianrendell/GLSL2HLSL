// CPPCLRNativeWrapperLibrary.h

#pragma once

#include "GLSLANG/ShaderLang.h"
using namespace System;
using namespace System::Runtime::InteropServices; 

namespace CLRAngleTranslatorWrapper {

	public ref class CLRAngleTranslator
	{
		// TODO: Add your methods for this class here.
	public:
		CLRAngleTranslator();
        virtual ~CLRAngleTranslator(void); 

		String ^ DoIt( String^ input);

	protected: 
        !CLRAngleTranslator(void);
	};
}

