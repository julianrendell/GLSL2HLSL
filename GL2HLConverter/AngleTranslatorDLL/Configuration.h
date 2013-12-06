#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>
#include "Angletypes.h"
#include "GLESUniform.h"
#include "ProgramBinary.h"

static CoordinateSystemType coordinateSystemType;

namespace gl
{
	class HlslBag
	{
		public:
			HlslBag()
			{
				vertexHlsl = NULL;
				fragmentHlsl = NULL;
				samplerVS = NULL;
			}

			~HlslBag()
			{
			}

			const char *vertexHlsl;
			const char *fragmentHlsl;

			std::vector<Uniform*> *uniforms;
			Attribute *linkedAttributes;
			Sampler *samplerVS;
			Sampler *samplerPS;
	};

}

#endif