from __future__ import absolute_import, unicode_literals

from .detail import *

### pre-load the default geometry with different readout patterns
import os
megat_root = os.environ['MEGAT_ROOT']
geometry_xml = os.path.join(megat_root, 'geometry/compact/Megat.xml')
readout_xml = os.path.join(megat_root, 'geometry/compact/TPC_readout.xml')

### converter and decoder mapping with the readout name as key (tpc or czt)
# [todo] I know there will be duplicate geometry in memory, but it's more flexible
__id_converter = {}
__tpc_decoder = {}
__czt_decoder = {}

def getIdConverter(readout='TpcDiagonalStripHits'):
    if not readout in __id_converter:
        loadGeometry([geometry_xml, readout_xml], readout)
        __id_converter[readout] = IdConverter(readout)
    return __id_converter[readout]

def getTpcDecoder(readout='TpcDiagonalStripHits'):
    if not readout in __tpc_decoder:
        _id_converter = getIdConverter(readout)
        __tpc_decoder[readout] = _id_converter.decoder('TPC')
    return __tpc_decoder[readout]

def getCztDecoder(readout='CztHits'):
    if not readout in __czt_decoder:
        _id_converter = getIdConverter(readout)
        __czt_decoder[readout] = _id_converter.decoder('Calorimeter')
    return __czt_decoder[readout]
