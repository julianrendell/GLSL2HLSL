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

            const string vertexShader = @"attribute vec4 vPosition;
                                  void main()
                                  { 
                                     gl_Position = vPosition;
                                  }";

            const string fragmentShader = @"precision mediump float;
                                    void main()                                  
                                    {                                            
                                    gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );
                                    }";
            
            clrAngleTranslator.GetShader(vertexShader, fragmentShader);

            clrAngleTranslator.Dispose();
            
            //Console.WriteLine(ret);

	        Console.WriteLine(@"Press return to continue");
	        Console.ReadLine();

        }
    }
}
