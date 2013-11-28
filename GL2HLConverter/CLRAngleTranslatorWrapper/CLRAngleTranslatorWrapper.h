// CPPCLRNativeWrapperLibrary.h

#pragma once

//#include "AngleTranslatorDLL.h"
#include "GLSLConverter.h"

using namespace System;
using namespace System::Runtime::InteropServices; 

namespace CLRAngleTranslatorWrapper {

	public ref class CLRAngleTranslator
	{
	private:
		static CLRAngleTranslator ^ instance = nullptr;

		CLRAngleTranslator();
		CLRAngleTranslator(const CLRAngleTranslator%);

	public:
		static property CLRAngleTranslator^ Instance { CLRAngleTranslator^ get(); };

        ~CLRAngleTranslator();

		String ^ ConvertToHLSL(String ^ shader);
		void GetShader(String ^ vShader, String ^ fShader);

	protected: 
        !CLRAngleTranslator();
	};
}

