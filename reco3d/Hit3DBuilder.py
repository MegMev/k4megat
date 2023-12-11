import numpy as np

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


def build3d(
        x_hits: np.ndarray,
        y_hits: np.ndarray,
        search_band: float,
        scale_factor: float,
        use_sigma=False
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
        find_pairs(hit, y_hits[lower:upper, :], scale_factor, pairs, use_sigma)
    pass
    return pairs


def find_pairs(
        target_hit: np.ndarray,
        cand_hits: np.ndarray,
        scale_factor: float,
        pairs: list[HitPair],
        use_sigma: bool
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
        if make_pair(target_hit, hit, scale_factor, use_sigma):
            pairs.append(HitPair(target_hit, hit))
        pass
    pass
    return pairs


def make_pair(hit1, hit2, scale_factor, use_sigma) -> bool:
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
    tor = scale_factor * (hit1[sigma_id] + hit2[sigma_id]) if use_sigma else scale_factor
    return abs(t1 - t2) <= tor

# %%
