from argparse import Namespace
from pathlib import Path
import blender.interface as bpyi
import blender.shaders as bpys
import IO.JSON as json
import IO.BIN as bin
import numpy as np
import mathutils
import bpy

__metadata__ = {}
__functions__ = {}

def register(func):
    __functions__[func.__name__] = func
    return func

@register
def add_marker(*, data_path: Path):
    data = json.read(data_path/'data')
    location = mathutils.Vector(data['location'])
    bpy.ops.object.empty_add(
        type='PLAIN_AXES',
        location=location,
        radius=0.2
    )

@register
def add_axes(*, data_path: Path):
    data = json.read(data_path/'data')
    location = mathutils.Vector(data['location'])
    rotation = mathutils.Matrix(data['basis'])

    bpy.ops.object.empty_add(
        type='ARROWS',
        location=location,
        radius=0.2
    )
    bpy.context.active_object.rotation_euler = rotation.to_euler()

@register
def add_points(*, data_path: Path):
    points = bin.read(data_path/'points', n_cols=3, dtype=np.float32)
    data = json.read(data_path/'data')

    points_obj = bpyi.render_vertices(points)
    bpys.apply_point_cloud_shader(points_obj)

@register
def add_mesh(*, data_path: Path):
    points =      bin.read(data_path/'points',      n_cols=3, dtype=np.float32)
    tri_indices = bin.read(data_path/'tri_indices', n_cols=3, dtype=np.uint32)

    mesh_obj = bpyi.render_faces(points, tri_indices)

@register
def add_plane(*, data_path: Path):
    # variables:
    data = json.read(data_path/'data')
    location = mathutils.Vector(data['location'])
    normal = mathutils.Vector(data['normal'])
    rotation = normal.to_track_quat('Z', 'Y')

    # create plane:
    bpy.ops.mesh.primitive_plane_add(
        size=2.0,
        location=location
    )
    bpy.context.active_object.rotation_mode = 'QUATERNION'
    bpy.context.active_object.rotation_quaternion = rotation

def run_all(data_path: Path):

    # generate frames:
    frames = __metadata__['frame_count']
    for i in range(frames):
        frame_path = data_path/f'frame_{i}'
        frame_metadata = json.read(frame_path/'metadata')
        frame_name = frame_metadata.get('name', f'frame-{str(i)}')
        frame_layer_collection = bpyi.create_layer_collection(frame_name, bpyi.workspace)

        # set destination for all generated operations:
        bpy.context.view_layer.active_layer_collection = frame_layer_collection

        # generate operations:
        operations = frame_metadata['operation_count']
        for j in range(operations):
            operation_path = frame_path/f'operation_{j}'
            operation_metadata = json.read(operation_path/'metadata')
            operation_type = operation_metadata['type']
            operation_func = __functions__[operation_type]
            operation_func(data_path=operation_path)

        # set frame visibility:
        bpyi.set_collection_visible(frame_layer_collection, False, frame=i)
        bpyi.set_collection_visible(frame_layer_collection, True,  frame=i + 1)
        bpyi.set_collection_visible(frame_layer_collection, False, frame=i + 2)

def main(args: Namespace):
    bpyi.init()

    global __metadata__
    __metadata__ = json.read(args.input/'metadata')

    run_all(args.input)
