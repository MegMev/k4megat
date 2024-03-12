from dataclasses import dataclass
from typing_extensions import deprecated
import numpy as np
from pandas import DataFrame

pos_id = 0
time_id = 1
sigma_id = 2


class HitPair:
    def __init__(self, x_hit: np.ndarray, y_hit: np.ndarray):
        self.x_hit = x_hit
        self.y_hit = y_hit

    def x(self):
        return self.x_hit[pos_id]

    def y(self):
        return self.y_hit[pos_id]

    def time(self):
        return (self.x_hit[time_id] + self.y_hit[time_id]) / 2


@deprecated("This function is for removal later. Use `buildHitPairs` instead.")
def build3d(
    x_hits: np.ndarray,
    y_hits: np.ndarray,
    search_band: float,
    scale_factor: float,
    use_sigma=False,
) -> list[HitPair]:
    """
    Rebuild 3d hits from 2d hits.
    input hits format:
              position time  sigma
      hit1       xx1    yy1   zz1
      hit2       xx2    yy2   zz2
      ...        ...    ...   ...

    :param x_hits: x hits
    :param y_hits: y hits
    :param search_band: width of time interval in which search for matching pairs
    :param scale_factor: scale factor of hit sigma
    :param use_sigma: use sigma to make pairs,
            if False, the scale_factor is used as tolerance
    :return: list of matched hit pairs
    """

    ny = y_hits.shape[0]

    # sort hits by time
    x_idx = x_hits.argsort(axis=0)[:, time_id]
    y_idx = y_hits.argsort(axis=0)[:, time_id]
    x_hits = x_hits[x_idx, :]
    y_hits = y_hits[y_idx, :]

    pairs = []  # resulted matching pairs

    lower = 0  # lower bound for searching
    upper = 0  # upper bound for searching
    # for each x hit, try to match with y hits
    for hit in x_hits:
        time = hit[1]
        lower_time = time - search_band
        upper_time = time + search_band
        while lower < ny and y_hits[lower, time_id] < lower_time:
            lower += 1
        pass
        while upper < ny and y_hits[upper, time_id] <= upper_time:
            upper += 1
        pass
        if lower == ny:
            break
        pass
        _find_pairs(hit, y_hits[lower:upper, :], scale_factor, pairs, use_sigma)
    pass
    return pairs


def buildHitPairs(
    x_hits: np.ndarray,
    y_hits: np.ndarray,
    tolerance: float = 20,
    col_spec: dict = {"position": 0, "time": 1, "z": 2, "edep": 3},
) -> DataFrame:
    """
    Rebuild 3D hits by matching hits in x and y planes according to their arrival time.

    Args:
        x_hits (np.ndarray): Hits in the x plane.
        y_hits (np.ndarray): Hits in the y plane.
        tolerance (float, optional): Tolerance of matching hits, the unit is us. Defaults to 20.
        col_spec (dict, optional): Specifying the columns of x and y hits, by not giving a "z" term to suppress z information. Defaults to {"position": 0, "time": 1, "z": 2, "edep": 3}.

    Returns:
        DataFrame: Dataframe of matching result. Following columns are available:
            "x_pos": x coordinates of matched hits.
            "y_pos": y coordinates of matched hits.
            "x_time": Arrival time on x planes of matched hits.
            "y_time": Arrival time on y planes of matched hits.
            "x_idx": Indices of matched x hits in the original x hits.
            "y_idx": Indices of matched y hits in the original y hits.
            "x_z" (optional): z coordinates of matched x hits.
            "y_z" (optional): z coordinates of matched y hits.
            "x_edep" (optional): Energy deposition of matched x hits.
            "y_edep" (optional): Energy deposition of matched y hits.
    """
    ny = y_hits.shape[0]

    pid = col_spec["position"]
    tid = col_spec["time"]
    zid = col_spec.get("z", -1)
    eid = col_spec.get("edep", -1)

    x_idx = np.argsort(x_hits[:, tid])
    y_idx = np.argsort(y_hits[:, tid])
    _x_hits = x_hits[x_idx, :]
    _y_hits = y_hits[y_idx, :]

    x_indices = []
    y_indices = []

    step = 0

    for hit, xid in zip(_x_hits, x_idx):
        time = hit[tid]
        lt = time - tolerance
        ut = time + tolerance
        while step < ny and _y_hits[step, tid] < lt:
            step += 1
        if step == ny:
            break
        i = step
        while i < ny and _y_hits[i, tid] < ut:
            x_indices.append(xid)
            y_indices.append(y_idx[i])
            i += 1
        # while i < ny and y_hits[i, tid] < ut

    n = len(x_indices)
    i = 6
    x_indices = np.asarray(x_indices)
    y_indices = np.asarray(y_indices)

    columns = ["x_pos", "y_pos", "x_time", "y_time", "x_idx", "y_idx"]
    data = np.zeros((n, 6 + (0 if zid == -1 else 2) + (0 if eid == -1 else 2)))
    data[:, 0] = x_hits[x_indices, pid]
    data[:, 1] = y_hits[y_indices, pid]
    data[:, 2] = x_hits[x_indices, tid]
    data[:, 3] = y_hits[y_indices, tid]
    data[:, 4] = x_indices
    data[:, 5] = y_indices
    if not zid == -1:
        columns += ["x_z", "y_z"]
        data[:, i + 0] = x_hits[x_indices, zid]
        data[:, i + 1] = y_hits[y_indices, zid]
        i += 2
    if not zid == -1:
        columns += ["x_edep", "y_edep"]
        data[:, i + 0] = x_hits[x_indices, eid]
        data[:, i + 1] = y_hits[y_indices, eid]
        i += 2
    return DataFrame(data, columns=columns)


def _find_pairs(
    target_hit: np.ndarray,
    cand_hits: np.ndarray,
    scale_factor: float,
    pairs: list[HitPair],
    use_sigma: bool,
) -> list[HitPair]:
    """
    Find matching hit pairs for given target hit and
    candidate hits.

    :param target_hit: target hit
    :param cand_hits: candidate hits
    :param scale_factor: scale factor of hit sigmas for matching
    :param pairs: container for matched hit pairs
    :param use_sigma: use sigma to make pairs
    :return: given container `pairs`
    """
    # loops over candidate hits and tries to make hit pairs
    for hit in cand_hits:
        if _make_pair(target_hit, hit, scale_factor, use_sigma):
            pairs.append(HitPair(target_hit, hit))
        pass
    pass
    return pairs


def _make_pair(hit1, hit2, scale_factor, use_sigma) -> bool:
    """
    Determine if the two hits match with each other.

    :param hit1: first hit
    :param hit2: second hit
    :param scale_factor: scale factor of hit sigmas for matching
    :param use_sigma: use sigma to make pairs
    :return: true if the two hits match
    """
    t1 = hit1[time_id]
    t2 = hit2[time_id]
    tor = (
        scale_factor * (hit1[sigma_id] + hit2[sigma_id]) if use_sigma else scale_factor
    )
    return abs(t1 - t2) <= tor


# %%
