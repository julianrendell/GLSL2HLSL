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

ManagedHlslBag ^ CLRAngleTranslator::ConvertToHlsl(String ^ vShader, String ^ fShader)
{
	IntPtr fip = Marshal::StringToHGlobalAnsi(vShader);
	const char* vSrc = static_cast<const char*>(fip.ToPointer());

	IntPtr vip = Marshal::StringToHGlobalAnsi(fShader);
	const char* fSrc = static_cast<const char*>(vip.ToPointer());
	
	HlslBag *bag = humongoid(vSrc, fSrc, LEFT_CARTESIAN);
	
	ManagedHlslBag ^mBag = gcnew ManagedHlslBag(); 
	mBag->vertexHLSL = Marshal::PtrToStringAnsi(static_cast<IntPtr>((char *)bag->vertexHlsl));
	mBag->fragmentHLSL = Marshal::PtrToStringAnsi(static_cast<IntPtr>((char *)bag->fragmentHlsl));

	Marshal::FreeHGlobal(fip);
	Marshal::FreeHGlobal(vip);

	return mBag;
}

