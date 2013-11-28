#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <set>
#include "GLES2\gl2.h"
#include "ProgramBinary.h"
#include "Shader.h"
#include "Constants.h"


class AttributeBindings
{
	public:
		AttributeBindings();
		~AttributeBindings();

		void bindAttributeLocation(GLuint index, const char *name);
		int getAttributeBinding(const std::string &name) const;

	private:
		std::set<std::string> mAttributeBinding[MAX_VERTEX_ATTRIBS];
};

class Program
{
	public:
		Program();
		~Program();

		void attachShaders(VertexShader *vShader, FragmentShader *fShader);
		void bindAttributeLocation(GLuint index, const char *name);
		void link();

	private:
		FragmentShader *mFragmentShader;
		VertexShader *mVertexShader;

		ProgramBinary *programBinary;

		AttributeBindings mAttributeBindings;
};

#endif