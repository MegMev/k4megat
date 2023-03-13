// print PODIO data types for a ROOT file
//
// USAGE:
// root -b -q tutorial/podio_datatypes.C'("my_root_file.root")'  // print all collections
// root -b -q tutorial/podio_datatypes.C'("my_root_file.root","MCParticles")'  // print MCParticles only
//

// R__LOAD_LIBRARY( podio )
R__LOAD_LIBRARY( podioDict )
// R__LOAD_LIBRARY( podioRootIO )
// R__LOAD_LIBRARY( edm4hep )
R__LOAD_LIBRARY( edm4hepDict )
R__LOAD_LIBRARY( fmt )

#include "fmt/format.h"
#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

void print_metadata( const char* root_file_name ) {

  // open event store
  podio::ROOTReader reader;
  podio::EventStore store;
  reader.openFile( root_file_name );
  store.setReader( &reader );

  // get list of collection names
  fmt::print( "\n{:+^55}\n", " podio collection id table " );
  const auto collIDTable = store.getCollectionIDTable();
  collIDTable->print();

  auto collNames = collIDTable->names();
  // print header
  fmt::print( "\n{:+^55}\n", " details " );
  fmt::print( "\n{:<4} {:<50}\n", "ID", "CollectionName" );
  fmt::print( "{:>50}\n{:>50}\n{:>50}\n", "CollectionTypeName", "ValueTypeName", "DataTypeName" );

  // print collection types
  for ( auto collName : collNames ) {
    auto collID = collIDTable->collectionID( collName );

    // get collection type
    fmt::print( "\n{:<4} {:<50}\n", collID, collName );
    podio::CollectionBase* collBase;
    if ( store.get( collID, collBase ) ) {
      if ( collBase != nullptr ) {
        fmt::print( "{:>50}\n{:>50}\n{:>50}\n", collBase->getTypeName(), collBase->getValueTypeName(),
                    collBase->getDataTypeName() );
      } else
        fmt::print( "{:>50}\n", "found in event store, but NULL" );
    } else
      fmt::print( "{:>50}\n", "not found in event store" );

    auto collParams = store.getCollectionMetaData( collID );
    for ( const auto& [name, params] : collParams.getStringMap() ) {
      fmt::print( " -> {}\n", name );
      for ( const auto& param : params ) fmt::print( "      {}\n", param );
    };
  }
  fmt::print( "\n{:+^55}\n", "" );
}
