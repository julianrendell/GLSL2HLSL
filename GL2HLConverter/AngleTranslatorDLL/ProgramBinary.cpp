#include "ProgramBinary.h"

namespace gl
{
	ProgramBinary::ProgramBinary()
	{
		for (int index = 0; index < MAX_VERTEX_ATTRIBS; index++)
		{
			mSemanticIndex[index] = -1;
		}

		for (int index = 0; index < MAX_TEXTURE_IMAGE_UNITS; index++)
		{
			mSamplersPS[index].active = false;
		}

		for (int index = 0; index < IMPLEMENTATION_MAX_VERTEX_TEXTURE_IMAGE_UNITS; index++)
		{
			mSamplersVS[index].active = false;
		}

		mUsedVertexSamplerRange = 0;
		mUsedPixelSamplerRange = 0;
	}


	ProgramBinary::~ProgramBinary()
	{
		while (!mUniforms.empty())
		{
			delete mUniforms.back();
			mUniforms.pop_back();
		}
	}

	// This method needs to match OutputHLSL::decorate
	std::string ProgramBinary::decorateAttribute(const std::string &name)
	{
		if (name.compare(0, 3, "gl_") != 0 && name.compare(0, 3, "dx_") != 0)
		{
			return "_" + name;
		}
    
		return name;
	}

	GLint ProgramBinary::getUniformLocation(std::string name)
	{
		unsigned int subscript = 0;

		// Strip any trailing array operator and retrieve the subscript
		size_t open = name.find_last_of('[');
		size_t close = name.find_last_of(']');
		if (open != std::string::npos && close == name.length() - 1)
		{
			subscript = atoi(name.substr(open + 1).c_str());
			name.erase(open);
		}

		unsigned int numUniforms = mUniformIndex.size();
		for (unsigned int location = 0; location < numUniforms; location++)
		{
			if (mUniformIndex[location].name == name &&
				mUniformIndex[location].element == subscript)
			{
				return location;
			}
		}

		return -1;
	}

	bool ProgramBinary::defineUniform(GLenum shader, const sh::Uniform &constant)
	{
		if (constant.type == GL_SAMPLER_2D ||
			constant.type == GL_SAMPLER_CUBE)
		{
			unsigned int samplerIndex = constant.registerIndex;
            
			do
			{
				if (shader == GL_VERTEX_SHADER)
				{
					if (samplerIndex < MAX_TEXTURE_IMAGE_UNITS_VTF_SN4)
					{
						mSamplersVS[samplerIndex].active = true;
						mSamplersVS[samplerIndex].textureType = (constant.type == GL_SAMPLER_CUBE) ? TEXTURE_CUBE : TEXTURE_2D;
						mSamplersVS[samplerIndex].logicalTextureUnit = 0;
						mUsedVertexSamplerRange = std::max(samplerIndex + 1, mUsedVertexSamplerRange);
					}
					else
					{
						std::string msg = std::string("Vertex shader sampler count exceeds the maximum vertex texture units (") + str(MAX_TEXTURE_IMAGE_UNITS_VTF_SN4) + ")";

						throw new std::runtime_error(msg.c_str());
					}
				}
				else if (shader == GL_FRAGMENT_SHADER)
				{
					if (samplerIndex < MAX_TEXTURE_IMAGE_UNITS)
					{
						mSamplersPS[samplerIndex].active = true;
						mSamplersPS[samplerIndex].textureType = (constant.type == GL_SAMPLER_CUBE) ? TEXTURE_CUBE : TEXTURE_2D;
						mSamplersPS[samplerIndex].logicalTextureUnit = 0;
						mUsedPixelSamplerRange = std::max(samplerIndex + 1, mUsedPixelSamplerRange);
					}
					else
					{
						std::string msg = std::string("Pixel shader sampler count exceeds MAX_TEXTURE_IMAGE_UNITS (") + str(MAX_TEXTURE_IMAGE_UNITS_VTF_SN4) + ")";

						throw new std::runtime_error(msg.c_str());
					}
				}
				else throw new std::runtime_error("Unidentified shader object");

				samplerIndex++;
			}
			while (samplerIndex < constant.registerIndex + constant.arraySize);
		}

		gl::Uniform *uniform = NULL;
		GLint location = getUniformLocation(constant.name);

		if (location >= 0)   // Previously defined, type and precision must match
		{
			uniform = mUniforms[mUniformIndex[location].index];

			if (uniform->type != constant.type)
			{
				std::string msg = std::string("Types for uniform ") + uniform->name + " do not match between the vertex and fragment shader";

				throw new std::runtime_error(msg);
			}

			if (uniform->precision != constant.precision)
			{
				std::string msg = std::string("Precisions for uniform ") + uniform->name + " do not match between the vertex and fragment shader";

				throw new std::runtime_error(msg);
			}
		}
		else
		{
			uniform = new gl::Uniform(constant.type, constant.precision, constant.name, constant.arraySize);
		}

		if (!uniform)
		{
			return false;
		}

		if (shader == GL_FRAGMENT_SHADER)
		{
			uniform->psRegisterIndex = constant.registerIndex;
		}
		else if (shader == GL_VERTEX_SHADER)
		{
			uniform->vsRegisterIndex = constant.registerIndex;
		}
		else throw new std::runtime_error("Unidentified shader object");

		if (location >= 0)
		{
			return uniform->type == constant.type;
		}

		mUniforms.push_back(uniform);
		unsigned int uniformIndex = mUniforms.size() - 1;

		for (unsigned int i = 0; i < uniform->elementCount(); i++)
		{
			mUniformIndex.push_back(UniformLocation(constant.name, i, uniformIndex));
		}

		if (shader == GL_VERTEX_SHADER)
		{
			if (constant.registerIndex + uniform->registerCount > 0 + MAX_VERTEX_UNIFORM_VECTORS_D3D11)
			{
				std::string msg = std::string("Vertex shader active uniforms exceed GL_MAX_VERTEX_UNIFORM_VECTORS (") + str(MAX_VERTEX_UNIFORM_VECTORS_D3D11) + ")";

				throw new std::runtime_error(msg.c_str());
			}
		}
		else if (shader == GL_FRAGMENT_SHADER)
		{
			if (constant.registerIndex + uniform->registerCount > 0 + MAX_VERTEX_UNIFORM_VECTORS_D3D11)
			{
				std::string msg = std::string("Fragment shader active uniforms exceed GL_MAX_FRAGMENT_UNIFORM_VECTORS (") + str(MAX_FRAGMENT_UNIFORM_VECTORS_D3D11) + ")";

				throw new std::runtime_error(msg.c_str());
			}
		}
		else throw new std::runtime_error("Unidentified shader object");

		return true;
	}

	bool ProgramBinary::linkUniforms(sh::ActiveUniforms vertexUniforms, sh::ActiveUniforms fragmentUniforms)
	{
		for (sh::ActiveUniforms::const_iterator uniform = vertexUniforms.begin(); uniform != vertexUniforms.end(); uniform++)
		{
			if (!defineUniform(GL_VERTEX_SHADER, *uniform))
			{
				return false;
			}
		}

		for (sh::ActiveUniforms::const_iterator uniform = fragmentUniforms.begin(); uniform != fragmentUniforms.end(); uniform++)
		{
			if (!defineUniform(GL_FRAGMENT_SHADER, *uniform))
			{
				return false;
			}
		}

		return true;
	}

	struct AttributeSorter
	{
		AttributeSorter(const int (&semanticIndices)[MAX_VERTEX_ATTRIBS])
			: originalIndices(semanticIndices)
		{
		}

		bool operator()(int a, int b)
		{
			return originalIndices[a] == -1 ? false : originalIndices[a] < originalIndices[b];
		}

		const int (&originalIndices)[MAX_VERTEX_ATTRIBS];
	};

	void ProgramBinary::initAttributesByLayout()
	{
		for (int i = 0; i < MAX_VERTEX_ATTRIBS; i++)
		{
			mAttributesByLayout[i] = i;
		}

		std::sort(&mAttributesByLayout[0], &mAttributesByLayout[MAX_VERTEX_ATTRIBS], AttributeSorter(mSemanticIndex));
	}

	bool ProgramBinary::linkAttributes(const AttributeBindings &attributeBindings, FragmentShader *fragmentShader, VertexShader *vertexShader)
	{
		unsigned int usedLocations = 0;

		// Link attributes that have a binding location
		for (AttributeArray::iterator attribute = vertexShader->mAttributes.begin(); attribute != vertexShader->mAttributes.end(); attribute++)
		{
			int location = attributeBindings.getAttributeBinding(attribute->name);

			if (location != -1)   // Set by glBindAttribLocation
			{
				if (!mLinkedAttribute[location].name.empty())
				{
					// Multiple active attributes bound to the same location; not an error
				}

				mLinkedAttribute[location] = *attribute;

				int rows = VariableRowCount(attribute->type);

				if (rows + location > MAX_VERTEX_ATTRIBS)
				{
					std::string msg = std::string("Active attribute ") + attribute->name +  "is too big to fit";
					throw std::runtime_error(msg.c_str());
				}

				for (int i = 0; i < rows; i++)
				{
					usedLocations |= 1 << (location + i);
				}
			}
		}

		// Link attributes that don't have a binding location
		for (AttributeArray::iterator attribute = vertexShader->mAttributes.begin(); attribute != vertexShader->mAttributes.end(); attribute++)
		{
			int location = attributeBindings.getAttributeBinding(attribute->name);

			if (location == -1)   // Not set by glBindAttribLocation
			{
				int rows = VariableRowCount(attribute->type);
				int availableIndex = AllocateFirstFreeBits(&usedLocations, rows, MAX_VERTEX_ATTRIBS);

				if (availableIndex == -1 || availableIndex + rows > MAX_VERTEX_ATTRIBS)
				{
					std::string msg = std::string("Too many active attributes (") + attribute->name +  ")";
					throw new std::runtime_error(msg.c_str());
				}

				mLinkedAttribute[availableIndex] = *attribute;
			}
		}

		for (int attributeIndex = 0; attributeIndex < MAX_VERTEX_ATTRIBS; )
		{
			int index = vertexShader->getSemanticIndex(mLinkedAttribute[attributeIndex].name);
			int rows = std::max(VariableRowCount(mLinkedAttribute[attributeIndex].type), 1);

			for (int r = 0; r < rows; r++)
			{
				mSemanticIndex[attributeIndex++] = index++;
			}
		}

		initAttributesByLayout();

		return true;
	}

	bool ProgramBinary::linkVaryings(int registers, const Varying *packing[][4],
										std::string& pixelHLSL, std::string& vertexHLSL,
										FragmentShader *fragmentShader, VertexShader *vertexShader)
	{
		if (pixelHLSL.empty() || vertexHLSL.empty())
		{
			return false;
		}

		bool usesMRT = fragmentShader->mUsesMultipleRenderTargets;
		bool usesFragColor = fragmentShader->mUsesFragColor;
		bool usesFragData = fragmentShader->mUsesFragData;
		if (usesFragColor && usesFragData)
		{
			throw new std::runtime_error("Cannot use both gl_FragColor and gl_FragData in the same fragment shader.");
		}

		// Write the HLSL input/output declarations
		const int shaderModel = D3D11_SHADER_MAJOR_VERSION;
		const int maxVaryingVectors = D3D11_VS_OUTPUT_REGISTER_COUNT;

		const int registersNeeded = registers + (fragmentShader->mUsesFragCoord ? 1 : 0) + (fragmentShader->mUsesPointCoord ? 1 : 0);

		// The output color is broadcast to all enabled draw buffers when writing to gl_FragColor 
		const bool broadcast = fragmentShader->mUsesFragColor;
		const unsigned int numRenderTargets = (broadcast || usesMRT ? D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT : 1);

		if (registersNeeded > maxVaryingVectors)
		{
			throw new std::runtime_error("No varying registers left to support gl_FragCoord/gl_PointCoord");
		}

		vertexShader->resetVaryingsRegisterAssignment();

		for (VaryingList::iterator input = fragmentShader->mVaryings.begin(); input != fragmentShader->mVaryings.end(); input++)
		{
			bool matched = false;

			for (VaryingList::iterator output = vertexShader->mVaryings.begin(); output != vertexShader->mVaryings.end(); output++)
			{
				if (output->name == input->name)
				{
					if (output->type != input->type || output->size != input->size)
					{
						std::string msg = std::string("Type of vertex varying ") + output->name + "does not match that of the fragment varying";

						throw new std::runtime_error(msg.c_str());
					}

					output->reg = input->reg;
					output->col = input->col;

					matched = true;
					break;
				}
			}

			if (!matched)
			{
				std::string msg = std::string("Fragment varying ") + input->name + "does not match any vertex varying";

				throw new std::runtime_error(msg.c_str());
			}
		}

		bool mUsesPointSize = vertexShader->mUsesPointSize;
		std::string varyingSemantic = (mUsesPointSize && shaderModel == 3) ? "COLOR" : "TEXCOORD";
		std::string targetSemantic = (shaderModel >= 4) ? "SV_Target" : "COLOR";
		std::string positionSemantic = (shaderModel >= 4) ? "SV_Position" : "POSITION";
		std::string depthSemantic = (shaderModel >= 4) ? "SV_Depth" : "DEPTH";

		// special varyings that use reserved registers
		int reservedRegisterIndex = registers;
		std::string fragCoordSemantic;
		std::string pointCoordSemantic;

		if (fragmentShader->mUsesFragCoord)
		{
			fragCoordSemantic = varyingSemantic + str(reservedRegisterIndex++);
		}

		if (fragmentShader->mUsesPointCoord)
		{
			// Shader model 3 uses a special TEXCOORD semantic for point sprite texcoords.
			// In DX11 we compute this in the GS.
			if (shaderModel == 3)
			{
				pointCoordSemantic = "TEXCOORD0";
			}
			else if (shaderModel >= 4)
			{
				pointCoordSemantic = varyingSemantic + str(reservedRegisterIndex++); 
			}
		}

		vertexHLSL += "struct VS_INPUT\n"
						"{\n";

		int semanticIndex = 0;
		for (AttributeArray::iterator attribute = vertexShader->mAttributes.begin(); attribute != vertexShader->mAttributes.end(); attribute++)
		{
			switch (attribute->type)
			{
				case GL_FLOAT:      vertexHLSL += "    float ";    break;
				case GL_FLOAT_VEC2: vertexHLSL += "    float2 ";   break;
				case GL_FLOAT_VEC3: vertexHLSL += "    float3 ";   break;
				case GL_FLOAT_VEC4: vertexHLSL += "    float4 ";   break;
				case GL_FLOAT_MAT2: vertexHLSL += "    float2x2 "; break;
				case GL_FLOAT_MAT3: vertexHLSL += "    float3x3 "; break;
				case GL_FLOAT_MAT4: vertexHLSL += "    float4x4 "; break;
				default:  throw new std::runtime_error("linkVaryings unknown attribute type");
			}

			vertexHLSL += decorateAttribute(attribute->name) + " : TEXCOORD" + str(semanticIndex) + ";\n";

			semanticIndex += VariableRowCount(attribute->type);
		}

		vertexHLSL += "};\n"
						"\n"
						"struct VS_OUTPUT\n"
						"{\n";

		if (shaderModel < 4)
		{
			vertexHLSL += "    float4 gl_Position : " + positionSemantic + ";\n";
		}

		for (int r = 0; r < registers; r++)
		{
			int registerSize = packing[r][3] ? 4 : (packing[r][2] ? 3 : (packing[r][1] ? 2 : 1));

			vertexHLSL += "    float" + str(registerSize) + " v" + str(r) + " : " + varyingSemantic + str(r) + ";\n";
		}

		if (fragmentShader->mUsesFragCoord)
		{
			vertexHLSL += "    float4 gl_FragCoord : " + fragCoordSemantic + ";\n";
		}

		if (vertexShader->mUsesPointSize && shaderModel >= 3)
		{
			vertexHLSL += "    float gl_PointSize : PSIZE;\n";
		}

		if (shaderModel >= 4)
		{
			vertexHLSL += "    float4 gl_Position : " + positionSemantic + ";\n";
		}

		vertexHLSL += "};\n"
						"\n"
						"VS_OUTPUT main(VS_INPUT input)\n"
						"{\n";

		for (AttributeArray::iterator attribute = vertexShader->mAttributes.begin(); attribute != vertexShader->mAttributes.end(); attribute++)
		{
			vertexHLSL += "    " + decorateAttribute(attribute->name) + " = ";

			if (VariableRowCount(attribute->type) > 1)   // Matrix
			{
				vertexHLSL += "transpose";
			}

			vertexHLSL += "(input." + decorateAttribute(attribute->name) + ");\n";
		}

		if (shaderModel >= 4)
		{
			vertexHLSL += "\n"
							"    gl_main();\n"
							"\n"
							"    VS_OUTPUT output;\n"
							"    output.gl_Position.x = gl_Position.x;\n";

			if (coordinateSystemType == LEFT_CARTESIAN)
			{
				vertexHLSL += "    output.gl_Position.y = gl_Position.y;\n";
			}
			else
			{
				vertexHLSL += "    output.gl_Position.y = -gl_Position.y;\n";
			}
                      
			vertexHLSL += "    output.gl_Position.z = (gl_Position.z + gl_Position.w) * 0.5;\n"
							"    output.gl_Position.w = gl_Position.w;\n";
		}
		else
		{
			vertexHLSL += "\n"
							"    gl_main();\n"
							"\n"
							"    VS_OUTPUT output;\n"
							"    output.gl_Position.x = gl_Position.x * dx_ViewAdjust.z + dx_ViewAdjust.x * gl_Position.w;\n"
							"    output.gl_Position.y = -(gl_Position.y * dx_ViewAdjust.w + dx_ViewAdjust.y * gl_Position.w);\n"
							"    output.gl_Position.z = (gl_Position.z + gl_Position.w) * 0.5;\n"
							"    output.gl_Position.w = gl_Position.w;\n";
		}

		if (vertexShader->mUsesPointSize && shaderModel >= 3)
		{
			vertexHLSL += "    output.gl_PointSize = gl_PointSize;\n";
		}

		if (fragmentShader->mUsesFragCoord)
		{
			vertexHLSL += "    output.gl_FragCoord = gl_Position;\n";
		}

		for (VaryingList::iterator varying = vertexShader->mVaryings.begin(); varying != vertexShader->mVaryings.end(); varying++)
		{
			if (varying->reg >= 0)
			{
				for (int i = 0; i < varying->size; i++)
				{
					int rows = VariableRowCount(varying->type);

					for (int j = 0; j < rows; j++)
					{
						int r = varying->reg + i * rows + j;
						vertexHLSL += "    output.v" + str(r);

						bool sharedRegister = false;   // Register used by multiple varyings
                    
						for (int x = 0; x < 4; x++)
						{
							if (packing[r][x] && packing[r][x] != packing[r][0])
							{
								sharedRegister = true;
								break;
							}
						}

						if(sharedRegister)
						{
							vertexHLSL += ".";

							for (int x = 0; x < 4; x++)
							{
								if (packing[r][x] == &*varying)
								{
									switch(x)
									{
										case 0: vertexHLSL += "x"; break;
										case 1: vertexHLSL += "y"; break;
										case 2: vertexHLSL += "z"; break;
										case 3: vertexHLSL += "w"; break;
									}
								}
							}
						}

						vertexHLSL += " = " + varying->name;
                    
						if (varying->array)
						{
							vertexHLSL += "[" + str(i) + "]";
						}

						if (rows > 1)
						{
							vertexHLSL += "[" + str(j) + "]";
						}
                    
						vertexHLSL += ";\n";
					}
				}
			}
		}

		vertexHLSL += "\n"
						"    return output;\n"
						"}\n";

		pixelHLSL += "struct PS_INPUT\n"
						"{\n";
    
		for (VaryingList::iterator varying = fragmentShader->mVaryings.begin(); varying != fragmentShader->mVaryings.end(); varying++)
		{
			if (varying->reg >= 0)
			{
				for (int i = 0; i < varying->size; i++)
				{
					int rows = VariableRowCount(varying->type);
					for (int j = 0; j < rows; j++)
					{
						std::string n = str(varying->reg + i * rows + j);
						pixelHLSL += "    float" + str(VariableColumnCount(varying->type)) + " v" + n + " : " + varyingSemantic + n + ";\n";
					}
				}
			}
			else throw new std::runtime_error("Varying is not assigned to register for fragment shader");
		}

		if (fragmentShader->mUsesFragCoord)
		{
			pixelHLSL += "    float4 gl_FragCoord : " + fragCoordSemantic + ";\n";
		}
        
		if (fragmentShader->mUsesPointCoord && shaderModel >= 3)
		{
			pixelHLSL += "    float2 gl_PointCoord : " + pointCoordSemantic + ";\n";
		}

		// Must consume the PSIZE element if the geometry shader is not active
		// We won't know if we use a GS until we draw
		if (vertexShader->mUsesPointSize && shaderModel >= 4)
		{
			pixelHLSL += "    float gl_PointSize : PSIZE;\n";
		}

		if (fragmentShader->mUsesFragCoord)
		{
			if (shaderModel >= 4)
			{
				pixelHLSL += "    float4 dx_VPos : SV_Position;\n";
			}
			else if (shaderModel >= 3)
			{
				pixelHLSL += "    float2 dx_VPos : VPOS;\n";
			}
		}

		pixelHLSL += "};\n"
						"\n"
						"struct PS_OUTPUT\n"
						"{\n";

		for (unsigned int renderTargetIndex = 0; renderTargetIndex < numRenderTargets; renderTargetIndex++)
		{
			pixelHLSL += "    float4 gl_Color" + str(renderTargetIndex) + " : " + targetSemantic + str(renderTargetIndex) + ";\n";
		}

		if (fragmentShader->mUsesFragDepth)
		{
			pixelHLSL += "    float gl_Depth : " + depthSemantic + ";\n";
		}

		pixelHLSL += "};\n"
						"\n";

		if (fragmentShader->mUsesFrontFacing)
		{
			if (shaderModel >= 4)
			{
				pixelHLSL += "PS_OUTPUT main(PS_INPUT input, bool isFrontFace : SV_IsFrontFace)\n"
								"{\n";
			}
			else
			{
				pixelHLSL += "PS_OUTPUT main(PS_INPUT input, float vFace : VFACE)\n"
								"{\n";
			}
		}
		else
		{
			pixelHLSL += "PS_OUTPUT main(PS_INPUT input)\n"
							"{\n";
		}

		if (fragmentShader->mUsesFragCoord)
		{
			pixelHLSL += "    float rhw = 1.0 / input.gl_FragCoord.w;\n";
        
			if (shaderModel >= 4)
			{
				pixelHLSL += "    gl_FragCoord.x = input.dx_VPos.x;\n"
								"    gl_FragCoord.y = input.dx_VPos.y;\n";
			}
			else if (shaderModel >= 3)
			{
				pixelHLSL += "    gl_FragCoord.x = input.dx_VPos.x + 0.5;\n"
								"    gl_FragCoord.y = input.dx_VPos.y + 0.5;\n";
			}
			else
			{
				// dx_ViewCoords contains the viewport width/2, height/2, center.x and center.y. See Renderer::setViewport()
				pixelHLSL += "    gl_FragCoord.x = (input.gl_FragCoord.x * rhw) * dx_ViewCoords.x + dx_ViewCoords.z;\n"
								"    gl_FragCoord.y = (input.gl_FragCoord.y * rhw) * dx_ViewCoords.y + dx_ViewCoords.w;\n";
			}
        
			pixelHLSL += "    gl_FragCoord.z = (input.gl_FragCoord.z * rhw) * dx_DepthFront.x + dx_DepthFront.y;\n"
							"    gl_FragCoord.w = rhw;\n";
		}

		if (fragmentShader->mUsesPointCoord && shaderModel >= 3)
		{
			pixelHLSL += "    gl_PointCoord.x = input.gl_PointCoord.x;\n";
			pixelHLSL += "    gl_PointCoord.y = 1.0 - input.gl_PointCoord.y;\n";
		}

		if (fragmentShader->mUsesFrontFacing)
		{
			if (shaderModel <= 3)
			{
				pixelHLSL += "    gl_FrontFacing = (vFace * dx_DepthFront.z >= 0.0);\n";
			}
			else
			{
				pixelHLSL += "    gl_FrontFacing = isFrontFace;\n";
			}
		}

		for (VaryingList::iterator varying = fragmentShader->mVaryings.begin(); varying != fragmentShader->mVaryings.end(); varying++)
		{
			if (varying->reg >= 0)
			{
				for (int i = 0; i < varying->size; i++)
				{
					int rows = VariableRowCount(varying->type);
					for (int j = 0; j < rows; j++)
					{
						std::string n = str(varying->reg + i * rows + j);
						pixelHLSL += "    " + varying->name;

						if (varying->array)
						{
							pixelHLSL += "[" + str(i) + "]";
						}

						if (rows > 1)
						{
							pixelHLSL += "[" + str(j) + "]";
						}

						switch (VariableColumnCount(varying->type))
						{
							case 1: pixelHLSL += " = input.v" + n + ".x;\n";   break;
							case 2: pixelHLSL += " = input.v" + n + ".xy;\n";  break;
							case 3: pixelHLSL += " = input.v" + n + ".xyz;\n"; break;
							case 4: pixelHLSL += " = input.v" + n + ";\n";     break;
							default: throw new std::runtime_error("Unknown varying type");
						}
					}
				}
			}
			else throw new std::runtime_error("Varying is not assigned to register for fragment shader");
		}

		pixelHLSL += "\n"
						"    gl_main();\n"
						"\n"
						"    PS_OUTPUT output;\n";

		for (unsigned int renderTargetIndex = 0; renderTargetIndex < numRenderTargets; renderTargetIndex++)
		{
			unsigned int sourceColorIndex = broadcast ? 0 : renderTargetIndex;

			pixelHLSL += "    output.gl_Color" + str(renderTargetIndex) + " = gl_Color[" + str(sourceColorIndex) + "];\n";
		}

		if (fragmentShader->mUsesFragDepth)
		{
			pixelHLSL += "    output.gl_Depth = gl_Depth;\n";
		}

		pixelHLSL += "\n"
						"    return output;\n"
						"}\n";

		return true;
	}

	// Packs varyings into generic varying registers, using the algorithm from [OpenGL ES Shading Language 1.00 rev. 17] appendix A section 7 page 111
	// Returns the number of used varying registers, or -1 if unsuccesful
	int ProgramBinary::packVaryings(const Varying *packing[][4], FragmentShader *fragmentShader)
	{
		const int maxVaryingVectors = D3D11_VS_OUTPUT_REGISTER_COUNT;

		fragmentShader->resetVaryingsRegisterAssignment();

		for (VaryingList::iterator varying = fragmentShader->mVaryings.begin(); varying != fragmentShader->mVaryings.end(); varying++)
		{
			int n = VariableRowCount(varying->type) * varying->size;
			int m = VariableColumnCount(varying->type);
			bool success = false;

			if (m == 2 || m == 3 || m == 4)
			{
				for (int r = 0; r <= maxVaryingVectors - n && !success; r++)
				{
					bool available = true;

					for (int y = 0; y < n && available; y++)
					{
						for (int x = 0; x < m && available; x++)
						{
							if (packing[r + y][x])
							{
								available = false;
							}
						}
					}

					if (available)
					{
						varying->reg = r;
						varying->col = 0;

						for (int y = 0; y < n; y++)
						{
							for (int x = 0; x < m; x++)
							{
								packing[r + y][x] = &*varying;
							}
						}

						success = true;
					}
				}

				if (!success && m == 2)
				{
					for (int r = maxVaryingVectors - n; r >= 0 && !success; r--)
					{
						bool available = true;

						for (int y = 0; y < n && available; y++)
						{
							for (int x = 2; x < 4 && available; x++)
							{
								if (packing[r + y][x])
								{
									available = false;
								}
							}
						}

						if (available)
						{
							varying->reg = r;
							varying->col = 2;

							for (int y = 0; y < n; y++)
							{
								for (int x = 2; x < 4; x++)
								{
									packing[r + y][x] = &*varying;
								}
							}

							success = true;
						}
					}
				}
			}
			else if (m == 1)
			{
				int space[4] = {0};

				for (int y = 0; y < maxVaryingVectors; y++)
				{
					for (int x = 0; x < 4; x++)
					{
						space[x] += packing[y][x] ? 0 : 1;
					}
				}

				int column = 0;

				for (int x = 0; x < 4; x++)
				{
					if (space[x] >= n && space[x] < space[column])
					{
						column = x;
					}
				}

				if (space[column] >= n)
				{
					for (int r = 0; r < maxVaryingVectors; r++)
					{
						if (!packing[r][column])
						{
							varying->reg = r;

							for (int y = r; y < r + n; y++)
							{
								packing[y][column] = &*varying;
							}

							break;
						}
					}

					varying->col = column;

					success = true;
				}
			}
			else return -1;

			if (!success)
			{
				std::string msg = std::string("Could not pack varying ") + varying->name;

				throw new std::runtime_error(msg.c_str());

				return -1;
			}
		}

		// Return the number of used registers
		int registers = 0;

		for (int r = 0; r < maxVaryingVectors; r++)
		{
			if (packing[r][0] || packing[r][1] || packing[r][2] || packing[r][3])
			{
				registers++;
			}
		}

		return registers;
	}

	void ProgramBinary::link(AttributeBindings mAttributeBindings, FragmentShader *fShader, VertexShader *vShader)
	{
		std::string vertexHLSL = vShader->getHLSL();
		std::string pixelHLSL = fShader->getHLSL();

		// Map the varyings to the register file
		const Varying *packing[IMPLEMENTATION_MAX_VARYING_VECTORS][4] = {NULL};
		int registers = packVaryings(packing, fShader);

		if (registers < 0)
		{
			throw new std::runtime_error("No registers available");
		}

		if (!linkVaryings(registers, packing, pixelHLSL, vertexHLSL, fShader, vShader))
		{
			throw std::runtime_error("Failed to link varyings");
		}

		if (!linkAttributes(mAttributeBindings, fShader, vShader))
		{
			throw std::runtime_error("Failed to link attributes");
		}

		if (!linkUniforms(vShader->mActiveUniforms, fShader->mActiveUniforms))
		{
			throw std::runtime_error("Failed to link uniforms");
		}

		// special case for gl_DepthRange, the only built-in uniform (also a struct)
		if (vShader->mUsesDepthRange || fShader->mUsesDepthRange)
		{
			mUniforms.push_back(new gl::Uniform(GL_FLOAT, GL_HIGH_FLOAT, "gl_DepthRange.near", 0));
			mUniforms.push_back(new gl::Uniform(GL_FLOAT, GL_HIGH_FLOAT, "gl_DepthRange.far", 0));
			mUniforms.push_back(new gl::Uniform(GL_FLOAT, GL_HIGH_FLOAT, "gl_DepthRange.diff", 0));
		}

		vShader->setHLSL(cstr(vertexHLSL));
		fShader->setHLSL(cstr(pixelHLSL));
	}

	int AttributeBindings::getAttributeBinding(const std::string &name) const
	{
		for (int location = 0; location < MAX_VERTEX_ATTRIBS; location++)
		{
			if (mAttributeBinding[location].find(name) != mAttributeBinding[location].end())
			{
				return location;
			}
		}

		return -1;
	}
}