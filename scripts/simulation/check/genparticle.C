{
    using ROOT::RDataFrame;
    using DVec= ROOT::RVec<double>;

    auto col_size = [](const DVec& v)
    {
      return v.size();
    };

    RDataFrame df("events", "./megat.gaudi.root", {"GenParticles"});
    auto hsize = df.Define("ngen", col_size, {"SimParticles.PDG"})
                   .Histo1D("ngen");

    auto c1 = new TCanvas();
    hsize->DrawCopy();
}
