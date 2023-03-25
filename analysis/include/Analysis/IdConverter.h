/**
 * @file      IdConverter.h
 * @brief     Header of IdConverter
 * @date      Fri Mar 24 18:43:59 2023
 * @author    yong
 * @copyright BSD-3-Clause
 *
 * This class is a hard fork of dd4hep IdConverter with more features specific to Megat
 * It's designed to be used in last-mile analysis scripts.
 */

#ifndef MEGAT_IdConverter_H
#define MEGAT_IdConverter_H

#include "DD4hep/Detector.h"
#include "DD4hep/Readout.h"
#include "DD4hep/VolumeManager.h"
#include "DDSegmentation/Segmentation.h"
#include <DD4hep/Volumes.h>
#include <DDSegmentation/BitFieldCoder.h>
#include <Parsers/Primitives.h>
#include <edm4hep/Vector3d.h>
#include <map>
#include <string>
#include <vector>

namespace megat {
  namespace utility {
    using namespace dd4hep;
    using dd4hep::DDSegmentation::BitFieldCoder;

    class IdConverter {
    public:
      IdConverter( std::string tag = "default" );
      virtual ~IdConverter(){};

      /** Return the global position for a given cellID of a sensitive volume.
       *  Alignment corrections are applied (TO BE DONE).
       *  If no sensitive volume is found, (0,0,0) is returned.
       */
      edm4hep::Vector3d position( const CellID& cellID ) const;

      /** Return the global cellID for the given global position.
       *  Note: this call is rather slow - only use it when really needed !
       *
       */
      CellID cellID( const edm4hep::Vector3d& global ) const;

      /** Returns the segmentation.cellDimensions.
       *
       */
      std::vector<double> cellDimensions( const CellID& cell ) const;

      /** Find the context with DetElement, placements etc for a given cellID of a sensitive volume.
       *  Returns NULL if not found (e.g. if the cellID does not correspond to a sensitive volume).
       */
      const VolumeManagerContext* findContext( const CellID& cellID ) const;

      /** Find the readout object for the given DetElement. If the DetElement is sensitive the corresondig
       *  Readout is returned, else a recursive search in the daughter volumes (nodes) of this DetElement's
       *  volume is performed and the first Readout object is returned.
       *
       */
      Readout findReadout( const DetElement& det ) const;

      /** Return this PlacedVolume's Readout or, if the volume is not sensitive, recursively search for
       * a Readout object in the daughter nodes (volumes).
       */
      Readout findReadout( const PlacedVolume& pv ) const;

      bool isStrip( std::string tpc_name = "TPC" );

      BitFieldCoder* decoder( std::string det_name = "TPC" );

    protected:
      VolumeManager   m_volumeManager{};
      const Detector* m_description;

    private:
      std::map<std::string, bool>           m_multiseg;
      std::map<std::string, BitFieldCoder*> m_decoders;
    };
  } // namespace utility
} // namespace megat

#endif
