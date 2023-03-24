import os
import sys

import dd4hep
from dd4hep import Detector, dd4hep_logger
from megat import loadGeometry

logger = dd4hep_logger('megat')
logger.setPrintLevel(dd4hep.ERROR)

megat_path = os.environ['MEGAT_ROOT']

xmlGeom=[f'{megat_path}/geometry/compact/Megat.xml',
         f'{megat_path}/geometry/compact/TPC_readout.xml'
         ]

def get_idspec(tag):
    det = Detector.getInstance(tag)
    sd = det.sensitiveDetector('TPC')
    return sd.idSpec()

det1 = loadGeometry(xmlGeom, 'TpcStripHits', 'strip')
det1_1 = Detector.getInstance('strip')
if det1 == det1_1:
    print(det1)
    print(det1_1)

det2 = loadGeometry(xmlGeom, 'TpcPixelHits', 'pixel')

id1=get_idspec('strip')
id2=get_idspec('pixel')
print(f'det1-strip: {id1.toString()} {id1.fieldDescription()}')
print(f'det2-pixel: {id2.toString()} {id2.fieldDescription()}')
