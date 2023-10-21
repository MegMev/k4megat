
import os
import dd4hep

base_dir = os.environ.get('MEGAT_PATH','')
if not base_dir:
    dd4hep.logger.error('check megat environment')
else:
    dd4hep.logger.info(base_dir)

def find_volume(path, mode='de'):
    geom_file = os.path.join(base_dir, 'geometry/compact/Megat.xml')
    desc = dd4hep.Detector.getInstance()
    desc.fromXML(geom_file)

    try:
        if mode == 'de':
            dd4hep.detail.tools.findElement(desc, path)
        else:
            search = dd4hep.detail.tools.findNode(desc.world().placement(), path)
    except Exception as X:
        dd4hep.logger.error("can't find the volume")

    
