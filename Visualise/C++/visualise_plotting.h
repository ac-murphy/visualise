#pragma once
#include "include_guard.h"
#include "visualise_base.h"
#include "system.h"
#include "IO/BIN.h"

namespace vis
{
    enum projection
    {
        none,
        dim_2,
        dim_3
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
    };

    template <typename value_type>
    struct plot_base
    {
        std::array<float, 4> rgba;
        std::string style;
        std::string label;
        vis::storage_order data_storage_order;
    };
    template <typename value_type>
    struct plot_2D : plot_base<value_type>
    {
        std::vector<value_type> x_values, y_values;

        plot_2D() = default;
        template <typename container_x_type,
                  typename container_y_type>
        plot_2D(const container_x_type& x_values,
                const container_y_type& y_values)
        : x_values(vis::vector_cast<value_type>(x_values)),
          y_values(vis::vector_cast<value_type>(y_values))
        {}
    };
    template <typename value_type>
    struct plot_3D : plot_base<value_type>
    {
        std::vector<value_type> x_values, y_values, z_values;

        plot_3D() = default;
        template <typename container_x_type,
                  typename container_y_type,
                  typename container_z_type>
        plot_3D(const container_x_type& x_values,
                const container_y_type& y_values,
                const container_z_type& z_values)
        : x_values(vis::vector_cast<value_type>(x_values)),
          y_values(vis::vector_cast<value_type>(y_values)),
          z_values(vis::vector_cast<value_type>(z_values))
        {}
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
        void _run() const override
        {
        #if PYTHON_TEST_MODE == 1
            sys::run_cmd({ PYTHON_EXE_DIR"python.exe", PYTHON_SOURCE_DIR"main.py",
                           "--mode", "plotting",
                           "--input", _dir.string() });
        #else
            sys::run_cmd({ PYTHON_DIST_DIR"/Visualise/Visualise.exe",
                           "--mode", "plotting",
                           "--input", _dir.string() });
        #endif
        }

    public:
        template <typename value_type>
        void add_plot(const vis::plot_2D<value_type>& plot)
        {
            visualise_base::begin_operation("add_plot_2D");
            visualise_plotting::_use_projection(dim_2);

            io::bin::write(plot.x_values, _active_dir, "x_values");
            io::bin::write(plot.y_values, _active_dir, "y_values");

            nlohmann::json data;
            data["label"] = plot.label;
            data["linestyle"] = plot.style;
            io::json::write(data, _active_dir, "data", 4);

            visualise_base::end_operation();
        }

        template <typename value_type>
        void add_plot(const std::vector<value_type>& x,
                      const std::vector<value_type>& y)
        {
            vis::plot_2D<value_type> plot;
            plot.x_values = x;
            plot.y_values = y;

            visualise_plotting::add_plot(plot);
        }

        template <typename value_type>
        void add_plot(const vis::plot_3D<value_type>& plot)
        {
            visualise_base::begin_operation("add_plot_3D");
            visualise_plotting::_use_projection(dim_3);

            io::bin::write(plot.x_values, _active_dir, "x_values");
            io::bin::write(plot.y_values, _active_dir, "y_values");
            io::bin::write(plot.z_values, _active_dir, "z_values");

            nlohmann::json data;
            data["label"] = plot.label;
            data["data_storage_order"] = plot.data_storage_order;
            io::json::write(data, _active_dir, "data", 4);

            visualise_base::end_operation();
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

            visualise_plotting::add_plot(plot);
        }

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

    private:
        void _use_projection(const vis::projection& projection)
        {
            // If projection hasn't yet been set, use specified.
            if (_frame.projection == vis::projection::none)
            {
                _frame.projection = projection;
                return;
            }

            // If the projection has been set, throw an error if there is a clash.
            if (_frame.projection != projection)
            {
                throw std::runtime_error("visualise_plotting::_use_projection \"all plots on the same canvas must use the same projection\"");
            }
        }
    };
}