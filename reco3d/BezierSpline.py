from typing import Optional
import numpy as np


def _Bernstein_basis(degree: int, t: np.ndarray) -> np.ndarray:
    n = len(t)
    shape = [n, 1]
    _shape = t.shape
    t.shape = shape
    ones = np.ones(shape)

    s = ones - t
    T = np.concatenate([ones, np.cumprod(np.repeat(t, degree, axis=1), axis=1)], axis=1)
    S = np.concatenate([ones, np.cumprod(np.repeat(s, degree, axis=1), axis=1)], axis=1)

    c = np.concatenate(
        [[1], np.cumprod(np.arange(degree, 0, -1) / np.arange(1, degree + 1))]
    )

    res = (T * np.flip(S, axis=1)) * c

    t.shape = _shape
    return res


class BezierCurve:
    """
    Bezier curve implementation.
    Interpolating points are computed through Bernstein basis.
    It is recommended to compute all at once.
    By default, parameters are within range [0, 1].
    However, parameters out of range can be computed.
    """

    def __init__(self, control_points: np.ndarray) -> None:
        """
        Construct a Bezier curve object.
        No computation occurs during the initialization.

        Args:
            control_points (np.ndarray): Matrix of control points, each row represents one control point
        """
        assert control_points.ndim == 2
        self.controls = control_points
        self.degree = control_points.shape[0] - 1
        self.dim = control_points.shape[1]
        pass

    def value(self, t: np.ndarray) -> np.ndarray:
        """
        Computes the interpolating values at given parameters.

        Args:
            t (np.ndarray): Parameters. The array is flatten.

        Returns:
            np.ndarray: Matrix of resulting points. Rows corresponds to parameters.
        """
        bases = _Bernstein_basis(self.degree, t)
        return bases @ self.controls


class BSpline:
    """
    C2 B-Spline implementation.
    For each spline, two more control points are required to build the local Bezier curve, which is done by C2 constrain, meaning that the local Bezier curve is of degree 3.
    Knots can be an arbitrary ordered sequence. If not given, Chordal parameterization is performed.
    Computing interpolating points out of range is not desireable.
    Natural end point conditions is applied to solve the system.
    Currently, closed curves are not supported.
    """

    def __init__(
        self,
        control_points: np.ndarray,
        knots: Optional[np.ndarray] = None,
    ) -> None:
        """
        Computes the required Bezier control points for local Bezier curves.

        Args:
            control_points (np.ndarray): Matrix of control points, each row represents a control point. The resulting B-Spline should pass through those points.
            knots (np.ndarray, optional): Ordered knots sequence for each control points. If not specified, computed using Chordal parameterization. Defaults to None.
        """
        self.controls = control_points
        self.n_controls = control_points.shape[0]
        self.n_splines = self.n_controls - 1
        self.dim = control_points.shape[1]
        self.weights, self.knots = (
            _Chordal_parameterization(control_points)
            if knots is None
            else (_knots_to_weights(knots), knots)
        )
        self._finish_construction_()
        pass

    def _finish_construction_(self):
        # We build equations for computing Bezier control points b_0, \dots, b_{3n+1}
        # C * B = A \implies B = C^{-1} * A
        A = np.concatenate([self.controls, np.zeros([2 * self.n_splines, self.dim])])
        C = np.zeros([1 + 3 * self.n_splines, 1 + 3 * self.n_splines])

        # C0 continuous constraints:
        #   b_{3i} = p_i, for i = 0, 1, \dots, n
        idx = 3 * np.arange(self.n_controls)
        C[0 : self.n_controls, idx] = np.eye(self.n_controls)

        for i in np.arange(1, self.n_splines):
            j = 3 * i
            # C1 continuous constrains:
            #   \frac{b_{3i} - b_{3i-1}}{t_i - t_{i-1}} = \frac{b_{3i+1} - b_{3i}}{t_{i+1} - t_i},
            #       for i = 1, \dots, n-1
            row = self.n_splines + i
            w1 = 1 / self.weights[i - 1]
            w2 = 1 / self.weights[i]
            C[row, [j - 1, j, j + 1]] = [w1, -w1 - w2, w2]
            # C2 continuous constrains:
            #     \frac{b_{3i} - 2b_{3i-1} + b_{3i-2}}{(t_i - t_{i-1})^2}
            #   = \frac{b_{3i+2} - 2b_{3i+1} + b_{3i}}{(t_{i+1} - t_i)^2},
            #       for i = 1, \dots, n-1
            row = row + self.n_splines - 1
            w1 = w1 * w1
            w2 = w2 * w2
            C[row, (j - 2) : (j + 3)] = [-w1, 2 * w1, w2 - w1, -2 * w2, w2]
            pass
        # Natural end condition
        C[-2, [0, 1, 2]] = [0.5, -1, 0.5]
        C[-1, [-3, -2, -1]] = [0.5, -1, 0.5]
        # Solve equations for Bezier control points
        B = np.linalg.solve(C, A)
        # Build Bezier curves
        curves = []
        for i in range(self.n_splines):
            j = 3 * i
            curves.append(BezierCurve(B[j : (j + 4),]))
        self.curves: list[BezierCurve] = curves
        pass

    def value(self, t: np.ndarray) -> np.ndarray:
        """
        Computes interpolating points at given parameters.

        Args:
            t (np.ndarray): Parameters. Must be ascending.

        Returns:
            np.ndarray: Interpolating points.
        """
        n = len(t)
        step = 0
        res = []
        for i in range(self.n_splines):
            if step == n:
                break
            upper = np.argmax(t > self.knots[i + 1])
            upper = n if upper == 0 and t[-1] <= self.knots[i + 1] else upper
            if upper == step:
                continue
            upper = n if upper < step else upper
            res.append(
                self.curves[i].value((t[step:upper] - self.knots[i]) / self.weights[i])
            )
            step = upper
        return np.concatenate(res)


def _Chordal_parameterization(controls: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    weights = np.sqrt(np.sum((controls[1:,] - controls[:-1,]) ** 2, axis=1))
    weights = weights / np.sum(weights)
    knots = np.concatenate([[0], np.cumsum(weights)])
    knots[-1] = 1.0
    return weights, knots


def _knots_to_weights(t: np.ndarray) -> np.ndarray:
    n = len(t)
    w = np.zeros(n - 1)
    for i in range(n - 1):
        w[i] = t[i + 1] - t[i]
    return w
