// This is the main DLL file.

#include "stdafx.h"
#include "AngleCLR.h"
#include "Header.h"

using namespace AngleCLR;
using namespace std;

String ^ AngleCLR::AngleCLRTest::Draw()
{
	string result = TaDa();
	String ^ greeting = gcnew String(result.c_str());

	return greeting;
}