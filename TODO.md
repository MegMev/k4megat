
# Table of Contents

1.  [Software Management](#orgdab9f3d)
    1.  [Spack-based dependence management](#orgeb3501c)
    2.  [Continuous integration](#org716c76c)
    3.  [Unit test framework](#org132a079)
    4.  [Version mechanism](#org0dc2b11)
        1.  [save version id/sting as a  metadata tree branch.](#orga0b3477)
    5.  [Documentation](#orgafd4937)
2.  [Simulation](#org6861d18)
    1.  [DDG4 as the back-end](#orge43c79d)
    2.  [TPC digi implementation](#org1e49dd4)
        1.  [try to migrate REST implementation](#org449e121)
    3.  [Generator](#org6770f4a)
    4.  [Garfield integration](#orgbfef8bc)
    5.  [Physics list checking](#orgf0e76d0)
        1.  [Compton scattering](#org0bd2e45)
    6.  [CAD model error in USTC key4hep stack](#org2faacc5)
3.  [Analysis](#org2b964fe)
    1.  [Spark & RDataFrame mini-framework](#org6e77e93)
4.  [Geometry model](#org3bed853)
    1.  [General](#org0b01283)
        1.  [Check whether alignment can be applied on assembly volume](#orgfee914a)
    2.  [CZT calo](#org7871cad)
        1.  [add DetElement to module sensor](#org5c839d0)
5.  [Visualization](#org8fee6f6)
    1.  [a demo site](#org9a7a0e1)
    2.  [Choose a technology direction](#org4f9e2e7)
    3.  [adapt geoWebDisplay to show more nodes by default](#org5c34f39)


<a id="orgdab9f3d"></a>

# Software Management


<a id="orgeb3501c"></a>

## Spack-based dependence management


<a id="org716c76c"></a>

## Continuous integration


<a id="org132a079"></a>

## Unit test framework


<a id="org0dc2b11"></a>

## Version mechanism

Semantic Versioning: <https://semver.org/>


<a id="orga0b3477"></a>

### save version id/sting as a  metadata tree branch.

Needed for tracable data production.


<a id="orgafd4937"></a>

## DONE Documentation

-   online publish
-   cross-reference:
    -   edm4hep
    -   dd4hep
    -   Gaudi
    -   k4megat


<a id="org6861d18"></a>

# Simulation


<a id="orge43c79d"></a>

## DDG4 as the back-end


<a id="org1e49dd4"></a>

## TPC digi implementation


<a id="org449e121"></a>

### try to migrate REST implementation

1.  DONE consider append extra metadata about tpc readout structure as dd4hep data extension

    -   see /home/yong/src/physics/key4hep/lcgeo/detector/tracker/TPC10\_geo.cpp for adding the info
    -   see /home/yong/src/physics/key4hep/DD4hep/DDRec/src/gear/createGearForILD.cpp for receivint the info

2.  DONE customize a volume surface for TPC readout PCB


<a id="org6770f4a"></a>

## Generator


<a id="orgbfef8bc"></a>

## Garfield integration

[the most sophisticated solution](https://garfieldpp.web.cern.ch/garfieldpp/examples/geant4-interface/)

[a examlpe](https://github.com/nimanthaperera/GEANT4_garfield_integration)


<a id="orgf0e76d0"></a>

## Physics list checking


<a id="org0bd2e45"></a>

### Compton scattering

1.  Doppler broadening

    -   **[check this example in G4](file:///home/yong/src/physics/geant4/examples/extended/electromagnetic/TestEm14/src/PhysListEmPenelope.cc):** G4PenelopeComptonModel
    -   **G4LowEnergyCompton:** <https://www.ge.infn.it/geant4/talks/nss2008/merged_mg.pdf>
    -   [Influence of Doppler broadening model accuracy in Compton camera list-mode MLEM reconstruction](https://hal.science/hal-03481082/file/manuscrit.pdf)


<a id="org2faacc5"></a>

## CAD model error in USTC key4hep stack

-   Disable VecGeom support may solve the issue.
-   need a new spack environment


<a id="org2b964fe"></a>

# Analysis


<a id="org6e77e93"></a>

## Spark & RDataFrame mini-framework


<a id="org3bed853"></a>

# Geometry model


<a id="org0b01283"></a>

## General


<a id="orgfee914a"></a>

### Check whether alignment can be applied on assembly volume


<a id="org7871cad"></a>

## CZT calo


<a id="org5c839d0"></a>

### DONE add DetElement to module sensor

see [EIC's NPDet impl](file:///home/yong/src/physics/eic/NPDet/src/detectors/trackers/src/GenericSiliconTrackerBarrel_geo.cpp), in which sensor is daughter of module, and the module is placed in layered array.
A DetElement is created for both the module and the sensor at each module placement.
While module\_DE's placement is the individual placement, all module\_sensor\_DE's placement share the same
placement (which is created once [before at this line](file:///home/yong/src/physics/eic/NPDet/src/detectors/trackers/src/GenericSiliconTrackerBarrel_geo.cpp)).
This configuration is exactly what CZT calo needed.


<a id="org8fee6f6"></a>

# Visualization


<a id="org9a7a0e1"></a>

## a demo site

<https://linev.github.io/>


<a id="org4f9e2e7"></a>

## Choose a technology direction

from easy to difficult:

1.  TEVE
2.  EVE-7
3.  PHOENIX
4.  game engines
    -   Unreal
    -   Unity
    -   Godbot
5.  Qt3D


<a id="org5c34f39"></a>

## DONE adapt geoWebDisplay to show more nodes by default

-   **REveGeomViewer::SetLimits:** DDEve/root7/WebDisplay.cpp:81

