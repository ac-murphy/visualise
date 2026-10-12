#pragma once
#include <array>
#include <vector>
#include <format>
#include <stdexcept>

#include "types.h"

namespace vis::converting
{
    template <typename T, size_t N>
    class array
    {
    public:
        using value_type      = T;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;
        using iterator        = std::array<value_type, N>::iterator;
        using const_iterator  = std::array<value_type, N>::const_iterator;
        using reference       = value_type&;
        using const_reference = const value_type&;
        using pointer         = value_type*;
        using const_pointer   = const value_type*;

    public:
        array() : _data{} {}
        ~array() = default;
        array(const array&) = default;
        array(array&&) = default;
        array& operator=(const array&) = default;
        array& operator=(array&&) = default;

    public:
        template <typename container_type>
        array(const container_type& container)
        {
            if (container.size() != N)
            {
                throw std::runtime_error(std::format("array::array \"could not convert \'{}\' to \'{}\'; size mismatch\"",
                                         typeid(container_type).name(),
                                         typeid(decltype(*this)).name()));
            }

            if constexpr (std::same_as<container_type, std::array<value_type, N>>)
                _data = container;

            if constexpr (vis::concepts::supports_linear_parentheses_index<container_type>)
                for (int32_t i = 0; i < container.size(); ++i)
                    _data[i] = container(i);

            else if constexpr (vis::concepts::supports_linear_bracket_index<container_type>)
                for (int32_t i = 0; i < container.size(); ++i)
                    _data[i] = container[i];

            else if constexpr (vis::concepts::supports_iteration<container_type>)
            {
                int32_t i = 0;
                for (const auto& value : container)
                    _data[i++] = value;
            }

            else
            {
                throw std::runtime_error(std::format("array::array \"could not convert \'{}\' to \'{}\'; unsupported type\"",
                                                     typeid(container_type).name(),
                                                     typeid(decltype(*this)).name()));
            }
        }

    public:
        reference       operator[](size_type i)       { return _data[i]; }
        const_reference operator[](size_type i) const { return _data[i]; }
        iterator       begin()       { return _data.begin(); }
        const_iterator begin() const { return _data.begin(); }
        iterator       end()       { return _data.end(); }
        const_iterator end() const { return _data.end(); }
        pointer       data()       { return _data.data(); }
        const_pointer data() const { return _data.data(); }
        size_type size() const { return _data.size(); }

    private:
        std::array<value_type, N> _data;
    };

    template <typename T>
    class vector
    {
    public:
        using value_type      = T;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;
        using iterator        = std::vector<value_type>::iterator;
        using const_iterator  = std::vector<value_type>::const_iterator;
        using reference       = value_type&;
        using const_reference = const value_type&;
        using pointer         = value_type*;
        using const_pointer   = const value_type*;

    public:
        vector() : _data{} {}
        ~vector() = default;
        vector(const vector&) = default;
        vector(vector&&) = default;
        vector& operator=(const vector&) = default;
        vector& operator=(vector&&) = default;

    public:
        template <typename container_type>
        vector(const container_type& container)
        {
            if constexpr (std::same_as<container_type, std::vector<value_type>>)
                _data = container;

            _data.resize(container.size());

            if constexpr (vis::concepts::supports_linear_parentheses_index<container_type>)
                for (int32_t i = 0; i < container.size(); ++i)
                    _data[i] = container(i);

            else if constexpr (vis::concepts::supports_linear_bracket_index<container_type>)
                for (int32_t i = 0; i < container.size(); ++i)
                    _data[i] = container[i];

            else if constexpr (vis::concepts::supports_iteration<container_type>)
            {
                int32_t i = 0;
                for (const auto& value : container)
                    _data[i++] = value;
            }

            else if constexpr (vis::concepts::supports_multidimensional_parentheses_index<container_type>)
            {
                const auto& [I, J] = dimensions(container);
                for (int32_t i = 0; i < I; ++i)
                    for (int32_t j = 0; j < J; ++j)
                        _data[index(i, j)] = container(i, j);

                return result;
            }

            else if constexpr (vis::concepts::supports_multidimensional_bracket_index<container_type>)
            {
                const auto& [I, J] = dimensions(container);
                for (int32_t i = 0; i < I; ++i)
                    for (int32_t j = 0; j < J; ++j)
                        result.push_back(container[i][j]);

                return result;
            }

            else
            {
                throw std::runtime_error(std::format("array::array \"could not convert \'{}\' to \'{}\'; unsupported type\"",
                                                     typeid(container_type).name(),
                                                     typeid(decltype(*this)).name()));
            }
        }

    public:
        reference       operator[](size_type i)       { return _data[i]; }
        const_reference operator[](size_type i) const { return _data[i]; }
        iterator       begin()       { return _data.begin(); }
        const_iterator begin() const { return _data.begin(); }
        iterator       end()       { return _data.end(); }
        const_iterator end() const { return _data.end(); }
        pointer       data()       { return _data.data(); }
        const_pointer data() const { return _data.data(); }
        size_type size() const { return _data.size(); }

    private:
        std::vector<T> _data;
    };
}
