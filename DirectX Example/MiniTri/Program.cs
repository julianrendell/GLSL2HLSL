// Copyright (c) 2010-2013 SharpDX - Alexandre Mutel
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// -----------------------------------------------------------------------------
// Original code from SlimDX project.
// Greetings to SlimDX Group. Original code published with the following license:
// -----------------------------------------------------------------------------
/*
* Copyright (c) 2007-2011 SlimDX Group
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
using System;
using System.IO;
using System.Runtime.InteropServices;
using SharpDX;
using SharpDX.D3DCompiler;
using SharpDX.Direct3D;
using SharpDX.Direct3D11;
using SharpDX.DXGI;
using SharpDX.Windows;
using Buffer = SharpDX.Direct3D11.Buffer;
using Device = SharpDX.Direct3D11.Device;
using CLRAngleTranslatorWrapper;

namespace MiniTri
{
    /// <summary>
    ///   SharpDX port of SharpDX-MiniTri Direct3D 11 Sample
    /// </summary>
    internal static class Program
    {
        [StructLayout(LayoutKind.Sequential)]
        struct ShaderUniforms
        {
            public float Base, P1, P2, P3, Bias, P4, P5, P6, Percentage, P7, P8, P9; // Padding
        }

        [STAThread]
        private static unsafe void Main()
        {
            var form = new RenderForm("SharpDX - MiniTri Direct3D 11 Sample");

            // SwapChain description
            var desc = new SwapChainDescription()
            {
                BufferCount = 1,
                ModeDescription =
                    new ModeDescription(form.ClientSize.Width, form.ClientSize.Height,
                                        new Rational(60, 1), Format.R8G8B8A8_UNorm),
                IsWindowed = true,
                OutputHandle = form.Handle,
                SampleDescription = new SampleDescription(1, 0),
                SwapEffect = SwapEffect.Discard,
                Usage = Usage.RenderTargetOutput
            };

            // Create Device and SwapChain
            Device device;
            SwapChain swapChain;
            Device.CreateWithSwapChain(DriverType.Hardware, DeviceCreationFlags.None, desc, out device, out swapChain);
            var context = device.ImmediateContext;

            // Ignore all windows events
            var factory = swapChain.GetParent<Factory>();
            factory.MakeWindowAssociation(form.Handle, WindowAssociationFlags.IgnoreAll);

            // New RenderTargetView from the backbuffer
            var backBuffer = Texture2D.FromSwapChain<Texture2D>(swapChain, 0);
            var renderView = new RenderTargetView(device, backBuffer);

            var blendDescription = new BlendStateDescription();
            blendDescription.RenderTarget[0].IsBlendEnabled = true;
            blendDescription.RenderTarget[0].SourceBlend = BlendOption.SourceAlpha;
            blendDescription.RenderTarget[0].DestinationBlend = BlendOption.InverseSourceAlpha;
            blendDescription.RenderTarget[0].BlendOperation = BlendOperation.Add;
            blendDescription.RenderTarget[0].SourceAlphaBlend = BlendOption.One;
            blendDescription.RenderTarget[0].DestinationAlphaBlend = BlendOption.Zero;
            blendDescription.RenderTarget[0].AlphaBlendOperation = BlendOperation.Add;
            blendDescription.RenderTarget[0].RenderTargetWriteMask = ColorWriteMaskFlags.All;

            var blendState = new BlendState(device, blendDescription);
            context.OutputMerger.SetBlendState(blendState, new SharpDX.Color4(1.0f), -1);


            // Read GLSL and convert to HLSL
            var clrAngleTranslator = CLRAngleTranslator.Instance;
            string vertexGlsl = File.ReadAllText(@"Reflection.vsh");
            string fragmentGlsl = File.ReadAllText(@"Reflection.psh");

            var hlslBag = clrAngleTranslator.ConvertToHlsl(vertexGlsl, fragmentGlsl);

            // Compile Vertex and Pixel shaders
            var vertexShaderByteCode = ShaderBytecode.Compile(hlslBag.vertexHLSL, "main", "vs_4_0", ShaderFlags.None, EffectFlags.None);
            var vertexShader = new VertexShader(device, vertexShaderByteCode);

            var pixelShaderByteCode = ShaderBytecode.Compile(hlslBag.fragmentHLSL, "main", "ps_4_0", ShaderFlags.None, EffectFlags.None);
            var pixelShader = new PixelShader(device, pixelShaderByteCode);

            // Layout from VertexShader input signature
            var layout = new InputLayout(
                device,
                ShaderSignature.GetInputSignature(vertexShaderByteCode),
                new[]
                    {
                        new InputElement("TEXCOORD", 0, Format.R32G32B32A32_Float, 0, 0),
                        new InputElement("TEXCOORD", 1, Format.R32G32_Float, 16, 0)
                        //new InputElement("COLOR", 0, Format.R32G32B32A32_Float, 16, 0)
                    });

            // Instantiate Vertex buiffer from vertex data
            var vertices = Buffer.Create(device, BindFlags.VertexBuffer, new[]
                                  {
                                      -1.0f, -1.0f, -1.0f, 1.0f,     0.0f, 1.0f, // Front
                                      -1.0f,  1.0f, -1.0f, 1.0f,     0.0f, 0.0f,
                                       1.0f,  1.0f, -1.0f, 1.0f,     1.0f, 0.0f,
                                      -1.0f, -1.0f, -1.0f, 1.0f,     0.0f, 1.0f,
                                       1.0f,  1.0f, -1.0f, 1.0f,     1.0f, 0.0f,
                                       1.0f, -1.0f, -1.0f, 1.0f,     1.0f, 1.0f,
                                  });


            //// Can check if uniforms exist
            //var shaderReflector = new ShaderReflection(pixelShaderByteCode);
            //var boundResources = shaderReflector.Description.BoundResources;

            //InputBindingDescription description1 = shaderReflector.GetResourceBindingDescription(0);
            //InputBindingDescription description2 = shaderReflector.GetResourceBindingDescription(1);
            //InputBindingDescription description3 = shaderReflector.GetResourceBindingDescription(2);

            // Prepare All the stages
            context.InputAssembler.InputLayout = layout;
            context.InputAssembler.PrimitiveTopology = PrimitiveTopology.TriangleList;
            context.InputAssembler.SetVertexBuffers(0, new VertexBufferBinding(vertices, 24, 0));
            context.VertexShader.Set(vertexShader);
            context.Rasterizer.SetViewport(new Viewport(0, 0, form.ClientSize.Width, form.ClientSize.Height, 0.0f, 1.0f));
            context.PixelShader.Set(pixelShader);
            context.OutputMerger.SetTargets(renderView);

            var constantsBuffer = new Buffer(device, new BufferDescription
            {
                Usage = ResourceUsage.Default,
                SizeInBytes = sizeof(ShaderUniforms),
                BindFlags = BindFlags.ConstantBuffer
            });

            var shaderUniforms = new ShaderUniforms
            {
                Base = 1.0f,
                Bias = 1.0f,
                Percentage = 1.0f
            };

            var data = new DataStream(sizeof(ShaderUniforms), true, true);
            data.Write(shaderUniforms);
            data.Position = 0;

            context.UpdateSubresource(new DataBox(data.PositionPointer, 0, 0), constantsBuffer, 0);
            context.PixelShader.SetConstantBuffer(0, constantsBuffer);



            var texture = Texture2D.FromFile<Texture2D>(device, "saddog.jpg");
            var textureView = new ShaderResourceView(device, texture);

            var sampler = new SamplerState(device, new SamplerStateDescription
            {
                Filter = Filter.Anisotropic,
                AddressU = TextureAddressMode.Wrap,
                AddressV = TextureAddressMode.Wrap,
                AddressW = TextureAddressMode.Wrap,
                BorderColor = Color.Black,
                ComparisonFunction = Comparison.Never,
                MaximumAnisotropy = 16,
                MipLodBias = 0,
                MinimumLod = 0,
                MaximumLod = 16,
            });

            context.PixelShader.SetSampler(0, sampler);
            context.PixelShader.SetShaderResource(0, textureView);

            // Main loop
            RenderLoop.Run(form, () =>
            {
                context.ClearRenderTargetView(renderView, Color.Black);
                context.Draw(6, 0);
                swapChain.Present(0, PresentFlags.None);
            });

            // Release all resources
            vertexShaderByteCode.Dispose();
            vertexShader.Dispose();
            pixelShaderByteCode.Dispose();
            pixelShader.Dispose();
            vertices.Dispose();
            layout.Dispose();
            renderView.Dispose();
            backBuffer.Dispose();
            context.ClearState();
            context.Flush();
            device.Dispose();
            context.Dispose();
            swapChain.Dispose();
            factory.Dispose();
        }
    }
}