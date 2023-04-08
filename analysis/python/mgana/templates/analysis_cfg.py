#Optional: List of analysis packages to load in runtime
analysesList = ['__pkgname__']

#Optional: name of this analysis
analysisName = 'dummyAnalysis'

#Optional: input directory, default is local running directory
inputDir  = "./workspace"

#Mandatory: List of processes
processList = {
            'dummy_process':{},
            # 'dummy_process':{'fraction'=0.8, 'chunks'=2, 'output'='dummy_output'}
            }

#Optional: output directory, default is local running directory
outputDir   = "./workspace"

#Optional: number of threads
nCPUS       = 4

#Optional: geometry and readout
geometryFile = ['Megat.xml', 'TPC_readout.xml']
readoutName = ['TpcStripHits', 'TpcPixelHits']

#Mandatory: RDFanalysis class where the use defines the operations on the TTree
class RDFanalysis():
    #__________________________________________________________
    #Mandatory: analysers function to define the analysers to process, please make sure you return the last dataframe, in this example it is df2
    def analysers(df):
        df2 = (df
               .Define("dummy_collection", "__pkgname__::dummy_collection(TpcSegStripHits)")
              )
        return df2

    #__________________________________________________________
    #Mandatory: output function, please make sure you return the branchlist as a python list
    def output():
        branchList = ['dummy_collection']
        return branchList
