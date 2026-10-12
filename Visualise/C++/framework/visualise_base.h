#pragma once
#include <filesystem>
#include <iostream>
#include <system_error>

#include "include_guard.h"
#include "json.hpp"
#include "system/json.h"

namespace vis
{
    struct frame_base
    {
        std::string name;
    };
    struct operation_base
    {
        std::string name;
        std::string type;
    };
    template <typename value_type>
    struct dataset_2D
    {
        std::vector<value_type> x, y;


        template <typename container_x_type,
                  typename container_y_type>
        dataset_2D(const container_x_type& x,
                   const container_y_type& y)
        : x(vis::vector_cast<value_type>(x)),
          y(vis::vector_cast<value_type>(y))
        {}
    };
    template <typename value_type>
    struct dataset_3D
    {
        std::vector<value_type> x, y, z;

        dataset_3D() = default;
        template <typename container_x_type,
                  typename container_y_type,
                  typename container_z_type>
        dataset_3D(const container_x_type& x,
                   const container_y_type& y,
                   const container_z_type& z)
        : x(vis::vector_cast<value_type>(x)),
          y(vis::vector_cast<value_type>(y)),
          z(vis::vector_cast<value_type>(z))
        {}
    };

    inline size_t to_linear_index(const size_t& i, const size_t& j, const size_t& I, const size_t& J)
    {
        return i * J + j;
    }

    template <typename frame_type = frame_base, typename operation_type = operation_base>
    requires (std::is_base_of_v<frame_base, frame_type> && std::is_base_of_v<operation_base, operation_type>)
    class visualise_base
    {
    public:
        visualise_base(const std::filesystem::path& dir = OUTPUT_DIR)
        : _dir(dir)
        {
            if (!std::filesystem::exists(_dir))
                throw std::runtime_error("visualise_base::visualise_base \"specified directory is invalid\"");

            visualise_base::refresh();
        }
        virtual ~visualise_base() = default;

    public:
        void refresh() const
        {
            // Ensure directory exists.
            std::error_code ec;
            std::filesystem::create_directories(_dir, ec);
            if (ec)
            {
                // todo: write to default directory that definitely exists and can be written to.
                throw std::runtime_error(std::format("visualise_base::refresh \"%s\"", ec.message()));
            }

            // Clear directory.
            for (const auto& entry : std::filesystem::directory_iterator(_dir))
            {
                std::filesystem::remove_all(entry.path());
            }
        }
        void set_dir(const std::filesystem::path& dir)
        {
            _dir = dir;
        }

    public:
        void begin_frame(const std::string& name = "")
        {
            if (_frame_open)
                throw std::runtime_error("visualise_base::begin_frame \"frame already initialised\"");
            if (_operation_open)
                throw std::runtime_error("visualise_base::begin_frame \"cannot initialise frame with unfinished operation\"");

            // Create new frame instance.
            _frame = frame_type{};
            _frame.name = name;
            _frame_open = true;

            // Reset operation counter.
            _operation_count = 0;

            // Create directory.
            std::filesystem::create_directories(visualise_base::_frame_dir());
            visualise_base::_point_to_frame();
        }
        void end_frame()
        {
            if (!_frame_open)
                throw std::runtime_error("visualise_base::end_frame \"no frame to end\"");
            if (_operation_open)
                throw std::runtime_error("visualise_base::end_frame \"cannot end frame with unfinished operation\"");

            // Write metadata.
            nlohmann::json metadata;
            metadata["name"] = _frame.name.empty() ? std::format("frame_{}", _frame_count) : _frame.name;
            metadata["operation_count"] = _operation_count;
            this->_end_frame(metadata);
            io::json::write(metadata, visualise_base::_frame_dir(), "metadata", 4);

            // Increment counter.
            ++_frame_count;
            _frame_open = false;
        }
        void begin_operation(const std::string& type = "")
        {
            if (!_frame_open)
                throw std::runtime_error("visualise_base::begin_operation \"cannot initialise operation without a frame\"");
            if (_operation_open)
                throw std::runtime_error("visualise_base::begin_operation \"operation already initialised\"");

            // Create new operation instance.
            _operation = operation_type{};
            _operation.type = type;
            _operation_open = true;

            // Create directory.
            std::filesystem::create_directories(visualise_base::_operation_dir());
            visualise_base::_point_to_operation();
        }
        void end_operation()
        {
            if (!_frame_open)
                throw std::runtime_error("visualise_base::end_operation \"cannot end operation without a frame\"");
            if (!_operation_open)
                throw std::runtime_error("visualise_base::end_operation \"no operation to end\"");

            // Write metadata.
            nlohmann::json metadata;
            metadata["type"] = _operation.type;
            metadata["name"] = _operation.name.empty() ? std::format("operation_{}", _operation_count) : _operation.name;
            this->_end_operation(metadata);
            io::json::write(metadata, visualise_base::_operation_dir(), "metadata", 4);

            // Increment counter.
            ++_operation_count;
            _operation_open = false;
        }
        void run() const
        {
            nlohmann::json metadata;
            metadata["frame_count"] = _frame_count;
            io::json::write(metadata, _dir, "metadata", 4);

            if (_frame_count == 0)
                std::cerr << "visualise_3D::run \"warning; called run() but generated no frames\"";

            this->_run();
        }

    protected:
        void virtual _run() const {}
        void virtual _end_frame(nlohmann::json& metadata) const {}
        void virtual _end_operation(nlohmann::json& metadata) const {}
        void _point_to_frame()
        {
            _active_dir = visualise_base::_frame_dir();
        }
        void _point_to_operation()
        {
            _active_dir = visualise_base::_operation_dir();
        }
        std::filesystem::path _frame_dir() const
        {
            return _dir / ("frame_" + std::to_string(_frame_count));
        }
        std::filesystem::path _operation_dir() const
        {
            return visualise_base::_frame_dir() / ("operation_" + std::to_string(_operation_count));
        }

    protected:
        uint32_t _frame_count = 0;
        uint32_t _operation_count = 0;
        operation_type _operation;
        frame_type _frame;
        std::filesystem::path _dir;
        std::filesystem::path _active_dir = _dir;

        bool _operation_open = false;
        bool _frame_open = false;
    };
}
