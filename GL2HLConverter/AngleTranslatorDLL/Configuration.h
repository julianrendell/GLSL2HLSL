#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>
#include "Angletypes.h"

static CoordinateSystemType coordinateSystemType;


class HlslBag
{
	public:
		HlslBag()
		{
			vertexHlsl = NULL;
			fragmentHlsl = NULL;
		}

		~HlslBag()
		{
		}

		const char *vertexHlsl;
		const char *fragmentHlsl;
};

#endif