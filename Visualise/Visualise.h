#pragma once
#include <cstdint>
#include <filesystem>
#include <iostream>

#include "json.hpp"
#include "C++/IO/JSON.h"
#include "C++/IO/BIN.h"
#include "C++/types.h"
#include "C++/system.h"

namespace vis
{
    struct frame
    {
        std::string name;

        uint32_t operation_count;
        std::vector<types::point_3D> points;
    };
    struct operation
    {
        std::string name;

        std::string type;
    };

    class visualise_3D
    {
    public:
        visualise_3D()
        {
            visualise_3D::refresh();
        }
        ~visualise_3D() = default;

    public:
        void refresh()
        {
            // Ensure directory exists.
            std::error_code ec;
            std::filesystem::create_directories(_dir, ec);
            if (ec)
            {
                // todo: write to default directory that definitely exists and can be written to.
                throw std::runtime_error(std::format("visualise_3D::refresh \"%s\"", ec.message()));
            }

            // Clear directory.
            for (const auto& entry : std::filesystem::directory_iterator(_dir))
            {
                std::filesystem::remove_all(entry.path());
            }
        }

        void begin_frame(const std::string& name = "")
        {
            if (_frame_open)
                throw std::runtime_error("visualise_3D::begin_frame \"frame already initialised\"");
            if (_operation_open)
                throw std::runtime_error("visualise_3D::begin_frame \"cannot initialise frame with unfinished operation\"");

            // Create new frame instance.
            _frame = vis::frame{};
            _frame.name = name;
            _frame_open = true;

            // Create directory.
            std::filesystem::create_directories(visualise_3D::_frame_dir());
            visualise_3D::_point_to_frame();
        }
        void end_frame()
        {
            if (!_frame_open)
                throw std::runtime_error("visualise_3D::end_frame \"no frame to end\"");
            if (_operation_open)
                throw std::runtime_error("visualise_3D::end_frame \"cannot end frame with unfinished operation\"");

            // Write metadata.
            nlohmann::json metadata;
            metadata["name"] = _frame.name;
            metadata["operation_count"] = _frame.operation_count;
            io::json::write(metadata, visualise_3D::_frame_dir(), "metadata", 4);

            // Increment counter.
            ++_frame_count;
            _frame_open = false;
        }
        void begin_operation(const std::string& type = "")
        {
            if (!_frame_open)
                throw std::runtime_error("visualise_3D::begin_operation \"cannot initialise operation without a frame\"");
            if (_operation_open)
                throw std::runtime_error("visualise_3D::begin_operation \"operation already initialised\"");

            // Create new operation instance.
            _operation = vis::operation{};
            _operation.type = type;
            _operation_open = true;

            // Create directory.
            std::filesystem::create_directories(visualise_3D::_operation_dir());
            visualise_3D::_point_to_operation();
        }
        void end_operation()
        {
            if (!_frame_open)
                throw std::runtime_error("visualise_3D::end_operation \"cannot end operation without a frame\"");
            if (!_operation_open)
                throw std::runtime_error("visualise_3D::end_operation \"no operation to end\"");

            // Write metadata.
            nlohmann::json metadata;
            metadata["type"] = _operation.type;
            io::json::write(metadata, visualise_3D::_operation_dir(), "metadata", 4);

            // Increment counter.
            ++_frame.operation_count;
            _operation_open = false;
        }

        void run()
        {
            nlohmann::json metadata;
            metadata["frame_count"] = _frame_count;
            io::json::write(metadata, _dir, "metadata", 4);

            if (_frame_count == 0)
                std::cerr << "visualise_3D::run \"warning; called run() but generated no frames\"";

            sys::run_cmd({ "blender",
                           (_dir / "vis.blend").string(),
                           "--python", PYTHON_DIR"main.py",
                           "--",
                           "--mode", "3D",
                           "--input", _dir.string(),});
        }

    public:
        template <concepts::point_3D point_type>
        void add_marker(const point_type& location)
        {
            visualise_3D::begin_operation("add_marker");

            nlohmann::json data;
            data["location"] = location;
            io::json::write(data, _active_dir, "data");

            visualise_3D::end_operation();
        }

        template <concepts::point_3D point_type, concepts::matrix_3D matrix_type>
        void add_axes(const point_type& location,
                      const matrix_type& basis)
        {
            visualise_3D::begin_operation("add_axes");

            nlohmann::json data;
            data["location"] = location;
            data["basis"] = {
                { basis[0], basis[1], basis[2] },
                { basis[3], basis[4], basis[5] },
                { basis[6], basis[7], basis[8] }
            };
            io::json::write(data, _active_dir, "data");

            visualise_3D::end_operation();
        }

        template <concepts::point_3D point_type>
        void set_points(const std::vector<point_type>& points)
        {
            visualise_3D::begin_operation("set_points");

            io::bin::write(points, _active_dir, "points");
            _frame.points = reinterpret_cast<const std::vector<types::point_3D>&>(points);

            visualise_3D::end_operation();
        }

        template <concepts::point_3D point_type>
        void add_mesh(const std::vector<point_type>& points,
                      const std::vector<std::array<types::id, 3>>& tri_indices)
        {
            visualise_3D::begin_operation("add_mesh");

            io::bin::write(points, _active_dir, "points");
            io::bin::write(tri_indices, _active_dir, "tri_indices");

            visualise_3D::end_operation();
        }

        template <concepts::point_3D point_type>
        void add_plane(const point_type& location,
                       const point_type& normal)
        {
            visualise_3D::begin_operation("add_plane");

            nlohmann::json data;
            data["location"] = location;
            data["normal"] = normal;
            io::json::write(data, _active_dir, "data");

            visualise_3D::end_operation();
        }

    public:
        void set_dir(const std::filesystem::path& dir)
        {
            _dir = dir;
        }

    private:
        void _point_to_frame() { _active_dir = visualise_3D::_frame_dir(); }
        void _point_to_operation() { _active_dir = visualise_3D::_operation_dir(); }

    private:
        std::filesystem::path _frame_dir() const
        {
            return _dir / ("frame_" + std::to_string(_frame_count));
        }
        std::filesystem::path _operation_dir() const
        {
            return visualise_3D::_frame_dir() / ("operation_" + std::to_string(_frame.operation_count));
        }

    private:
        uint32_t _frame_count = 0;
        vis::operation _operation;
        vis::frame _frame;
        std::filesystem::path _dir = OUTPUT_DIR;
        std::filesystem::path _active_dir = _dir;

        bool _operation_open = false;
        bool _frame_open = false;
    };
}