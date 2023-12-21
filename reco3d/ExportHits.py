from podio.root_io import Reader
from RecoUtils import decodeRawHits
import numpy as np
import pandas as pd

reader = Reader("/data/home/lixp/src/digi/options/simple_digi.root")
events = reader.get("events")

n = 50

lens = np.zeros(n, dtype="int")

event = events[0]
hits = event.get("TpcHits")
dhits =  decodeRawHits(hits)
_raws = dhits.raw_hits
_times = dhits.raw_times
_layers = dhits.layers
_edeps = np.array(hits.eDep())
lens[0] = len(hits)

for i in range(1, n):
    event = events[i]
    hits = event.get("TpcHits")
    dhits =  decodeRawHits(hits)
    raws = dhits.raw_hits
    times = dhits.raw_times
    layers = dhits.layers
    edeps = np.array(hits.eDep())
    _raws = np.concatenate((_raws, raws))
    _times = np.concatenate((_times, times))
    _layers = np.concatenate((_layers, layers))
    _edeps = np.concatenate((_edeps, edeps))
    lens[i] = len(hits)

res = np.stack((
    _raws[:,0], # x
    _raws[:,1], # y
    _times,     # time
    _edeps,     # edep
    _layers,    # layer
    np.repeat(np.arange(n), lens) # evnet no.
), axis=1)

np.savetxt("simple_digi.csv", res, 
           fmt = "%g, %g, %g, %g, %d, %d",
           header = "x, y, time, edep, layer, eventNo.",
           comments="")

