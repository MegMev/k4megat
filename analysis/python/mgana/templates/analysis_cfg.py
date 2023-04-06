#Optional: List of analysis packages to load in runtime
analysesList = ['__pkgname__']

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
               .Define("dummy_collection", "__pkgname__::dummy_collection(TpcStripHits)")
              )
        return df2

    #__________________________________________________________
    #Mandatory: output function, please make sure you return the branchlist as a python list
    def output():
        branchList = ['dummy_collection']
        return branchList
