#pragma once
#include <array>
#include <vector>
#include <format>
#include <stdexcept>

#include "framework/type_rules.h"

namespace vis::converting
{
    template <typename to_container_type, typename from_container_type>
    void convert(to_container_type& to_container, const from_container_type& from_container)
    {
        if (to_container.size() != from_container.size())
            throw std::runtime_error(std::format("array::array \"could not convert \'{}\' to \'{}\'; size mismatch\"",
                                     typeid(from_container_type).name(),
                                     typeid(to_container_type).name()));

        if constexpr (std::same_as<to_container_type, from_container_type>)
            to_container = from_container;

        else if constexpr (vis::concepts::supports_linear_parentheses_index<from_container_type>)
        {
            const auto size = from_container.size();
            for (int32_t i = 0; i < size; ++i)
                to_container[i] = from_container(i);
        }

        else if constexpr (vis::concepts::supports_linear_bracket_index<from_container_type>)
        {
            const auto size = from_container.size();
            for (int32_t i = 0; i < size; ++i)
                to_container[i] = from_container[i];
        }

        else if constexpr (vis::concepts::supports_iteration<from_container_type>)
        {
            int32_t i = 0;
            for (const auto& value : from_container)
                to_container[i++] = value;
        }

        else if constexpr (vis::concepts::supports_multidimensional_parentheses_index<from_container_type>)
        {
            const auto& [I, J] = dimensions(from_container);
            for (int32_t i = 0; i < I; ++i)
                for (int32_t j = 0; j < J; ++j)
                    to_container[i * J + j] = from_container(i, j);

        }

        else if constexpr (vis::concepts::supports_multidimensional_bracket_index<from_container_type>)
        {
            const auto& [I, J] = dimensions(from_container);
            for (int32_t i = 0; i < I; ++i)
                for (int32_t j = 0; j < J; ++j)
                    to_container[i * J + j] = from_container[i][j];
        }

        else
            throw std::runtime_error(std::format("array::array \"could not convert \'{}\' to \'{}\'; unsupported type\"",
                                                 typeid(from_container_type).name(),
                                                 typeid(to_container_type).name()));
    }

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
        array(std::initializer_list<T> list)
        {
            if (list.size() != N)
                throw std::runtime_error("array::array \"could not initialise array from list; size mismatch\"");

            std::copy(list.begin(), list.end(), _data.begin());
        }

    public:
        template <typename container_type>
        array(const container_type& container)
        {
            vis::converting::convert(_data, container);
            /* if (container.size() != N)
            // {
            //     throw std::runtime_error(std::format("array::array \"could not convert \'{}\' to \'{}\'; size mismatch\"",
            //                              typeid(container_type).name(),
            //                              typeid(decltype(*this)).name()));
            // }
            //
            // if constexpr (std::same_as<container_type, std::array<value_type, N>>)
            //     _data = container;
            //
            // if constexpr (vis::concepts::supports_linear_parentheses_index<container_type>)
            //     for (int32_t i = 0; i < container.size(); ++i)
            //         _data[i] = container(i);
            //
            // else if constexpr (vis::concepts::supports_linear_bracket_index<container_type>)
            //     for (int32_t i = 0; i < container.size(); ++i)
            //         _data[i] = container[i];
            //
            // else if constexpr (vis::concepts::supports_iteration<container_type>)
            // {
            //     int32_t i = 0;
            //     for (const auto& value : container)
            //         _data[i++] = value;
            // }
            //
            // else if constexpr (vis::concepts::supports_multidimensional_parentheses_index<container_type>)
            // {
            //     const auto& [I, J] = dimensions(container);
            //     for (int32_t i = 0; i < I; ++i)
            //         for (int32_t j = 0; j < J; ++j)
            //             _data[i * J + j] = container(i, j);
            //
            // }
            //
            // else if constexpr (vis::concepts::supports_multidimensional_bracket_index<container_type>)
            // {
            //     const auto& [I, J] = dimensions(container);
            //     for (int32_t i = 0; i < I; ++i)
            //         for (int32_t j = 0; j < J; ++j)
            //             _data[i * J + j] = container[i][j];
            // }
            //
            // else
            // {
            //     throw std::runtime_error(std::format("array::array \"could not convert \'{}\' to \'{}\'; unsupported type\"",
            //                                          typeid(container_type).name(),
            //                                          typeid(decltype(*this)).name()));
            // } */
        }
        const std::array<T, N>& to_std() const { return _data; }

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
        vector(std::initializer_list<T> list)
        {
            _data.resize(list.size());
            std::copy(list.begin(), list.end(), _data.begin());
        }

    public:
        template <typename container_type>
        vector(const container_type& container)
        {
            _data.resize(container.size());
            vis::converting::convert(_data, container);
        }
        const std::vector<T>& to_std() const { return _data; }

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

    template <typename T, size_t M, size_t N>
    using matrix = array<array<T, M>, N>;

    template <typename T, size_t N>
    bool operator==(const vis::converting::array<T, N>& array, const std::array<T, N>& check)
    {
        for (size_t i = 0; i < N; i++)
            if (array[i] != check[i])
                return false;

        return true;
    }
    template <typename T, size_t N>
    bool operator==(const std::array<T, N>& check, const vis::converting::array<T, N>& array)
    {
        return array == check;
    }
    template <typename T, size_t N>
    bool operator!=(const vis::converting::array<T, N>& array, const std::array<T, N>& check)
    {
        return !(array == check);
    }
    template <typename T, size_t N>
    bool operator!=(const std::array<T, N>& check, const vis::converting::array<T, N>& array)
    {
        return !(check == array);
    }
}
