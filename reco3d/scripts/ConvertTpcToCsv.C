#include <fstream>
#include <sstream>

void ConvertTpcToCsv( string infile, string tree, int start, int end, string out_prefix = "event" ) {

  using namespace ROOT::VecOps;

  int i = start;

  auto WriteCsv = [&i, out_prefix]( RVec<float> energy, RVec<long> layer, RVec<double> position, RVec<float> time ) {
    stringstream fmt;
    fmt << out_prefix << i << ".csv";

    ofstream fout;
    fout.open( fmt.str(), ios::out );
    fout << "layer, position, time, energy " << std::endl;

    auto itr_e = energy.begin();
    auto itr_l = layer.begin();
    auto itr_p = position.begin();
    auto itr_t = time.begin();
    auto end_e = energy.end();

    while ( itr_e != end_e ) {
      fout << *itr_l << ", " << *itr_p << ", " << *itr_t << ", " << *itr_e << std::endl;
      itr_e++;
      itr_l++;
      itr_p++;
      itr_t++;
    }

    i++;
  };

  ROOT::RDataFrame df( tree, infile );

  auto n = df.Count().GetValue();

  if ( start > n ) {
    std::cout << "Fetal: start: " << start << " too large for length: " << n << std::endl;
    return;
  }
  if ( end > n ) {
    std::cout << "Warning: end: " << end << " too large for length: " << n << ". Set to " << n << std::endl;
    end = n;
  }

  df.Range( start, end ).Foreach( WriteCsv, { "energy", "layer", "position", "time" } );
}
