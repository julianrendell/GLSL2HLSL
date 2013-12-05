using namespace System;
using namespace System::Runtime::InteropServices; 

namespace CLRAngleTranslatorWrapper
{
	public ref class ManagedHlslBag
	{
		public:
			String ^vertexHLSL;
			String ^fragmentHLSL;
	};
}