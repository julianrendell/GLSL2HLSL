#include <iostream>

using namespace std;


__declspec(dllexport) string TaDa()
{
	return "Ta da!";
}

//extern "C" 
//{
//	__declspec(dllexport) string __cdecl TaDa()
//	{
//		return "Ta da!";
//	}
//}