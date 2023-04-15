
# Table of Contents

1.  [Software Management](#org09f5394)
    1.  [Spack-based dependence management](#org4b52397)
    2.  [Continuous integration](#org7a0b238)
    3.  [Unit test framework](#org5d0e415)
    4.  [Version mechanism](#orga6e2353)
        1.  [save version id/sting as a  metadata tree branch.](#org89e753f)
    5.  [Fine-tune CMake Config file for downstream projects using](#org3fb36b9)
    6.  [Documentation](#orge1ec8c8)
2.  [Event Store](#org5cd4b60)
    1.  [auto-fetch collection metadata so that previous CellIDEncodingString is saved](#orge56c0aa)
3.  [Simulation](#org31baf74)
    1.  [DDG4 as the back-end](#org87c9604)
    2.  [TPC digi implementation](#orgb3c577f)
        1.  [try to migrate REST implementation](#org8198c87)
    3.  [Generator](#orgd109685)
    4.  [Garfield integration](#orgc4df6f9)
    5.  [Physics list checking](#org89f4151)
        1.  [Compton scattering](#orgab2f7a3)
    6.  [CAD model error in USTC key4hep stack](#org8064fb9)
4.  [Analysis](#org62f84a6)
    1.  [mgana](#orgcf4a256)
        1.  [plugin manager <code>[1/5]</code>](#orga2e6a50)
        2.  [update subcommand](#orgaa8f565)
        3.  [plot subcommand](#org235fae1)
5.  [Geometry model](#org12f9282)
    1.  [General](#orgacda40c)
        1.  [Check whether alignment can be applied on assembly volume](#orgdcec4ef)
    2.  [CZT calo](#org38e1681)
        1.  [add DetElement to module sensor](#orgf219393)
6.  [Visualization](#org55456a7)
    1.  [Choose a technology direction <code>[1/3]</code>](#orgc99dba2)
    2.  [adapt geoWebDisplay to show more nodes by default](#org4a3e7a8)


<a id="org09f5394"></a>

# Software Management


<a id="org4b52397"></a>

## Spack-based dependence management


<a id="org7a0b238"></a>

## Continuous integration


<a id="org5d0e415"></a>

## Unit test framework


<a id="orga6e2353"></a>

## Version mechanism

Semantic Versioning: <https://semver.org/>


<a id="org89e753f"></a>

### save version id/sting as a  metadata tree branch.

Needed for tracable data production.


<a id="org3fb36b9"></a>

## Fine-tune CMake Config file for downstream projects using


<a id="orge1ec8c8"></a>

## DONE Documentation

-   online publish
-   cross-reference:
    -   edm4hep
    -   dd4hep
    -   Gaudi
    -   k4megat


<a id="org5cd4b60"></a>

# Event Store


<a id="orge56c0aa"></a>

## auto-fetch collection metadata so that previous CellIDEncodingString is saved


<a id="org31baf74"></a>

# Simulation


<a id="org87c9604"></a>

## DDG4 as the back-end


<a id="orgb3c577f"></a>

## TPC digi implementation


<a id="org8198c87"></a>

### try to migrate REST implementation

1.  DONE consider append extra metadata about tpc readout structure as dd4hep data extension

    -   see /home/yong/src/physics/key4hep/lcgeo/detector/tracker/TPC10\_geo.cpp for adding the info
    -   see /home/yong/src/physics/key4hep/DD4hep/DDRec/src/gear/createGearForILD.cpp for receivint the info

2.  DONE customize a volume surface for TPC readout PCB


<a id="orgd109685"></a>

## Generator


<a id="orgc4df6f9"></a>

## Garfield integration

[the most sophisticated solution](https://garfieldpp.web.cern.ch/garfieldpp/examples/geant4-interface/)

[a examlpe](https://github.com/nimanthaperera/GEANT4_garfield_integration)


<a id="org89f4151"></a>

## Physics list checking


<a id="orgab2f7a3"></a>

### Compton scattering

1.  Doppler broadening

    -   **[check this example in G4](file:///home/yong/src/physics/geant4/examples/extended/electromagnetic/TestEm14/src/PhysListEmPenelope.cc):** G4PenelopeComptonModel
    -   **G4LowEnergyCompton:** <https://www.ge.infn.it/geant4/talks/nss2008/merged_mg.pdf>
    -   [Influence of Doppler broadening model accuracy in Compton camera list-mode MLEM reconstruction](https://hal.science/hal-03481082/file/manuscrit.pdf)


<a id="org8064fb9"></a>

## CAD model error in USTC key4hep stack

-   Disable VecGeom support may solve the issue.
-   need a new spack environment


<a id="org62f84a6"></a>

# Analysis


<a id="orgcf4a256"></a>

## mgana


<a id="orga2e6a50"></a>

### plugin manager <code>[1/5]</code>

1.  [X] dynamic loading in python: importlib.import\_module
2.  [ ] central registry
3.  [ ] package manifest
4.  [ ] dependency resolving and lock file
5.  [ ] CMakesList.txt dual function: as independent project or subdirectory package


<a id="orgaa8f565"></a>

### update subcommand


<a id="org235fae1"></a>

### plot subcommand


<a id="org12f9282"></a>

# Geometry model


<a id="orgacda40c"></a>

## General


<a id="orgdcec4ef"></a>

### Check whether alignment can be applied on assembly volume


<a id="org38e1681"></a>

## CZT calo


<a id="orgf219393"></a>

### DONE add DetElement to module sensor

see [EIC's NPDet impl](file:///home/yong/src/physics/eic/NPDet/src/detectors/trackers/src/GenericSiliconTrackerBarrel_geo.cpp), in which sensor is daughter of module, and the module is placed in layered array.
A DetElement is created for both the module and the sensor at each module placement.
While module\_DE's placement is the individual placement, all module\_sensor\_DE's placement share the same
placement (which is created once [before at this line](file:///home/yong/src/physics/eic/NPDet/src/detectors/trackers/src/GenericSiliconTrackerBarrel_geo.cpp)).
This configuration is exactly what CZT calo needed.


<a id="org55456a7"></a>

# Visualization


<a id="orgc99dba2"></a>

## Choose a technology direction <code>[1/3]</code>

from easy to difficult:

1.  [ ] EVE7
2.  [X] PHOENIX
3.  [ ] game engines
    -   Unreal
    -   Unity
    -   Godbot


<a id="org4a3e7a8"></a>

## DONE adapt geoWebDisplay to show more nodes by default

-   **REveGeomViewer::SetLimits:** DDEve/root7/WebDisplay.cpp:81

