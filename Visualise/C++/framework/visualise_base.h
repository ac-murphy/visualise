#pragma once
#include <deque>
#include <filesystem>
#include <iostream>
#include <system_error>
#include <unordered_set>

#include "include_guard.h"
#include "json.hpp"
#include "system/json.h"

namespace vis::base
{
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

    class operation
    {
    public:
        template <typename derived_frame_type, typename operation_type> friend class frame;
        template <typename frame_type> friend class visualise;

    public:
        operation(const std::filesystem::path& dir, const size_t& id)
        {
            _dir = dir;
            _id = id;
            _sequence_id = -1;
            std::filesystem::create_directories(dir);
        }

    public:
        template <typename value_type>
        void register_type()
        {
            if constexpr (vis::type_name_v<value_type> != "")
                _data_types.push_back(std::string(vis::type_name_v<value_type>));
            else
                throw std::runtime_error(std::format("visualise_plotting::_use_type \"unrecognised data type \'{}\'\"", typeid(value_type).name()));
        }

    protected:
        std::vector<std::string> _data_types;
        std::filesystem::path _dir;
        std::string _name;
        std::string _type;
        int32_t _sequence_id;
        size_t _id;
    };

    template <typename T = frame, typename U = operation>
    class frame
    {
    public:
        using derived_frame_type = typename T;
        using operation_type = typename U;
        template <typename frame_type> friend class visualise;

    public:
        frame(const std::filesystem::path& dir, const size_t& id)
        {
            _dir = dir;
            _id = id;
            _sequence_id = -1;
            std::filesystem::create_directories(dir);
        }

    public:
        operation_type new_operation()
        {
            operation_type operation(_dir/std::format("operation_{}", _operation_id_counter), _operation_id_counter++);

            return operation;
        }
        derived_frame_type new_frame()
        {
            derived_frame_type frame(_dir/std::format("frame_{}", _subframe_id_counter), _subframe_id_counter++);

            return frame;
        }
        void add(operation_type& operation)
        {
            if (!_subframes.empty())
                throw std::runtime_error("cannot add operations to parent frames");

            operation._sequence_id = _operation_sequence_id_counter++;
            _operations.push_back(std::move(operation));
        }
        void add(derived_frame_type& frame)
        {
            if (!_operations.empty())
                throw std::runtime_error("cannot add frames to operational frame");

            frame._sequence_id = _subframe_sequence_id_counter++;
            _subframes.push_back(std::move(frame));
        }

    public:
        void set_size(const size_t& i, const size_t& j)
        {
            _size = { i, j };
        }

    protected:
        std::filesystem::path _dir;
        size_t _id;
        size_t _subframe_id_counter = 0;
        size_t _operation_id_counter = 0;
        int32_t _sequence_id;
        int32_t _subframe_sequence_id_counter = 0;
        int32_t _operation_sequence_id_counter = 0;
        std::vector<operation_type> _operations;
        std::vector<derived_frame_type> _subframes;
        std::vector<std::string> _data_types;
        std::pair<size_t, size_t> _size = { 1, 1 };
        std::string _type;
        std::string _name;
    };

    template <typename frame_type = frame>
    class visualise
    {
    public:
        using operation_type = typename frame_type::operation_type;

    public:
        visualise(const std::filesystem::path& output_dir = VISUALISE_DATA_OUTPUT_DIR) : _root_dir(output_dir), _dir(output_dir)
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

            // Initialise temporary directory.
            _dir /= "tmp";
            std::filesystem::create_directories(_dir);
        }
        virtual ~visualise() = default;

    public:
        frame_type new_frame(const std::string& = "")
        {
            frame_type frame(_dir/std::format("frame_{}", _frame_id_counter), _frame_id_counter++);

            return frame;
        }
        void add(frame_type& frame)
        {
            frame._sequence_id = _frame_sequence_id_counter++;
            _frames.push_back(frame);
        }
        void run()
        {
            visualise::_write_visualisation();
            this->_run_hook();
        }

    protected:
        void virtual _write_operation_metadata_hook(const operation_type& frame, nlohmann::json& metadata) {}
        void virtual _write_frame_metadata_hook(const frame_type& frame, nlohmann::json& metadata) {}
        void virtual _run_hook() = 0;
        size_t virtual _supported_depth() { return 0; }

    private:
        void _write_operation_metadata(const std::filesystem::path& dir, operation_type& operation)
        {
            // Write metadata.
            nlohmann::json metadata;
            metadata["type"] = "operation";
            metadata["func"] = operation._type;
            metadata["name"] = operation._name.empty() ? std::format("operation_{}", operation._sequence_id) : operation._name;
            metadata["data_types"] = operation._data_types;

            this->_write_operation_metadata_hook(operation, metadata);
            io::json::write(metadata, dir, "metadata", 4);
        }
        void _write_frame_metadata(const std::filesystem::path& dir, frame_type& frame)
        {
            nlohmann::json metadata;
            metadata["type"] = "frame";
            metadata["name"] = frame._name.empty() ? std::format("frame_{}", frame._sequence_id) : frame._name;
            metadata["operation_count"] = frame._operations.size();
            metadata["subframe_count"] = frame._subframes.size();

            this->_write_frame_metadata_hook(frame, metadata);
            io::json::write(metadata, dir, "metadata", 4);
        }
        void _write_visualisation_recursive(const std::filesystem::path& dir, frame_type& frame, size_t& frame_counter)
        {
            // Create directory in filesystem.
            std::filesystem::path new_frame_dir = dir/std::format("frame_{}", frame_counter);
            std::filesystem::create_directories(new_frame_dir);
            visualise::_write_frame_metadata(new_frame_dir, frame);

            // Frame contains other frames only.
            if (!frame._subframes.empty())
            {
                size_t new_frame_counter = 0;
                for (auto& subframe : frame._subframes)
                    _write_visualisation_recursive(new_frame_dir, subframe, new_frame_counter);

                return;
            }

            // Frame contains operations only.
            size_t operation_counter = 0;

            for (auto& operation : frame._operations)
            {
                std::filesystem::path old_operation_dir = operation._dir;
                std::filesystem::path new_operation_dir = new_frame_dir/std::format("operation_{}", operation_counter);
                std::filesystem::create_directories(new_operation_dir.parent_path());
                std::filesystem::rename(old_operation_dir, new_operation_dir);

                visualise::_write_operation_metadata(new_operation_dir, operation);
                ++operation_counter;
            }

            ++frame_counter;
        }
        void _write_visualisation()
        {
            // Write visualisation metadata.
            nlohmann::json metadata;
            metadata["frame_count"] = _frames.size(); // todo: need to count recursive frames too.
            io::json::write(metadata, _root_dir, "metadata", 4);

            // Write frames in "root/tmp" to "root" in correct order and hierarchy.
            size_t frame_counter = 0;
            for (auto& frame : _frames)
                visualise::_write_visualisation_recursive(_root_dir, frame, frame_counter);

            // Remove temporary directory.
            for (const auto& entry : std::filesystem::directory_iterator(_dir))
                std::filesystem::remove_all(entry.path());
            std::filesystem::remove(_dir);
        }

    protected:
        size_t _frame_id_counter = 0;
        int32_t _frame_sequence_id_counter = 0;
        std::deque<frame_type> _frames;
        std::filesystem::path _root_dir;
        std::filesystem::path _dir;
    };

    inline size_t to_linear_index(const size_t& i, const size_t& j, const size_t& I, const size_t& J)
    {
        return i * J + j;
    }
}
