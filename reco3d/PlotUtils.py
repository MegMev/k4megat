from dataclasses import dataclass
import datar
import pandas as pd
import numpy as np
from pandas import DataFrame
from numpy.typing import ArrayLike
import plotly.graph_objects as go


class aes_:
    def __init__(
        self,
        x: str | int = -1,
        y: str | int = -1,
        z: str | int = -1,
        size: str | int = -1,
        color: str | int = -1,
        shape: str | int = -1,
    ):
        self._dict = dict(
            x=x,
            y=y,
            z=z,
            size=size,
            color=color,
            shape=shape,
        )


@dataclass
class pplot:
    aes: aes_ = aes_()
    data: DataFrame | ArrayLike | None = None

    _fig = go.Figure()
    _layers = []

    def show(self):
        traces = [layer.prepare(self, i) for i, layer in enumerate(self._layers)]
        self._fig.add_traces(traces).show()


class _GeomObject:
    def __radd__(self, other: pplot):
        other._layers.append(self)

    def prepare(self, p: pplot, i: int):
        pass


@dataclass
class scatter3d(_GeomObject):
    data: DataFrame | ArrayLike | None = None
    aes: aes_ = aes_()
    x: ArrayLike | str | int = ""
    y: ArrayLike | str | int = ""
    z: ArrayLike | str | int = ""
    size: float = 1.0
    color: str = "black"
    alpha: float = 1.0
    shape: str = "circle"
    name: str = ""
    palette: str = "Rainbow"

    def prepare(self, p: pplot, i: int):
        _i = str(i)
        _data = self.data or p.data
        _x, _ = _getComponent(self.aes, p.aes, _data, self.x, "x")
        _y, _ = _getComponent(self.aes, p.aes, _data, self.y, "y")
        _z, _ = _getComponent(self.aes, p.aes, _data, self.z, "z")

        _color, inheret = _getComponent(self.aes, p.aes, _data, self.color, "color")
        _color_axis = "ColorAxis" if inheret else "ColorAxis" + _i

        # go.Scatter3d(
        #     x=df_rebuilt["x"],
        #     y=df_rebuilt["y"],
        #     z=df_rebuilt["z"],
        #     mode="markers",
        #     marker=dict(
        #         size=3,
        #         color=df_traj["eDep"],
        #         colorscale="Rainbow",
        #         opacity=0.8,
        #         coloraxis="coloraxis",
        #     ),
        #     name="Rebuilt Points",
        # )

        pass


def _makeData(data, aes):
    pass


def _getComponent(aes: aes_, inh_aes: aes_, data, loc, name: str):
    inhereted = False
    spec = aes._dict.get(name, "")
    if _isEmpty(spec):
        inhereted = True
        spec = inh_aes._dict.get(name, "")
    if _isEmpty(spec):
        return loc, False
    return data[spec], inhereted


def _isEmpty(spec) -> bool:
    return spec == "" or spec == -1
