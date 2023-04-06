import ROOT
from megat import simcalo

# we need MT for long-time job, here is for demo
ROOT.EnableImplicitMT()

# real job
df=ROOT.RDataFrame("events", "./data/megat.edm4hep.root")
df2=(
    df.Define("x1","SimCalo::hit_x(CztHits)")
    .Define("x2","CztHits.position.x")
    .Define("dx", "x1-x2")
    .Define("e","CztHits.energy")
    .Define("pos", "CztHits.position.z")
   )


h1=df2.Histo1D("x1")
h2=df2.Histo1D("e")
hrel = df2.Histo2D(('x1_vs_x2','x1 vs x2', 400,0,400,400,0,400),'x1','x2')

# draw & print
c=ROOT.TCanvas('c', 'c', 1200,500)
c.Divide(3 ,1)
c.cd(1)
h1.Draw()
c.cd(2)
h2.Draw()
c.cd(3)
hrel.Draw('colz')
c.SaveAs('demo_fillx.png')

df2.Describe()

from ROOT import gSystem
gSystem.Exec('open demo_fillx.png')
