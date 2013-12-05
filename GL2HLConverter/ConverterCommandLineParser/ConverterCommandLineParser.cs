using System;
using System.IO;
using CLRAngleTranslatorWrapper;

namespace ConverterCommandLineParser
{
    public class ConverterCommandLineParser
    {
        private static void Main(string[] args)
        {
            var clrAngleTranslator = CLRAngleTranslator.Instance;
            var options = new Options();

            if (!CommandLine.Parser.Default.ParseArguments(args, options))
            {
                throw new ArgumentException("Command line arguments are invalid!");
            }

            try
            {
                ManagedHlslBag hlslBag;

                if (!string.IsNullOrEmpty(options.InputFile))
                {
                    string vertexGlsl = File.ReadAllText(@options.InputFile + ".vsh");
                    string fragmentGlsl = File.ReadAllText(@options.InputFile + ".psh");

                    hlslBag = clrAngleTranslator.ConvertToHlsl(vertexGlsl, fragmentGlsl);
                }
                else
                {
                    throw new FileLoadException("Could not read from specified input file.");
                }

                if (!string.IsNullOrEmpty(options.OutputFile))
                {
                    WriteToFiles(hlslBag.vertexHLSL, hlslBag.fragmentHLSL, options.OutputFile);
                }
                else
                {
                    throw new FileLoadException("Failed to write to specified output file.");
                }
            }
            catch (Exception e)
            {
               throw new FileLoadException("Oops...Something went wrong!", e.Message);
            }
        }

        public static void WriteToFiles(string vertexHlsl, string fragmentHlsl, string outputFile)
        {
            var writeVertex = new StreamWriter(outputFile + ".vsh");
            writeVertex.Write(vertexHlsl);
            writeVertex.Close();

            var writeFragment = new StreamWriter(outputFile + ".psh");
            writeFragment.Write(fragmentHlsl);
            writeFragment.Close();
        }
    }
}
