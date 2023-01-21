#include "DetCommon/DetUtils.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// Geant
#include "G4NavigationHistory.hh"

// ROOT
#include "TGeoBBox.h"

#ifdef HAVE_GEANT4_UNITS
#define MM_2_CM 1.0
#else
#define MM_2_CM 0.1
#endif


namespace megat {
namespace utils {
dd4hep::xml::Component getNodeByStrAttr(const dd4hep::xml::Handle_t& mother, const std::string& nodeName,
                                        const std::string& attrName, const std::string& attrValue) {
  for (dd4hep::xml::Collection_t xCompColl(mother, nodeName.c_str()); nullptr != xCompColl; ++xCompColl) {
    if (xCompColl.attr<std::string>(attrName.c_str()) == attrValue) {
      return static_cast<dd4hep::xml::Component>(xCompColl);
    }
  }
  // in case there was no xml daughter with matching name
  return dd4hep::xml::Component(nullptr);
}

double getAttrValueWithFallback(const dd4hep::xml::Component& node, const std::string& attrName,
                                const double& defaultValue) {
  if (node.hasAttr(_Unicode(attrName.c_str()))) {
    return node.attr<double>(attrName.c_str());
  } else {
    return defaultValue;
  }
}

uint64_t cellID(const dd4hep::Segmentation& aSeg, const G4Step& aStep, bool aPreStepPoint) {
  dd4hep::sim::Geant4VolumeManager volMgr = dd4hep::sim::Geant4Mapping::instance().volumeManager();
  dd4hep::VolumeID volID = volMgr.volumeID(aStep.GetPreStepPoint()->GetTouchable());
  if (aSeg.isValid()) {
    G4ThreeVector global;
    if (aPreStepPoint) {
      global = aStep.GetPreStepPoint()->GetPosition();
    } else {
      global = 0.5 * (aStep.GetPreStepPoint()->GetPosition() + aStep.GetPostStepPoint()->GetPosition());
    }
    G4ThreeVector local =
        aStep.GetPreStepPoint()->GetTouchable()->GetHistory()->GetTopTransform().TransformPoint(global);
    dd4hep::Position loc(local.x() * MM_2_CM, local.y() * MM_2_CM, local.z() * MM_2_CM);
    dd4hep::Position glob(global.x() * MM_2_CM, global.y() * MM_2_CM, global.z() * MM_2_CM);
    dd4hep::VolumeID cID = aSeg.cellID(loc, glob, volID);
    return cID;
  }
  return volID;
}

std::vector<std::vector<uint>> combinations(int N, int K) {
  std::vector<std::vector<uint>> indexes;
  std::string bitmask(K, 1);  // K leading 1's
  bitmask.resize(N, 0);       // N-K trailing 0's
  // permute bitmask
  do {
    std::vector<uint> tmp;
    for (int i = 0; i < N; ++i) {  // [0..N-1] integers
      if (bitmask[i]) {
        tmp.push_back(i);
      }
    }
    indexes.push_back(tmp);
  } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
  return indexes;
}

std::vector<std::vector<int>> permutations(int K) {
  std::vector<std::vector<int>> indexes;
  int N = pow(2, K);  // number of permutations with repetition of 2 numbers (0,1)
  for (int i = 0; i < N; i++) {
    // permutation = binary representation of i
    std::vector<int> tmp;
    tmp.assign(K, 0);
    uint res = i;
    // dec -> bin
    for (int j = 0; j < K; j++) {
      tmp[K - 1 - j] = -1 + 2 * (res % 2);
      res = floor(res / 2);
    }
    indexes.push_back(tmp);
  }
  return indexes;
}

int cyclicNeighbour(int aCyclicId, std::pair<int, int> aFieldExtremes) {
  if (aCyclicId < aFieldExtremes.first) {
    return aFieldExtremes.second + aCyclicId;
  } else if (aCyclicId > aFieldExtremes.second) {
    return aCyclicId % (aFieldExtremes.second + 1);
  }
  return aCyclicId;
}

std::vector<uint64_t> neighbours(const dd4hep::DDSegmentation::BitFieldCoder& aDecoder,
                                 const std::vector<std::string>& aFieldNames,
                                 const std::vector<std::pair<int, int>>& aFieldExtremes, uint64_t aCellId,
                                 const std::vector<bool>& aFieldCyclic, bool aDiagonal) {
  std::vector<uint64_t> neighbours;
  dd4hep::DDSegmentation::CellID cID = aCellId;
  for (uint itField = 0; itField < aFieldNames.size(); itField++) {
    const auto& field = aFieldNames[itField];
    dd4hep::DDSegmentation::CellID id = aDecoder.get(cID,field);
    if (aFieldCyclic[itField]) {
      aDecoder[field].set(cID, cyclicNeighbour(id - 1, aFieldExtremes[itField]));
      neighbours.emplace_back(cID);
      aDecoder[field].set(cID, cyclicNeighbour(id + 1, aFieldExtremes[itField]));
      neighbours.emplace_back(cID);
    } else {
      if (id > aFieldExtremes[itField].first) {
        aDecoder.set(cID, field, id - 1);
        neighbours.emplace_back(cID);
      }
      if (id < aFieldExtremes[itField].second) {
        aDecoder.set(cID, field, id + 1);
        neighbours.emplace_back(cID);
      }
    }
    aDecoder.set(cID, field, id);
  }
  if (aDiagonal) {
    std::vector<int> fieldIds;  // initial IDs
    fieldIds.assign(aFieldNames.size(), 0);
    // for each field get current Id
    for (uint iField = 0; iField < aFieldNames.size(); iField++) {
      const auto& field = aFieldNames[iField];
      fieldIds[iField] = aDecoder.get(cID, field);
    }
    for (uint iLength = aFieldNames.size(); iLength > 1; iLength--) {
      // get all combinations for a given length
      const auto& indexes = combinations(aFieldNames.size(), iLength);
      for (uint iComb = 0; iComb < indexes.size(); iComb++) {
        // for current combination get all permutations of +- 1 operation on IDs
        const auto& calculation = permutations(iLength);
        // do the increase/decrease of bitfield
        for (uint iCalc = 0; iCalc < calculation.size(); iCalc++) {
          // set new Ids for each field combination
          bool add = true;
          for (uint iField = 0; iField < indexes[iComb].size(); iField++) {
            if (aFieldCyclic[indexes[iComb][iField]]) {
              aDecoder[aFieldNames[indexes[iComb][iField]]].set(cID, cyclicNeighbour(fieldIds[indexes[iComb][iField]] + calculation[iCalc][iField],
										     aFieldExtremes[indexes[iComb][iField]]) );
            } else if ((calculation[iCalc][iField] > 0 &&
                        fieldIds[indexes[iComb][iField]] < aFieldExtremes[indexes[iComb][iField]].second) ||
                       (calculation[iCalc][iField] < 0 &&
                        fieldIds[indexes[iComb][iField]] > aFieldExtremes[indexes[iComb][iField]].first)) {
              aDecoder[aFieldNames[indexes[iComb][iField]]].set(cID, fieldIds[indexes[iComb][iField]] + calculation[iCalc][iField]);
            } else {
              add = false;
            }
          }
          // add new cellId to neighbours (unless it's beyond extrema)
          if (add) {
            neighbours.emplace_back(cID);
          }
          // reset ids
          for (uint iField = 0; iField < indexes[iComb].size(); iField++) {
            aDecoder[aFieldNames[indexes[iComb][iField]]].set(cID, fieldIds[indexes[iComb][iField]]);
          }
        }
      }
    }
  }
  return neighbours;
}

std::vector<std::pair<int, int>> bitfieldExtremes(const dd4hep::DDSegmentation::BitFieldCoder& aDecoder,
                                                  const std::vector<std::string>& aFieldNames) {
  std::vector<std::pair<int, int>> extremes;
  int width = 0;
  for (const auto& field : aFieldNames) {
    width = aDecoder[field].width();
    if (aDecoder[field].isSigned()) {
      extremes.emplace_back(std::make_pair(-(1 << (width - 1)), (1 << (width - 1)) - 1));
    } else {
      extremes.emplace_back(std::make_pair(0, (1 << width) - 1));
    }
  }
  return extremes;
}

CLHEP::Hep3Vector envelopeDimensions(uint64_t aVolumeId) {
  dd4hep::VolumeManager volMgr = dd4hep::Detector::getInstance().volumeManager();
  auto pvol = volMgr.lookupVolumePlacement(aVolumeId);
  auto solid = pvol.volume().solid();
  // get the envelope of the shape
  TGeoBBox* box = (dynamic_cast<TGeoBBox*>(solid.ptr()));
  // get half-widths
  return CLHEP::Hep3Vector(box->GetDX(), box->GetDY(), box->GetDZ());
}

std::array<uint, 2> numberOfCells(uint64_t aVolumeId, const dd4hep::DDSegmentation::CartesianGridXY& aSeg) {
  // // get half-widths
  auto halfSizes = envelopeDimensions(aVolumeId);
  // get segmentation cell widths
  double xCellSize = aSeg.gridSizeX();
  double yCellSize = aSeg.gridSizeY();
  // calculate number of cells, the middle cell is centred at 0 (no offset)
  uint cellsX = ceil((halfSizes.x() - xCellSize / 2.) / xCellSize) * 2 + 1;
  uint cellsY = ceil((halfSizes.y() - yCellSize / 2.) / yCellSize) * 2 + 1;
  return {cellsX, cellsY};
}

std::array<uint, 3> numberOfCells(uint64_t aVolumeId, const dd4hep::DDSegmentation::CartesianGridXYZ& aSeg) {
  // // get half-widths
  auto halfSizes = envelopeDimensions(aVolumeId);
  // get segmentation cell widths
  double xCellSize = aSeg.gridSizeX();
  double yCellSize = aSeg.gridSizeY();
  double zCellSize = aSeg.gridSizeZ();
  // calculate number of cells, the middle cell is centred at 0 (no offset)
  uint cellsX = ceil((halfSizes.x() - xCellSize / 2.) / xCellSize) * 2 + 1;
  uint cellsY = ceil((halfSizes.y() - yCellSize / 2.) / yCellSize) * 2 + 1;
  uint cellsZ = ceil((halfSizes.z() - zCellSize / 2.) / zCellSize) * 2 + 1;
  return {cellsX, cellsY, cellsZ};
}

unsigned int countPlacedVolumes(TGeoVolume* aHighestVolume, const std::string& aMatchName) {
  int numberOfPlacedVolumes = 0;
  TGeoNode* node;
  TGeoIterator next(aHighestVolume);
  while ((node = next())) {
    std::string currentNodeName = node->GetName();
    if (currentNodeName.find(aMatchName) != std::string::npos) {
      ++numberOfPlacedVolumes;
    }
  }
  return numberOfPlacedVolumes;
}
}
}
