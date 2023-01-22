{
  ROOT::EnableImplicitMT();

  using ROOT::RDataFrame;
  using MCP = edm4hep::MCParticleData;
  using MCPVec = ROOT::RVec<edm4hep::MCParticleData>;
  using LVecM = ROOT::Math::PxPyPzMVector;

  auto getE =
      [](const MCPVec &particles) {
        return ROOT::VecOps::Map(particles, [](const MCP& p){
          return LVecM(p.momentum.x,p.momentum.y,p.momentum.z,p.mass).E();});
      };

  RDataFrame df("events", "./megat.gaudi.root");
  auto hetrack = df.Define("etrack", getE, {"GenParticles"}).Histo1D("etrack");
  auto hmass = df.Histo1D("GenParticles.mass");

  auto c1 = new TCanvas();
  hetrack->DrawCopy();
  auto c2 = new TCanvas();
  hmass->Draw();
}
