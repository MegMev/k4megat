import sys, os
import ROOT
import copy
import re

from .utility import get_io_directory

#__________________________________________________________
def removekey(d, key):
    r = dict(d)
    del r[key]
    return r

def sortedDictValues(dic):
    keys = sorted(dic)
    return [dic[key] for key in keys]

#__________________________________________________________
def mapHistos(var,   # histogram name
              label, # label string (key of 'plots')
              sel,   # selection tag string
              param  # param python script
              ):
    print (f'run plots for var:{var}     label:{label}     selection:{sel}')
    signal=param.plots[label]['signal']
    backgrounds=param.plots[label]['backgrounds']

    # get input directory
    inputDir = get_io_directory(param.inputDir, False)

    # get signal hists
    hsignal = {}
    for s in signal:
        hsignal[s]=[]
        for f in signal[s]:
            fin=f'{inputDir}/{f}_{sel}_histo.root'
            if not os.path.isfile(fin):
                print (f'War: {fin} does not exist, skip')
            else:
                tf=ROOT.TFile(fin)
                h=tf.Get(var)
                hh = copy.deepcopy(h)
                if len(hsignal[s])==0:
                    hsignal[s].append(hh)
                else:
                    hh.Add(hsignal[s][0])
                    hsignal[s][0]=hh


    # get bkg hists
    hbackgrounds = {}
    for b in backgrounds:
        hbackgrounds[b]=[]
        for f in backgrounds[b]:
            fin=param.inputDir+f+'_'+sel+'_histo.root'
            if not os.path.isfile(fin):
                print ('file {} does not exist, skip'.format(fin))
            else:
                tf=ROOT.TFile(fin)
                h=tf.Get(var)
                hh = copy.deepcopy(h)
                if len(hbackgrounds[b])==0:
                    hbackgrounds[b].append(hh)
                else:
                    hh.Add(hbackgrounds[b][0])
                    hbackgrounds[b][0]=hh

    for s in hsignal:
        if len(hsignal[s])==0:
            hsignal=removekey(hsignal,s)

    for b in hbackgrounds:
        if len(hbackgrounds[b])==0:
            hbackgrounds=removekey(hbackgrounds,b)

    return hsignal,hbackgrounds

#__________________________________________________________
def runPlots(var,
             sel, # list sel tags
             param, # param config file
             hsignal, # map of signal histograms
             hbackgrounds, # map of bkg histograms
             extralab,
             splitLeg: bool):
    #Below are settings for separate signal and background legends
    if(splitLeg):
        legsize = 0.04*(len(hsignal))
        legsize2 = 0.04*(len(hbackgrounds))
        legCoord = [0.15,0.60 - legsize,0.50,0.62]
        leg2 = ROOT.TLegend(0.60,0.60 - legsize2,0.88,0.62)
        leg2.SetFillColor(0)
        leg2.SetFillStyle(0)
        leg2.SetLineColor(0)
        leg2.SetShadowColor(10)
        leg2.SetTextSize(0.035)
        leg2.SetTextFont(42)
    else:
        legsize = 0.04*(len(hbackgrounds)+len(hsignal))
        legCoord=[0.68, 0.86-legsize, 0.96, 0.88]
        try:
            legCoord=param.legendCoord
        except AttributeError:
            print ('no legCoord, using default one...')
            legCoord=[0.68, 0.86-legsize, 0.96, 0.88]
        leg2 = None

    leg = ROOT.TLegend(legCoord[0],legCoord[1],legCoord[2],legCoord[3])
    leg.SetFillColor(0)
    leg.SetFillStyle(0)
    leg.SetLineColor(0)
    leg.SetShadowColor(10)
    leg.SetTextSize(0.035)
    leg.SetTextFont(42)

    for b in hbackgrounds:
        if(splitLeg):
            leg2.AddEntry(hbackgrounds[b][0],param.legend[b],"f")
        else:
            leg.AddEntry(hbackgrounds[b][0],param.legend[b],"f")
    for s in hsignal:
        leg.AddEntry(hsignal[s][0],param.legend[s],"l")

    # get list of hists
    histos=[]
    colors=[]

    nsig=len(hsignal)
    nbkg=len(hbackgrounds)

    for s in hsignal:
        histos.append(hsignal[s][0])
        colors.append(param.colors[s])

    for b in hbackgrounds:
        histos.append(hbackgrounds[b][0])
        colors.append(param.colors[b])

    # get plot label
    lt = "Megat: Simulation"
    rt = "#sqrt{{s}} = {:.1f} TeV".format(param.energy)

    customLabel=""
    try:
        customLabel=param.customLabel
    except AttributeError:
        print ('no customLable, using nothing...')

    # draw the plots, available combinations: [stack, nostack] + [lin, log]
    if 'stack' in param.stacksig:
        if 'lin' in param.yaxis:
            drawStack(var+"_stack_lin", 'events', leg, lt, rt, param.formats, param.outputDir+"/"+sel, False , True , histos, colors, param.ana_tex, extralab, customLabel, nsig, nbkg, leg2)
        if 'log' in param.yaxis:
            drawStack(var+"_stack_log", 'events', leg, lt, rt, param.formats, param.outputDir+"/"+sel, True , True , histos, colors, param.ana_tex, extralab, customLabel, nsig, nbkg, leg2)
        if 'lin' not in param.yaxis and 'log' not in param.yaxis:
            print ('unrecognised option in formats, should be [\'lin\',\'log\']'.format(param.formats))

    if 'nostack' in param.stacksig:
        if 'lin' in param.yaxis:
            drawStack(var+"_nostack_lin", 'events', leg, lt, rt, param.formats, param.outputDir+"/"+sel, False , False , histos, colors, param.ana_tex, extralab, customLabel, nsig, nbkg, leg2)
        if 'log' in param.yaxis:
            drawStack(var+"_nostack_log", 'events', leg, lt, rt, param.formats, param.outputDir+"/"+sel, True , False , histos, colors, param.ana_tex, extralab, customLabel, nsig, nbkg, leg2)
        if 'lin' not in param.yaxis and 'log' not in param.yaxis:
            print ('unrecognised option in formats, should be [\'lin\',\'log\']'.format(param.formats))

    if 'stack' not in param.stacksig and 'nostack' not in param.stacksig:
        print ('unrecognised option in stacksig, should be [\'stack\',\'nostack\']'.format(param.formats))


#___________________________________________________________________________________________
def drawStack(name, ylabel, legend, leftText, rightText,
              formats, directory,
              logY, stacksig, histos,
              colors, ana_tex, extralab, customLabel,
              nsig, nbkg,
              legend2=None):
    # canvas
    canvas = ROOT.TCanvas(name, name, 600, 600)
    canvas.SetLogy(logY)
    canvas.SetTicks(1,1)
    canvas.SetLeftMargin(0.14)
    canvas.SetRightMargin(0.08)


    # first retrieve maximum
    sumhistos = histos[0].Clone()
    iterh = iter(histos)
    next(iterh)

    unit = 'GeV'
    if 'TeV' in str(histos[0].GetXaxis().GetTitle()):
        unit = 'TeV'

    if unit in str(histos[0].GetXaxis().GetTitle()):
        bwidth=sumhistos.GetBinWidth(1)
        if bwidth.is_integer():
            ylabel+=' / {} {}'.format(int(bwidth), unit)
        else:
            ylabel+=' / {:.2f} {}'.format(bwidth, unit)

    # get y-axis max
    for h in iterh:
      sumhistos.Add(h)
    maxh = sumhistos.GetMaximum()
    minh = sumhistos.GetMinimum()

    # define stacked histo
    hStack    = ROOT.THStack("hstack","")
    hStackBkg = ROOT.THStack("hstackbkg","")
    BgMCHistYieldsDic = {}

    # first plot backgrounds
    if(nbkg>0):
        # get the yield for each background 
        iterh = iter(histos)
        for i in range(nsig):
            next(iterh)

        k = nsig
        for h in iterh:
            h.SetLineWidth(0)
            h.SetLineColor(ROOT.kBlack)
            h.SetFillColor(colors[k])
            if h.Integral()>0:
                BgMCHistYieldsDic[h.Integral()] = h
            else:
                BgMCHistYieldsDic[-1*nbkg] = h
            k += 1

        # sort stack by yields (smallest to largest)
        BgMCHistYieldsDic = sortedDictValues(BgMCHistYieldsDic)
        for h in BgMCHistYieldsDic:
            hStack.Add(h)
            hStackBkg.Add(h)

        # draw bkgs directly if signals are not stacked
        if not stacksig:
            hStack.Draw("hist")

    # define stacked signal histo
    hStackSig = ROOT.THStack("hstacksig","")

    # finally add signal on top
    for l in range(nsig):
      histos[l].SetLineWidth(3)
      histos[l].SetLineColor(colors[l])
      if stacksig:
        hStack.Add(histos[l])
      else:
        hStackSig.Add(histos[l])

    # draw sig+bkg stacks
    if stacksig:
        hStack.Draw("hist")
    elif nbkg == 0:
        hStackSig.Draw("hist nostack")

    # set axis labels
    xlabel = histos[0].GetXaxis().GetTitle()
    if (not stacksig) and nbkg==0:
        hStackSig.GetXaxis().SetTitle(xlabel)
        hStackSig.GetYaxis().SetTitle(ylabel)

        hStackSig.GetYaxis().SetTitleOffset(1.95)
        hStackSig.GetXaxis().SetTitleOffset(1.40)
    else:
        hStack.GetXaxis().SetTitle(xlabel)
        hStack.GetYaxis().SetTitle(ylabel)

        hStack.GetYaxis().SetTitleOffset(1.95)
        hStack.GetXaxis().SetTitleOffset(1.40)

    # set y-axis range
    lowY=0.
    if logY:
        highY=200.*maxh/ROOT.gPad.GetUymax()
        threshold=0.5
        if (not stacksig) and nbkg==0:
            bin_width=hStackSig.GetXaxis().GetBinWidth(1)
        else:
            bin_width=hStack.GetXaxis().GetBinWidth(1)
        lowY=threshold*bin_width
        if (not stacksig) and nbkg==0:
            hStackSig.SetMaximum(highY)
            hStackSig.SetMinimum(lowY)
        else:
            hStack.SetMaximum(highY)
            hStack.SetMinimum(lowY)
    else:
        if (not stacksig) and nbkg==0:
            hStackSig.SetMaximum(1.3*maxh)
            hStackSig.SetMinimum(0.)
        else:
            hStack.SetMaximum(1.3*maxh)
            hStack.SetMinimum(0.)

    # todo: why here? last stack draw
    if not stacksig and nbkg:
        hStackSig.Draw("same hist nostack")

    # draw legend and other annotations
    legend.Draw()
    if legend2 != None:
        legend2.Draw()

    Text = ROOT.TLatex()
    Text.SetNDC()
    Text.SetTextAlign(31);
    Text.SetTextSize(0.04)

    text = '#it{' + leftText +'}'
    Text.DrawLatex(0.90, 0.94, text)

    text = '#it{'+customLabel+'}'
    Text.SetTextAlign(12);
    Text.SetNDC(ROOT.kTRUE)
    Text.SetTextSize(0.04)
    Text.DrawLatex(0.18, 0.85, text)

    text = '#bf{#it{' + rightText +'}}'
    Text.SetTextAlign(12);
    Text.SetNDC(ROOT.kTRUE)
    Text.SetTextSize(0.04)
    Text.DrawLatex(0.18, 0.81, text)

    text = '#bf{#it{' + ana_tex +'}}'
    Text.SetTextSize(0.04)
    Text.DrawLatex(0.18, 0.71, text)

    text = '#bf{#it{' + extralab +'}}'
    Text.SetTextSize(0.025)
    Text.DrawLatex(0.18, 0.66, text)

    # update and print canvas
    canvas.RedrawAxis()
    canvas.GetFrame().SetBorderSize( 12 )
    canvas.Modified()
    canvas.Update()

    printCanvas(canvas, name, formats, directory)


#____________________________________________________
def printCanvas(canvas, name, formats, directory):

    if format != "":
        if not os.path.exists(directory) :
                os.system("mkdir -p "+directory)
        for f in formats:
            outFile = os.path.join(directory, name) + "." + f
            canvas.SaveAs(outFile)


#__________________________________________________________
def ana_plot(param):
    ROOT.gROOT.SetBatch(True)
    ROOT.gErrorIgnoreLevel = ROOT.kWarning

    if hasattr(param, "splitLeg"):
        splitLeg = param.splitLeg
    else:
        splitLeg = False

    for var in param.variables:
        for label, sels in param.selections.items():
            for sel in sels:
                hsignal,hbackgrounds=mapHistos(var,label,sel, param)
                runPlots(var+"_"+label,sel,param,hsignal,hbackgrounds,param.extralabel[sel],splitLeg)
