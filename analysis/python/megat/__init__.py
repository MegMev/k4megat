from __future__ import absolute_import, unicode_literals

from .detail import *

## pre-load the default geometry with different readout patterns
megat_root = os.environ['MEGAT_ROOT']
geometry_xml = os.path.join(megat_root, 'geometry/compact/Megat.xml')
readout_xml = os.path.join(megat_root, 'geometry/compact/TPC_readout.xml')

id_converter = {}
tpc_decoder = {}

# cartesian strip
loadGeometry([geometry_xml, readout_xml], 'TpcCartesianStripHits','cartesian_strip')
id_converter['cartesian'] = IdConverter('cartesian_strip')
tpc_decoder['cartesian'] = id_converter['cartesian'].decoder('TPC')
czt_decoder = id_converter['cartesian'].decoder('Calorimeter')


# diagonal strip
loadGeometry([geometry_xml, readout_xml], 'TpcDiagonalStripHits','diagonal_strip')
id_converter['diagonal'] = IdConverter('diagonal_strip')
tpc_decoder['diagonal'] = id_converter['diagonal'].decoder('TPC')

# pixel
loadGeometry([geometry_xml, readout_xml], 'TpcPixelHits','pixel')
id_converter['pixel'] = IdConverter('pixel')
tpc_decoder['pixel'] = id_converter['pixel'].decoder('TPC')
