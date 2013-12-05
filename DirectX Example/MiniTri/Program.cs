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
using System.Runtime.InteropServices;
using SharpDX;
using SharpDX.D3DCompiler;
using SharpDX.Direct3D;
using SharpDX.Direct3D11;
using SharpDX.DXGI;
using SharpDX.Windows;
using Buffer = SharpDX.Direct3D11.Buffer;
using Device = SharpDX.Direct3D11.Device;

namespace MiniTri
{
    /// <summary>
    ///   SharpDX port of SharpDX-MiniTri Direct3D 11 Sample
    /// </summary>
    internal static class Program
    {
        [StructLayout(LayoutKind.Sequential)]
        struct ColorUniforms
        {
            public float R, G, B, A;
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

            // Compile Vertex and Pixel shaders
            var vertexShaderByteCode = ShaderBytecode.CompileFromFile("HelloTriangleVertex.fx", "main", "vs_4_0", ShaderFlags.None, EffectFlags.None);
            var vertexShader = new VertexShader(device, vertexShaderByteCode);

            var pixelShaderByteCode = ShaderBytecode.CompileFromFile("HelloTriangleFragment.fx", "main", "ps_4_0", ShaderFlags.None, EffectFlags.None);
            var pixelShader = new PixelShader(device, pixelShaderByteCode);

            // Layout from VertexShader input signature
            var layout = new InputLayout(
                device,
                ShaderSignature.GetInputSignature(vertexShaderByteCode),
                new[]
                    {
                        new InputElement("TEXCOORD", 0, Format.R32G32B32A32_Float, 0, 0),
                        //new InputElement("COLOR", 0, Format.R32G32B32A32_Float, 16, 0)
                    });

            // Instantiate Vertex buiffer from vertex data
            var vertices = Buffer.Create(device, BindFlags.VertexBuffer, new[]
                                  {
                                      -1.0f, -1.0f, -1.0f, 1.0f,//     0.0f, 1.0f, // Front
                                      -1.0f,  1.0f, -1.0f, 1.0f,//     0.0f, 0.0f,
                                       1.0f,  1.0f, -1.0f, 1.0f,//     1.0f, 0.0f,
                                      -1.0f, -1.0f, -1.0f, 1.0f,//     0.0f, 1.0f,
                                       1.0f,  1.0f, -1.0f, 1.0f,//     1.0f, 0.0f,
                                       1.0f, -1.0f, -1.0f, 1.0f,//     1.0f, 1.0f,
                                  });

            // Prepare All the stages
            context.InputAssembler.InputLayout = layout;
            context.InputAssembler.PrimitiveTopology = PrimitiveTopology.TriangleList;
            context.InputAssembler.SetVertexBuffers(0, new VertexBufferBinding(vertices, 16, 0));
            context.VertexShader.Set(vertexShader);
            context.Rasterizer.SetViewport(new Viewport(0, 0, form.ClientSize.Width, form.ClientSize.Height, 0.0f, 1.0f));
            context.PixelShader.Set(pixelShader);
            context.OutputMerger.SetTargets(renderView);

            var getUniform = new Buffer(device, new BufferDescription
            {
                Usage = ResourceUsage.Default,
                SizeInBytes = sizeof(ColorUniforms),
                BindFlags = BindFlags.ConstantBuffer
            });

            var cb = new ColorUniforms
                {
                    R = 0.79f,
                    G = 0.5f,
                    B = 1.0f,
                    A = 0.1f
                };

            var data = new DataStream(sizeof(ColorUniforms), true, true);
            data.Write(cb);
            data.Position = 0;

            context.UpdateSubresource(new DataBox(data.PositionPointer, 0, 0), getUniform, 0);
            context.PixelShader.SetConstantBuffer(0, getUniform);

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