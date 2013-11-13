// CPPCLRNativeWrapperLibrary.h

#pragma once

#include "AngleTranslatorDLL.h"
using namespace System;
using namespace System::Runtime::InteropServices; 

namespace CLRAngleTranslatorWrapper {

	public ref class CLRAngleTranslator
	{
	private:
		CLRAngleTranslator();

		CLRAngleTranslator(const CLRAngleTranslator%);
		static CLRAngleTranslator ^ instance = nullptr;

	public:
		static property CLRAngleTranslator^ Instance { CLRAngleTranslator^ get(); };

        ~CLRAngleTranslator();

		String ^ DoIt( String^ input);

	protected: 
        !CLRAngleTranslator();
	};
}

