#Input directory where the files produced at the pre-selection level are
inputDir  = "stage1"

#Input directory where the files produced at the pre-selection level are
outputDir  = "final"

processList = {
    #Run over the full statistics from previous input file <inputDir>/batch.root. Keep the same output name as input
    'batch':{}
}

#Number of CPUs to use
nCPUS = 2

#produces ROOT TTrees, default is False
saveCutTree = False

#Optinally Define new variables
defineList = {"strip_stdx":"StdDev(strip_x)",
              "strip_meanx":"Mean(strip_x)",
              "pixel_meanx":"Mean(pixel_x)"
              }

#Dictionnay of the list of cuts. The key is the name of the selection that will be added to the output file
cutList = {"sel0":"strip_meanx > -5 && strip_meanx < 10",
           "sel1":"pixel_meanx < 10",
           }


#Dictionary for the output histograms. The key is the name of the histogram in the output files.
#"variable" is the name of the variable in the input file, "title" is the x-axis label of the histogram, "bin" the number of bins of the histogram, "xmin" the minimum x-axis value and "xmax" the maximum x-axis value.
histoList = {
    "pixel_X":{"variable":"pixel_x","title":"Pixel X","bin":500,"xmin":-250,"xmax":250}, #1D histogram
    "strip_1D":{"cols":["strip_x"],"title":"Strip X", "bins": [(400,-200,200)]}, # 1D histogram (alternative syntax)
    "strip_2D":{"cols":["strip_x", "strip_x"],"title":"Strip X VS X", "bins": [(100,-250,250), (100,-250,250)]}, # 2D histogram
    "strip_3D":{"cols":["strip_x", "strip_x", "strip_x"],"title":"Strip X VS X VS X", "bins": [(100,-250,250), (100,-250,250), (100,-250,250)]}, # 3D histogram
}
