import os, sys
import glob, time, json
import importlib.util
from array import array

import ROOT, megat
from .utility import is_absolute_path, expand_absolute_directory
from .utility import megat_geometry_path, mgana_lib_path, mgana_workspace_path, get_io_directory
from .logger import rootLogger

#__________________________________________________________
def getElement(rdfModule, element, isFinal=False):
    try:
        return getattr(rdfModule, element)
    except AttributeError:

        #return default values or crash if mandatory
        if element=='processList':
            print(f'Err: Variable <{element}> is mandatory in your analysis script, will exit')
            sys.exit(3)

        elif element=='analysers':
            print(f'Err: Function <{element}> is mandatory in your analysis script, will exit')
            if isFinal: print('The function <{}> is not part of final analysis'.format(element))
            sys.exit(3)

        elif element=='analysisName':
            rootLogger.warning(f'Warn: <{element}> is not specified in your analysis script, will use script file name')
            return ""

        elif element=='nCPUS':
            print(f'Warn: Variable <{element}> not specified in your analysis script, will use 4')
            return 4

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

        elif element=='saveCutTree':
            if isFinal:
                print('The variable <{}> is optional in your analysis_final.py file return default value False'.format(element))
                return False
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
def getElementDict(d, element):
    try:
        value=d[element]
        return value
    except KeyError:
        rootLogger.debug(f'{element} not exist, using default value')
        return None

#__________________________________________________________
def getProcessInfoFiles(process, inputDir):
    '''
    Get the list of files and nevents.
    If inputDir/process.root exist, return this file;
    otherwise, return all root files under inputDir/process/.
    '''
    if not inputDir:
        inputDir = mgana_workspace_path()
    else:
        inputDir = expand_absolute_directory(inputDir)

    filelist=[]
    eventlist=[]
    filetest='{}/{}.root'.format(inputDir, process)
    dirtest='{}/{}'.format(inputDir, process)

    if os.path.isfile(filetest) and os.path.isdir(dirtest):
        print ("----> For process {} both a file {} and a directory {} exist".format(process,filetest,dirtest))
        print ("----> Exactly one should be used, please check. Exit")
        sys.exit(3)

    if not os.path.isfile(filetest) and not os.path.isdir(dirtest):
        print ("----> For process {} neither a file {} nor a directory {} exist".format(process,filetest,dirtest))
        print ("----> Exactly one should be used, please check. Exit")
        sys.exit(3)

    if os.path.isfile(filetest):
        filelist.append(filetest)
        eventlist.append(getEntries(filetest))

    if os.path.isdir(dirtest):
        flist=glob.glob(dirtest+"/*.root")
        for f in flist:
            filelist.append(f)
            eventlist.append(getEntries(f))

    return filelist, eventlist

#__________________________________________________________
def getEntries(f):
    '''
    Return nentries in a ROOT file.
    '''
    tf=ROOT.TFile.Open(f,"READ")
    tf.cd()
    tt=tf.Get("events")
    nevents=tt.GetEntries()
    tf.Close()
    return nevents

#__________________________________________________________
def getsubfileList(fileList, eventList, fraction):
    '''
    Return the min list of files with sum nevents > fraction*total_nevents
    '''
    nevts=sum(eventList)
    nevts_target=nevts*fraction
    nevts_real=0
    tmplist=[]
    for ev in range(len(eventList)):
        if nevts_real>=nevts_target:break
        nevts_real+=eventList[ev]
        tmplist.append(fileList[ev])
    return tmplist

#__________________________________________________________
def getchunkList(fileList, chunks):
    '''
    Divide the fileList into 'chunks' list on average and return the chunked list
    '''
    chunklist=[]
    if chunks>len(fileList):chunks=len(fileList)
    nfilesperchunk=int(len(fileList)/chunks)
    for ch in range(chunks):
        filecount=0
        listtmp=[]
        for fileName in fileList:
            if (filecount>=ch*nfilesperchunk and filecount<(ch+1)*nfilesperchunk) or (filecount>=ch*nfilesperchunk and ch==chunks-1):
                listtmp.append(fileName)
            filecount+=1
        chunklist.append(listtmp)

    return chunklist

#__________________________________________________________
def saveBenchmark(outfile, benchmark):
    '''
    Save the benchmark into a json file
    '''
    import json

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

    # run RDF
    print("----> Init done:")
    print("      about to run {} events on {} CPUs".format(nevt, ncpus))
    df1 = getElement(rdfModule.RDFanalysis, "analysers")(df)

    # save snapshot
    branchList = getElement(rdfModule.RDFanalysis, "output")()
    branchListVec = ROOT.vector('string')()
    for branchName in branchList:
        branchListVec.push_back(branchName)

    df1.Snapshot("events", outFile, branchListVec)

#__________________________________________________________
def runLocal(rdfModule, fileList, outputDir, args):
    '''
    Process a list of ROOT files.
    '''
    # meta: the initial nevents in the analysis chain
    nevents_meta = 0
    # local: the nevents processed in the current analysis
    nevents_local = 0

    # create list of files to be Processed and aggregate total nevents
    print ("----> Create dataframe object from files: ", )
    fileListRoot = ROOT.vector('string')()
    for fileName in fileList:
        fileListRoot.push_back(fileName)
        print ("     ",fileName)
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

    # run RDF
    start_time = time.time()
    outFile = os.path.join(outputDir, args.output)
    runRDF(rdfModule, fileListRoot, outFile, nevents_local, args)

    # pass-down the initial nevents
    outf = ROOT.TFile( outFile, "update" )
    outt = outf.Get("events")
    outn = outt.GetEntries()
    n = array( "i", [ 0 ] )
    n[0] = nevents_local
    if nevents_meta > nevents_local: n[0] = nevents_meta
    p = ROOT.TParameter(int)( "eventsProcessed", n[0])
    p.Write()
    outf.Write()
    outf.Close()

    # print benchmarks
    elapsed_time = time.time() - start_time
    print  ()
    print  ("==============================SUMMARY==============================")
    print  ("Output file              :  ",outFile)
    print  ("Elapsed time (H:M:S)     :  ",time.strftime("%H:%M:%S", time.gmtime(elapsed_time)))
    print  ("Events Processed/Second  :  ",int(nevents_local/elapsed_time))
    print  ("Total Events Processed   :  ",int(nevents_local))
    if (nevents_local>0): print  ("Reduction factor local   :  ",outn/nevents_local)
    if (nevents_meta>0):  print  ("Reduction factor total   :  ",outn/nevents_meta)
    print  ("===================================================================")
    print  (" ")
    print  (" ")

    # save the benchmarks
    if args.bench:
        analysis_name = getElement(rdfModule, 'analysisName')
        if not analysis_name:
            analysis_name = expand_absolute_directory(args.pathToAnalysisScript)

        bench_time = {}
        bench_time['name'] = 'Time spent running the analysis: '
        bench_time['name'] += analysis_name
        bench_time['unit'] = 'Seconds'
        bench_time['value'] = elapsed_time
        bench_time['range'] = 10
        bench_time['extra'] = 'Analysis path: ' + expand_absolute_directory(args.pathToAnalysisScript)
        saveBenchmark('benchmarks_smaller_better.json', bench_time)

        bench_evt_per_sec = {}
        bench_evt_per_sec['name'] = 'Events processed per second: '
        bench_evt_per_sec['name'] += analysis_name
        bench_evt_per_sec['unit'] = 'Evt/s'
        bench_evt_per_sec['value'] = nevents_local / elapsed_time
        bench_time['range'] = 1000
        bench_time['extra'] = 'Analysis path: ' + expand_absolute_directory(args.pathToAnalysisScript)
        saveBenchmark('benchmarks_bigger_better.json', bench_evt_per_sec)


#__________________________________________________________
def runStages(rdfModule, args):
    '''
    Load and run the analysis script for pre-processing stages.
    New branches to be used for later usage are defined in this step.
    Filtering and histogramming are the tasks done in runFinal.
    There are two sources of input list of files (mutual exclusive):
    1) specified in command line option '--files'
    2) generated from the analysis script parameter 'processList'
    '''
    #check if outputDir exist and if not create it
    outputDir = get_io_directory(getElement(rdfModule,"outputDir"))

    print("----> Info: Output top-level path:")
    print(f"      {outputDir}")
    print  ("===================================================================")

    # option 1: files are specified in command line, then run rdf directly
    if len(args.files)>0:
        print("----> Running with user defined list of files")
        runLocal(rdfModule, args.files, outputDir, args)
        sys.exit(0)

    # option 2: files specified as process list
    processList = getElement(rdfModule,"processList")

    for process in processList:
        fileList, eventList = getProcessInfoFiles(process, getElement(rdfModule, "inputDir"))
        if len(fileList)==0:
            print('----> ERROR: No files to process. Exit')
            sys.exit(3)

        processDict={}
        fraction = 1
        output = process
        chunks = 1
        try:
            processDict=processList[process]
            if getElementDict(processList[process], 'fraction') != None: fraction = getElementDict(processList[process], 'fraction')
            if getElementDict(processList[process], 'output')   != None: output   = getElementDict(processList[process], 'output')
            if getElementDict(processList[process], 'chunks')   != None: chunks   = getElementDict(processList[process], 'chunks')

        except TypeError:
            print ('----> no values set for process {} will use default values'.format(process))

        print ('\n----> Running process {} with fraction={}, output={}, chunks={}'.format(process, fraction, output, chunks))

        if fraction < 1: fileList = getsubfileList(fileList, eventList, fraction)
        chunkList=[fileList]
        if chunks > 1: chunkList = getchunkList(fileList, chunks)

        #create dir if more than 1 chunk
        if chunks > 1:
            outputdir = os.path.join(outputDir, output)
            if not os.path.exists(outputdir):
                os.makedirs(outputdir)

        for ch in range(len(chunkList)):
            outputchunk=''
            if len(chunkList) > 1: outputchunk = "{}/chunk{}.root".format(output,ch)
            else:                outputchunk = "{}.root".format(output)
            args.output = outputchunk
            runLocal(rdfModule, chunkList[ch], outputDir, args)

#__________________________________________________________
def runFinal(rdfModule):
    '''
    Run the analysis script for simple defining, filtering and histogramming.
    Generate an output root file for every cut specification of each input process.
    The output file contains the specified histograms and optionally the filtered tree.
    '''
    ROOT.ROOT.EnableImplicitMT(getElement(rdfModule,"nCPUS", True))

    #
    start_time = time.time()
    nevents_real=0
    processEvents={}
    eventsTTree={}
    processList={}
    saveTab=[]
    efficiencyList=[]

    # init directories
    inputDir = get_io_directory(getElement(rdfModule,"inputDir", False))
    outputDir = get_io_directory(getElement(rdfModule,"outputDir"))

    # init list of cuts
    cutList = getElement(rdfModule,"cutList", True)
    length_cuts_names = max([len(cut) for cut in cutList])
    cutLabels = getElement(rdfModule,"cutLabels", True)

    # init a table in a separate tex file
    saveTabular = getElement(rdfModule,"saveTabular", True)
    if saveTabular:
        # option to rewrite the cuts in a better way for the table
        if cutLabels:
            cutNames = list(cutLabels.values())
        else:
            cutNames = [cut for cut in cutList]
        # 
        cutNames.insert(0,' ')
        saveTab.append(cutNames)
        efficiencyList.append(cutNames)

    # init file list and nevents of each process
    for pr in getElement(rdfModule,"processList"):
        processEvents[pr]=0
        eventsTTree[pr]=0
        fileListRoot = ROOT.vector('string')()

        # check if it's file
        fin  = f'{inputDir}/{pr}.root'
        if not os.path.isfile(fin):
            print ('----> file ',fin,'  does not exist. Try if it is a directory as it was processed with batch')
        else:
            print ('----> open file ',fin)
            tfin = ROOT.TFile.Open(fin)
            tfin.cd()
            found=False
            for key in tfin.GetListOfKeys():
                if 'eventsProcessed' == key.GetName():
                    events = tfin.eventsProcessed.GetVal()
                    processEvents[pr]=events
                    found=True
            if not found:
                processEvents[pr]=1
            tt=tfin.Get("events")
            eventsTTree[pr]=tt.GetEntries()
            tfin.Close()
            fileListRoot.push_back(fin)

        # check if it's directory
        fin  = f'{inputDir}/{pr}'
        if os.path.isdir(fin):
            print ('----> open directory ',fin)
            flist=glob.glob(f'{fin}/chunk*.root')
            for f in flist:
                tfin = ROOT.TFile.Open(f)
                print ('  ----> ',f)
                tfin.cd()
                found=False
                for key in tfin.GetListOfKeys():
                    if 'eventsProcessed' == key.GetName():
                        events = tfin.eventsProcessed.GetVal()
                        processEvents[pr]+=events
                        found=True
                if not found:
                    processEvents[pr]=1
                tt=tfin.Get("events")
                eventsTTree[pr]+=tt.GetEntries()
                tfin.Close()
                fileListRoot.push_back(f)

        # append
        processList[pr]=fileListRoot

    print('processed events ',processEvents)
    print('events in ttree  ',eventsTTree)

    # run the rdf for each process and each cut
    histoList = getElement(rdfModule,"histoList", True)
    saveCutTree = getElement(rdfModule,"saveCutTree", True)

    for pr in getElement(rdfModule,"processList"):
        print ('\n---->  Running over process : ',pr)
        RDF = ROOT.ROOT.RDataFrame
        df  = RDF("events", processList[pr])

        # Define some new columns
        defineList = getElement(rdfModule,"defineList", True)
        if len(defineList)>0:
            print ('----> Running extra Define')
            for define in defineList:
                df=df.Define(define, defineList[define])

        # Create all histos, snapshots, etc...
        fout_list = []
        histos_list = []
        tdf_list = []
        count_list = []
        cuts_list = []
        cuts_list.append(pr)
        eff_list=[]
        eff_list.append(pr)

        print ('----> Defining snapshots and histograms for each cut')
        for cut in cutList:
            df_cut = df.Filter(cutList[cut])
            count_list.append(df_cut.Count())

            # define the histos
            histos = []
            for v in histoList:
                if "variable" in histoList[v]: # default 1D histogram
                    model = ROOT.RDF.TH1DModel(v, ";{};".format(histoList[v]["title"]), histoList[v]["bin"], histoList[v]["xmin"], histoList[v]["xmax"])
                    histos.append(df_cut.Histo1D(model,histoList[v]["variable"]))
                elif "cols" in histoList[v]: # multi dim histogram (1, 2 or 3D)
                    cols = histoList[v]['cols']
                    bins = histoList[v]['bins']
                    bins_unpacked = tuple([i for sub in bins for i in sub])
                    if len(bins) != len(cols):
                        print ('----> Amount of columns should be equal to the amount of bin configs.')
                        sys.exit(3)
                    if len(cols) == 1:
                        histos.append(df_cut.Histo1D((v, "", *bins_unpacked), cols[0]))
                    elif len(cols) == 2:
                        histos.append(df_cut.Histo2D((v, "", *bins_unpacked), cols[0], cols[1]))
                    elif len(cols) == 3:
                        histos.append(df_cut.Histo3D((v, "", *bins_unpacked), cols[0], cols[1], cols[2]))
                    else:
                        print ('----> Only 1, 2 or 3D histograms supported.')
                        sys.exit(3)
                else:
                    print ('----> Error parsing the histogram config. Provide either name or cols.')
                    sys.exit(3)
            histos_list.append(histos)

            # save the filtered input tree
            fout = f'{outputDir}/{pr}_{cut}.root'
            fout_list.append(fout)

            if saveCutTree:
                opts = ROOT.RDF.RSnapshotOptions()
                opts.fLazy = True
                snapshot_tdf = df_cut.Snapshot("events", fout, "", opts)
                # Needed to avoid python garbage collector messing around with the snapshot
                tdf_list.append(snapshot_tdf)

        # Now perform the loop and evaluate everything at once.
        print ('----> Evaluating...')
        all_events = df.Count().GetValue()
        print ('----> Done')

        nevents_real += all_events
        uncertainty = ROOT.Math.sqrt(all_events)

        print ('----> Cutflow')
        print ('       {cutname:{width}} : {nevents}'.format(cutname='All events', width=16+length_cuts_names, nevents=all_events))

        # Write the histos into output root file
        print ('----> Saving outputs')
        for i, cut in enumerate(cutList):
            fhisto = f'{outputDir}/{pr}_{cut}_histo.root'
            tf    = ROOT.TFile.Open(fhisto,'RECREATE')
            for h in histos_list[i]:
                h.Write()
            tf.Close()

        # append to tex tabular
        if saveTabular:
            cuts_list.append('{nevents:.2e} $\\pm$ {uncertainty:.2e}'.format(nevents=all_events,uncertainty=uncertainty)) # scientific notation - recomended for backgrounds
            # cuts_list.append('{nevents:.3f} $\\pm$ {uncertainty:.3f}'.format(nevents=all_events,uncertainty=uncertainty)) # float notation - recomended for signals with few events
            eff_list.append(1.) #start with 100% efficiency

        for i, cut in enumerate(cutList):
            neventsThisCut = count_list[i].GetValue()
            neventsThisCut_raw = neventsThisCut
            uncertainty = ROOT.Math.sqrt(neventsThisCut_raw)

            # Saving the number of events, uncertainty and efficiency for the output-file
            if saveTabular and cut != 'selNone':
                if neventsThisCut != 0:
                    cuts_list.append('{nevents:.2e} $\\pm$ {uncertainty:.2e}'.format(nevents=neventsThisCut,uncertainty=uncertainty)) # scientific notation - recomended for backgrounds
                    # cuts_list.append('{nevents:.3f} $\\pm$ {uncertainty:.3f}'.format(nevents=neventsThisCut,uncertainty=uncertainty)) # # float notation - recomended for signals with few events
                    prevNevents = cuts_list[-2].split()
                    eff_list.append('{eff:.3f}'.format(eff=1.*neventsThisCut/all_events))
                # if number of events is zero, the previous uncertainty is saved instead:
                elif '$\\pm$' in cuts_list[-1]:
                    cut = (cuts_list[-1]).split()
                    cuts_list.append('$\\leq$ {uncertainty}'.format(uncertainty=cut[2]))
                    eff_list.append('0.')
                else:
                    cuts_list.append(cuts_list[-1])
                    eff_list.append('0.')

        if saveTabular and cut != 'selNone':
            saveTab.append(cuts_list)
            efficiencyList.append(eff_list)

    # save the statistic into tex tabular
    if saveTabular:
        f = open(f'{outputDir}/outputTabular.txt','w')
        # Printing the number of events in format of a LaTeX table
        print('\\begin{table}[H] \n    \\centering \n    \\resizebox{\\textwidth}{!}{ \n    \\begin{tabular}{|l||',end='',file=f)
        print('c|' * (len(cuts_list)-1),end='',file=f)
        print('} \hline',file=f)
        for i, row in enumerate(saveTab):
            print('        ', end='', file=f)
            print(*row, sep = ' & ', end='', file=f)
            print(' \\\\ ', file=f)
            if (i == 0):
                print('        \\hline',file=f)
        print('        \\hline \n    \\end{tabular}} \n    \\caption{Caption} \n    \\label{tab:my_label} \n\\end{table}', file=f)

        # Efficiency:
        print('\n\nEfficiency: ', file=f)
        print('\\begin{table}[H] \n    \\centering \n    \\resizebox{\\textwidth}{!}{ \n    \\begin{tabular}{|l||',end='',file=f)
        print('c|' * (len(cuts_list)-1),end='',file=f)
        print('} \hline',file=f)
        for i in range(len(eff_list)):
            print('        ', end='', file=f)
            v = [row[i] for row in efficiencyList]
            print(*v, sep = ' & ', end='', file=f)
            print(' \\\\ ', file=f)
            if (i == 0):
                print('        \\hline',file=f)
        print('        \\hline \n    \\end{tabular}} \n    \\caption{Caption} \n    \\label{tab:my_label} \n\\end{table}', file=f)
        f.close()

    elapsed_time = time.time() - start_time
    print  ('==============================SUMMARY==============================')
    print  ('Elapsed time (H:M:S)     :  ',time.strftime("%H:%M:%S", time.gmtime(elapsed_time)))
    print  ('Events Processed/Second  :  ',int(nevents_real/elapsed_time))
    print  ('Total Events Processed   :  ',nevents_real)
    print  ('===================================================================')

#__________________________________________________________
def runPlots(param):
    from .ana_plot import ana_plot
    ana_plot(param)

#__________________________________________________________
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

#__________________________________________________________
def run_analysis(mainparser):
    """
    Set things in motion.
    """
    args, _ = mainparser.parse_known_args()
    analysisFile = args.pathToAnalysisScript
    if not os.path.isfile(analysisFile):
        rootLogger.error(f'Script {analysisFile} not exist')
        sys.exit(3)

    #
    print ("----> Info: Loading MegatAnalyzer Runtime... ",)
    ROOT.gSystem.Load("libMegatAnalysis")
    ROOT.gErrorIgnoreLevel = ROOT.kFatal
    # force auto-loading [todo: ugly as a hack]
    ROOT.LoadMegat()

    # set the RDF ELogLevel
    try:
        ROOT.Experimental.RLogScopedVerbosity(ROOT.Detail.RDF.RDFLogChannel(), getattr(ROOT.Experimental.ELogLevel,args.loglevel))
    except AttributeError:
        pass

    # load the analysis script
    analysisFile = os.path.abspath(analysisFile)
    print('----> Info: Loading analysis file:')
    print('      ' + analysisFile)

    rdfSpec   = importlib.util.spec_from_file_location("rdfanalysis", analysisFile)
    rdfModule = importlib.util.module_from_spec(rdfSpec)
    rdfSpec.loader.exec_module(rdfModule)

    # load current pkg and dependency pkgs
    libPath = mgana_lib_path()
    analysesList = getElement(rdfModule, "analysesList")
    if analysesList and len(analysesList) > 0:
        _ana = []
        for analysis in analysesList:
            print(f'----> Info: Loading analyzer package {analysis}...')
            if analysis.startswith('libMegatAnalyzer_'):
                ROOT.gSystem.Load(os.path.join(libPath, analysis))
            else:
                ROOT.gSystem.Load(os.path.join(libPath, f'libMegatAnalyzer_{analysis}'))
            if not hasattr(ROOT, analysis):
                rootLogger.error(f'----> ERROR: analysis "{analysis}" not properly loaded. Exit')
                sys.exit(4)
            # todo: ugly hack to auto-load dictionary
            _ana.append(getattr(ROOT, analysis).dictionary)

    # for convenience
    ROOT.gInterpreter.Declare("using namespace megat;")
    ROOT.gInterpreter.Declare("using namespace ROOT;")
    ROOT.gInterpreter.Declare("using namespace ROOT::VecOps;")

    # load geometry (the 'default' one)
    geometryFile = getElement(rdfModule, "geometryFile")
    geomFileList = ROOT.vector('string')()
    if geometryFile:
        for geofile in geometryFile:
            realfile = geofile if is_absolute_path(geofile) else os.path.join(megat_geometry_path(), geofile)
            geomFileList.push_back(realfile)
        # load default geometry, with default readout specification
        megat.loadGeometry(geomFileList)

    # load extra geometry for each separate readout specification of TPC
    readoutName  = getElement(rdfModule, "readoutName")
    readoutList = ROOT.vector('string')()
    if readoutName:
        for ro in readoutName:
            # readout name is the tag of this geometry
            megat.loadGeometry(geomFileList, ro, ro, "TPC")
            readoutList.push_back(ro)

    # execute specific command
    if hasattr(args, 'command'):
        if args.command == "run":
            try:
                runStages(rdfModule, args)
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
        elif args.command == "plot":
            try:
                runPlots(rdfModule)
            except Exception as excp:
                print('----> Error: During the execution of the plots file:')
                print('      ' + analysisFile)
                print('      exception occurred:')
                print(excp)
        return
