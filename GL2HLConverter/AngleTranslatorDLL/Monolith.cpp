#include "Monolith.h"

std::string str(int i)
{
	int size = _scprintf("%d", i);
	char *buffer = new char[size+1];

	sprintf(buffer, "%d", i);
	buffer[size+1] = '\0';

	return buffer;
}

// This method needs to match OutputHLSL::decorate
std::string decorateAttribute(const std::string &name)
{
    if (name.compare(0, 3, "gl_") != 0 && name.compare(0, 3, "dx_") != 0)
    {
        return "_" + name;
    }
    
    return name;
}

void linkUniforms()
{

}

void linkAttributes()
{

}

bool linkVaryings(int registers, const Varying *packing[][4],
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
                      "    output.gl_Position.x = gl_Position.x;\n"
                      "    output.gl_Position.y = -gl_Position.y;\n"
                      "    output.gl_Position.z = (gl_Position.z + gl_Position.w) * 0.5;\n"
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

void Shader::resetVaryingsRegisterAssignment()
{
    for (VaryingList::iterator var = mVaryings.begin(); var != mVaryings.end(); var++)
    {
        var->reg = -1;
        var->col = -1;
    }
}

int VariableColumnCount(GLenum type)
{
    switch (type)
    {
      case GL_NONE:
        return 0;
      case GL_BOOL:
      case GL_FLOAT:
      case GL_INT:
      case GL_SAMPLER_2D:
      case GL_SAMPLER_CUBE:
        return 1;
      case GL_BOOL_VEC2:
      case GL_FLOAT_VEC2:
      case GL_INT_VEC2:
      case GL_FLOAT_MAT2:
        return 2;
      case GL_INT_VEC3:
      case GL_FLOAT_VEC3:
      case GL_BOOL_VEC3:
      case GL_FLOAT_MAT3:
        return 3;
      case GL_BOOL_VEC4:
      case GL_FLOAT_VEC4:
      case GL_INT_VEC4:
      case GL_FLOAT_MAT4:
        return 4;
      default:
        return 0;
    }

    return 0;
}

int VariableRowCount(GLenum type)
{
    switch (type)
    {
      case GL_NONE:
        return 0;
      case GL_BOOL:
      case GL_FLOAT:
      case GL_INT:
      case GL_BOOL_VEC2:
      case GL_FLOAT_VEC2:
      case GL_INT_VEC2:
      case GL_INT_VEC3:
      case GL_FLOAT_VEC3:
      case GL_BOOL_VEC3:
      case GL_BOOL_VEC4:
      case GL_FLOAT_VEC4:
      case GL_INT_VEC4:
      case GL_SAMPLER_2D:
      case GL_SAMPLER_CUBE:
        return 1;
      case GL_FLOAT_MAT2:
        return 2;
      case GL_FLOAT_MAT3:
        return 3;
      case GL_FLOAT_MAT4:
        return 4;
      default:
        return 0;
    }

    return 0;
}

// Packs varyings into generic varying registers, using the algorithm from [OpenGL ES Shading Language 1.00 rev. 17] appendix A section 7 page 111
// Returns the number of used varying registers, or -1 if unsuccesful
int packVaryings(const Varying *packing[][4], FragmentShader *fragmentShader)
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

bool link()
{
	std::string pixelHLSL = fShader->mHlsl;
	std::string vertexHLSL = vShader->mHlsl;

	// Map the varyings to the register file
    const Varying *packing[IMPLEMENTATION_MAX_VARYING_VECTORS][4] = {NULL};
	int registers = packVaryings(packing, fShader);

	if (registers < 0)
    {
        return false;
    }

	if (!linkVaryings(registers, packing, pixelHLSL, vertexHLSL, fShader, vShader))
    {
        return false;
    }

    bool success = true;

	return false;
}

void bindAttributeLocation(GLuint index, const GLchar* name)
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
            attributeBinding[i].erase(name);
        }

        attributeBinding[index].insert(name);
    }
}

// This is taken from Shader.cpp
// true if varying x has a higher priority in packing than y
bool compareVarying(const Varying &x, const Varying &y)
{
    if(x.type == y.type)
    {
        return x.size > y.size;
    }

    switch (x.type)
    {
      case GL_FLOAT_MAT4: return true;
      case GL_FLOAT_MAT2:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return true;
          case GL_FLOAT_VEC4: return true;
          case GL_FLOAT_MAT3: return true;
          case GL_FLOAT_VEC3: return true;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT_VEC4:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return false;
          case GL_FLOAT_VEC4: return true;
          case GL_FLOAT_MAT3: return true;
          case GL_FLOAT_VEC3: return true;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT_MAT3:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return false;
          case GL_FLOAT_VEC4: return false;
          case GL_FLOAT_MAT3: return true;
          case GL_FLOAT_VEC3: return true;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT_VEC3:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return false;
          case GL_FLOAT_VEC4: return false;
          case GL_FLOAT_MAT3: return false;
          case GL_FLOAT_VEC3: return true;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT_VEC2:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return false;
          case GL_FLOAT_VEC4: return false;
          case GL_FLOAT_MAT3: return false;
          case GL_FLOAT_VEC3: return false;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT: return false;
      default: return false;
    }

    return false;
}

bool IsCompiled(char * hlsl)
{
	return hlsl != NULL;
}

GLenum parseType(const std::string &type)
{
    if (type == "float")
    {
        return GL_FLOAT;
    }
    else if (type == "float2")
    {
        return GL_FLOAT_VEC2;
    }
    else if (type == "float3")
    {
        return GL_FLOAT_VEC3;
    }
    else if (type == "float4")
    {
        return GL_FLOAT_VEC4;
    }
    else if (type == "float2x2")
    {
        return GL_FLOAT_MAT2;
    }
    else if (type == "float3x3")
    {
        return GL_FLOAT_MAT3;
    }
    else if (type == "float4x4")
    {
        return GL_FLOAT_MAT4;
    }
    else
	{
		return GL_NONE;
	}
}

void VertexShader::parseAttributes()
{
	if (mHlsl)
    {
		const char *input = strstr(mHlsl, "// Attributes") + 14;

        while(true)
        {
            char attributeType[256];
            char attributeName[256];

            int matches = sscanf(input, "static %255s _%255s", attributeType, attributeName);

            if (matches != 2)
            {
                break;
            }

			mAttributes.push_back(Attribute(parseType(attributeType), attributeName));

            input = strstr(input, ";") + 2;
        }
    }
}

void Shader::parseVaryings()
{
	if (mHlsl)
    {
		const char *input = strstr(mHlsl, "// Varyings") + 12;

        while(true)
        {
            char varyingType[256];
            char varyingName[256];

            int matches = sscanf(input, "static %255s %255s", varyingType, varyingName);

            if (matches != 2)
            {
                break;
            }

            char *array = strstr(varyingName, "[");
            int size = 1;

            if (array)
            {
                size = atoi(array + 1);
                *array = '\0';
            }

			mVaryings.push_back(Varying(parseType(varyingType), varyingName, size, array != NULL));

            input = strstr(input, ";") + 2;
        }

		mUsesMultipleRenderTargets = strstr(mHlsl, "GL_USES_MRT") != NULL;
        mUsesFragColor = strstr(mHlsl, "GL_USES_FRAG_COLOR") != NULL;
        mUsesFragData = strstr(mHlsl, "GL_USES_FRAG_DATA") != NULL;
        mUsesFragCoord = strstr(mHlsl, "GL_USES_FRAG_COORD") != NULL;
        mUsesFrontFacing = strstr(mHlsl, "GL_USES_FRONT_FACING") != NULL;
        mUsesPointSize = strstr(mHlsl, "GL_USES_POINT_SIZE") != NULL;
        mUsesPointCoord = strstr(mHlsl, "GL_USES_POINT_COORD") != NULL;
        mUsesDepthRange = strstr(mHlsl, "GL_USES_DEPTH_RANGE") != NULL;
        mUsesFragDepth = strstr(mHlsl, "GL_USES_FRAG_DEPTH") != NULL;
        mUsesDiscardRewriting = strstr(mHlsl, "ANGLE_USES_DISCARD_REWRITING") != NULL;
    }
}

void compileToHLSL(Shader *shader)
{
	int result = ShCompile(shader->compiler, &shader->mSource, 1, SH_OBJECT_CODE);

	if (result)
    {
        size_t objCodeLen = 0;
		ShGetInfo(shader->compiler, SH_OBJECT_CODE_LENGTH, &objCodeLen);
		shader->mHlsl = new char[objCodeLen];
		ShGetObjectCode(shader->compiler, shader->mHlsl);

        void *activeUniforms;
		ShGetInfoPointer(shader->compiler, SH_ACTIVE_UNIFORMS_ARRAY, &activeUniforms);
        activeUniforms = (ActiveUniforms*)activeUniforms;
    }
    else
    {
        size_t infoLogLen = 0;
		ShGetInfo(shader->compiler, SH_INFO_LOG_LENGTH, &infoLogLen);
		shader->mInfoLog = new char[infoLogLen];
		ShGetInfoLog(shader->compiler, shader->mInfoLog);

		throw new std::runtime_error(shader->mInfoLog);
    }
}

void compileVertexShader(VertexShader *vShader)
{
	compileToHLSL(vShader);

	if (!IsCompiled(vShader->mHlsl))
	{
		throw new std::runtime_error("Vertex shader compilation has failed! (HLSL string is empty)");
	}

	vShader->parseAttributes();
	vShader->parseVaryings();
}

void compileFragmentShader(FragmentShader *fShader)
{
	compileToHLSL(fShader);

	if (!IsCompiled(fShader->mHlsl))
	{
		throw new std::runtime_error("Fragment shader compilation has failed! (HLSL string is empty)");
	}

	fShader->parseVaryings();
	fShader->mVaryings.sort(compareVarying);
}

void constructCompiler(ShBuiltInResources resources)
{
	fShader->compiler = ShConstructCompiler(SH_FRAGMENT_SHADER, SH_GLES2_SPEC, SH_HLSL11_OUTPUT, &resources);
	if (NULL == fShader->compiler)
	{
		throw new std::runtime_error("Fragment compiler construction failed!");
	}

	vShader->compiler = ShConstructCompiler(SH_VERTEX_SHADER, SH_GLES2_SPEC, SH_HLSL11_OUTPUT, &resources);
	if (NULL == vShader->compiler)
	{
		throw new std::runtime_error("Vertex compiler construction failed!");
	}
}

ShBuiltInResources initBuiltInResources()
{
	ShBuiltInResources resources;

	ShInitBuiltInResources(&resources);

	// These are values specific to DX11, from Renderer11.cpp
    resources.MaxVertexAttribs = 16;
    resources.MaxVertexUniformVectors = 1024;
    resources.MaxVaryingVectors = 32;
    resources.MaxVertexTextureImageUnits = 16;
    resources.MaxCombinedTextureImageUnits = 16 + 16;
    resources.MaxTextureImageUnits = 16;
    resources.MaxFragmentUniformVectors = 1024;
    resources.MaxDrawBuffers = 8;

	resources.OES_standard_derivatives = true;
	resources.EXT_draw_buffers = 8 > 1;
	resources.FragmentPrecisionHigh = 1;
	resources.EXT_frag_depth = 1;

	return resources;
}

void humongoid(const char * vertexShaderSrc, const char * fragmentShaderSrc)
{
	vShader = new VertexShader();
	vShader->mSource = (char*) vertexShaderSrc;

	fShader = new FragmentShader();
	fShader->mSource = (char*) fragmentShaderSrc;

	ShBuiltInResources resources = initBuiltInResources();
	constructCompiler(resources);

	compileVertexShader(vShader);
	compileFragmentShader(fShader);
	
	bindAttributeLocation(0, "vPosition");

	link();
};




