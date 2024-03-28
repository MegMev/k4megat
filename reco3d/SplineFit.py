from __future__ import annotations
import numpy as np
from typing import Callable, Optional, Literal
from BezierSpline import BSpline


def _euclidean(x: np.ndarray, y: np.ndarray) -> float:
    return np.sqrt(np.sum((x - y) ** 2))


def _stop_criteria(prev: _PCACluster, next: _PCACluster) -> bool:
    return prev.contains(next.centroid, 0.75) or prev.supersets(next)


class _PCACluster:
    def __init__(
        self,
        s: np.ndarray,
        v: np.ndarray,
        center: np.ndarray,
        centroid: np.ndarray,
        radius: float,
        indices: np.ndarray,
        metric: Callable[[np.ndarray, np.ndarray], float],
    ):
        self.singular_value = s
        self.ndim = len(s)
        self.rotation = v
        self.center = center
        self.centroid = centroid
        self.radius = radius
        self.indices = indices
        self.dist = metric

    def __str__(self) -> str:
        return f"_PCACluster(center={self.center}, centroid={self.centroid}, radius={self.radius}, mld={self.main_local_direction()})"

    def correct_with(self, dir: np.ndarray) -> None:
        if np.dot(self.main_local_direction(), dir) < 0:
            self.rotation[0,] = -self.rotation[0,]
        pass

    def main_local_direction(self, reverse: bool = False) -> np.ndarray:
        return self.rotation[0,] if not reverse else -self.rotation[0,]

    def next_center(self, reverse: bool = False) -> np.ndarray:
        return self.centroid + self.radius * self.main_local_direction(reverse)

    def contains(self, point: np.ndarray, regular: float = 1.0) -> bool:
        return self.dist(self.center, point) < regular * self.radius

    def supersets(self, other: _PCACluster):
        return np.all(np.isin(other.indices, self.indices))


class SplineFit:
    """
    Fit unordered point cloud with B-Spline by PCA.
    """

    def __init__(
        self,
        points: np.ndarray,
        weights: np.ndarray,
        radius: float = 10.0,
        criteria: Callable[[np.ndarray], bool] = lambda s: 0.9 * s[0] > s[-1],
        r_step: Callable[[float], float] = lambda x: x * 1.5,
        metric: Callable[[np.ndarray, np.ndarray], float] = _euclidean,
        verbose: int = 0,
    ):
        """
        Initializes a fit task for unordered point cloud.

        Args:
            points (np.ndarray): Point cloud points, each row represents a point.
            weights (np.ndarray): Weights for each point, currently not used.
            radius (float, optional): Initial radius for clustering. Defaults to 10.0.
            criteria (_type_, optional): Clustering retry criteria. Ratio represents the difference in PCA singular value of clusters. If the ratio approaches 1, the cluster has no preferred local direction, and radius should be increased. Defaults to lambda s:0.9*s[0]>s[-1].
            r_step (_type_, optional): Function for increasing clustering radius. Defaults to lambda x:x*1.5.
            metric (Callable[[np.ndarray, np.ndarray], float], optional): Distance metric. Defaults to _euclidean.


        Usage:
            fit = SplineFit(points, weights, radius)
            fit.build_clusters(0)
            fit.order()
        """
        self.pts = points
        self.ws = weights
        self.radius = radius
        self.criteria = criteria
        self.step = r_step
        self.dist = metric
        self.npt = points.shape[0]
        self.dim = points.shape[1]
        self.verbose = verbose
        # assert points.shape[1] == 2
        # assert self.npt == len(weights)
        pass

    def build_clusters(
        self, init_point_idx: int | Callable[[], int] = 0
    ) -> list[_PCACluster]:
        """
        Build clusters of given points.

        Args:
            init_point_idx (int | Callable[[], int], optional): Index of initial point. Defaults to 0.

        Raises:
            RuntimeError: If failed to cluster
            RuntimeError: _description_
            RuntimeError: _description_

        Returns:
            list[_PCACluster]: List of clusters, in the order that the last cluster at the first, and the local main direction goes from the last cluster to the first cluster (the last one in the returned list).
        """
        if init_point_idx is Callable:
            init_point_idx = init_point_idx()
        first_cluster = self.cal_cluster(self.pts[init_point_idx,], self.radius)
        if first_cluster is None:
            raise RuntimeError(
                "Failed to build first cluster, retry with different initial point.",
                f"Initial point index: {init_point_idx}",
            )
        if self.verbose >= 1:
            print(f"First cluster built: {first_cluster}.")
        prev = first_cluster
        clusters = [prev]
        # move forward
        if self.verbose >= 2:
            print("Moving forward...")
        while True:
            next = self.cal_cluster(prev.next_center(), self.radius)
            if next is None:
                raise RuntimeError(
                    "Failed to cluster in forward process.",
                    f"Clusters built: {len(clusters)}",
                    f"Current geometric center: {prev.next_center()}",
                    f"Initial radius: {self.radius}",
                )
            next.correct_with(prev.main_local_direction())
            clusters.append(next)
            if self.verbose >= 1:
                print(f"Built forward cluster: {next}.")
            if _stop_criteria(prev, next):
                if self.verbose >= 2:
                    print("Forward process done.")
                break
            prev = next
        # end move forward
        n_forward = len(clusters)
        prev = first_cluster
        # move backward
        if self.verbose >= 2:
            print("Moving backward...")
        while True:
            next = self.cal_cluster(prev.next_center(reverse=True), self.radius)
            if next is None:
                raise RuntimeError(
                    "Failed to cluster in backward process.",
                    f"Clusters built (backward/overall): {len(clusters)-n_forward}/{len(clusters)}",
                    f"Current geometric center: {prev.next_center()}",
                    f"Initial radius: {self.radius}",
                )
            next.correct_with(prev.main_local_direction())
            clusters.insert(0, next)
            if self.verbose >= 1:
                print(f"Built backward cluster: {next}.")
            if _stop_criteria(prev, next):
                if self.verbose >= 2:
                    print("Backward process done.")
                break
            prev = next
        # end move backward
        self.clusters = clusters
        self.n_clusters = len(clusters)
        self.n_forward = n_forward
        if self.verbose >= 2:
            print(
                f"PCA clustering process done. Total {n_forward}/{self.n_clusters} clusters built."
            )
        return clusters

    def cal_cluster(self, center: np.ndarray, radius: float) -> Optional[_PCACluster]:
        idx = []
        # -TODO: replace with quad-tree or BSP (k-d) tree
        for r in range(self.npt):
            if self.dist(center, self.pts[r,]) < radius:
                idx.append(r)
        if len(idx) == self.npt:
            return
        # Centralize interior points
        cpt = self.pts[idx,]
        centroid = np.average(cpt, axis=0, weights=self.ws[idx])
        lpt = cpt - centroid
        # Now, we perform svd on points' local coordinates
        _, s, v = np.linalg.svd(lpt)

        return (
            _PCACluster(s, v, center, centroid, radius, idx, self.dist)
            if self.criteria(s)
            else self.cal_cluster(center, self.step(radius))
        )

    def _find_closest(
        self,
        point: np.ndarray,
        prev_samples: np.ndarray,
        next_samples: np.ndarray,
        prev_t: np.ndarray,
        next_t: np.ndarray,
        n_samples: int,
    ) -> float:
        pd = np.zeros(n_samples)
        nd = np.zeros(n_samples)
        for i in range(n_samples):
            pd[i] = self.dist(point, prev_samples[i,])
            nd[i] = self.dist(point, next_samples[i,])
        pi = np.argmin(pd)
        ni = np.argmin(nd)
        return prev_t[pi] if pd[pi] < nd[ni] else next_t[ni]

    def order(self):
        """
        Order points based on clustering results.
        Only points within clusters are ordered.
        Orders are determined by their principal components.
        """
        centroids = np.zeros([self.n_clusters, self.dim])
        for i in range(self.n_clusters):
            centroids[i,] = self.clusters[i].centroid
        oc = BSpline(centroids)
        projections = np.zeros(self.npt)
        n_samples = 2 * self.n_clusters
        visited = dict()
        pt = np.arange(
            oc.knots[0],
            oc.knots[1],
            (oc.knots[1] - oc.knots[0]) / n_samples,
        )
        prev_samples = oc.value(pt)
        for c in np.arange(1, self.n_clusters - 1):
            cluster = self.clusters[c]
            nt = np.arange(
                oc.knots[c],
                oc.knots[c + 1],
                (oc.knots[c] + oc.knots[c + 1]) / n_samples,
            )
            next_samples = oc.value(nt)
            for i in cluster.indices:
                if visited.get(i) is not None:
                    projections[i] = self._find_closest(
                        self.pts[i], prev_samples, next_samples, pt, nt, n_samples
                    )
                    visited[i] = 0
            pt = nt
            prev_samples = next_samples
        self.oc = oc
        self.projections = projections
        pass

    def end_points(self) -> np.ndarray:
        """
        Returns estimated end points of the fitting spline.
        End points are estimated to be the next centers of the first and last clusters.

        Returns:
            np.ndarray: A matrix of 2 times ndim, each row corresponds to one end point
        """
        return np.stack(
            [
                self.clusters[self.n_clusters - 1].next_center(),
                self.clusters[0].next_center(True),
            ]
        )
