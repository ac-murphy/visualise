# Visualise for C++

## Introduction
There are two main feature sets of this library: *plotting* and *geometry*. At present they use separate, open-source backends. Plotting is supported by Python's `matplotlib` package, and geometry uses `Blender` and its bundled Python environment.

## Usage

The syntax for this project is object-orientated, similar to `matplotlib`. To begin visualising, first instantiate the type of visualiser needed. For this example, we will use the plotting framework.
```c++
vis::visualise_plotting visualiser;
```

Regardless of framework, visualisation is done in frames (even if an animation is not the desired product). Frames are created with `begin_frame` and must be saved with `end_frame`. If frames are left dangling at any stage the visualisation won't run. 
```c++
std::vector<float> x = ...;
std::vector<float> y = ...;

visualiser.begin_frame();
vis::plot_2D<float> plot;
plot.x = x;
plot.y = y;
visualiser.add(plot);
visualiser.end_frame();
```

Depending on what `x` and `y` represent, this creates a frame for a single 2D plot. Multiple plots can be drawn on the same frame, and there can be multiple frames in a single visualisation. At this point however, nothing will happen. The following line is needed to actually run the visualisation.
```c++
visualiser.run();
```

### Supported data types

The aim of this library is to be as generic as possible. As an inevitable side effect, it is not performant and shouldn't be used in an environment that has such requirements. The upside, however, is that you can do any of the following (and far more!):

```c++
std::vector<std::array<float, 3>> x_vector = { ... };
std::deque<std::vector<float>> y_deque = { ... }; // Assume each element has 3 components.
std::array<Eigen::Vector<float, 3>> z_array = { ... };

vis::visualise_plotting visualiser;

visualiser.begin_frame();
vis::plot_3D<float> plot;
plot.x = x_vector;
plot.y = y_deque;
plot.z = z_array;

visualiser.end_frame();

visualiser.run();
```

So long as each data list is properly formed (i.e. `y_deque` has elements of length exactly 3), this will run exactly as expected. This is thanks to the underlying `vis::converting` sub-library, which is able to convert anything that supports an STL-like interface (`.begin()` and `.end()` for example) to an appropriately-wrapping container via *copy-assignment*. However, there are only a handful of containers that are officially tested, most notably:

- `std::vector<T>`
- `std::array<T, N>`
- `Eigen::Vector<T>`
- `Eigen::Vector<T, N>`
- `Eigen::Matrix<T, M, N>`

So long as the desired datatype is listed above, it will work both on its own or nested in any combination of the above. For example,

```c++

```

## Development
To set up Python so that it uses source files, the project must be opened as the top level (not as a subproject). This ensures that its CMakeLists.txt files are run, which tells the program to use source files. The next step is ensuring there is a virtual environment (venv) located in `Visualise/Python` and that it has the correct packages installed.

### Windows
Open a terminal inside `Visualise/Visualise/Python` and run the following commands:

1. Create a virtual environment called ".venv" in the Python folder.
```
.../Visualise/Python> python -m venv .venv
```
2. Install packages using the requirements list.
```
.../Visualise/Python> .venv\Scripts\python.exe -m pip install -r requirements.txt
```

### Linux
Open a terminal inside `Visualise/Visualise/Python` and run the following commands:

1. Create a virtual environment called ".venv" in the Python folder.
```
.../Visualise/Python> python -m venv .venv
```
2. Install packages using the requirements list.
```
.../Visualise/Python> .venv/bin/python -m pip install -r requirements.txt
```