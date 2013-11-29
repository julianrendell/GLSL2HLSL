#ifndef UTILITIES_H_
#define UTILITIES_H_

#define _CRT_SECURE_NO_WARNINGS

#include <stdexcept>
#include "GLES2\gl2.h"

int VariableColumnCount(GLenum type);
int VariableRowCount(GLenum type);

int AllocateFirstFreeBits(unsigned int *bits, unsigned int allocationSize, unsigned int bitsSize);

std::string str(int i);
char* cstr(std::string str);

GLenum UniformComponentType(GLenum type);
size_t UniformInternalSize(GLenum type);
size_t UniformExternalSize(GLenum type);


#endif