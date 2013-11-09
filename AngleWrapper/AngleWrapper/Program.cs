using System;
using AngleCLR;

namespace AngleWrapper
{
    public class Program
    {
        static void Main(string[] args)
        {
            var instance = new AngleCLRTest();

            Console.Write(instance.Hello());
            Console.ReadKey();
        }
    }
}
