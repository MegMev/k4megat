
# Table of Contents

1.  [Software Management](#org4749eb0)
    1.  [Spack-based dependence management](#org57f2e07)
    2.  [Continuous integration](#org85e477e)
    3.  [Unit test framework](#org56afbb3)
    4.  [Version mechanism](#org81447d7)
        1.  [save version id/sting as a  metadata tree branch.](#org3d9422e)
    5.  [Fine-tune CMake Config file for downstream projects using](#orgd6cac79)
    6.  [Documentation](#org6317785)
2.  [Event Store](#orge06e750)
    1.  [auto-fetch collection metadata so that previous CellIDEncodingString is saved](#org0f46f57)
3.  [Simulation](#orgd594a6d)
    1.  [DDG4 as the back-end](#org86909be)
    2.  [TPC digi implementation](#org2548a78)
        1.  [try to migrate REST implementation](#org1aef4a8)
    3.  [Generator](#org45d37a5)
    4.  [Garfield integration](#org2e8789f)
    5.  [Physics list checking](#org9610883)
        1.  [Compton scattering](#orgbfe6181)
    6.  [CAD model error in USTC key4hep stack](#orge4a8540)
4.  [Analysis](#org430cf70)
    1.  [Spark & RDataFrame mini-framework](#org78b50a2)
5.  [Geometry model](#orgc5d6a5f)
    1.  [General](#orgf4a27d8)
        1.  [Check whether alignment can be applied on assembly volume](#org9e092c2)
    2.  [CZT calo](#orgf072065)
        1.  [add DetElement to module sensor](#org63ef19a)
6.  [Visualization](#org4760905)
    1.  [Choose a technology direction <code>[1/3]</code>](#org91d18c0)
    2.  [adapt geoWebDisplay to show more nodes by default](#orgaf81e81)


<a id="org4749eb0"></a>

# Software Management


<a id="org57f2e07"></a>

## Spack-based dependence management


<a id="org85e477e"></a>

## Continuous integration


<a id="org56afbb3"></a>

## Unit test framework


<a id="org81447d7"></a>

## Version mechanism

Semantic Versioning: <https://semver.org/>


<a id="org3d9422e"></a>

### save version id/sting as a  metadata tree branch.

Needed for tracable data production.


<a id="orgd6cac79"></a>

## Fine-tune CMake Config file for downstream projects using


<a id="org6317785"></a>

## DONE Documentation

-   online publish
-   cross-reference:
    -   edm4hep
    -   dd4hep
    -   Gaudi
    -   k4megat


<a id="orge06e750"></a>

# Event Store


<a id="org0f46f57"></a>

## auto-fetch collection metadata so that previous CellIDEncodingString is saved


<a id="orgd594a6d"></a>

# Simulation


<a id="org86909be"></a>

## DDG4 as the back-end


<a id="org2548a78"></a>

## TPC digi implementation


<a id="org1aef4a8"></a>

### try to migrate REST implementation

1.  DONE consider append extra metadata about tpc readout structure as dd4hep data extension

    -   see /home/yong/src/physics/key4hep/lcgeo/detector/tracker/TPC10\_geo.cpp for adding the info
    -   see /home/yong/src/physics/key4hep/DD4hep/DDRec/src/gear/createGearForILD.cpp for receivint the info

2.  DONE customize a volume surface for TPC readout PCB


<a id="org45d37a5"></a>

## Generator


<a id="org2e8789f"></a>

## Garfield integration

[the most sophisticated solution](https://garfieldpp.web.cern.ch/garfieldpp/examples/geant4-interface/)

[a examlpe](https://github.com/nimanthaperera/GEANT4_garfield_integration)


<a id="org9610883"></a>

## Physics list checking


<a id="orgbfe6181"></a>

### Compton scattering

1.  Doppler broadening

    -   **[check this example in G4](file:///home/yong/src/physics/geant4/examples/extended/electromagnetic/TestEm14/src/PhysListEmPenelope.cc):** G4PenelopeComptonModel
    -   **G4LowEnergyCompton:** <https://www.ge.infn.it/geant4/talks/nss2008/merged_mg.pdf>
    -   [Influence of Doppler broadening model accuracy in Compton camera list-mode MLEM reconstruction](https://hal.science/hal-03481082/file/manuscrit.pdf)


<a id="orge4a8540"></a>

## CAD model error in USTC key4hep stack

-   Disable VecGeom support may solve the issue.
-   need a new spack environment


<a id="org430cf70"></a>

# Analysis


<a id="org78b50a2"></a>

## Spark & RDataFrame mini-framework


<a id="orgc5d6a5f"></a>

# Geometry model


<a id="orgf4a27d8"></a>

## General


<a id="org9e092c2"></a>

### Check whether alignment can be applied on assembly volume


<a id="orgf072065"></a>

## CZT calo


<a id="org63ef19a"></a>

### DONE add DetElement to module sensor

see [EIC's NPDet impl](file:///home/yong/src/physics/eic/NPDet/src/detectors/trackers/src/GenericSiliconTrackerBarrel_geo.cpp), in which sensor is daughter of module, and the module is placed in layered array.
A DetElement is created for both the module and the sensor at each module placement.
While module\_DE's placement is the individual placement, all module\_sensor\_DE's placement share the same
placement (which is created once [before at this line](file:///home/yong/src/physics/eic/NPDet/src/detectors/trackers/src/GenericSiliconTrackerBarrel_geo.cpp)).
This configuration is exactly what CZT calo needed.


<a id="org4760905"></a>

# Visualization


<a id="org91d18c0"></a>

## Choose a technology direction <code>[1/3]</code>

from easy to difficult:

1.  [ ] EVE7
2.  [X] PHOENIX
3.  [ ] game engines
    -   Unreal
    -   Unity
    -   Godbot


<a id="orgaf81e81"></a>

## DONE adapt geoWebDisplay to show more nodes by default

-   **REveGeomViewer::SetLimits:** DDEve/root7/WebDisplay.cpp:81

