#pragma once
#include "include_guard.h"
#include "framework/converting_types.h"
#include "framework/visualise_base.h"
#include "system/system.h"
#include "system/BIN.h"

namespace vis
{
    enum projection
    {
        dim_2 = 0,
        dim_3 = 1
    };
    enum storage_order
    {
        row_major = 0,
        column_major = 1
    };
    struct canvas : frame_base
    {
        std::array<std::string, 3> axis_labels;
        vis::projection projection;
        bool legend_visible;

        bool projection_set = false;
    };

    template <typename value_type> struct plot_base
    {
        std::array<float, 4> rgba = { 1.0f, 1.0f, 1.0f, 1.0f };
        std::string style = "";
        std::string label = "";
        vis::storage_order data_storage_order = vis::storage_order::row_major;
    };
    template <typename value_type> struct plot_2D : plot_base<value_type>
    {
        vis::converting::vector<value_type> x_values, y_values;
    };
    template <typename value_type> struct plot_3D : plot_base<value_type>
    {
        converting::vector<value_type> x_values, y_values, z_values;
    };

    class visualise_plotting : public visualise_base<canvas>
    {
    private:
        void _end_frame(nlohmann::json& metadata) const override
        {
            metadata["axis_labels"] = _frame.axis_labels;
            metadata["projection"] = _frame.projection == dim_3 ? "3d" : "2d";
            metadata["legend_visible"] = _frame.legend_visible;
        }
        void _end_vis(nlohmann::json& metadata) const override
        {
            metadata["display_as_anim"] = _display_as_anim;
        }
        void _run() const override
        {
        #if PYTHON_TEST_MODE == 1
            std::string exe_line = PYTHON_EXE_DIR"python.exe " PYTHON_SOURCE_DIR"main.py";
        #else
            std::string exe_line = PYTHON_DIST_DIR"/Visualise/Visualise.exe";
        #endif
            sys::run_cmd({ exe_line,
                           "--mode", "plotting",
                           "--input", _dir.string(),
                           "--settings", SETTINGS_DIR"plotting/" });
        }

    /// Main methods.
    public:
        template <typename value_type>
        void add(const vis::plot_2D<value_type>& plot)
        {
            visualise_base::begin_operation("add_plot_2D");
            visualise_plotting::_use_type<value_type>();
            visualise_plotting::_use_projection(dim_2);

            const auto x_values_std = plot.x_values.to_std();
            const auto y_values_std = plot.y_values.to_std();
            io::bin::write(x_values_std, _active_dir, "x_values");
            io::bin::write(y_values_std, _active_dir, "y_values");
            io::bin::write(visualise_plotting::_nan_mask(x_values_std), _active_dir, "x_values_nan_mask");
            io::bin::write(visualise_plotting::_nan_mask(y_values_std), _active_dir, "y_values_nan_mask");

            nlohmann::json data;
            data["label"] = plot.label;
            data["linestyle"] = plot.style;
            io::json::write(data, _active_dir, "data", 4);

            visualise_base::end_operation();
        }
        template <typename value_type>
        void add(const vis::plot_3D<value_type>& plot)
        {
            visualise_base::begin_operation("add_plot_3D");
            visualise_plotting::_use_projection(dim_3);

            io::bin::write(plot.x_values.to_std(), _active_dir, "x_values");
            io::bin::write(plot.y_values.to_std(), _active_dir, "y_values");
            io::bin::write(plot.z_values.to_std(), _active_dir, "z_values");

            nlohmann::json data;
            data["label"] = plot.label;
            data["data_storage_order"] = plot.data_storage_order;
            io::json::write(data, _active_dir, "data", 4);

            visualise_base::end_operation();
        }

    /// Shortcuts.
    public:
        template <typename value_type>
        void add_plot(const std::vector<value_type>& x,
                      const std::vector<value_type>& y)
        {
            vis::plot_2D<value_type> plot;
            plot.x_values = x;
            plot.y_values = y;

            visualise_plotting::add(plot);
        }
        template <typename value_type>
        void add_plot(const std::vector<value_type>& x,
                      const std::vector<value_type>& y,
                      const std::vector<value_type>& z,
                      const vis::storage_order& order = row_major)
        {
            vis::plot_3D<value_type> plot;
            plot.x_values = x;
            plot.y_values = y;
            plot.z_values = z;
            plot.data_storage_order = order;

            visualise_plotting::add(plot);
        }

    /// Canvas methods.
    public:
        void set_axis_labels(const std::string& x_label,
                             const std::string& y_label)
        {
            _frame.axis_labels[0] = x_label;
            _frame.axis_labels[1] = y_label;
        }
        void set_axis_labels(const std::string& x_label,
                             const std::string& y_label,
                             const std::string& z_label)
        {
            _frame.axis_labels[0] = x_label;
            _frame.axis_labels[1] = y_label;
            _frame.axis_labels[2] = z_label;
        }
        void set_legend_visible(const bool& visible)
        {
            _frame.legend_visible = visible;
        }

    /// Visualise methods.
    public:
        void set_display_as_anim(const bool& display_as_anim)
        {
            _display_as_anim = display_as_anim;
        }

    private:
        void _use_projection(const vis::projection& projection)
        {
            // If projection hasn't yet been set, use specified.
            if (!_frame.projection_set)
            {
                _frame.projection = projection;
                _frame.projection_set = true;
                return;
            }

            // If the projection has been set, throw an error if there is a clash.
            if (_frame.projection != projection)
            {
                throw std::runtime_error("visualise_plotting::_use_projection \"all plots on the same canvas must use the same projection\"");
            }
        }
        template <typename type>
        void _use_type()
        {
            if constexpr (std::same_as<type, float>)
                _operation.data_type = "float32";
            else if constexpr (std::same_as<type, int>)
                _operation.data_type = "int32";
            else
                throw std::runtime_error(std::format("visualise_plotting::_use_type \"unrecognised data type \'{}\'\"", typeid(type).name()));
        }

    private:
        template <typename container_type>
        std::vector<bool> _nan_mask(const container_type& data)
        {
            using value_type = typename container_type::value_type;
            std::vector<bool> nan_mask(data.size());

            for (auto i = 0; i < data.size(); ++i)
            {
                if constexpr (std::same_as<value_type, int>)
                    nan_mask[i] = data[i] == std::numeric_limits<int>::max();
                else if constexpr (std::same_as<value_type, float>)
                    nan_mask[i] = data[i] == std::numeric_limits<float>::max() || std::isnan(data[i]);
                else
                    throw std::runtime_error(std::format("visualise_plotting::_nan_mask \"unrecognised data type \'{}\'\"", typeid(value_type).name()));
            }

            return nan_mask;
        }

    private:
        bool _display_as_anim = false;
    };
}