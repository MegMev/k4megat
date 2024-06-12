# 2D重建

## 文件

- `RecoUtils.py`:    读取`root`文件的工具类
- `Hit3DBuilder.py`: 匹配2D命中生成3D命中
- `BezierSpline.py`: Bezier曲线和B样条
- `SplineFit.py`:    2D散点拟合
- `SplineReco.py`:   多次拟合重建

## RecoUtils

### tidyHits

使用`tidyHits`函数处理`Podio`读取的每次事件的数据. 

该函数返回`pandas.DataFrame`

```python
from podio.root_io import Reader

compton_event_path = "./data.root"

reader = Reader(compton_event_path)
events = reader.get("events")
# Do not write (events[0]).get("xxx") # ERROR
event = events[42]                 
hits  = event.get("TpcDriftHits")
df    = tidyHits(hits, "TrackerHits")
```

### vec3d2mat

该函数将代理`cpp`的3D向量的向量 (`std::vector<edm4hep::Vector3d>`) 转换为`numpy.ndarray`. 

返回的矩阵的行数与3D向量的数量相同, 列数为3.

### decodeRawHits

类似与`tidyHits`, 将`tpcHits`转换为`numpy.ndarray`. 

在使用`Hit3DBuilder`时, 使用该函数可能更方便. 
但是该函数可以处理的数据类型少于`tidyHits`.

## SplineFit

### SplineFit 

单次PCA样条拟合. 

```python
fit = SplineFit(points, weights, radius)
fit.build_clusters(0)
fit.order()
```

获取端点`fit.end_points()`, 
获取端点方向`fit.end_directions()`. 
包含在曲线拟合里的散点`fit.managedPoints()`, 
散点对应的曲线参数`fit.projections`, 这个不是对象方法.

### ContSplineFit

连续的曲线拟合. 
使用同上, 此方法不提供散点的曲线参数. 

`ContSplineFitConfig`保存方法参数. 

## SplineReco

多次样条拟合.

```python
config = SplineRecoConfig
res    = config.reconstruct(points, weights)
eps    = res.average_end_point()
```

重建包含以下结果:
- `average_end_point()`: 所有拟合的端点平均
- `end_points()`: 所有拟合的端点
- `circular_average_electron_direction()`: 方向的循环平均
- `average_electron_direction()`: 方向的简单算数平均

注意, 简单算术平均并不一定适用于高维单位向量. 

