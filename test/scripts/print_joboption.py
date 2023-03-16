import sys
import ROOT

print(sys.argv[1])
f = ROOT.TFile.Open(sys.argv[1])
t = f.Get('metadata')
v = ROOT.std.vector('std::string')()
t.SetBranchAddress('gaudiConfigOptions',v)
t.GetEntry(0)
print(v)
