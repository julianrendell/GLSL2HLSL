#include <iostream>

using namespace std;


__declspec(dllexport) string HelloWorld()
{
	return "Hello World!";
}

//extern "C" 
//{
//	__declspec(dllexport) string __cdecl TaDa()
//	{
//		return "Ta da!";
//	}
//}