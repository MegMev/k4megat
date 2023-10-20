
import os
import dd4hep
import logging

logger = logging.getLogger()
logger.setLevel(logging.ERROR)

base_dir = os.environ.get('MEGAT_ROOT','')
if not base_dir:
    logger.error('check megat environment')
else:
    logger.info(base_dir)

def find_volume(path, mode='de'):
    geom_file = os.path.join(base_dir, 'geometry/compact/Megat.xml')
    desc = dd4hep.Detector.getInstance()
    desc.fromXML(geom_file)

    if mode == 'de':
        dd4hep.detail.tools.findElement(desc, path)
    else:
        pv=dd4hep.detail.tools.findNode(desc.world().placement(), path)
        if not pv.isValid():
            logger.error("can't find the volume")

    dd4hep.Detector.destroyInstance()
    
