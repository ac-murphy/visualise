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

### Configs

### Frame methods

### Visualisation methods

### Visualisation shortcuts

---

