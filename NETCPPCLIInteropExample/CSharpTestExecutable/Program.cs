using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


using CPPCLRNativeWrapperLibrary; 

namespace CSharpTestExecutable
{
    class Program
    {
        static void Main(string[] args)
        {
           	Console.WriteLine(@"Hello there!");

            var cppClrWrapper = new CPPCLRWrapper();
            var output = cppClrWrapper.DoIt(@"String from C# excecutable"); 

            Console.WriteLine(@"Output from calling CLR wrapper library code:");
	        Console.WriteLine(output);

	        Console.WriteLine(@"Press return to continue");
	        Console.ReadLine();



        }
    }
}
