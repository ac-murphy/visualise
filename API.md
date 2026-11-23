# Visualise for C++ (API documentation)

This documentation covers all available frameworks (currently, *plotting* and *geometry*).

## Shared
Note, the functions in the initialisation section use `visualise_base` which is a type alias for `visualise_base<T, U>` where `T = frame_base` and `U = operation_base`. If modification of the library is not of concern, this explanation can be safely ignored.

### Initialisation

`visualise_base::begin_frame`
> 1. `name (std::string)` Name of frame. Shows up differently depending on the framework that is being used. For example, with *plotting* it sets both the window title and the plot title.  
*default: `""` (empty string)*

Initialise a new frame.

---

`visualise_base::end_frame`

Save the current frame.

---

`visualise_base::begin_operation`
> 1. `type (std::string)` Name of operation type.  
*default: `""` (empty string)*
  
Initialise a new operation.

---

`visualise_base::end_operation`

Save the current operation.

---


## Plotting

### Configs

`vis::plot_base`
> 1. `value_type (any)` Data type for scalar values.

> - `rgba (std::array<float, 4>)` Plot colour (red, green, blue, alpha) with normalised values.  
*default: `{ 1.0f, 1.0f, 1.0f, 1.0f }` (white, solid)*
> - `style (std::string)` Line-style keyword.  
*default: `""` (empty string; default style)*
> - `label (std::string)` Plot label (appears in legend).  
*default: `""` (empty string; no label)*
> - `data_storage_order (vis::storage_order)` Signals how 2-dimensional data is stored in the plot. Note that this only has an affect for plots that store data in 2-dimensional arrays.  
*default: `row_major`* 

---

`vis::plot_2D` *(inherits `vis::plot_base<value_type>`)*
> 1. `value_type (any)` Data type for scalar values.

> - `x_values (vis::converting::vector<value_type>)` Data for x-axis.
> - `y_values (vis::converting::vector<value_type>)` Data for y-axis.

---

`vis::plot_3D` *(inherits `vis::plot_base<value_type>`)*
> 1. `value_type (any)` Data type for scalar values.

> - `x_values (vis::converting::vector<value_type>)` Data for x-axis.
> - `y_values (vis::converting::vector<value_type>)` Data for y-axis.
> - `z_values (vis::converting::vector<value_type>)` Data for z-axis.

### Frame methods

`vis::visualise_plotting::set_axis_labels`
> 1. `x_label (std::string)` x-axis label
> 2. `y_label (std::string)` y-axis label

Set axis labels for a 2D plot.

---

`vis::visualise_plotting::set_axis_labels`
> 1. `x_label (std::string)` x-axis label
> 2. `y_label (std::string)` y-axis label
> 3. `z_label (std::string)` z-axis label

Set axis labels for a 3D plot.

---

`vis::visualise_plotting::set_legend_visible`
> 1. `visible (bool)`

Set whether the plot legend is visible. 

### Visualisation methods

`vis::visualise_plotting::add`
> 1. `plot (vis::plot_2D<T>)` 2D plot config

Add custom-style 2-dimensional plot.

Type: `add_plot_2D`

---

`vis::visualise_plotting::add`
> 1. `plot (vis::plot_3D<T>)` 3D plot config

Add custom-style 3-dimensional plot.

Type: `add_plot_3D`

---

### Visualisation shortcuts

`vis::visualise_plotting::add_plot`
> 1. `x (std::vector<T>)` x-axis data
> 2. `y (std::vector<T>)` y-axis data

Add default-style 2-dimensional plot.

Type: `add_plot_2D`

---

`vis::visualise_plotting::add_plot`
> 1. `x (std::vector<T>)` x-axis data
> 2. `y (std::vector<T>)` y-axis data
> 3. `z (std::vector<T>)` z-axis data

Type: `add_plot_3D`

---

## Geometry

Note, some data types in this section can seem verbose, so for easier reading the following alias has been made: `conv = vis::converting`.

### Configs

`vis::point_cloud`
> 1. `value_type (any)` Data type for scalar values.

> - `points (conv::vector<conv::array<value_type, 3>>)` List of 3-dimensional points.
> - `rgba (conv::array<float, 4>)` Colour of point markers.

---

`vis::box`
> 1. `value_type (any)` Data type for scalar values.

> - `vertices (conv::array<conv::array<value_type, 3>, 8>)` List of 8 points that describe the corners of the box. (#todo: describe winding order)

---

`vis::surface` *(inherits `vis::dataset_3D<value_type>`)*
> 1. `value_type (any)` Data type for scalar values.

> - `points (std::vector<std::array<value_type, 3>>)` List of 3-dimensional points. Note: do not set this value manually; the constructor generates this automatically.

This struct must be instantiated via its constructor.

---

`vis::plane`
> 1. `value_type (any)` Data type for scalar values.

> - `location (conv::array<value_type, 3>)` Centre of the plane.
> - `normal (conv::array<value_type, 3>)` Direction the plane is facing.

---

#### Empty objects

`vis::empty_base`
> 1. `value_type (any)` Data type for scalar values.

> - `location (conv::array<value_type, 3>)` Position in global space.
> - `size (float)` Size of empty. This is usually the radius of the object, but is object-dependent.

This should not be used on its own.

---

`vis::marker` *(inherits `vis::empty_base<value_type>`)*
> 1. `value_type (any)` Data type for scalar values.

An axis-aligned empty axes object.

---

`vis::arrow` *(inherits `vis::empty_base<value_type>`)*
> 1. `value_type (any)` Data type for scalar values.

> - `direction (conv::array<value_type, 3>)` Direction to point.

A plain arrow pointing in a specific direction.

---

`vis::axes` *(inherits `vis::empty_base<value_type>`)*
> 1. `value_type (any)` Data type for scalar values.

> - `rotation (conv::matrix<value_type, 3, 3>)` Rotation (local basis).

Similar to `vis::marker` but is not necessarily axis-aligned and has *x, y, z* labels for each axis.

---

`vis::sphere` *(inherits `vis::empty_base<value_type>`)*
> 1. `value_type (any)` Data type for scalar values.

> - `radius (float&)` Variable alias for `vis::empty_base<value_type>::size` - either can be set to specify the radius of the sphere.
 
A wireframe sphere.

---

#### Mesh objects

`vis::mesh_base`
> 1. `value_type (any)` Data type for scalar values.

> - `points (conv::vector<conv::array<value_type, 3>>)` List of 3-dimensional points.

This should not be used on its own.

---

`vis::tri_mesh` *(inherits `vis::mesh_base<value_type>`)*
> 1. `value_type (any)` Data type for scalar values.

> - `tri_indices (conv::vector<conv::array<vis::types::id, 3>>)` List of integer-triples that each represent the indices of vertices (in `vis::mesh_base<value_type>::points`) that make up a triangular face.

A way of representing a mesh with triangular faces.

---

`vis::quad_mesh` *(inherits `vis::mesh_base<value_type>`)*
> 1. `value_type (any)` Data type for scalar values.

> - `quad_indices (conv::vector<conv::array<vis::types::id, 4>>)` List of integer-quadruples that each represent the indices of vertices (in `vis::mesh_base<value_type>::points`) that make up a 4-sided face.

A way of representing a mesh with 4-sided faces.

---

### Frame methods

### Visualisation methods

### Visualisation shortcuts

---

