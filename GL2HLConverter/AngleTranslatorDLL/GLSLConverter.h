#ifndef MONOLITH_H_
#define MONOLITH_H_

#include "AngleTranslatorDLL.h"
#include "Program.h"

HlslBag *bag;

ANGLETRANSLATORDLL_API HlslBag* humongoid(const char * fragmentShaderSrc, const char * vertexShaderSrc, CoordinateSystemType coordinateSystem);


#endif