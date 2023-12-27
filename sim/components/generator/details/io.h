#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace {
  inline bool startsWith( const std::string& str, const std::string& prefix ) {
    return str.size() >= prefix.size() && str.compare( 0, prefix.size(), prefix ) == 0;
  }

  /*
   * Return the lines from file 'filename', with 'prefix' at the beginning of each line
   */
  std::vector<std::string> getLinesWithPrefix( const std::string& filename, const std::string& prefix ) {
    std::ifstream fin( filename );

    std::vector<std::string> lines;
    std::string              buf;
    while ( std::getline( fin, buf ) ) {
      if ( startsWith( buf, prefix ) ) lines.push_back( buf );
    }

    return lines;
  }

} // namespace
