#ifndef UTILITIES_H_
#define UTILITIES_H_

#define _CRT_SECURE_NO_WARNINGS

#include <stdexcept>
#include "GLES2\gl2.h"

int VariableColumnCount(GLenum type);
int VariableRowCount(GLenum type);

int AllocateFirstFreeBits(unsigned int *bits, unsigned int allocationSize, unsigned int bitsSize);

std::string str(int i);


#endif