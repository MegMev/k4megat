def setup_run_parser(parser):
    publicOptions = parser.add_argument_group('User options')
    publicOptions.add_argument("pathToAnalysisScript", help="path to analysis script")
    publicOptions.add_argument("--loglevel", help="Specify the RDataFrame ELogLevel", type=str, default="kUnset", choices = ['kUnset','kFatal','kError','kWarning','kInfo','kDebug'])
    publicOptions.add_argument("--files", help="Specify input file to bypass the processList", default=[], nargs='+')
    publicOptions.add_argument("--output", help="Specify output file name to bypass the processList and or outputList, default output.root", type=str, default="output.root")
    publicOptions.add_argument("--nevents", help="Specify max number of events to process", type=int, default=-1)
    publicOptions.add_argument('--bench', action='store_true', help='Output benchmark results to a JSON file')
    publicOptions.add_argument("--ncpus", help="Set number of threads", type=int)
    #publicOptions.add_argument("--final", action='store_true', help="Run final analysis (produces final histograms and trees)")
    #publicOptions.add_argument("--plots", action='store_true', help="Run analysis plots")

def setup_run_parser_final(parser):
    publicOptions = parser.add_argument_group('User final options')
    publicOptions.add_argument("pathToAnalysisScript", help="path to analysis_final script")
    publicOptions.add_argument("--loglevel", help="Specify the RDataFrame ELogLevel", type=str, default="kUnset", choices = ['kUnset','kFatal','kError','kWarning','kInfo','kDebug'])

def setup_run_parser_plots(parser):
    publicOptions = parser.add_argument_group('User plots options')
    publicOptions.add_argument("pathToAnalysisScript", help="path to analysis_plots script")
