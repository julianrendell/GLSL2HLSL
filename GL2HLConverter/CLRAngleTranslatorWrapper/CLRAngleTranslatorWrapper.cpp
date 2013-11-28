// This is the main DLL file.


#include "stdafx.h"
#include <vector>

// marshall.h must be included first as it blindly redefines things... leading to compiler errors if it's not included first.
#include <msclr/marshal.h> 
using namespace msclr::interop; 

#include "CLRAngleTranslatorWrapper.h"
using namespace CLRAngleTranslatorWrapper; 

CLRAngleTranslator ^ CLRAngleTranslator::Instance::get()
{
	if (instance == nullptr) 
	{
		instance = gcnew CLRAngleTranslator();
	}
		
	return instance; 
}


CLRAngleTranslator::CLRAngleTranslator()
{
	int ret = ShaderInitialize();

	if (ret == 0)
	{
		throw gcnew ApplicationException("Could not initialize Angle translate library");
	}
}

CLRAngleTranslator::CLRAngleTranslator(const CLRAngleTranslator%)
{ 
	throw gcnew System::InvalidOperationException("Singleton cannot be copy-constructed");
}

CLRAngleTranslator::~CLRAngleTranslator()
{
	this->!CLRAngleTranslator();
}

CLRAngleTranslator::!CLRAngleTranslator()
{
	int ret = ShaderFinalize();

	if (ret == 0)
	{
		throw gcnew ApplicationException("Failed to finalize Angle translate library");
	}
}

String ^ CLRAngleTranslator::ConvertToHLSL(String ^ shader)
{
	ShBuiltInResources resources;

	ShaderInitBuiltInResources(&resources);

    resources.MaxVertexAttribs = 8;
    resources.MaxVertexUniformVectors = 128;
    resources.MaxVaryingVectors = 8;
    resources.MaxVertexTextureImageUnits = 0;
    resources.MaxCombinedTextureImageUnits = 8;
    resources.MaxTextureImageUnits = 8;
    resources.MaxFragmentUniformVectors = 16;
    resources.MaxDrawBuffers = 1;

    resources.OES_standard_derivatives = 0;
    resources.OES_EGL_image_external = 0;

	// WARNING: THIS IS ONLY FOR FRAGMENT SHADER!
	ShHandle handle = ShaderConstructCompiler(SH_FRAGMENT_SHADER, SH_GLES2_SPEC, SH_HLSL11_OUTPUT, &resources);

	IntPtr ip = Marshal::StringToHGlobalAnsi(shader);
	const char* str = static_cast<const char*>(ip.ToPointer());

	// Some examples split num of strings to 2 (or more)...
	int ret = ShaderCompile(handle, &str, 1, SH_OBJECT_CODE);

	str = NULL;
	Marshal::FreeHGlobal(ip);

	char* buffer = NULL;
	size_t bufferLen = 0;

	if (ret)
	{
		ShaderGetInfo(handle, SH_OBJECT_CODE_LENGTH, &bufferLen);
		buffer = (char*) realloc( buffer, bufferLen * sizeof(char));

		ShaderGetObjectCode(handle, buffer);
	}
	else
	{
		throw gcnew ApplicationException("AHHHHHHHHHH");
	}
	
	String^ ms = Marshal::PtrToStringAnsi(static_cast<IntPtr>(buffer));

	if (buffer)
	{
		free(buffer);
	}

	return ms;
}

void CLRAngleTranslator::GetShader(String ^ vShader, String ^ fShader)
{
	IntPtr fip = Marshal::StringToHGlobalAnsi(vShader);
	const char* vSrc = static_cast<const char*>(fip.ToPointer());

	IntPtr vip = Marshal::StringToHGlobalAnsi(fShader);
	const char* fSrc = static_cast<const char*>(vip.ToPointer());
	
	humongoid(vSrc, fSrc, LEFT_CARTESIAN);
	
	Marshal::FreeHGlobal(fip);
	Marshal::FreeHGlobal(vip);
}

