
# Table of Contents

1.  [Software Management](#org3e27fc1)
    1.  [Spack-based dependence management](#org1fb0d3c)
    2.  [Continuous integration](#org17bbab7)
    3.  [Unit test framework](#org27f0186)
    4.  [Version mechanism](#org7587054)
        1.  [save version id/sting as a  metadata tree branch.](#org88a4dd6)
    5.  [Documentation](#org24f59c8)
2.  [Event Store](#orgcb7f268)
    1.  [auto-fetch collection metadata so that previous CellIDEncodingString is saved](#org8d0f906)
3.  [Simulation](#org76eaf46)
    1.  [DDG4 as the back-end](#orgf8c6505)
    2.  [TPC digi implementation](#orgeea192e)
        1.  [try to migrate REST implementation](#org9ae7c86)
    3.  [Generator](#orge8cda74)
    4.  [Garfield integration](#org96ad3b1)
    5.  [Physics list checking](#org9655849)
        1.  [Compton scattering](#orga88d00b)
    6.  [CAD model error in USTC key4hep stack](#org8b00ba6)
4.  [Analysis](#org84a1e2e)
    1.  [Spark & RDataFrame mini-framework](#orgeec5333)
5.  [Geometry model](#org285882e)
    1.  [General](#orgc4ffab1)
        1.  [Check whether alignment can be applied on assembly volume](#org58578a1)
    2.  [CZT calo](#org8f2e196)
        1.  [add DetElement to module sensor](#orgeb28e82)
6.  [Visualization](#org0518792)
    1.  [Choose a technology direction <code>[1/3]</code>](#org7a23f17)
    2.  [adapt geoWebDisplay to show more nodes by default](#org1737f5f)


<a id="org3e27fc1"></a>

# Software Management


<a id="org1fb0d3c"></a>

## Spack-based dependence management


<a id="org17bbab7"></a>

## Continuous integration


<a id="org27f0186"></a>

## Unit test framework


<a id="org7587054"></a>

## Version mechanism

Semantic Versioning: <https://semver.org/>


<a id="org88a4dd6"></a>

### save version id/sting as a  metadata tree branch.

Needed for tracable data production.


<a id="org24f59c8"></a>

## DONE Documentation

-   online publish
-   cross-reference:
    -   edm4hep
    -   dd4hep
    -   Gaudi
    -   k4megat


<a id="orgcb7f268"></a>

# Event Store


<a id="org8d0f906"></a>

## auto-fetch collection metadata so that previous CellIDEncodingString is saved


<a id="org76eaf46"></a>

# Simulation


<a id="orgf8c6505"></a>

## DDG4 as the back-end


<a id="orgeea192e"></a>

## TPC digi implementation


<a id="org9ae7c86"></a>

### try to migrate REST implementation

1.  DONE consider append extra metadata about tpc readout structure as dd4hep data extension

    -   see /home/yong/src/physics/key4hep/lcgeo/detector/tracker/TPC10\_geo.cpp for adding the info
    -   see /home/yong/src/physics/key4hep/DD4hep/DDRec/src/gear/createGearForILD.cpp for receivint the info

2.  DONE customize a volume surface for TPC readout PCB


<a id="orge8cda74"></a>

## Generator


<a id="org96ad3b1"></a>

## Garfield integration

[the most sophisticated solution](https://garfieldpp.web.cern.ch/garfieldpp/examples/geant4-interface/)

[a examlpe](https://github.com/nimanthaperera/GEANT4_garfield_integration)


<a id="org9655849"></a>

## Physics list checking


<a id="orga88d00b"></a>

### Compton scattering

1.  Doppler broadening

    -   **[check this example in G4](file:///home/yong/src/physics/geant4/examples/extended/electromagnetic/TestEm14/src/PhysListEmPenelope.cc):** G4PenelopeComptonModel
    -   **G4LowEnergyCompton:** <https://www.ge.infn.it/geant4/talks/nss2008/merged_mg.pdf>
    -   [Influence of Doppler broadening model accuracy in Compton camera list-mode MLEM reconstruction](https://hal.science/hal-03481082/file/manuscrit.pdf)


<a id="org8b00ba6"></a>

## CAD model error in USTC key4hep stack

-   Disable VecGeom support may solve the issue.
-   need a new spack environment


<a id="org84a1e2e"></a>

# Analysis


<a id="orgeec5333"></a>

## Spark & RDataFrame mini-framework


<a id="org285882e"></a>

# Geometry model


<a id="orgc4ffab1"></a>

## General


<a id="org58578a1"></a>

### Check whether alignment can be applied on assembly volume


<a id="org8f2e196"></a>

## CZT calo


<a id="orgeb28e82"></a>

### DONE add DetElement to module sensor

see [EIC's NPDet impl](file:///home/yong/src/physics/eic/NPDet/src/detectors/trackers/src/GenericSiliconTrackerBarrel_geo.cpp), in which sensor is daughter of module, and the module is placed in layered array.
A DetElement is created for both the module and the sensor at each module placement.
While module\_DE's placement is the individual placement, all module\_sensor\_DE's placement share the same
placement (which is created once [before at this line](file:///home/yong/src/physics/eic/NPDet/src/detectors/trackers/src/GenericSiliconTrackerBarrel_geo.cpp)).
This configuration is exactly what CZT calo needed.


<a id="org0518792"></a>

# Visualization


<a id="org7a23f17"></a>

## Choose a technology direction <code>[1/3]</code>

from easy to difficult:

1.  [ ] EVE7
2.  [X] PHOENIX
3.  [ ] game engines
    -   Unreal
    -   Unity
    -   Godbot


<a id="org1737f5f"></a>

## DONE adapt geoWebDisplay to show more nodes by default

-   **REveGeomViewer::SetLimits:** DDEve/root7/WebDisplay.cpp:81

