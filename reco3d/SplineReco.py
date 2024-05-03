from __future__ import annotations
from ast import Tuple
from typing import Optional
import numpy as np
import SplineFit as sf


class SplineRecoResult:

    def __init__(
        self,
        ep: np.ndarray,
        eps: np.ndarray,
        dirs: np.ndarray,
        config: SplineRecoConfig,
    ) -> None:
        self.ep = ep
        self.eps = eps
        self.dirs = dirs
        self.config = config

    # !__init__()

    def average_end_point(self):
        return self.ep

    def end_points(self) -> np.ndarray:
        return self.eps

    def circular_average_electron_direction(self) -> float:
        return np.mean(
            np.arctan2(np.mean(np.sin(self.dirs)), np.mean(np.cos(self.dirs)))
        )

    def average_electron_direction(self) -> float:
        return np.mean(self.dirs)


# !SplineRecoResult


class SplineRecoConfig:

    def __init__(
        self,
        n_try: int = 10,
        sample_range: float = 0.1,
        reject_dist: float = 15,
        fit_config: sf.ContSplineFitConfig = sf.ContSplineFitConfig(),
    ) -> None:
        self.n_try = n_try
        self.sample_range = sample_range
        self.reject_dist = reject_dist
        self.fit_config = fit_config
        pass

    def reconstruct(
        self,
        points: np.ndarray,
        weights: Optional[np.ndarray] = None,
    ) -> SplineRecoResult:

        ## ======== First, sample some random initial points ======== ##

        fit = self.fit_config.make_fit(points, weights)
        z_cut = np.percentile(points[:, 1], 100 * (1 - self.sample_range))
        init_indices = np.random.choice(
            np.flatnonzero(points[:, 1] >= z_cut),
            size=self.n_try,
        )

        ## ======== Second, build fittings ======== ##

        eps = np.zeros((2 * self.n_try, 2))
        dirs = np.zeros(2 * self.n_try)
        head_first = np.zeros(self.n_try, "bool")

        for i in range(self.n_try):
            fit.build_continuously(init_point_idx=init_indices[i])
            tmp_idx = [2 * i, 1 + 2 * i]
            eps[tmp_idx, :] = fit.singular_end_points()
            dirs[tmp_idx] = fit.end_directions()
        # !for i in range(self.n_try)

        ## ======== Third, cluster to determine some ``true'' end points ======== ##
        ep, indices = __find_end_point(eps, self.reject_dist)

        return SplineRecoResult(ep, eps[indices, :], dirs[np.indices])

    # !reconstruct()


# !SplineRecoConfig


def __dist(x, ys):
    return np.sqrt(np.sum((ys - x) ** 2, axis=1))


def __find_end_point(eps, radius) -> Tuple[[np.ndarray, np.ndarray]]:
    n = np.shape[0]
    d = np.zeros(n)
    clusters = []
    sizes = []
    unvisited = np.ones(n, "bool")
    while np.any(unvisited):
        d.fill(radius)
        idx = np.argmax(unvisited)
        d[unvisited] = __dist(eps[idx, :], eps[unvisited, :])
        cluster = np.flatnonzero(d < radius)
        clusters.append(cluster)
        sizes.append(len(cluster))
    # !while True
    cluster = clusters[np.argmax(sizes)]
    ep = np.mean(eps[cluster, :], axis=0)
    return ep, cluster
