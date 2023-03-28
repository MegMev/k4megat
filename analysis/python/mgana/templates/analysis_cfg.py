#Optional: List of analysis packages to load in runtime
analysesList = ['__pkgname__']

#Mandatory: List of processes
processList = {
    'p8_noBES_ee_H_Hbb_ecm125':{'fraction': 0.01, 'chunks': 1, 'output': 'test_out'}
}

#Optional: output directory, default is local running directory
outputDir   = "."

#Optional
nCPUS       = 4

#Mandatory: RDFanalysis class where the use defines the operations on the TTree
class RDFanalysis():
    #__________________________________________________________
    #Mandatory: analysers function to define the analysers to process, please make sure you return the last dataframe, in this example it is df2
    def analysers(df):
        df2 = (df
               .Define("dummy_collection", "__pkgname__::dummy_collection(TpcTestHits)")
              )
        return df2

    #__________________________________________________________
    #Mandatory: output function, please make sure you return the branchlist as a python list
    def output():
        branchList = ['dummy_collection']
        return branchList
