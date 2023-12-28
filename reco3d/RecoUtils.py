from dataclasses import dataclass
from typing import Any, Literal, Optional
from typing_extensions import deprecated
import numpy as np
from numpy.typing import ArrayLike
from pandas import DataFrame
from megat import getTpcDecoder, getIdConverter


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


class CellIDPositionMapper:
    def __init__(self, readout: str = "TpcDiagonalStripHits") -> None:
        """
        A converter that maps a list of cell ids to Tpc coordinate matrix using megat id converter.
        The resulting matrix is of $n \times 3$ whose columns corresponds to x, y and z.

        Args:
            readout (str, optional): Readout name. Defaults to "TpcDiagonalStripHits".

        Usage:
            ```
            cell2pos = CellIDPositionMapper()
            poss = cell2pos(cells)
            ```
        """
        self._converter = getIdConverter(readout)
        self._mapper = np.frompyfunc(lambda id: self._converter.position(id), 1, 1)

    def __call__(self, cellID: ArrayLike) -> np.ndarray:
        """
        Maps a list of cell ids to layer ids.

        Args:
            cellID (ArrayLike): List of cell ids.

        Returns:
            np.ndarray: Numpy array of layer ids.
        """
        return vec3d2mat(self._mapper(cellID))


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


def tidyHits(
    hits,
    type: Literal["SimTrackerHits", "TrackerHits", "RawTimeSeries"] = "TrackerHits",
    cell2layer: Optional[CellIDLayerMapper] = CellIDLayerMapper(),
    time2z: Optional[TimeZMapper] = TimeZMapper(),
    cell2pos: Optional[CellIDPositionMapper] = CellIDPositionMapper(),
) -> DataFrame:
    """
    Converts Tpc hits to tidy dataframe. For valid type, resulting columns are listed below:

    SimTrackerHits:
        Types for stored simulation points. The folowing columns are available:

            `x`     - x coordinate.
            `y`     - y coordinate.
            `raw_z` - z coordiante that arrival time refers to.
            `z`     - absolute z coordinate, if `cell2z` is provided.
            `edep`  - energy deposit.
            `time`  - arrival time.
            `cell`  - cell id.

    TrackerHits:
        Types for stored observed points. The folowing columns are available:

            `x`     - x coordinate.
            `y`     - y coordinate.
            `raw_z` - z coordiante that arrival time refers to.
            `z`     - absolute z coordinate, if `cell2z` is provided.
            `edep`  - energy deposit.
            `time`  - arrival time.
            `cell`  - cell id.
            `layer` - layer id of hits, if `cell2layer` is provided.

    RawTimeSeries:
        Types for waveform hits. The following columns are available:

            `cell`  - cell id.
            `time`  - arric=val time.
            `layer` - layer id, if `cell2layer` is provided.
            'adc'   - adc counts, int32.
            `x`     - x coordinate, if `cell2pos` is provided.
            `y`     - y coordinate, if `cell2pos` is provided.
            `raw_z` - relative z coordinate, if `cell2pos` is provided.
            `z`     - z coordinate, if `time2z` is provided along with `cell2pos`.

    Args:
        hits (edm hit collections): Collection of hits.
        type (Literal[&quot;SimTrackerHits&quot;, &quot;TrackerHits&quot;, &quot;RawTimeSeries&quot;], optional): Underlying data types. Defaults to "TrackerHits".
        cell2layer (Optional[CellIDLayerMapper], optional): Mapper. Defaults to CellIDLayerMapper().
        time2z (Optional[TimeZMapper], optional): Mapper. Defaults to TimeZMapper().
        cell2pos (Optional[CellIDPositionMapper], optional): Mapper. Defaults to CellIDPositionMapper().

    Returns:
        pandas.DataFrame: Tidy dataframe.
    """
    time = np.asarray(hits.time())
    cell = np.asarray(hits.cellID())
    match type:
        case "SimTrackerHits":
            pos = vec3d2mat(hits.position())
            edep = np.asarray(hits.EDep())
            d = dict(
                x=pos[:, 0],
                y=pos[:, 1],
                raw_z=pos[:, 2],
                edep=edep,
                time=time,
                cell=cell,
            )
            if time2z is not None:
                d["z"] = time2z(time, pos[:, 2])
        case "TrackerHits":
            pos = vec3d2mat(hits.position())
            edep = np.asarray(hits.eDep())
            d = dict(
                x=pos[:, 0],
                y=pos[:, 1],
                raw_z=pos[:, 2],
                edep=edep,
                time=time,
                cell=cell,
            )
            if cell2layer is not None:
                d["layer"] = cell2layer(cell)
            if time2z is not None:
                d["z"] = time2z(time, pos[:, 2])
        case "RawTimeSeries":
            charge = hits.charge()
            interval = hits.interval()

            _n_hits = hits.size()
            _temp_size = np.zeros(_n_hits, dtype=np.int32)
            _temp_time = np.zeros(_n_hits, dtype="object")
            _temp_adc = np.zeros(_n_hits, dtype="object")
            for i in range(_n_hits):
                _hit = hits[i]
                _n_adc = _hit.adcCounts_size()
                _int = interval[i]
                _time = time[i]
                # FIXME: a faster copy
                _raw_adc = _hit.getAdcCounts()
                _adc = np.zeros(_n_adc, dtype=np.int32)
                for j in range(_n_adc):
                    _adc[j] = _raw_adc[j]
                _temp_adc[i] = _adc
                _temp_size[i] = _n_adc
                _temp_time[i] = _time + _int * np.arange(_n_adc)
            # for i in rang(_n_hits)

            adc = np.concatenate(_temp_adc)
            time = np.concatenate(_temp_time)
            cell = np.repeat(cell, _temp_size)
            d = dict(adc=adc, time=time, cell=cell)
            if cell2pos is not None:
                pos = cell2pos(cell)
                d["x"] = np.repeat(pos[:, 0], _temp_size)
                d["y"] = np.repeat(pos[:, 1], _temp_size)
                d["raw_z"] = np.repeat(pos[:, 2], _temp_size)
                if time2z is not None:
                    d["z"] = time2z(time, d["raw_z"])
            if cell2layer is not None:
                d["layer"] = np.repeat(cell2layer(cell), _temp_size)
        # case "RawTimeSeries"

    return DataFrame(d)


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
