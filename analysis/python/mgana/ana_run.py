import ROOT
import os, sys
import time
import glob
import json
import subprocess
import importlib.util
from array import array
import datetime

from .logger import rootLogger

DATE = datetime.datetime.fromtimestamp(datetime.datetime.now().timestamp()).strftime('%Y-%m-%d_%H-%M-%S')

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
def getElementDict(d, element):
    try:
        value=d[element]
        return value
    except KeyError:
#        print (element, "does not exist using default value")
        return None

#__________________________________________________________
def getProcessInfo(process, inputDir):
    if inputDir!=None:
        return getProcessInfoFiles(process, inputDir)
    else:
        print('inputDir is mandatory')
        sys.exist(3)

#__________________________________________________________
def getProcessInfoFiles(process, inputDir):
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
    tf=ROOT.TFile.Open(f,"READ")
    tf.cd()
    tt=tf.Get("events")
    nevents=tt.GetEntries()
    tf.Close()
    return nevents

#__________________________________________________________
def getsubfileList(fileList, eventList, fraction):
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
def getCommandOutput(command):
    p = subprocess.Popen(command, shell = True, stdout = subprocess.PIPE, stderr = subprocess.PIPE,universal_newlines=True)
    (stdout,stderr) = p.communicate()
    return {"stdout":stdout, "stderr":stderr, "returncode":p.returncode}

#__________________________________________________________
def runRDF(rdfModule, inputlist, outFile, nevt, args):
    # for convenience and compatibility with user code
    ROOT.gInterpreter.Declare("using namespace FCCAnalyses;")
    geometryFile = getElement(rdfModule, "geometryFile")
    readoutName  = getElement(rdfModule, "readoutName")
    if geometryFile!="" and readoutName!="":
        ROOT.CaloNtupleizer.loadGeometry(geometryFile, readoutName)
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

    preprocess=False
    if preprocess:
        df2 = runPreprocess(df)

    print("----> Init done, about to run {} events on {} CPUs".format(nevt, ncpus))

    df2 = getElement(rdfModule.RDFanalysis, "analysers")(df)

    branchList = getElement(rdfModule.RDFanalysis, "output")()
    branchListVec = ROOT.vector('string')()
    for branchName in branchList:
        branchListVec.push_back(branchName)

    df2.Snapshot("events", outFile, branchListVec)


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

    if args.batch == False:
        outFile+=args.output
    else:
        outFile=args.output
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

#    if args.test:
#        outf2 = ROOT.TFile(fileListRoot[0])
#        outt2_1 = outf2.Get("metadata")
#        outt2_2 = outf2.Get("run_metadata")
#        outt2_3 = outf2.Get("evt_metadata")
#        outt2_4 = outf2.Get("col_metadata")
#        outf.cd()
#        outt2_1.Write()
#        outt2_2.Write()
#        outt2_3.Write()
#        outt2_4.Write()

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
def runStages(args, rdfModule, preprocess, analysisFile):
    analysesList = getElement(rdfModule, "analysesList")
    if analysesList and len(analysesList) > 0:
        _ana = []
        for analysis in analysesList:
            print(f'----> Load cxx analyzers from {analysis}...')
            if analysis.startswith('libFCCAnalysis_'):
                ROOT.gSystem.Load(analysis)
            else:
                ROOT.gSystem.Load(f'libFCCAnalysis_{analysis}')
            if not hasattr(ROOT, analysis):
                print(f'----> ERROR: analysis "{analysis}" not properly loaded. Exit')
                sys.exit(4)
            _ana.append(getattr(ROOT, analysis).dictionary)

    #check if outputDir exist and if not create it
    outputDir = getElement(rdfModule,"outputDir")
    if not os.path.exists(outputDir) and outputDir!='':
        os.system("mkdir -p {}".format(outputDir))

    #check if outputDir exist and if not create it
    outputDirEos = getElement(rdfModule,"outputDirEos")
    if not os.path.exists(outputDirEos) and outputDirEos!='':
        os.system("mkdir -p {}".format(outputDirEos))

    #check if files are specified, and if so run the analysis on it/them (this will exit after)
    if len(args.files_list)>0:
        print("----> Running with user defined list of files (either locally or from batch)")
        path, filename = os.path.split(args.output)
        if path!='': os.system("mkdir -p {}".format(path))
        runLocal(rdfModule, args.files_list, args)
        sys.exit(0)

    #check if batch mode and set start and end file from original list
    runBatch = getElement(rdfModule,"runBatch")

    #check if the process list is specified
    processList = getElement(rdfModule,"processList")

    for process in processList:
        fileList, eventList = getProcessInfo(process, getElement(rdfModule, "inputDir"))
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

        print ('----> Running process {} with fraction={}, output={}, chunks={}'.format(process, fraction, output, chunks))

        if fraction<1:fileList = getsubfileList(fileList, eventList, fraction)
        chunkList=[fileList]
        if chunks>1: chunkList = getchunkList(fileList, chunks)

        #create dir if more than 1 chunk
        if chunks>1:
            outputdir=outputDir+"/"+output

            if not os.path.exists(outputdir) and outputDir!='':
                os.system("mkdir -p {}".format(outputdir))

        for ch in range(len(chunkList)):
            outputchunk=''
            if len(chunkList)>1: outputchunk="/{}/chunk{}.root".format(output,ch)
            else:                outputchunk="{}.root".format(output)
            #run locally
            if runBatch == False:
                print ('----> Running Locally')
                args.output = outputchunk
                runLocal(rdfModule, chunkList[ch], args)

#__________________________________________________________
def runFinal(rdfModule):

    procFile = getElement(rdfModule,"procDict", True)
    procDict = None
    if 'https://fcc-physics-events.web.cern.ch' in procFile:
        print ('----> getting process dictionary from the web')
        import urllib.request
        req = urllib.request.urlopen(procFile).read()
        procDict = json.loads(req.decode('utf-8'))

    else:
        procFile = os.path.join(os.getenv('FCCDICTSDIR', deffccdicts), '') + procFile
        if not os.path.isfile(procFile):
            print ('----> No procDict found: ==={}===, exit'.format(procFile))
            sys.exit(3)
        with open(procFile, 'r') as f:
            procDict=json.load(f)


    procDictAdd = getElement(rdfModule,"procDictAdd", True)
    for procAdd in procDictAdd:
        if getElementDict(procDict, procAdd) == None:
            procDict[procAdd] = procDictAdd[procAdd]

    ROOT.ROOT.EnableImplicitMT(getElement(rdfModule,"nCPUS", True))

    nevents_real=0
    start_time = time.time()

    processEvents={}
    eventsTTree={}
    processList={}
    saveTab=[]
    efficiencyList=[]

    inputDir = getElement(rdfModule,"inputDir", True)
    if inputDir!="":
        if inputDir[-1]!="/":inputDir+="/"

    outputDir = getElement(rdfModule,"outputDir", True)
    if outputDir!="":
        if outputDir[-1]!="/":outputDir+="/"

    if not os.path.exists(outputDir) and outputDir!='':
        os.system("mkdir -p {}".format(outputDir))

    cutList = getElement(rdfModule,"cutList", True)
    length_cuts_names = max([len(cut) for cut in cutList])
    cutLabels = getElement(rdfModule,"cutLabels", True)

    # save a table in a separate tex file
    saveTabular = getElement(rdfModule,"saveTabular", True)
    if saveTabular:
        # option to rewrite the cuts in a better way for the table. otherwise, take them from the cutList
        if cutLabels:
            cutNames = list(cutLabels.values())
        else:
            cutNames = [cut for cut in cutList]

        cutNames.insert(0,' ')
        saveTab.append(cutNames)
        efficiencyList.append(cutNames)

    for pr in getElement(rdfModule,"processList", True):
        processEvents[pr]=0
        eventsTTree[pr]=0

        fileListRoot = ROOT.vector('string')()
        fin  = inputDir+pr+'.root' #input file
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
            eventsTTree[pr]+=tt.GetEntries()

            tfin.Close()
            fileListRoot.push_back(fin)

        if os.path.isdir(inputDir+pr):
            print ('----> open directory ',fin)
            flist=glob.glob(inputDir+pr+"/chunk*.root")
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
        processList[pr]=fileListRoot

    print('processed events ',processEvents)
    print('events in ttree  ',eventsTTree)

    histoList = getElement(rdfModule,"histoList", True)
    doScale = getElement(rdfModule,"doScale", True)
    intLumi = getElement(rdfModule,"intLumi", True)

    doTree = getElement(rdfModule,"doTree", True)
    for pr in getElement(rdfModule,"processList", True):
        print ('\n---->  Running over process : ',pr)

        RDF = ROOT.ROOT.RDataFrame
        df  = RDF("events", processList[pr])
        defineList = getElement(rdfModule,"defineList", True)
        if len(defineList)>0:
            print ('----> Running extra Define')
            for define in defineList:
                df=df.Define(define, defineList[define])

        fout_list = []
        histos_list = []
        tdf_list = []
        count_list = []
        cuts_list = []
        cuts_list.append(pr)
        eff_list=[]
        eff_list.append(pr)

        # Define all histos, snapshots, etc...
        print ('----> Defining snapshots and histograms')
        for cut in cutList:
            fout = outputDir+pr+'_'+cut+'.root' #output file for tree
            fout_list.append(fout)

            df_cut = df.Filter(cutList[cut])
            count_list.append(df_cut.Count())

            histos = []

            for v in histoList:
                if "name" in histoList[v]: # default 1D histogram
                    model = ROOT.RDF.TH1DModel(v, ";{};".format(histoList[v]["title"]), histoList[v]["bin"], histoList[v]["xmin"], histoList[v]["xmax"])
                    histos.append(df_cut.Histo1D(model,histoList[v]["name"]))
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

            if doTree:
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

        if doScale:
            all_events = all_events*1.*procDict[pr]["crossSection"]*procDict[pr]["kfactor"]*procDict[pr]["matchingEfficiency"]*intLumi/processEvents[pr]
            uncertainty = ROOT.Math.sqrt(all_events)*procDict[pr]["crossSection"]*procDict[pr]["kfactor"]*procDict[pr]["matchingEfficiency"]*intLumi/processEvents[pr]
            print('  Printing scaled number of events!!! ')

        print ('----> Cutflow')
        print ('       {cutname:{width}} : {nevents}'.format(cutname='All events', width=16+length_cuts_names, nevents=all_events))

        if saveTabular:
            cuts_list.append('{nevents:.2e} $\\pm$ {uncertainty:.2e}'.format(nevents=all_events,uncertainty=uncertainty)) # scientific notation - recomended for backgrounds
            # cuts_list.append('{nevents:.3f} $\\pm$ {uncertainty:.3f}'.format(nevents=all_events,uncertainty=uncertainty)) # float notation - recomended for signals with few events
            eff_list.append(1.) #start with 100% efficiency

        for i, cut in enumerate(cutList):
            neventsThisCut = count_list[i].GetValue()
            neventsThisCut_raw = neventsThisCut
            uncertainty = ROOT.Math.sqrt(neventsThisCut_raw)
            if doScale:
                neventsThisCut = neventsThisCut*1.*procDict[pr]["crossSection"]*procDict[pr]["kfactor"]*procDict[pr]["matchingEfficiency"]*intLumi/processEvents[pr]
                uncertainty = ROOT.Math.sqrt(neventsThisCut_raw)*procDict[pr]["crossSection"]*procDict[pr]["kfactor"]*procDict[pr]["matchingEfficiency"]*intLumi/processEvents[pr]
            print ('       After selection {cutname:{width}} : {nevents}'.format(cutname=cut, width=length_cuts_names, nevents=neventsThisCut))

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

        # And save everything
        print ('----> Saving outputs')
        for i, cut in enumerate(cutList):
            fhisto = outputDir+pr+'_'+cut+'_histo.root' #output file for histograms
            tf    = ROOT.TFile.Open(fhisto,'RECREATE')
            for h in histos_list[i]:
                try :
                    h.Scale(1.*procDict[pr]["crossSection"]*procDict[pr]["kfactor"]*procDict[pr]["matchingEfficiency"]/processEvents[pr])
                except KeyError:
                    print ('----> No value defined for process {} in dictionary'.format(pr))
                    if h.Integral(0,-1)>0:h.Scale(1./h.Integral(0,-1))
                h.Write()
            tf.Close()

            if doTree:
                # test that the snapshot worked well
                validfile = testfile(fout_list[i])
                if not validfile: continue

        if saveTabular and cut != 'selNone':
            saveTab.append(cuts_list)
            efficiencyList.append(eff_list)

    if saveTabular:
        f = open(outputDir+"outputTabular.txt","w")
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
def runPlots(analysisFile):
    import mgana.ana_plot as apl
    apl.run(analysisFile)

#__________________________________________________________
def runValidate(jobdir):
    listdir=os.listdir(jobdir)
    if jobdir[-1]!="/":jobdir+="/"
    for dir in listdir:
        if not os.path.isdir(jobdir+dir): continue
        listfile=glob.glob(jobdir+dir+"/*.sh")
        for file in listfile:
            with open(file) as f:
                for line in f:
                    pass
                lastLine = line
            print(line)

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
                runStages(args, rdfModule, args.preprocess, analysisFile)
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
