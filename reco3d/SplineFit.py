from __future__ import annotations
import numpy as np
from typing import Callable, Optional, Literal
from BezierSpline import BSpline
from random import randint


def _euclidean(x: np.ndarray, y: np.ndarray) -> float:
    return np.sqrt(np.sum((x - y) ** 2))


def _dist(x, ys):
    return np.sqrt(np.sum((ys - x) ** 2, axis=1))


def _stop_criteria(prev: _PCACluster, next: _PCACluster) -> bool:
    return prev.contains(next.centroid, 0.75) or prev.supersets(next)  # type: ignore


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

    def next_center(self, reverse: bool = False, extension: float = 1) -> np.ndarray:
        return self.centroid + extension * self.radius * self.main_local_direction(
            reverse
        )

    def contains(self, point: np.ndarray, regular: float = 1.0) -> bool:
        return self.dist(self.center, point) < regular * self.radius

    def supersets(self, other: _PCACluster):
        return np.all(np.isin(other.indices, self.indices))


class SplineFit:
    """
    Fit unordered point cloud with B-Spline by PCA.
    """

    def __getattr__(self, name):
        return None

    def __init__(
        self,
        points: np.ndarray,
        weights: Optional[np.ndarray] = None,
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
            verbose (int): Verbose level, 0, 1 or 2.

        Usage:
            fit = SplineFit(points, weights, radius)
            fit.build_clusters(0)
            fit.order()
        """
        self.pts = points
        self.ws = weights
        self.radius = radius
        self.isValidCluster = criteria
        self.step = r_step
        self.dist = metric
        self.npt = points.shape[0]
        self.dim = points.shape[1]
        self.verbose = verbose
        # assert points.shape[1] == 2
        # assert self.npt == len(weights)
        pass

    def build_clusters(
        self,
        init_point_idx: int | Callable[[int], int] = 0,
        init_point: Optional[np.ndarray] = None,
    ) -> list[_PCACluster]:
        """
        Build clusters of given points.

        Args:
            init_point_idx (int | Callable[[], int], optional): Index of initial point. Defaults to 0.
            init_point (np.ndarray, optional): Coordinates of initial point. If not `None`, `init_point_idx` is ignored.

        Raises:
            RuntimeError: If failed to cluster

        Returns:
            list[_PCACluster]: List of clusters, in the order that the last cluster at the first, and the local main direction goes from the last cluster to the first cluster (the last one in the returned list).
        """

        # Initializes first cluster
        if init_point is not None:
            first_cluster = self._cal_cluster(init_point, self.radius)
        else:
            if init_point_idx is Callable:
                init_point_idx = init_point_idx(self.npt)
            first_cluster = self._cal_cluster(self.pts[init_point_idx,], self.radius)

        if first_cluster is None:
            raise RuntimeError(
                "Failed to build first cluster, retry with different initial point.",
                f"Initial point index: {init_point_idx}",
            )

        if self.verbose >= 1:
            print(f"[build_cluster] First cluster built: {first_cluster}.")

        prev = first_cluster
        clusters = [prev]

        # move forward
        if self.verbose >= 2:
            print("[build_cluster] Moving forward...")
        while True:
            next = self._cal_cluster(prev.next_center(), self.radius)
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
                print(f"[build_cluster] Built forward cluster: {next}.")
            if _stop_criteria(prev, next):
                if self.verbose >= 2:
                    print("[build_cluster] Forward process done.")
                break
            prev = next

        # end move forward

        n_forward = len(clusters)
        prev = first_cluster

        # move backward
        if self.verbose >= 2:
            print("[build_cluster] Moving backward...")
        while True:
            next = self._cal_cluster(prev.next_center(reverse=True), self.radius)
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
                print(f"[build_cluster] Built backward cluster: {next}.")
            if _stop_criteria(prev, next):
                if self.verbose >= 2:
                    print("[build_cluster] Backward process done.")
                break
            prev = next
        # end move backward

        self.clusters = clusters
        self.n_clusters = len(clusters)
        self.n_forward = n_forward
        if self.verbose >= 2:
            print(
                f"[build_cluster] PCA clustering process done. Total {n_forward}/{self.n_clusters} clusters built."
            )
        return clusters

    def _cal_cluster(self, center: np.ndarray, radius: float) -> Optional[_PCACluster]:

        if self.verbose >= 3:
            print(
                f"[_cal_cluster] Building cluster at {center} with radius = {radius}."
            )

        idx = []
        # -TODO: replace with quad-tree or BSP (k-d) tree
        for r in range(self.npt):
            if self.dist(center, self.pts[r,]) < radius:
                idx.append(r)

        if len(idx) == self.npt:

            if self.verbose >= 3:
                print("[_cal_cluster] Failed to build cluster: all points included.")

            return

        if len(idx) == 0:

            if self.verbose >= 3:
                print("[_cal_cluster] Failed to build cluster: nothing included.")

            return self._cal_cluster(center, self.step(radius))

        # Centralize interior points
        cpt = self.pts[idx,]
        centroid = (
            np.average(cpt, axis=0, weights=self.ws[idx])
            if self.ws is not None
            else np.mean(cpt, axis=0)
        )

        if self.verbose >= 3:
            print(
                f"[_cal_cluster] Built cluster of {len(idx)} points centroids at {centroid}."
            )

        lpt = cpt - centroid

        # Now, we perform svd on points' local coordinates
        _, s, v = np.linalg.svd(lpt)

        if self.isValidCluster(s):
            return _PCACluster(s, v, center, centroid, radius, idx, self.dist)
        else:
            if self.verbose >= 3:
                print(
                    "[_cal_cluster] Cluster criteria not satisfied. Retry with enlarged radius."
                )
            return self._cal_cluster(center, self.step(radius))

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

    def order(self) -> None:
        """
        Orders points based on clustering results.
        Only points within clusters are ordered.
        Orders are determined by their principal components.

        Note:
            This method requires `build_cluster` invoked.

        """

        centroids = self.centroids()
        oc = BSpline(centroids)
        projections = np.full(self.npt, np.nan, "float")
        n_samples = 2 * self.n_clusters + 1

        visited = np.zeros(self.npt, "bool")
        pt = np.arange(
            oc.knots[0],
            oc.knots[1],
            (oc.knots[1] - oc.knots[0]) / n_samples,
        )
        prev_samples = oc.value(pt)
        for c in np.arange(1, self.n_clusters - 1):
            cluster = self.clusters[c]
            nt = np.linspace(oc.knots[c], oc.knots[c + 1], n_samples)
            next_samples = oc.value(nt)
            for i in cluster.indices:
                if not visited[i]:
                    projections[i] = self._find_closest(
                        self.pts[i], prev_samples, next_samples, pt, nt, n_samples
                    )
                    visited[i] = True
            pt = nt
            prev_samples = next_samples
        self.oc = oc
        self.projections = projections
        pass

    def end_points(self, extension: float = 1) -> np.ndarray:
        """
        Returns estimated end points of the fitting spline.
        End points are estimated to be the next centers of the first and last clusters.

        Returns:
            np.ndarray: A matrix of 2 times ndim, each row corresponds to one end point
        """
        return np.stack(
            [
                self.clusters[self.n_clusters - 1].next_center(extension=extension),
                self.clusters[0].next_center(True, extension=extension),
            ]
        )

    def centroids(self) -> np.ndarray:
        """
        Returns the centroids of this fit.

        Note:
            This method requires `build_cluster` invoked.

        Returns:
            np.ndarray: A matrix of n_clusters \times n_dim,
                in the order from the last cluster to the first one.
        """
        centroids = self._centroids
        if centroids is None:
            centroids = np.zeros([self.n_clusters, self.dim])
            for i in range(self.n_clusters):
                centroids[i,] = self.clusters[i].centroid
            self._centroids = centroids
        return centroids

    def managedPoints(self) -> np.ndarray:
        """
        Returns a boolean array indicating which points are included in
        this spline fit. Projections of included points to spline's
        curvilinear coordinates can be computed via `SplineFit::order`.

        Note:
            This method requires `build_cluster` invoked.

        Returns:
            np.ndarray: A boolean array, `True` if included.
        """
        idx = np.zeros(self.npt, "bool")
        for c in np.arange(1, self.n_clusters - 1):
            cluster = self.clusters[c]
            for i in cluster.indices:
                idx[i] = True
        return idx


class ContSplineFit:

    def __init__(
        self,
        points: np.ndarray,
        weights: Optional[np.ndarray] = None,
        radius: float = 10.0,
        criteria: Callable[[np.ndarray], bool] = lambda s: 0.9 * s[0] > s[-1],
        r_step: Callable[[float], float] = lambda x: x * 1.5,
        threshold: float = 0.1,
        max_try: int = 5,
        merge_factor: float = 2,
        search_factor: float = 3,
        extend_factor: float = 1.5,
        min_pts: int = 5,
        verbose: int = 0,
        verbose_f: int = 0,
    ) -> None:
        """
        `ContSplineFit` continuously fitting points with `SplineFit`.

        Args:
            points (np.ndarray): Data points.
            weights (Optional[np.ndarray], optional): Weights. Defaults to None.
            radius (float, optional): Radius of cluster, passed to `SplineFits`. Defaults to 10.0.
            criteria (Callable[[np.ndarray], bool], optional): Cluster validator, passed to `SplineFits`. Defaults to lambda s:0.9*s[0]>s[-1].
            r_step (Callable[[float], float], optional): Radius enlarge function, passed to `SplineFit`. Defaults to lambda x:x*1.5.
            threshold (float, optional): Ratio of unfitted points allowed when stop. Defaults to 0.1.
            max_try (int, optional): Max number of try fitting. Defaults to 5.
            merge_factor (float, optional): Not used. Defaults to 2.
            search_factor (float, optional): Factor of radius to determine which side to fit. Defaults to 3.
            extend_factor (float, optional): Extension factor for end points. Defaults to 1.5.
            min_pts (int, optional): Min number of points for determine which side to fit. Defaults to 5.
            verbose (int, optional): Verbose level. Defaults to 0.
            verbose_f (int, optional): Verbose level of `SplineFit`. Defaults to 0.
        """
        self.pts = points
        self.ws = weights
        self.radius = radius
        self.isValidCluster = criteria
        self.step = r_step
        self.dist = _euclidean
        self.npt = points.shape[0]
        self.dim = points.shape[1]
        self.threshold = round(threshold * self.npt)
        self.max_try = max_try
        self.n_tried = 0
        self.merge_range = merge_factor * radius
        self.search_range = search_factor * radius
        self.extend_factor = extend_factor
        self.min_pts = min_pts
        self.verbose = verbose
        self.verbose_f = verbose_f
        pass

    def build_continuously(
        self,
        init_point_idx: int | Callable[[int], int] = lambda n: randint(0, n - 1),
    ):

        if self.verbose >= 4:
            print("[build_continuously] ========== Fitting first spline ==========")

        # We first build one spline fit

        first_fit = SplineFit(
            self.pts,
            self.ws,
            self.radius,
            self.isValidCluster,
            self.step,
            self.dist,
            self.verbose_f,
        )
        first_fit.build_clusters(init_point_idx)

        self.fits = [first_fit]
        self.ep_idx = [0, 1]
        self.clusters = first_fit.clusters
        self.centroids = first_fit.centroids()
        eps = first_fit.end_points(self.extend_factor)
        points = self.pts
        weights = self.ws
        prev_fit = first_fit

        n_pt_total = first_fit.npt
        n_managed_total = 0

        if self.verbose >= 2:
            print(
                f"[build_continuously] First fit with {first_fit.n_clusters} clusters."
            )

        while True:

            # ----- First, checking for stop ----- #
            # Gets managed points by last fit
            managed = prev_fit.managedPoints()
            n_managed = np.sum(managed)
            n_managed_total += n_managed

            if self.verbose >= 1:
                print(
                    f"[build_continuously] New fit managed {n_managed}/{n_managed_total}/{n_pt_total} new points."
                )

            # If remaining points are less than threshold, aborts
            if (prev_fit.npt - n_managed) < self.threshold:
                break

            if self.verbose >= 4:
                print(
                    "[build_continuously] ========== Fitting continuous spline =========="
                )

            # ----- Second, determine which side to go ----- #
            # Records remaining points and weights
            points = points[~managed,]
            weights = weights[~managed] if weights is not None else None

            nr_head = np.sum(_dist(eps[0,], points) < self.search_range)
            nr_tail = np.sum(_dist(eps[1,], points) < self.search_range)

            # ----- Third, fit new spline ----- #

            if nr_head >= nr_tail and nr_head >= self.min_pts:
                if self.verbose >= 3:
                    print("[build_continuously] Fitting new spline forward.")
                prev_fit = self._update_spline(points, weights, eps[0,])
                eps[0,], idx = self._update_end_points(
                    eps[0,], prev_fit.end_points(self.extend_factor)
                )
                self.ep_idx[0] = idx
            elif nr_tail > nr_head and nr_tail >= self.min_pts:
                if self.verbose >= 3:
                    print("[build_continuously] Fitting new spline backward.")
                prev_fit = self._update_spline(points, weights, eps[1,], False)
                eps[1,], idx = self._update_end_points(
                    eps[1,], prev_fit.end_points(self.extend_factor)
                )
                self.ep_idx[1] = idx
            else:
                # Neither side satisfied, choose a more likely one
                if self.n_tried >= self.max_try:
                    if self.verbose >= 3:
                        print(
                            f"[build_continuously] Fitting tries exceeded limit {self.max_try}, abort."
                        )
                    break
                self.n_tried += 1
                if nr_head >= nr_tail:
                    if self.verbose >= 3:
                        print("[build_continuously] Trying to fit new spline forward.")
                    prev_fit = self._update_spline(points, weights, eps[0,])
                    eps[0,], idx = self._update_end_points(
                        eps[0,], prev_fit.end_points(self.extend_factor)
                    )
                    self.ep_idx[0] = idx
                else:
                    if self.verbose >= 3:
                        print("[build_continuously] Trying to fit new spline backward.")
                    prev_fit = self._update_spline(points, weights, eps[1,], False)
                    eps[1,], idx = self._update_end_points(
                        eps[1,], prev_fit.end_points(self.extend_factor)
                    )
                    self.ep_idx[1] = idx

        pass  # !while

    pass  # !build_continuously

    def end_points(self, extend_factor=1):
        return np.stack(
            [
                self.fits[1].end_points(extend_factor)[self.ep_idx[0],],
                self.fits[0].end_points(extend_factor)[self.ep_idx[1],],
            ]
        )

    def _update_spline(self, points, weights, init_point, forward=True):
        fit = SplineFit(
            points,
            weights,
            self.radius,
            self.isValidCluster,
            self.step,
            self.dist,
            self.verbose_f,
        )
        fit.build_clusters(init_point=init_point)
        if forward:
            self.fits.append(fit)
            self.clusters = self.clusters + fit.clusters
            self.centroids = np.concatenate([self.centroids, fit.centroids()])
        else:
            self.fits.insert(0, fit)
            self.clusters = fit.clusters + self.clusters
            self.centroids = np.concatenate([fit.centroids(), self.centroids])
        return fit

    def _update_end_points(self, ep, new_eps):
        d = _dist(ep, new_eps)
        if d[0] >= d[1]:
            res = new_eps[0,]
            idx = 0
            d = d[1]
        else:
            res = new_eps[1,]
            d = d[0]
            idx = 1
        # Reject can be based on clusters' local radius
        # Currently disabled
        # if d > self.merge_range:
        # raise RuntimeError("Failed to merge spline. End points too far.")
        return res, idx
