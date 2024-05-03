import numpy as np
import SplineFit as sf


class SplineRecoResult:

    def __init__(self) -> None:
        pass

    def end_points(self) -> np.ndarray:
        pass

    def electron_direction(self) -> np.ndarray:
        pass


# !SplineRecoResult


class SplineRecoConfig:

    def __init__(self) -> None:
        pass

    def reconstruct(self, data: np.ndarray) -> SplineRecoResult:
        pass


# !SplineRecoConfig
