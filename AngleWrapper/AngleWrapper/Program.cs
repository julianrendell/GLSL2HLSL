using System;
using System.Runtime.InteropServices;
using System.Text;
using AngleCLR;

namespace AngleWrapper
{
    public class Program
    {
        static void Main(string[] args)
        {
            var instance = new AngleCLRTest();

            Console.Write(instance.Draw());
            Console.ReadKey();

            //SetDllDirectory(@"C:\git\GLSL2HLSL\TestCPlusPlus\Debug");

            //Console.Write(TaDa());
            //Console.ReadKey();
        }

        //[DllImport("kernel32.dll", SetLastError = true)]
        //public static extern bool SetDllDirectory(string path);

        //[DllImport("TestCPlusPlus.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        //[return: MarshalAs(UnmanagedType.LPWStr)]
        //public static extern string TaDa();
    }
}
