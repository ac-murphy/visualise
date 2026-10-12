from argparse import Namespace
from functools import singledispatch
import bpy
import numpy as np

@singledispatch
def get_layer_collection(layer_collection_identifier) -> bpy.types.LayerCollection:
    raise TypeError(f'get_layer_collection "unsupported type: {type(layer_collection_identifier)}"')

@get_layer_collection.register
def _(collection: bpy.types.Collection) -> bpy.types.LayerCollection:

    def recurse(layer_collection: bpy.types.LayerCollection):
        if layer_collection.collection == collection:
            return layer_collection

        for child in layer_collection.children:
            result = recurse(child)
            if result is not None:
                return result

        return None

    return recurse(bpy.context.view_layer.layer_collection)

@get_layer_collection.register
def _(collection_name: str):
    collection: bpy.types.Collection = bpy.data.collections.get(collection_name)
    if collection is None:
        return None

    return get_layer_collection(collection)

def create_layer_collection(collection_name: str, parent: bpy.types.LayerCollection = None) -> bpy.types.LayerCollection:
    if bpy.data.collections.get(collection_name) is not None:
        raise ValueError(f'create_layer_collection "collection with name \'{collection_name}\' already exists"')

    collection: bpy.types.Collection = bpy.data.collections.new(collection_name)
    parent_collection = parent.collection if parent is not None else bpy.context.scene.collection
    if collection.name not in parent_collection.children:
        parent_collection.children.link(collection)

    return get_layer_collection(collection)

def get_or_create_collection(name: str, *, parent: bpy.types.LayerCollection = None) -> bpy.types.LayerCollection:
    layer_collection = get_layer_collection(name)

    if layer_collection is None:
        return create_layer_collection(name, parent)
    else:
        return layer_collection

def set_collection_visible(layer_collection: bpy.types.LayerCollection, visible: bool, frame: int):
    for obj in [obj for obj in layer_collection.collection.all_objects if obj.type == 'MESH' or obj.type == 'EMPTY']:
        obj.hide_viewport = not visible
        obj.keyframe_insert('hide_viewport', frame=frame)


def _create_empty_obj(name: str = 'obj', parent = None) -> tuple[bpy.types.Object, bpy.types.Mesh]:
    # initialise:
    mesh = bpy.data.meshes.new(name)
    obj = bpy.data.objects.new(name, mesh)

    # align to parent if given:
    if parent is not None:
        obj.matrix_parent_inverse = parent.matrix_world.inverted()
        obj.parent = parent

    # link to collection and return:
    bpy.context.view_layer.active_layer_collection .collection.objects.link(obj)
    return obj, mesh

def render_vertices(points: np.ndarray, parent = None) -> bpy.types.Object:
    # initialise:
    obj, mesh = _create_empty_obj('points', parent)

    mesh.from_pydata(points, [], [])
    mesh.update()
    return obj

def render_edges(): ...
def render_faces(points: np.ndarray, tri_indices, parent = None) -> bpy.types.Object:
    # initialise:
    obj, mesh = _create_empty_obj('mesh', parent)

    mesh.from_pydata(points, [], tri_indices)
    mesh.update()
    return obj

def init():
    # clear workspace:
    for obj in workspace.collection.objects:
        bpy.data.objects.remove(obj, do_unlink=True)
    for child in list(workspace.children):
        bpy.data.collections.remove(child.collection)

    # reset frame:
    bpy.context.scene.frame_set(0)


workspace = get_or_create_collection('Workspace')

