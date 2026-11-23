#pragma once
#include "include_guard.h"
#include "framework/converting_types.h"
#include "framework/visualise_base.h"
#include "system/system.h"
#include "system/bin.h"

namespace vis::plotting
{
    enum projection
    {
        dim_2 = 0,
        dim_3 = 1
    };
    enum storage_order
    {
        row_major    = 0,
        column_major = 1
    };
    enum axis_scale_type
    {
        linear      = 0,
        logarithmic = 1
    };

    template <typename value_type> struct plot_base
    {
        std::array<float, 4> rgba = { 1.0f, 1.0f, 1.0f, 1.0f };
        std::string style = "";
        std::string label = "";
        storage_order data_storage_order = storage_order::row_major;
    };
    template <typename x_value_type, typename y_value_type = x_value_type> struct plot_2D : plot_base<x_value_type>
    {
        vis::converting::vector<x_value_type> x_values;
        vis::converting::vector<y_value_type> y_values;
    };
    template <typename x_value_type, typename y_value_type = x_value_type, typename z_value_type = x_value_type> struct plot_3D : plot_base<x_value_type>
    {
        vis::converting::vector<x_value_type> x_values;
        vis::converting::vector<y_value_type> y_values;
        vis::converting::vector<z_value_type> z_values;
    };

    class operation : public vis::base::operation
    {
    private:
        using super = vis::base::operation;

    public:
        using super::operation;
        friend class frame;
    };

    class frame : public vis::base::frame<frame, vis::plotting::operation>
    {
    private:
        using super = vis::base::frame<frame, vis::plotting::operation>;

    public:
        using super::frame;
        using super::add;
        friend class visualise;

    /// Main methods.
    public:
        template <typename x_value_type, typename y_value_type>
        void add(const plot_2D<x_value_type, y_value_type>& plot)
        {
            frame::_use_projection(dim_2);
            auto operation = super::new_operation();
            operation._type = "add_plot_2D";

            operation.register_type<x_value_type>();
            operation.register_type<y_value_type>();

            const auto x_values_std = plot.x_values.to_std();
            const auto y_values_std = plot.y_values.to_std();

            // Write values.
            io::bin::write(x_values_std, operation._dir, "x_values");
            io::bin::write(y_values_std, operation._dir, "y_values");

            // Write nan masks.
            io::bin::write(frame::_nan_mask(x_values_std), operation._dir, "x_values_nan_mask");
            io::bin::write(frame::_nan_mask(y_values_std), operation._dir, "y_values_nan_mask");

            nlohmann::json data;
            data["label"] = plot.label;
            data["linestyle"] = plot.style;
            io::json::write(data, operation._dir, "data", 4);

            super::add(operation);
        }
        template <typename x_value_type, typename y_value_type = x_value_type, typename z_value_type = y_value_type>
        void add(const plot_3D<x_value_type, y_value_type, z_value_type>& plot)
        {
            frame::_use_projection(dim_3);
            auto operation = super::new_operation();
            operation._type = "add_plot_3D";

            operation.register_type<x_value_type>();
            operation.register_type<y_value_type>();
            operation.register_type<z_value_type>();

            io::bin::write(plot.x_values.to_std(), operation._dir, "x_values");
            io::bin::write(plot.y_values.to_std(), operation._dir, "y_values");
            io::bin::write(plot.z_values.to_std(), operation._dir, "z_values");

            nlohmann::json data;
            data["label"] = plot.label;
            data["data_storage_order"] = plot.data_storage_order;
            io::json::write(data, operation._dir, "data", 4);

            super::add(operation);
        }

    /// Shortcuts.
    public:
        template <typename value_type>
        void add_plot(const std::vector<value_type>& x,
                      const std::vector<value_type>& y)
        {
            plot_2D<value_type> plot;
            plot.x_values = x;
            plot.y_values = y;

            frame::add(plot);
        }
        template <typename value_type>
        void add_plot(const std::vector<value_type>& x,
                      const std::vector<value_type>& y,
                      const std::vector<value_type>& z,
                      const storage_order& order = row_major)
        {
            plot_3D<value_type> plot;
            plot.x_values = x;
            plot.y_values = y;
            plot.z_values = z;
            plot.data_storage_order = order;

            frame::add(plot);
        }

    public:
        void set_axis_labels(const std::string& x_label,
                             const std::string& y_label)
        {
            _axis_labels[0] = x_label;
            _axis_labels[1] = y_label;
        }
        void set_axis_labels(const std::string& x_label,
                             const std::string& y_label,
                             const std::string& z_label)
        {
            _axis_labels[0] = x_label;
            _axis_labels[1] = y_label;
            _axis_labels[2] = z_label;
        }
        void set_axis_scale_types(const axis_scale_type& x_axis_scale_type,
                                  const axis_scale_type& y_axis_scale_type)
        {
            _axis_scale_types[0] = x_axis_scale_type;
            _axis_scale_types[1] = y_axis_scale_type;
        }
        void set_axis_scale_types(const axis_scale_type& x_axis_scale_type,
                                  const axis_scale_type& y_axis_scale_type,
                                  const axis_scale_type& z_axis_scale_type)
        {
            _axis_scale_types[0] = x_axis_scale_type;
            _axis_scale_types[1] = y_axis_scale_type;
            _axis_scale_types[2] = z_axis_scale_type;
        }
        void set_legend_visible(const bool& visible)
        {
            _legend_visible = visible;
        }
        void set_display_as_anim(const bool& display_as_anim)
        {
            _display_as_anim = display_as_anim;
        }

    private:
        void _use_projection(const projection& projection)
        {
            // If projection hasn't yet been set, use specified.
            if (!_projection_set)
            {
                _projection = projection;
                _projection_set = true;
                return;
            }

            // If the projection has been set, throw an error if there is a clash.
            if (_projection != projection)
            {
                throw std::runtime_error("visualise_plotting::_use_projection \"all plots on the same canvas must use the same projection\"");
            }
        }

    private:
        /// Create a mask that maps, for a container @code C@endcode with value type @code T@endcode,
        ///  - @code t == std::numeric_limits<T>::max() -> 1@endcode,
        ///  - @code std::isnan(t) -> 1@endcode,
        ///  - @code otherwise -> 0@endcode.
        template <typename container_type>
        std::vector<bool> _nan_mask(const container_type& data)
        {
            using value_type = typename container_type::value_type;
            std::vector<bool> nan_mask(data.size());

            for (size_t i = 0; i < data.size(); ++i)
                nan_mask[i] = data[i] == std::numeric_limits<value_type>::max() || std::isnan(data[i]);

            return nan_mask;
        }

    private:
        std::array<std::string, 3> _axis_labels{};
        std::array<axis_scale_type, 3> _axis_scale_types{};
        vis::plotting::projection _projection{};
        bool _display_as_anim = false;
        bool _legend_visible = false;

        // Internal flags.
        bool _projection_set = false;
    };

    class visualise : public vis::base::visualise<vis::plotting::frame>
    {
    protected:
        void _write_frame_metadata_hook(const frame& frame, nlohmann::json& data) override
        {
            data["size"] = frame._size;

            if (frame._subframes.empty())
            {
                data["axis_labels"]      = frame._axis_labels;
                data["axis_scale_types"] = frame._axis_scale_types;
                data["legend_visible"]   = frame._legend_visible;
                data["projection"]       = frame._projection == dim_3 ? "3d" : "rectilinear";
            }

            if (frame._operations.empty())
            {
            }
        }
        void _run_hook() override
        {
#if PYTHON_TEST_MODE == 1
            std::string exe_line = PYTHON_EXE_PATH " " PYTHON_SOURCE_DIR "main.py";
#else
            std::string exe_line = PYTHON_DIST_DIR"/Visualise/Visualise.exe";
#endif
            sys::run_cmd({ exe_line,
                           "--mode", "plotting",
                           "--input", _root_dir.string(),
                           "--settings", SETTINGS_DIR"plotting/" });
        }
        size_t _supported_depth() override { return 1; }
    };
}