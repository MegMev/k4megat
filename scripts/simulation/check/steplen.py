import ROOT as r

# df=r.RDataFrame('events','./nolimit.megat.gaudi.root')
df=r.RDataFrame('events','./megat.gaudi.root')

hstepLen=df.Histo1D('TpcHits.pathLength')
hedep=df.Define('edep','1000*TpcHits.EDep').Histo1D('edep')

c=r.TCanvas('c','c',900,420)
c.Divide(2,1)
c.cd(1)
hstepLen.Draw()
c.cd(2)
hedep.Draw()
c.Update()
