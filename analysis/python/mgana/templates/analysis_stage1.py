#Optional: List of analysis packages to load in runtime
analysesList = ['__pkgname__']

#Optional: name of this analysis
analysisName = 'dummyAnalysis'

#Optional: input directory, default is pkg's workspace directory
#          relative path is under pkg's workspacke dircetory
#          absolute path is supported as well
inputDir  = "/home/yong/src/physics/rest/test_scripts/data"

#Optional: output directory, default is pkg's workspace directory
#          relative path is under pkg's workspacke dircetory
#          absolute path is supported as well
outputDir = "stage1"

#Mandatory: List of processes (files)
# key: string, input file name (without .root) or directory name
# value: map, 'fraction', 'chunks', 'output' is output filename (without .root)
processList = {
    'batch':{}, # option1: key is directory name
    # 'dummy_process':{}, # option2: key is file name
    # 'dummy_process':{'fraction'=0.8, 'chunks'=2, 'output'='dummy_output'} # valid settings
}

#Optional: number of threads
nCPUS       = 4

#Optional: geometry and readout
geometryFile = ['Megat.xml', 'TPC_readout.xml']
readoutName = ['TpcDiagonalStripHits', 'TpcPixelHits']

#Mandatory: RDFanalysis class where the use defines the operations on the TTree
class RDFanalysis():
    #__________________________________________________________
    #Mandatory: analysers function to define the analysers to process
    #           please make sure you return the last dataframe, in this example it is df2
    def analysers(df):
        df2 = (df
               # .Define("dummy_collection", "__pkgname__::dummy_collection(TpcSegStripHits)")
               .Define("strip_x", "__pkgname__::dummy_collection(TpcSegStripHits)")
               .Define("pixel_x", "__pkgname__::dummy_collection(TpcSegPixelHits)")
              )
        return df2

    #__________________________________________________________
    #Mandatory: output function
    #           please make sure you return the branchlist as a python list
    def output():
        # branchList = ['dummy_collection']
        branchList = ['strip_x', 'pixel_x']
        return branchList
