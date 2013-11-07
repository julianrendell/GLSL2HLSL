// ConsoleApplication1.cpp : main project file.

#include "stdafx.h"

using namespace System;

int main(array<System::String ^> ^args)
{
    AngleCLR::AngleCLRTest ^ a = gcnew AngleCLR::AngleCLRTest();
	Console::WriteLine(a->Draw());
	Console::ReadKey();

    return 0;
}
