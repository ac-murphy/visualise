from pathlib import Path
from mpl_toolkits.mplot3d.axes3d import Axes3D
from matplotlib.figure import Figure
from matplotlib.gridspec import GridSpec
from enum import Enum
import system.json as json
import system.bin as bin
import matplotlib.pyplot as plt
import numpy as np
import matplotlib
import types

matplotlib.use("QtAgg")

__metadata__ = {}
__functions__ = {}
__defaults__ = {}
__supported_data_types__ = {
    # specific types:
    'int8': np.int8,
    'int16': np.int16,
    'int32': np.int32,
    'int64': np.int64,
    'uint8': np.uint8,
    'uint16': np.uint16,
    'uint32': np.uint32,
    'uint64': np.uint64,
    'float32': np.float32,

    # assumed types:
    'int': np.int32,
    'float': np.float32,
}
__fig__: Figure
__ax__: Axes3D
__gs__: GridSpec


def register(func):
    __functions__[func.__name__] = func
    return func


class data_storage_order(Enum):
    row_major = 0
    column_major = 1


class axis_scale_type(Enum):
    linear = 0
    logarithmic = 1


__axis_scale_type_keywords__ = {
    axis_scale_type.linear: "linear",
    axis_scale_type.logarithmic: "log"
}


def _get_dtypes(data_path: Path):
    metadata = json.read(data_path / 'metadata')
    dtype_strings = metadata.get('data_types')
    dtypes = []
    for dtype_str in dtype_strings:
        dtypes.append(__supported_data_types__[dtype_str])

    return dtypes


@register
def add_plot_2D(plot_metadata: json, data_path: Path):

    x_dtype, y_dtype = _get_dtypes(data_path)
    x_values = bin.read(data_path / 'x_values', n_cols=1, dtype=x_dtype)
    y_values = bin.read(data_path / 'y_values', n_cols=1, dtype=y_dtype)
    data = json.read(data_path / 'data')

    x_values_nan_mask = bin.read(data_path / 'x_values_nan_mask', n_cols=1, dtype=np.uint8)
    if x_values_nan_mask is not None:
        x_values = np.ma.masked_array(x_values, mask=x_values_nan_mask)

    y_values_nan_mask = bin.read(data_path / 'y_values_nan_mask', n_cols=1, dtype=np.uint8)
    if y_values_nan_mask is not None:
        y_values = np.ma.masked_array(y_values, mask=y_values_nan_mask)

    plot_label = data.get('label', plot_metadata['name'])
    plot_linestyle = data.get('linestyle', '-')

    # draw plot:
    __ax__.plot(x_values, y_values, label=plot_label, linestyle=plot_linestyle or '-')


@register
def add_plot_3D(plot_metadata: json, data_path: Path):
    x_dtype, y_dtype, z_dtype = _get_dtypes(data_path)
    x_values = bin.read(data_path / 'x_values', n_cols=1, dtype=x_dtype)
    y_values = bin.read(data_path / 'y_values', n_cols=1, dtype=y_dtype)

    data = json.read(data_path / 'data')
    storage_order = data_storage_order(data.get('data_storage_order', __defaults__['data_storage_order']))
    colour_map = data.get('colour_map', __defaults__['colour_map'])
    if storage_order == data_storage_order.row_major:
        Z_values = bin.read(data_path / 'z_values', n_cols=len(x_values), dtype=z_dtype)
    else:
        Z_values = bin.read(data_path / 'z_values', n_cols=len(y_values), dtype=z_dtype).T

    X_values, Y_values = np.meshgrid(x_values, y_values)
    print(X_values.shape, Y_values.shape, Z_values.shape)

    # draw surface:
    __ax__.plot_surface(X_values, Y_values, Z_values, cmap=colour_map)


def new_fig(rows, cols):
    global __fig__
    __fig__ = plt.figure()

    global __gs__
    __gs__ = __fig__.add_gridspec(rows, cols)


def new_ax(frame_path: Path, frame_metadata: json, rows=1, cols=1, index=1):
    canvas_projection = frame_metadata['projection']
    canvas_name = frame_metadata['name']

    # set up fig and ax:
    global __ax__
    __ax__ = __fig__.add_subplot(rows, cols, index, projection=canvas_projection)

    # set axis labels:
    x_label, y_label, z_label = frame_metadata['axis_labels'] or ('x', 'y', 'z')
    __ax__.set_xlabel(x_label)
    __ax__.set_ylabel(y_label)
    if canvas_projection == '3d':
        __ax__.set_zlabel(z_label)

    # set axis scale types:
    x_scale_type, y_scale_type, z_scale_type = [axis_scale_type(s) for s in frame_metadata['axis_scale_types']]
    __ax__.set_xscale(__axis_scale_type_keywords__[x_scale_type])
    __ax__.set_yscale(__axis_scale_type_keywords__[y_scale_type])
    if canvas_projection == '3d':
        __ax__.set_zscale(__axis_scale_type_keywords__[z_scale_type])

    # set canvas name:
    __fig__.canvas.manager.set_window_title(canvas_name)
    __fig__.suptitle(canvas_name)

    # draw plots:
    for j in range(frame_metadata['operation_count']):
        plot_path = frame_path / f'operation_{j}'
        plot_metadata = json.read(plot_path / 'metadata')
        plot_type = plot_metadata['func']
        plot_func = __functions__[plot_type]
        plot_func(plot_metadata, data_path=plot_path)

    # finish:
    if frame_metadata['legend_visible']:
        __ax__.legend()


def run_all(data_path: Path):
    # render canvases:
    frames = __metadata__['frame_count']
    for i in range(frames):
        # initialise:
        frame_path = data_path / f'frame_{i}'
        frame_metadata = json.read(frame_path / 'metadata')
        subframes = frame_metadata['subframe_count']
        rows, cols = frame_metadata['size']
        new_fig(rows, cols)

        # draw frame:
        if subframes == 0:
            new_ax(frame_path, frame_metadata)
        # draw multi-frame:
        else:
            for j in range(subframes):
                subframe_path = frame_path / f'frame_{j}'
                subframe_metadata = json.read(subframe_path / 'metadata')
                new_ax(subframe_path, subframe_metadata, rows, cols, j + 1)

        # render all plots on canvas:
        plt.show()


def main(data_path: Path, settings_path: Path):
    global __metadata__
    __metadata__ = json.read(data_path / 'metadata')
    global __defaults__
    __defaults__ = json.read(settings_path / 'defaults')

    run_all(data_path)
