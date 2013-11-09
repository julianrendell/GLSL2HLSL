using System;
using AngleCLR;

namespace SharpWrapper
{
    class Program
    {
        static void Main(string[] args)
        {
            var instance = new AngleCLRTest();

            Console.Write(instance.Hello());
            Console.ReadKey();
        }
    }
}
