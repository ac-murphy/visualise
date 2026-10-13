#pragma once
#include <filesystem>
#include <system_error>

#include "include_guard.h"
#include "json.hpp"
#include "framework/visualise_base.h"
#include "framework/type_rules.h"
#include "system/system.h"
#include "system/bin.h"
#include "system/json.h"

namespace vis
{
    struct point_cloud
    {
        // std::vector<types::point_3D> points;
        converting::vector<std::array<float, 3>> points;
        std::array<float, 4> rgba;
    };
    struct frame : frame_base
    {
    };
    template <typename value_type>
    struct box
    {
        std::vector<vis::types::vector<value_type, 3>> vertices;
    };
    template <typename value_type>
    struct surface : vis::dataset_3D<value_type>
    {
        template <typename container_x_type,
                  typename container_y_type,
                  typename container_z_type>
        requires (vis::concepts::supports_iteration<container_x_type> &&
                  vis::concepts::supports_iteration<container_y_type> &&
                  vis::concepts::supports_multidimensional_index<container_z_type>)
        surface(const container_x_type& x,
                const container_y_type& y,
                const container_z_type& z)
        : vis::dataset_3D<value_type>(x, y, z)
        {
            size_t i = 0;
            for (const auto& x_value : x)
            {
                size_t j = 0;
                for (const auto& y_value : y)
                {
                    points.push_back({ x_value, y_value, vis::index(z, i, j) });

                    ++j;
                }

                ++i;
            }
        }

        std::vector<vis::types::vector<value_type, 3>> points;
    };
    template <typename value_type>
    struct mesh
    {

    };

    template <typename value_type>
    struct empty_base
    {
        vis::types::vector<value_type, 3> location;
        float size;
    };
    template <typename value_type>
    struct marker : empty_base<value_type>
    {
    };
    template <typename value_type>
    struct arrow : empty_base<value_type>
    {
        vis::types::vector<value_type, 3> direction;
    };
    template <typename value_type>
    struct axes : empty_base<value_type>
    {
        vis::types::matrix<value_type, 3, 3> basis;
    };
    template <typename value_type>
    struct sphere : empty_base<value_type>
    {
        float& radius = this->size;
    };

    class visualise_geometry : public visualise_base<frame>
    {
    private:
        void _run() const override
        {
            sys::run_cmd({ "blender",
                           (_dir / "vis.blend").string(),
                           "--python", PYTHON_SOURCE_DIR"main.py",
                           "--",
                           "--mode", "geometry",
                           "--input", _dir.string(),
                           "--settings", SETTINGS_DIR"geometry/" });
        }

    /// Empty/dimensionless objects.
    public:
        template <typename vector_type>
        void add_marker(const vector_type& location)
        {
            visualise_base::begin_operation("add_marker");

            nlohmann::json data;
            data["location"] = location;
            io::json::write(data, _active_dir, "data", 4);

            visualise_base::end_operation();
        }

        template <typename vector_type, typename matrix_type>
        void add_axes(const vector_type& location,
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

        template <typename vector_type>
        void add_arrow(const vector_type& location,
                       const vector_type& direction)
        {
            visualise_base::begin_operation("add_arrow");

            nlohmann::json data;
            data["location"] = location;
            data["direction"] = direction;

            visualise_base::end_operation();
        }

        void add_points(const vis::point_cloud& point_cloud)
        {
            visualise_base::begin_operation("add_points");

            nlohmann::json data;
            data["rgba"] = point_cloud.rgba;
            io::json::write(data, _active_dir, "data", 4);

            io::bin::write(point_cloud.points.to_std(), _active_dir, "points");

            visualise_base::end_operation();
        }

        template <typename vector_type>
        void add_points(const std::vector<vector_type>& points)
        {
            vis::point_cloud point_cloud;
            point_cloud.points = points;
            point_cloud.rgba = { 1.0f, 1.0f, 1.0f, 1.0f };

            visualise_geometry::add_points(point_cloud);
        }

    /// Solid objects.
    public:
        template <typename vector_type>
        void add_mesh(const std::vector<vector_type>& points,
                      const std::vector<std::array<types::id, 3>>& tri_indices)
        {
            visualise_base::begin_operation("add_mesh_from_tris");

            io::bin::write(points, _active_dir, "points");
            io::bin::write(tri_indices, _active_dir, "tri_indices");

            visualise_base::end_operation();
        }

        template <typename vector_type>
        void add_mesh(const std::vector<vector_type>& points,
                      const std::vector<std::array<types::id, 4>>& quad_indices)
        {
            visualise_base::begin_operation("add_mesh_from_quads");

            io::bin::write(points, _active_dir, "points");
            io::bin::write(quad_indices, _active_dir, "quad_indices");

            visualise_base::end_operation();
        }

        template <typename vector_type>
        void add_plane(const vector_type& location,
                       const vector_type& normal)
        {
            visualise_base::begin_operation("add_plane");

            nlohmann::json data;
            data["location"] = location;
            data["normal"] = normal;
            io::json::write(data, _active_dir, "data");

            visualise_base::end_operation();
        }

        template <typename value_type>
        void add_surface(const vis::surface<value_type>& surface)
        {
            const size_t rows = surface.x.size();
            const size_t columns = surface.y.size();
            // std::vector<std::array<types::id, 3>> tri_indices;
            std::vector<std::array<vis::types::id, 4>> quad_indices;
            for (size_t i = 0; i < rows - 1; ++i)
                for (size_t j = 0; j < columns - 1; ++j)
                {
                    const auto TL = static_cast<vis::types::id>(vis::to_linear_index(    i,     j, rows, columns));
                    const auto TR = static_cast<vis::types::id>(vis::to_linear_index(i + 1,     j, rows, columns));
                    const auto BL = static_cast<vis::types::id>(vis::to_linear_index(i,     j + 1, rows, columns));
                    const auto BR = static_cast<vis::types::id>(vis::to_linear_index(i + 1, j + 1, rows, columns));
                    quad_indices.push_back({ TL, TR, BR, BL });
                    // tri_indices.push_back({ TR, TL, BR });
                    // tri_indices.push_back({ TL, BL, BR });
                }

            // visualise_geometry::add_mesh(surface.points, tri_indices);
            visualise_geometry::add_mesh(surface.points, quad_indices);
        }

        // todo: finish.
        template <typename value_type>
        void add_quadratic_surface()
        {

        }

        template <typename value_type>
        void add_box(const vis::box<value_type>& box)
        {
            std::vector<std::array<types::id, 4>> quad_indices = {
                { 0, 2, 3, 1 },
                { 0, 1, 5, 4 },
                { 0, 4, 6, 2 },
                { 7, 5, 1, 3 },
                { 7, 3, 2, 6 },
                { 7, 6, 4, 5 }
            };

            visualise_geometry::add_mesh(box.vertices, quad_indices);
        }
    };
}
