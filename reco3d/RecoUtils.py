import numpy as np
import os

def clusterCenter(X: np.ndarray, labels: np.ndarray) -> np.ndarray:
    """
    Compute the center of each cluster from DBSCAN.
    Distinct points whose label is -1 will be left as is.

    :param X: Design matrix for DBSCAN, each row is one point
    :param labels: Cluster labels of points
    :return: Matrix whose first few columns are the centers, and the last column is the label
    """
    distinct_idx = np.argwhere(labels == -1)
    n_cluster = 1 + labels.max()
    n_dim = X.shape[1]
    pt_idx = range(n_dim)
    ii = n_cluster
    centers = np.zeros((n_cluster + len(distinct_idx), 1 + n_dim), dtype="double")
    for i in range(len(labels)):
        label = labels[i]
        if label == -1:
            centers[ii, pt_idx] = X[i, :]
            centers[ii, n_dim] = -1
            ii += 1
        else:
            centers[label, pt_idx] = centers[label, pt_idx] + X[i, :]
            centers[label, n_dim] = centers[label, n_dim] + 1
    for r in range(n_cluster):
        centers[r, pt_idx] /= centers[r, n_dim]
        centers[r, n_dim] = r
    return centers


class TpcHits:
    def __init__(self,
                 x_hits: np.ndarray, 
                 y_hits: np.ndarray,
                 layers: np.ndarray | None = None, 
                 raw_hits: np.ndarray | None = None,
                 raw_times: np.ndarray | None = None,):
        self.x_hits = x_hits
        self.y_hits = y_hits
        self.layers = layers
        self.raw_hits = raw_hits
        self.raw_times = raw_times

def decodeRawHits(hits) -> TpcHits:
    decoder = getDecoder()
    # Obeserved data: std::vector like
    times = np.array(hits.time())
    cells = np.array(hits.cellID())
    positions = np.array(hits.position())
    # Converts vector3d to numpy matrix
    raw_mapper = np.frompyfunc(lambda pos: (pos.x, pos.y, pos.z), 1, 3)
    # Converts cell id to layer indicator 
    cell_mapper = np.frompyfunc(lambda id: decoder.get(id, "layer"), 1, 1)

    raw_hits = np.stack(raw_mapper(positions), axis=1)
    layers = cell_mapper(cells)
    idx = (layers == 0)

    x_hits = np.stack((raw_hits[idx, 0], times[idx]), axis=1)
    y_hits = np.stack((raw_hits[~idx, 1], times[~idx]), axis=1)

    return TpcHits(x_hits, y_hits, layers, raw_hits, times)


_initialized = False
_readout_name = "TpcStripHits"

def getDecoder():
    from megat import IdConverter
    init()
    id_conv = IdConverter(_readout_name)
    decoder = id_conv.decoder()
    return decoder

def init():
    global _initialized
    if not _initialized:
        from megat import loadGeometry
        detector_path = os.environ.get("MEGAT_ROOT", "")
        xml_list = [os.path.join(detector_path, "geometry/compact/Megat.xml"),
                    os.path.join(detector_path, "geometry/compact/TPC_readout.xml")]
        loadGeometry(xml_list, _readout_name)
        _initialized = True
