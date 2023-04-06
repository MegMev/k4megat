import os, sys
import time
import glob
import json
import importlib.util
from array import array

import ROOT, megat
from .utility import is_absolute_path, megat_geometry_path, mgana_lib_path
from .logger import rootLogger

#__________________________________________________________
def getElement(rdfModule, element, isFinal=False):
    try:
        return getattr(rdfModule, element)
    except AttributeError:

        #return default values or crash if mandatory
        if element=='processList':
            print('The variable <{}> is mandatory in your analysis.py file, will exit'.format(element))
            sys.exit(3)

        elif element=='analysers':
            print('The function <{}> is mandatory in your analysis.py file, will exit'.format(element))
            if isFinal: print('The function <{}> is not part of final analysis'.format(element))
            sys.exit(3)

        elif element=='output':
            print('The function <{}> is mandatory in your analysis.py file, will exit'.format(element))
            if isFinal: print('The function <{}> is not part of final analysis'.format(element))
            sys.exit(3)

        elif element=='analysisName':
            print('The variable <analysisName> is optional in your analysis.py file, return default value ""')
            return ""

        elif element=='nCPUS':
            print('The variable <{}> is optional in your analysis.py file, return default value 4'.format(element))
            return 4

        elif element=='outputDir':
            print('The variable <{}> is optional in your analysis.py file, return default value running dir'.format(element))
            return ""

        elif element=='geometryFile':
            print('The variable <{}> is optional in your analysis.py file, return default value empty string'.format(element))
            if isFinal: print('The option <{}> is not available in final analysis'.format(element))
            return ""

        elif element=='readoutName':
            print('The variable <{}> is optional in your analysis.py file, return default value empty string'.format(element))
            if isFinal: print('The option <{}> is not available in final analysis'.format(element))
            return ""

        elif element=='procDict':
            if isFinal:
                print('The variable <{}> is mandatory in your analysis_final.py file, exit'.format(element))
                sys.exit(3)
            else: print('The option <{}> is not available in presel analysis'.format(element))

        elif element=='cutList':
            if isFinal:
                print('The variable <{}> is optional in your analysis_final.py file, return empty dictonary'.format(element))
                return {}
            else: print('The option <{}> is not available in presel analysis'.format(element))

        elif element=='defineList':
            if isFinal:
                print('The variable <{}> is optional in your analysis_final.py file, return empty dictonary'.format(element))
                return {}
            else: print('The option <{}> is not available in presel analysis'.format(element))

        elif element=='histoList':
            if isFinal:
                print('The variable <{}> is mandatory in your analysis_final.py file, exit'.format(element))
                sys.exit(3)
            else: print('The option <{}> is not available in presel analysis'.format(element))

        elif element=='doTree':
            if isFinal:
                print('The variable <{}> is optional in your analysis_final.py file return default value False'.format(element))
                return False
            else: print('The option <{}> is not available in presel analysis'.format(element))

        elif element=='procDictAdd':
            if isFinal:
                print('The variable <{}> is optional in your analysis_final.py file return empty dictionary'.format(element))
                return {}
            else: print('The option <{}> is not available in presel analysis'.format(element))

        elif element=='doScale':
            if isFinal:
                print('The variable <{}> is optional in your analysis_final.py file return empty dictionary'.format(element))
                return {}
            else: print('The option <{}> is not available in presel analysis'.format(element))

        elif element=='intLumi':
            if isFinal:
                print('The variable <{}> is optional in your analysis_final.py file, return empty dictionary. However, if you set doScale, then it is mandatory!'.format(element))
                return {}
            else: print('The option <{}> is not available in presel analysis'.format(element))

        elif element=='saveTabular':
            if isFinal:
                print('The variable <{}> is optional in your analysis_final.py file return empty dictionary'.format(element))
                return {}
            else: print('The option <{}> is not available in presel analysis'.format(element))

        elif element=='cutLabels':
            if isFinal:
                print('The variable <{}> is optional in your analysis_final.py file return empty dictionary'.format(element))
                return {}
            else: print('The option <{}> is not available in presel analysis'.format(element))

        return None

#__________________________________________________________
def saveBenchmark(outfile, benchmark):
    benchmarks = []
    try:
        with open(outfile, 'r') as benchin:
            benchmarks = json.load(benchin)
    except OSError:
        pass

    benchmarks = [b for b in benchmarks if b['name'] != benchmark['name']]
    benchmarks.append(benchmark)

    with open(outfile, 'w') as benchout:
        json.dump(benchmarks, benchout, indent=2)

#__________________________________________________________
def runRDF(rdfModule, inputlist, outFile, nevt, args):
    # for convenience and compatibility with user code
    ROOT.gInterpreter.Declare("using namespace megat;")

    # geometry management
    geometryFile = getElement(rdfModule, "geometryFile")
    geomFileList = ROOT.vector('string')()
    if geometryFile:
        for geofile in geometryFile:
            realfile = geofile if is_absolute_path(geofile) else os.path.join(megat_geometry_path(), geofile)
            geomFileList.push_back(realfile)
        # the default geometry
        megat.loadGeometry(geomFileList)

    readoutName  = getElement(rdfModule, "readoutName")
    readoutList = ROOT.vector('string')()
    if readoutName:
        for ro in readoutName:
            megat.loadGeometry(geomFileList, ro, ro, "TPC")
            readoutList.push_back(ro)

    # MT config
    ncpus = 1
    # cannot use MT with Range()
    if args.nevents < 0:
        if isinstance(args.ncpus, int) and args.ncpus >= 1:
            ncpus = args.ncpus
        else:
            ncpus = getElement(rdfModule, "nCPUS")

        ROOT.ROOT.EnableImplicitMT(ncpus)

    ROOT.EnableThreadSafety()
    df = ROOT.RDataFrame("events", inputlist)

    # limit number of events processed
    if args.nevents > 0:
      df = df.Range(0, args.nevents)

    print("----> Init done, about to run {} events on {} CPUs".format(nevt, ncpus))

    df1 = getElement(rdfModule.RDFanalysis, "analysers")(df)

    branchList = getElement(rdfModule.RDFanalysis, "output")()
    branchListVec = ROOT.vector('string')()
    for branchName in branchList:
        branchListVec.push_back(branchName)

    df1.Snapshot("events", outFile, branchListVec)

#__________________________________________________________
def runLocal(rdfModule, fileList, args):
    #Create list of files to be Processed
    print ("----> Create dataframe object from files: ", )
    fileListRoot = ROOT.vector('string')()
    nevents_meta = 0
    nevents_local = 0
    for fileName in fileList:
        fileListRoot.push_back(fileName)
        print ("   ",fileName)
        tf=ROOT.TFile.Open(str(fileName),"READ")
        tf.cd()
        for key in tf.GetListOfKeys():
            if 'eventsProcessed' == key.GetName():
                nevents_meta += tf.eventsProcessed.GetVal()
                break
        tt=tf.Get("events")
        nevents_local+=tt.GetEntries()

    # adjust number of events in case --nevents was specified
    if args.nevents > 0:
      nevents_local = args.nevents
    print ("----> nevents original={}  local={}".format(nevents_meta,nevents_local))
    outFile = getElement(rdfModule,"outputDir")
    if outFile!="" and outFile[-1]!="/": outFile+="/"
    outFile+=args.output

    start_time = time.time()
    #run RDF
    runRDF(rdfModule, fileListRoot, outFile, nevents_local, args)

    outf = ROOT.TFile( outFile, "update" )
    outt = outf.Get("events")
    outn = outt.GetEntries()
    n = array( "i", [ 0 ] )
    n[0]=nevents_local
    if nevents_meta>nevents_local:n[0]=nevents_meta
    p = ROOT.TParameter(int)( "eventsProcessed", n[0])
    p.Write()
    outf.Write()
    outf.Close()

    elapsed_time = time.time() - start_time
    print  ("==============================SUMMARY==============================")
    print  ("Elapsed time (H:M:S)     :  ",time.strftime("%H:%M:%S", time.gmtime(elapsed_time)))
    print  ("Events Processed/Second  :  ",int(nevents_local/elapsed_time))
    print  ("Total Events Processed   :  ",int(nevents_local))
    if (nevents_local>0): print  ("Reduction factor local   :  ",outn/nevents_local)
    if (nevents_meta>0):  print  ("Reduction factor total   :  ",outn/nevents_meta)
    print  ("===================================================================")
    print  (" ")
    print  (" ")

    if args.bench:
        import json

        analysis_name = getElement(rdfModule, 'analysisName')
        if not analysis_name:
            analysis_name = args.pathToAnalysisScript

        bench_time = {}
        bench_time['name'] = 'Time spent running the analysis: '
        bench_time['name'] += analysis_name
        bench_time['unit'] = 'Seconds'
        bench_time['value'] = elapsed_time
        bench_time['range'] = 10
        bench_time['extra'] = 'Analysis path: ' + args.pathToAnalysisScript
        saveBenchmark('benchmarks_smaller_better.json', bench_time)

        bench_evt_per_sec = {}
        bench_evt_per_sec['name'] = 'Events processed per second: '
        bench_evt_per_sec['name'] += analysis_name
        bench_evt_per_sec['unit'] = 'Evt/s'
        bench_evt_per_sec['value'] = nevents_local / elapsed_time
        bench_time['range'] = 1000
        bench_time['extra'] = 'Analysis path: ' + args.pathToAnalysisScript
        saveBenchmark('benchmarks_bigger_better.json', bench_evt_per_sec)


#__________________________________________________________
def runStages(args, rdfModule, analysisFile):
    libPath = mgana_lib_path()
    analysesList = getElement(rdfModule, "analysesList")
    if analysesList and len(analysesList) > 0:
        _ana = []
        for analysis in analysesList:
            print(f'----> Load cxx analyzers from {analysis}...')
            if analysis.startswith('libMegatAnalyzer_'):
                ROOT.gSystem.Load(os.path.join(libPath, analysis))
            else:
                ROOT.gSystem.Load(os.path.join(libPath, f'libMegatAnalyzer_{analysis}'))
            if not hasattr(ROOT, analysis):
                print(f'----> ERROR: analysis "{analysis}" not properly loaded. Exit')
                sys.exit(4)
            _ana.append(getattr(ROOT, analysis).dictionary)

    #check if outputDir exist and if not create it
    outputDir = getElement(rdfModule,"outputDir")
    if not os.path.exists(outputDir) and outputDir!='':
        os.system("mkdir -p {}".format(outputDir))

    #check if files are specified, and if so run the analysis on it/them (this will exit after)
    if len(args.files_list)>0:
        print("----> Running with user defined list of files (either locally or from batch)")
        path, filename = os.path.split(args.output)
        if path!='': os.system("mkdir -p {}".format(path))
        runLocal(rdfModule, args.files_list, args)
        sys.exit(0)

#__________________________________________________________
def run(mainparser):
    """
    Set things in motion.
    """
    args, _ = mainparser.parse_known_args()
    analysisFile = args.pathToAnalysisScript
    if not os.path.isfile(analysisFile):
        print("Script ", analysisFile, " does not exist")
        print("specify a valid analysis script in the command line arguments")
        sys.exit(3)

    print ("----> Info: Loading analyzers from libMegatAnalysis... ",)
    ROOT.gSystem.Load("libMegatAnalysis")
    ROOT.gErrorIgnoreLevel = ROOT.kFatal
    # force auto-loading [todo: ugly as a hack]
    ROOT.LoadMegat()

    #set the RDF ELogLevel
    try:
        verbosity = ROOT.Experimental.RLogScopedVerbosity(ROOT.Detail.RDF.RDFLogChannel(), getattr(ROOT.Experimental.ELogLevel,args.eloglevel))
    except AttributeError:
        pass

    #load the analysis
    analysisFile = os.path.abspath(analysisFile)
    print('----> Info: Loading analysis file:')
    print('      ' + analysisFile)

    rdfSpec   = importlib.util.spec_from_file_location("rdfanalysis", analysisFile)
    rdfModule = importlib.util.module_from_spec(rdfSpec)
    rdfSpec.loader.exec_module(rdfModule)

    if hasattr(args, 'command'):
        if args.command == "run":
            try:
                runStages(args, rdfModule, analysisFile)
            except Exception as excp:
                print('----> Error: During the execution of the stage file:')
                print('      ' + analysisFile)
                print('      exception occurred:')
                print(excp)
        elif args.command == "final":
            try:
                runFinal(rdfModule)
            except Exception as excp:
                print('----> Error: During the execution of the final stage file:')
                print('      ' + analysisFile)
                print('      exception occurred:')
                print(excp)
        elif args.command == "plots":
            try:
                runPlots(analysisFile)
            except Exception as excp:
                print('----> Error: During the execution of the plots file:')
                print('      ' + analysisFile)
                print('      exception occurred:')
                print(excp)
        return
