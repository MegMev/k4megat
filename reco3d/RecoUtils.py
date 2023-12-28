from dataclasses import dataclass
from typing import Any
from typing_extensions import deprecated
import numpy as np
from numpy.typing import ArrayLike
from megat import getTpcDecoder


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


@dataclass
class TpcHits:
    x_hits: np.ndarray
    y_hits: np.ndarray
    times: np.ndarray | None = None
    edeps: np.ndarray | None = None
    layers: np.ndarray | None = None
    raw_hits: np.ndarray | None = None


def decodeRawHits(hits) -> TpcHits:
    """
    Converts a collection of tpc hits to numpy::ndarray.
    Components are retrieved and stored in the data class TpcHits

    Args:
        hits (cppyy.gbl.edm4hep.TrackerHitCollection): Collection of tpc hits.

    Returns:
        TpcHits: Data class of each components. The following fields are available:
            x_hit (np.ndarray): Matrix of observed x hits, columns are {"position", "time", "z"}.
            y_hit (np.ndarray): Matrix of observed y hits, columns are {"position", "time", "z"}.
            layers (np.ndarray): Vector of layers w.r.t. original hits.
            raw_hits (np.ndarray): Matrix of original hit coordinates, columns are {"x", "y", "z"}.
            times (np.ndarray): Vector of original times.
    """
    # Get tpc decoder from megat
    decoder = getTpcDecoder()
    # Obeserved data: std::vector like
    times = np.array(hits.time())
    cells = np.array(hits.cellID())
    edeps = np.array(hits.eDep())
    positions = np.array(hits.position())
    # Converts cell id to layer indicator
    cell_mapper = np.frompyfunc(lambda id: decoder.get(id, "layer"), 1, 1)

    raw_hits = vec3d2mat(positions)
    layers = cell_mapper(cells)
    idx = layers != 0

    # Prepare for x and y observed hits matrix
    xy_hits = raw_hits.copy()  # Make a copy of raw hits
    # Second column will be set to time and y coordinates of y hits are supposed to be observed
    xy_hits[idx, 0] = xy_hits[idx, 1]
    xy_hits[:, 1] = times
    xy_hits = np.concatenate((xy_hits, edeps.reshape((-1, 1))), axis=1)

    return TpcHits(xy_hits[~idx, :], xy_hits[idx, :], times, edeps, layers, raw_hits)


_raw_mapper = np.frompyfunc(
    lambda pos: (float(pos.x), float(pos.y), float(pos.z)), 1, 3
)


def vec3d2mat(v, dtype="double") -> np.ndarray:
    """
    Converts a vector of vector3d to numpy matrix (ndarray).
    Notice that each row of the matrix corresponds to a vector3d.

    Args:
        v (cppyy.gbl.std.vector<edm4hep::Vector3d>): Vector of vector3d of length n.

    Returns:
        np.ndarray: Matrix of $n \\times 3$
    """
    return np.stack(_raw_mapper(v), axis=1, dtype=dtype, casting="unsafe")


class CellIDLayerMapper:
    def __init__(self, readout: str = "TpcDiagonalStripHits") -> None:
        """
        A converter that maps a list of cell ids to tpc layer using megat decoder.

        Args:
            readout (str, optional): Readout name. Defaults to "TpcDiagonalStripHits".

        Usage:
            ```
            layer_mapper = CellIDLayerMapper()
            layers = alyer_mapper(cells)
            ```
        """
        self._decoder = getTpcDecoder(readout)
        self._mapper = np.frompyfunc(
            lambda id: np.int32(self._decoder.get(id, "layer")), 1, 1
        )

    def __call__(self, cellID: ArrayLike) -> np.ndarray:
        """
        Maps a list of cell ids to layer ids.

        Args:
            cellID (ArrayLike): List of cell ids.

        Returns:
            np.ndarray: Numpy array of layer ids.
        """
        return self._mapper(cellID).astype(dtype=np.int32, copy=False)


class TimeZMapper:
    def __init__(self, drift_velocity: float = 60, time_factor: float = 1000) -> None:
        """
        Converts the arrival times and z coordinates time relative to of hits to absolute detector z coordinates.

        The transformation is:
            $$z_{abs} = z_{rel} - time / factor * velocity$$

        Args:
            drift_velocity (float, optional): Drift velocity of electrons in mm/ms. Defaults to 60.
            time_factor (float, optional): Time unit scale factor. The drift time may be delivered in ns, converts to ms by divided by 1000. Defaults to 1000.

        Usage:
            ```
            z_poss = poss[:, 2]
            times = np.array(hits.time())
            z_mapper = TimeZMapper()
            z_abs = z_mapper(times, z_poss)
            ```
        """
        self._drift_velocity = drift_velocity
        self._time_factor = time_factor

    def __call__(self, time: ArrayLike, z_pos: ArrayLike = 255) -> np.ndarray:
        """
        Converts arrival times and reference z coordinates to absolute z coordinates.

        Args:
            time (ArrayLike): Arrival time.
            z_pos (ArrayLike, optional): Detector z coordinates related to, generally very close to 255. Defaults to 255.

        Returns:
            np.ndarray: Absolute z coordinates in detector coordinate system.
        """
        return (
            np.asarray(z_pos)
            - np.asarray(time) / self._time_factor * self._drift_velocity
        )  # type: ignore


@deprecated("Use TimeZMapper instead.")
class PairFlater:
    def __init__(self, drift_velocity=60, z_lower=255) -> None:
        self.pair_mapper = np.frompyfunc(
            lambda pair: (
                pair.x(),
                pair.y(),
                z_lower - pair.time() / 1000 * drift_velocity,
            ),
            1,
            3,
        )

    def toMat(self, pairs):
        return np.stack(self.pair_mapper(np.array(pairs)), axis=1)
