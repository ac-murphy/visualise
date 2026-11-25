#pragma once
#include <filesystem>
#include <system_error>

#include "include_guard.h"
#include "json.hpp"
#include "Eigen"
#include "framework/visualise_base.h"
#include "framework/type_rules.h"
#include "framework/converting_types.h"
#include "framework/maths.h"
#include "system/system.h"
#include "system/bin.h"
#include "system/json.h"

namespace vis::geometry
{
    enum display_type
    {
        vertices = 0,
        edges = 1,
        faces = 2
    };

    struct mesh_options
    {
        vis::geometry::display_type display_type = faces;
    };

    template <typename value_type> struct point_cloud
    {
        vis::converting::vector<vis::converting::array<value_type, 3>> points;
        vis::converting::array<float, 4> rgba;
        float point_radius;
        bool use_shader;
    };
    template <typename value_type> struct box
    {
        vis::geometry::mesh_options options;
        vis::converting::array<vis::converting::array<value_type, 3>, 8> vertices;
    };
    template <typename value_type> struct surface : vis::base::dataset_3D<value_type>
    {
        surface(const auto& x,
                const auto& y,
                const auto& z)
        : vis::base::dataset_3D<value_type>(x, y, z)
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

        std::vector<std::array<value_type, 3>> points;
    };
    template <typename value_type> struct empty_base
    {
        vis::converting::array<value_type, 3> location;
        float size;
    };
    template <typename value_type> struct marker : empty_base<value_type>
    {
    };
    template <typename value_type> struct arrow : empty_base<value_type>
    {
        vis::converting::array<value_type, 3> direction;
    };
    template <typename value_type> struct axes : empty_base<value_type>
    {
        vis::converting::matrix<value_type, 3, 3> rotation;
    };
    template <typename value_type> struct sphere : empty_base<value_type>
    {
        float& radius = this->size;
    };
    template <typename value_type> struct mesh_base
    {
        vis::geometry::mesh_options options;
        vis::converting::vector<vis::converting::array<value_type, 3>> points;
    };
    template <typename value_type> struct tri_mesh : mesh_base<value_type>
    {
        vis::converting::vector<vis::converting::array<vis::types::id, 3>> tri_indices;
    };
    template <typename value_type> struct quad_mesh : mesh_base<value_type>
    {
        vis::converting::vector<vis::converting::array<vis::types::id, 4>> quad_indices;
    };
    template <typename value_type> struct plane
    {
        vis::converting::array<value_type, 3> location;
        vis::converting::array<value_type, 3> normal;
    };

    template <typename T> struct make_obj;
    template <typename T, typename... Args>
    auto make(Args&&... args)
    {
        return vis::geometry::make_obj<T>{}(std::forward<Args>(args)...);
    }

    template <typename value_type>
    struct make_obj<box<value_type>>
    {
        box<value_type> operator()(const vis::converting::array<value_type, 3>& dimensions,
                                   const vis::converting::array<value_type, 3>& location,
                                   const vis::converting::matrix<value_type, 3, 3>& rotation)
        {
            box<value_type> box;
            const value_type& length = dimensions[0];
            const value_type& width = dimensions[1];
            const value_type& height = dimensions[2];
            const auto location_eigen = vis::converting::to_eigen_vector<value_type, 3>(location);
            const auto rotation_eigen = vis::converting::to_eigen_matrix<value_type, 3, 3>(rotation);

            size_t idx = 0;
            for (auto i : { 1.0f, -1.0f })
                for (auto j : { -1.0f, 1.0f })
                    for (auto k : { -1.0f, 1.0f })
                    {
                        Eigen::Vector<value_type, 3> vertex_eigen(0.5f * length * i, 0.5f * width * j, 0.5f * height * k);
                        vertex_eigen = rotation_eigen * vertex_eigen;
                        vertex_eigen += location_eigen;
                        box.vertices[idx++] = vertex_eigen;
                    }

            return box;
        }
    };

    class operation : public vis::base::operation
    {
    private:
        using super = vis::base::operation;

    public:
        using super::operation;
        friend class frame;
    };

    class frame : public vis::base::frame<frame, vis::geometry::operation>
    {
    private:
        using super = vis::base::frame<frame, vis::geometry::operation>;

    public:
        using super::frame;
        using super::add;
        friend class visualise;

    /// Empty objects: main methods.
    public:
        template <typename value_type>
        void add(const vis::geometry::marker<value_type>& marker)
        {
            auto operation = super::new_operation();
            operation._type = "add_marker";

            nlohmann::json data;
            data["location"] = marker.location.to_std();
            io::json::write(data, operation._dir, "data", 4);

            super::add(operation);
        }
        template <typename value_type>
        void add(const vis::geometry::axes<value_type>& axes)
        {
            auto operation = super::new_operation();
            operation._type = "add_axes";

            nlohmann::json data;
            data["location"] = axes.location.to_std();
            data["rotation"] = axes.rotation.to_std();
            io::json::write(data, operation._dir, "data", 4);

            super::add(operation);
        }
        template <typename value_type>
        void add(const vis::geometry::arrow<value_type>& arrow)
        {
            auto operation = super::new_operation();
            operation._type = "add_arrow";

            nlohmann::json data;
            data["location"] = arrow.location.to_std();
            data["direction"] = arrow.direction.to_std();
            io::json::write(data, operation._dir, "data", 4);

            super::add(operation);
        }
        template <typename value_type>
        void add(const vis::geometry::point_cloud<value_type>& point_cloud)
        {
            auto operation = super::new_operation();
            operation._type = "add_points";

            nlohmann::json data;
            data["rgba"] = point_cloud.rgba;
            data["point_radius"] = point_cloud.point_radius;
            data["use_shader"] = point_cloud.use_shader;
            io::json::write(data, operation._dir, "data", 4);

            io::bin::write(point_cloud.points.to_std(), operation._dir, "points");

            super::add(operation);
        }

    /// Empty objects: shortcuts.
    public:
        template <typename vector_type>
        void add_marker(const vector_type& location)
        {
            vis::geometry::marker<typename vector_type::value_type> marker;
            marker.location = location;

            frame::add(marker);
        }
        template <typename vector_type, typename matrix_type>
        void add_axes(const vector_type& location, const matrix_type& rotation)
        {
            vis::geometry::axes<typename vector_type::value_type> axes;
            axes.location = location;
            axes.rotation = rotation;

            frame::add(axes);
        }
        template <typename vector_type>
        void add_points(const std::vector<vector_type>& points)
        {
            vis::geometry::point_cloud<typename vector_type::value_type> point_cloud;
            point_cloud.points = points;
            point_cloud.point_radius = 0.005f;
            point_cloud.rgba = { 1.0f, 1.0f, 1.0f, 1.0f };
            point_cloud.use_shader = false;

            frame::add(point_cloud);
        }

    /// Solid objects: main methods.
    public:
        template <typename value_type>
        void add(const vis::geometry::tri_mesh<value_type>& tri_mesh)
        {
            auto operation = super::new_operation();
            operation._type = "add_mesh_from_tris";

            io::bin::write(tri_mesh.points.to_std(), operation._dir, "points");
            io::bin::write(tri_mesh.tri_indices.to_std(), operation._dir, "tri_indices");

            nlohmann::json data;
            data["display_type"] = tri_mesh.options.display_type;
            io::json::write(data, operation._dir, "data", 4);

            super::add(operation);
        }
        template <typename value_type>
        void add(const vis::geometry::quad_mesh<value_type>& quad_mesh)
        {
            auto operation = super::new_operation();
            operation._type = "add_mesh_from_quads";

            io::bin::write(quad_mesh.points.to_std(), operation._dir, "points");
            io::bin::write(quad_mesh.quad_indices.to_std(), operation._dir, "quad_indices");

            nlohmann::json data;
            data["display_type"] = quad_mesh.options.display_type;
            io::json::write(data, operation._dir, "data", 4);

            super::add(operation);
        }
        template <typename value_type>
        void add(const vis::geometry::plane<value_type>& plane)
        {
            auto operation = super::new_operation();
            operation._type = "add_plane";

            nlohmann::json data;
            data["location"] = plane.location.to_std();
            data["normal"] = plane.normal.to_std();
            io::json::write(data, operation._dir, "data");

            super::add(operation);
        }
        template <typename value_type>
        void add(const vis::geometry::surface<value_type>& surface)
        {
            const size_t rows = surface.x.size();
            const size_t columns = surface.y.size();
            std::vector<std::array<vis::types::id, 4>> quad_indices;
            for (size_t i = 0; i < rows - 1; ++i)
                for (size_t j = 0; j < columns - 1; ++j)
                {
                    const auto TL = static_cast<vis::types::id>(vis::base::to_linear_index(    i,     j, rows, columns));
                    const auto TR = static_cast<vis::types::id>(vis::base::to_linear_index(i + 1,     j, rows, columns));
                    const auto BL = static_cast<vis::types::id>(vis::base::to_linear_index(i,     j + 1, rows, columns));
                    const auto BR = static_cast<vis::types::id>(vis::base::to_linear_index(i + 1, j + 1, rows, columns));
                    quad_indices.push_back({ TL, TR, BR, BL });
                }

            // visualise_geometry::add_mesh(surface.points, tri_indices);
            frame::add_mesh(surface.points, quad_indices);
        }
        template <typename value_type>
        void add(const vis::geometry::box<value_type>& box)
        {
            std::vector<std::array<types::id, 4>> quad_indices = {
                { 0, 2, 3, 1 },
                { 0, 1, 5, 4 },
                { 0, 4, 6, 2 },
                { 7, 5, 1, 3 },
                { 7, 3, 2, 6 },
                { 7, 6, 4, 5 }
            };

            auto vertex_array = box.vertices.to_std();
            auto vertex_vector = std::vector(vertex_array.begin(), vertex_array.end());
            frame::add_mesh(vertex_vector, quad_indices, box.options);
        }

    /// Solid objects: shortcuts.
    public:
        template <typename vector_type>
        void add_mesh(const std::vector<vector_type>& points,
                      const std::vector<std::array<types::id, 3>>& tri_indices,
                      const vis::geometry::mesh_options& options = {})
        {
            vis::geometry::tri_mesh<typename vector_type::value_type> tri_mesh;
            tri_mesh.points = points;
            tri_mesh.tri_indices = tri_indices;
            tri_mesh.options = options;

            frame::add(tri_mesh);
        }
        template <typename vector_type>
        void add_mesh(const std::vector<vector_type>& points,
                      const std::vector<std::array<types::id, 4>>& quad_indices,
                      const vis::geometry::mesh_options& options = {})
        {
            vis::geometry::quad_mesh<typename vector_type::value_type> quad_mesh;
            quad_mesh.points = points;
            quad_mesh.quad_indices = quad_indices;
            quad_mesh.options = options;

            frame::add(quad_mesh);
        }
        template <typename vector_type>
        void add_plane(const vector_type& location,
                       const vector_type& normal)
        {
            vis::geometry::plane<typename vector_type::value_type> plane;
            plane.location = location;
            plane.normal = normal;

            frame::add(plane);
        }
        template <typename vector_type, typename matrix_type>
        void add_box(const vector_type& dimensions,
                     const vector_type& location,
                     const matrix_type& rotation)
        {
            using value_type = typename vector_type::value_type;

            auto box_ = vis::geometry::make<box<value_type>>(dimensions, location, rotation);

            frame::add(box_);
        }

        // todo: finish.
        template <typename value_type>
        void add_quadratic_surface()
        {

        }
    };

    class visualise : public vis::base::visualise<vis::geometry::frame>
    {
    protected:
        void _run_hook() override
        {
            sys::run_cmd({ BLENDER_EXE_PATH,
                           (_root_dir / "vis.blend").string(),
                           "--python", PYTHON_SOURCE_DIR"main.py",
                           "--",
                           "--mode", "geometry",
                           "--input", _root_dir.string(),
                           "--settings", SETTINGS_DIR"geometry/" });
        }
    };
}
