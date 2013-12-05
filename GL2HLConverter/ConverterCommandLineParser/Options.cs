using CommandLine;
using CommandLine.Text;

namespace ConverterCommandLineParser
{
    public class Options
    {
        [Option('i', "input", Required = true, HelpText = "Input files for GLSL to be processed.")]
        public string InputFile { get; set; }

        [Option('o', "output", Required = true, HelpText = "Output files for HLSL after processing as finished.")]
        public string OutputFile { get; set; }

        [ParserState]
        public IParserState LastParserState { get; set; }

        [HelpOption]
        public string GetUsage()
        {
            return HelpText.AutoBuild(this, current => HelpText.DefaultParsingErrorsHandler(this, current));
        }
    }
}