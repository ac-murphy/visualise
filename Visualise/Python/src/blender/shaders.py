from pathlib import Path
import sys

# boilerplate to ensure local files are visible:
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

import interface as bpyi
import bpy

class point_cloud:
    def __init__(self):
        # node group setup:
        self.inputs = dict()
        self.node_group = bpy.data.node_groups.new(
            'PointSprites',
            'GeometryNodeTree'
        )
        self.in_geometry = self.node_group.interface.new_socket(
            name='Geometry',
            in_out='INPUT',
            socket_type='NodeSocketGeometry'
        )
        self.in_percentage_size = self.node_group.interface.new_socket(
            name='PercentageSize',
            in_out='INPUT',
            socket_type='NodeSocketFloat'
        )
        self.in_percentage_size.default_value = 1.0
        self.out_geometry = self.node_group.interface.new_socket(
            name='Geometry',
            in_out='OUTPUT',
            socket_type='NodeSocketGeometry'
        )

        # shorthand:
        nodes = self.node_group.nodes
        links = self.node_group.links

        # create sprite obj:
        # bpy.ops.mesh.primitive_ico_sphere_add(
        #     radius=0.0025,
        #     subdivisions=1
        # )
        bpy.ops.mesh.primitive_cube_add(size=0.05)
        sprite = bpy.context.active_object
        sprite.name = 'point_sprite'
        sprite.hide_set(True)

        for poly in sprite.data.polygons:
            poly.use_smooth = True

        for collection in list(sprite.users_collection):
            collection.objects.unlink(sprite)

        assets = bpyi.get_or_create_collection('Shader Assets')
        assets.collection.objects.link(sprite)

        # write shader:
        nodes.clear()

        group_in = nodes.new('NodeGroupInput')
        group_in.name = 'Group Input'
        group_in.label = 'Input'
        group_in.location = (-500, 0)

        group_out = nodes.new('NodeGroupOutput')
        group_out.name = 'Group Output'
        group_out.label = 'Output'
        group_out.location = (300, 0)

        mesh_to_points = nodes.new('GeometryNodeMeshToPoints')
        mesh_to_points.name = 'Mesh to Points'
        mesh_to_points.location = (-300, 100)
        mesh_to_points.mode = 'VERTICES'
        mesh_to_points.inputs['Radius'].default_value = 0.002

        object_info = nodes.new('GeometryNodeObjectInfo')
        object_info.name = 'Object Info'
        object_info.location = (-300, 150)
        object_info.inputs['Object'].default_value = sprite
        object_info.inputs['As Instance'].default_value = True
        object_info.transform_space = 'RELATIVE'

        instance_on_points = nodes.new('GeometryNodeInstanceOnPoints')
        instance_on_points.name = "Instance on Points"
        instance_on_points.location = (0, 100)

        # links:
        links.new(
            group_in.outputs["Geometry"],
            mesh_to_points.inputs["Mesh"]
        )
        links.new(
            mesh_to_points.outputs["Points"],
            instance_on_points.inputs["Points"]
        )
        links.new(
            object_info.outputs["Geometry"],
            instance_on_points.inputs["Instance"]
        )
        links.new(
            instance_on_points.outputs["Instances"],
            group_out.inputs["Geometry"]
        )
        links.new(
            group_in.outputs["PercentageSize"],
            instance_on_points.inputs["Scale"]
        )

__shaders__ = {'point_cloud': point_cloud()}

def apply_point_cloud_shader(obj: bpy.types.Object):
    modifier = obj.modifiers.new(
        name='Point Sprites',
        type='NODES'
    )

    shader = __shaders__['point_cloud']
    modifier.node_group = shader.node_group
    modifier[shader.in_percentage_size.identifier] = 1.0