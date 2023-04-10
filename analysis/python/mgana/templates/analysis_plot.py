import ROOT

# global parameters
ana_tex        = 'A test plots'
energy         = 240.0
# customLabel    = 'Your Label'
# legCoord       = [0.5, 0.9, 0.9, 0.8]
formats        = ['png','pdf']
yaxis          = ['lin','log']
stacksig       = ['stack','nostack']

#IO directory
inputDir       = 'final'
outputDir      = 'plots'

#List of 1D histogram names to be assembled (usually represents one physical variable)
variables = ['pixel_X','strip_1D']

#Dictionary with the analysis name as a key, and the list of selections to be plotted for this analysis. The name of the selections should be the same than in the final selection
selections = {}
selections['ZH']   = ["sel0","sel1"]

plots = {}
plots['ZH'] = {'signal':{'signal1':['batch']},
               'backgrounds':{}
               }

#Extra labels: key is selection tag
extralabel = {}
extralabel['sel0'] = "strip_meanx > -5 && strip_meanx < 10"
extralabel['sel1'] = "pixel_meanx < 10"

# properties of each histogram: key is hist tag
legend = {}
legend['signal1'] = 'signal1'

colors = {}
colors['signal1'] = ROOT.kRed
