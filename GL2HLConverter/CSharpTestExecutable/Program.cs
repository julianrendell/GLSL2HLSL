using System;
using CLRAngleTranslatorWrapper;

namespace CSharpTestExecutable
{
    class Program
    {
        static void Main(string[] args)
        {
           	Console.WriteLine(@"Hello there!");

            var clrAngleTranslator = Cl();
            var output = clrAngleTranslator.DoIt(@"String from C# excecutable"); 

            Console.WriteLine(@"Output from calling CLR wrapper library code:");
	        Console.WriteLine(output);

	        Console.WriteLine(@"Press return to continue");
	        Console.ReadLine();



        }
    }
}
