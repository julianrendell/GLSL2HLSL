#include "Program.h"

namespace gl
{
	AttributeBindings::AttributeBindings()
	{
	}

	AttributeBindings::~AttributeBindings()
	{
	}

	Program::Program()
	{
		programBinary = new ProgramBinary();
	}

	Program::~Program()
	{
	}

	void AttributeBindings::bindAttributeLocation(GLuint index, const char *name)
	{
		if (index >= MAX_VERTEX_ATTRIBS)
		{
			throw new std::runtime_error("Unable to bind attribute locations. Index exceeds max vertex attribute count.");
		}

		if (strncmp(name, "gl_", 3) == 0)
		{
			throw new std::runtime_error("The attribute name does not start with 'gl_'");
		}

		if (index < MAX_VERTEX_ATTRIBS)
		{
			for (int i = 0; i < MAX_VERTEX_ATTRIBS; i++)
			{
				mAttributeBinding[i].erase(name);
			}

			mAttributeBinding[index].insert(name);
		}
	}

	void Program::attachShaders(VertexShader *vShader, FragmentShader *fShader)
	{
		mVertexShader = vShader;
		mFragmentShader = fShader;
	}

	void Program::bindAttributeLocation(GLuint index, const char *name)
	{
		mAttributeBindings.bindAttributeLocation(index, name);
	}

	void Program::link()
	{
		programBinary->link(mAttributeBindings, mFragmentShader, mVertexShader);
	}
}