import numpy as np
import pandas as pd
from RecoUtils import tidyHits, CellIDLayerMapper, TimeZMapper, vec3d2mat
from podio.root_io import Reader
from plotnine.ggplot import ggplot
from plotnine import aes, facet_wrap, geom_point, theme
from spline.SplineFit import SplineFit

compton_event_path = "~/src/reco3d/compton_digi.root"
reader = Reader(compton_event_path)

events = reader.get("events")

event = events[42]
hits = event.get("TpcWaveformHits")

df = tidyHits(hits, "RawTimeSeries")


fit = SplineFit(df[df["layer"] == 0][["x", "z"]].to_numpy(), np.zeros(1))
fit.build_clusters(5000)

fit.order()
