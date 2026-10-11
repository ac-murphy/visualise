from argparse import Namespace
from pathlib import Path
from mpl_toolkits.mplot3d.axes3d import Axes3D
from matplotlib.figure import Figure
from enum import Enum
import IO.JSON as json
import IO.BIN as bin
import matplotlib.pyplot as plt
import numpy as np

import matplotlib
matplotlib.use("QtAgg")

__metadata__ = {}
__functions__ = {}
__fig__: Figure
__ax__: Axes3D

def register(func):
    __functions__[func.__name__] = func
    return func


class data_storage_order(Enum):
    row_major=0
    column_major=1


@register
def add_plot_2D(data_path: Path):
    x_values = bin.read(data_path/'x_values', n_cols=1, dtype=np.float32)
    y_values = bin.read(data_path/'y_values', n_cols=1, dtype=np.float32)
    data = json.read(data_path/'data')
    metadata = json.read(data_path/'metadata')

    plot_label = data.get('label', metadata['name'])
    plot_linestyle = data.get('linestyle', '-')

    __ax__.plot(x_values, y_values, label=plot_label, linestyle=plot_linestyle or '-')

@register
def add_plot_3D(data_path: Path):
    x_values = bin.read(data_path/'x_values', n_cols=1, dtype=np.float32)
    y_values = bin.read(data_path/'y_values', n_cols=1, dtype=np.float32)

    data = json.read(data_path/'data')
    storage_order = data_storage_order(data.get('data_storage_order', 0))
    z_data_columns = len(x_values) if storage_order == data_storage_order.row_major else len(y_values)
    Z_values = bin.read(data_path/'z_values', n_cols=z_data_columns, dtype=np.float32)

    X_values, Y_values = np.meshgrid(x_values, y_values)
    print(X_values.shape, Y_values.shape, Z_values.shape)

    # draw surface:
    __ax__.plot_surface(X_values, Y_values, Z_values, cmap='viridis')


def create_canvas(metadata):
    canvas_projection = metadata['projection']
    canvas_name = metadata['name']

    # set up fig and ax:
    global __fig__, __ax__
    if canvas_projection == '3d':
        __fig__ = plt.figure()
        __ax__ = __fig__.add_subplot(1, 1, 1, projection='3d')
    else:
        __fig__, __ax__ = plt.subplots()

    # set axis labels:
    x_label, y_label, z_label = metadata['axis_labels'] or ('x', 'y', 'z')
    __ax__.set_xlabel(x_label)
    __ax__.set_ylabel(y_label)
    if canvas_projection == '3d':
        __ax__.set_zlabel(z_label)

    # set canvas name:
    __fig__.canvas.manager.set_window_title(canvas_name)
    __fig__.suptitle(canvas_name)


def finish_canvas(metadata):
    canvas_legend_visible = metadata['legend_visible']

    # other:
    if canvas_legend_visible:
        __ax__.legend()



def run_all(data_path: Path):

    # render canvases:
    canvases = __metadata__['frame_count']
    for i in range(canvases):
        canvas_path = data_path/f'frame_{i}'
        canvas_metadata = json.read(canvas_path/'metadata')

        # set up:
        create_canvas(canvas_metadata)

        # draw plots:
        plots = canvas_metadata['operation_count']
        for j in range(plots):
            plot_path = canvas_path/f'operation_{j}'
            plot_metadata = json.read(plot_path/'metadata')
            plot_type = plot_metadata['type']
            plot_func = __functions__[plot_type]
            plot_func(data_path=plot_path)

        # finish:
        finish_canvas(canvas_metadata)

        # render all plots on canvas:
        plt.show()

def main(args: Namespace):
    # todo: clear dirs here.

    global __metadata__
    __metadata__ = json.read(args.input/'metadata')

    run_all(args.input)