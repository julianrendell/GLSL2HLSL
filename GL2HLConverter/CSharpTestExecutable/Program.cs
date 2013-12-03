using System;
using CLRAngleTranslatorWrapper;

namespace CSharpTestExecutable
{
    class Program
    {
        static void Main(string[] args)
        {
           	Console.WriteLine(@"Hello there!");

            var clrAngleTranslator = CLRAngleTranslator.Instance;

//            const string vertexShader = @"attribute vec4 vPosition;
//                                          void main()
//                                          {
//                                             gl_Position = vPosition;
//                                          }";

//            const string fragmentShader = @"precision mediump float;
//                                           void main()
//                                           {
//                                             gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );
//                                           }";

            const string vertexShader = @"// World Space

                                        //uniform mat4 ModelViewProjection;

                                        attribute vec4 Position;
                                        attribute vec2 TextureCoordinates;

                                        varying vec2 textureCoordinateVarying;

                                        void main()
                                        {
                                            gl_Position = Position; //* ModelViewProjection;
    
                                            textureCoordinateVarying = TextureCoordinates;
                                        }";

            const string fragmentShader = @"varying lowp vec2 textureCoordinateVarying;

                                            uniform lowp float Percentage;
                                            uniform lowp float Bias;
                                            uniform lowp float Base;

                                            uniform sampler2D Texture0;

                                            void main()
                                            {
	                                            lowp vec2 inverseCoordinates = textureCoordinateVarying;

	                                            inverseCoordinates.y = 1.0 - inverseCoordinates.y;

	                                            gl_FragColor = texture2D(Texture0, inverseCoordinates);
	
	                                            lowp float alpha = (1.0 - min(pow(textureCoordinateVarying.y / Percentage, Bias), 1.0)) * Base;

	                                            gl_FragColor.w *= alpha;
	                                            gl_FragColor.rgb *= alpha;
                                            }";
            
            clrAngleTranslator.GetShader(vertexShader, fragmentShader);

            clrAngleTranslator.Dispose();
            
            //Console.WriteLine(ret);

	        Console.WriteLine(@"Press return to continue");
	        Console.ReadLine();

        }
    }
}
