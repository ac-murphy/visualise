#pragma once
#include <filesystem>
#include <system_error>

#include "visualise_base.h"
#include "json.hpp"
#include "system.h"
#include "types.h"
#include "IO/BIN.h"
#include "IO/JSON.h"

namespace vis
{
    struct point_cloud
    {
        std::vector<types::point_3D> points;
        std::array<float, 4> rgba;
    };

    struct frame : frame_base
    {
    };

    class visualise_3D : public visualise_base<>
    {
    private:
        void _run() const override
        {
            sys::run_cmd({ "blender",
                           (_dir / "vis.blend").string(),
                           "--python", PYTHON_DIR"main.py",
                           "--",
                           "--mode", "3D",
                           "--input", _dir.string() });
        }

    public:
        template <typename point_type>
        void add_marker(const point_type& location)
        {
            visualise_base::begin_operation("add_marker");

            nlohmann::json data;
            data["location"] = location;
            io::json::write(data, _active_dir, "data", 4);

            visualise_base::end_operation();
        }

        template <typename point_type, typename matrix_type>
        void add_axes(const point_type& location,
                      const matrix_type& basis)
        {
            visualise_base::begin_operation("add_axes");

            nlohmann::json data;
            data["location"] = location;
            data["basis"] = {
                { basis(0, 0), basis(0, 1), basis(0, 2) },
                { basis(1, 0), basis(1, 1), basis(1, 2) },
                { basis(2, 0), basis(2, 1), basis(2, 2) }
            };
            io::json::write(data, _active_dir, "data", 4);

            visualise_base::end_operation();
        }

        void add_points(const vis::point_cloud& point_cloud)
        {
            visualise_base::begin_operation("add_points");

            nlohmann::json data;
            data["rgba"] = point_cloud.rgba;
            io::json::write(data, _active_dir, "data", 4);

            io::bin::write(point_cloud.points, _active_dir, "points");

            visualise_base::end_operation();
        }

        template <typename point_type>
        void add_points(const std::vector<point_type>& points)
        {
            vis::point_cloud point_cloud;
            point_cloud.points = points;
            point_cloud.rgba = { 1.0f, 1.0f, 1.0f, 1.0f };

            visualise_3D::add_points(point_cloud);
        }

        template <typename point_type>
        void add_mesh(const std::vector<point_type>& points,
                      const std::vector<std::array<types::id, 3>>& tri_indices)
        {
            visualise_base::begin_operation("add_mesh");

            io::bin::write(points, _active_dir, "points");
            io::bin::write(tri_indices, _active_dir, "tri_indices");

            visualise_base::end_operation();
        }

        template <typename point_type>
        void add_plane(const point_type& location,
                       const point_type& normal)
        {
            visualise_base::begin_operation("add_plane");

            nlohmann::json data;
            data["location"] = location;
            data["normal"] = normal;
            io::json::write(data, _active_dir, "data");

            visualise_base::end_operation();
        }

        template <typename point_type>
        void add_surface(const std::vector<point_type>& points)
        {

        }
    };
}
