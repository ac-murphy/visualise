#pragma once
#include <array>
#include <vector>
#include <format>
#include <stdexcept>

#include "include_guard.h"
#include "framework/type_rules.h"

namespace vis::converting
{
    template <typename T> class vector;
    template <typename T, size_t N> class array;

    template <typename T>           struct converting_type_trait                               : std::false_type {};
    template <typename T>           struct converting_type_trait<vis::converting::vector<T>>   : std::true_type {};
    template <typename T, size_t N> struct converting_type_trait<vis::converting::array<T, N>> : std::true_type {};
    template <typename T> concept converting_type = converting_type_trait<std::remove_cvref_t<T>>::value;

    template <typename T>                                    struct std_type_trait                               { using value = T; };
    template <typename T>                                    struct std_type_trait<std::vector<T>>               { using value = std::vector<typename std_type_trait<T>::value>; };
    template <typename T>                                    struct std_type_trait<vis::converting::vector<T>>   { using value = std::vector<typename std_type_trait<T>::value>; };
    template <typename T, size_t N>                          struct std_type_trait<std::array<T, N>>             { using value = std::array<typename std_type_trait<T>::value, N>; };
    template <typename T, size_t N>                          struct std_type_trait<vis::converting::array<T, N>> { using value = std::array<typename std_type_trait<T>::value, N>; };
    template <typename T> using std_type = std_type_trait<T>::value;

    template <typename to_container_type, typename from_container_type>
    void convert(to_container_type& to_container, const from_container_type& from_container)
    {
        if (to_container.size() != from_container.size())
            throw std::runtime_error(std::format("array::array \"could not convert \'{}\' to \'{}\'; size mismatch\"",
                                     typeid(from_container_type).name(),
                                     typeid(to_container_type).name()));

        if constexpr (std::same_as<to_container_type, from_container_type>)
            to_container = from_container;

        else if constexpr (vis::supports_linear_parentheses_index<from_container_type>)
        {
            const auto size = from_container.size();
            for (int32_t i = 0; i < size; ++i)
                to_container[i] = from_container(i);
        }

        else if constexpr (vis::supports_linear_bracket_index<from_container_type>)
        {
            const auto size = from_container.size();
            for (int32_t i = 0; i < size; ++i)
                to_container[i] = from_container[i];
        }

        else if constexpr (vis::supports_iteration<from_container_type>)
        {
            int32_t i = 0;
            for (const auto& value : from_container)
                to_container[i++] = value;
        }

        else if constexpr (vis::supports_multidimensional_parentheses_index<from_container_type>)
        {
            const auto& [I, J] = dimensions(from_container);
            for (int32_t i = 0; i < I; ++i)
                for (int32_t j = 0; j < J; ++j)
                    to_container[i * J + j] = from_container(i, j);

        }

        else if constexpr (vis::supports_multidimensional_bracket_index<from_container_type>)
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

    template <typename std_container_type, typename container_type>
    void make_std_recursive(std_container_type& std_container, const container_type& container)
    {
        // If the inferred STL container is dynamically sized, set it here.
        if constexpr (requires(size_t i) { std_container.resize(i); })
            std_container.resize(container.size());

        // If the value type is a range, descend.
        if constexpr (std::ranges::range<typename std_container_type::value_type>)
        {
            for (size_t i = 0; i < container.size(); ++i)
                make_std_recursive(std_container[i], container[i]);

        }
        // If the value type is scalar, copy.
        else
        {
            for (size_t i = 0; i < container.size(); ++i)
            {
                // auto x = std_container[i];
                // auto y = container[i];
                // std::cout << x << ", " << y << std::endl;
                // std::cout << std::format("std_container[{}] = container[{}];", i, i) << std::endl;
                std_container[i] = container[i];
            }
        }
    }
    template <typename container_type>
    auto make_std(const container_type& container)
    {
        std_type<container_type> std_container;
        make_std_recursive(std_container, container);
        return std_container;
    }

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
        auto to_std() const {
            return make_std(_data);
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
        }
        auto to_std() const
        {
            return make_std(_data);
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

    protected:
        std::array<value_type, N> _data;
    };
    template <typename T, size_t M, size_t N>
    class matrix : array<T, M * N>
    {
    public:
        matrix() : array<T, M * N>() {}
        ~matrix() = default;
        matrix(const matrix&) = default;
        matrix(matrix&&) = default;
        matrix& operator=(const matrix&) = default;
        matrix& operator=(matrix&&) = default;

    public:
        template <typename container_type>
        matrix(const container_type& container)
        {
            vis::converting::convert(this->_data, container);
        }

    public:
        std::array<std::array<T, M>, N> to_std() const
        {
            std::array<std::array<T, M>, N> result;

            for (size_t i = 0; i < N; i++)
                for (size_t j = 0; j < M; j++)
                    result[i][j] = this->_data[i * M + j];

            return result;
        }
    };

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

    // std_type_trait<T> tests.
    // Scalar.
    static_assert(std::same_as<float, std_type<float>>);
    static_assert(std::same_as<int, std_type<int>>);

    // Vectors & nested.
    static_assert(std::same_as<std::vector<float>, std_type<std::vector<float>>>);
    static_assert(std::same_as<std::vector<float>, std_type<vis::converting::vector<float>>>);
    static_assert(std::same_as<std::vector<std::vector<float>>, std_type<vis::converting::vector<vis::converting::vector<float>>>>);
    static_assert(std::same_as<std::vector<std::vector<float>>, std_type<std::vector<vis::converting::vector<float>>>>);
    static_assert(std::same_as<std::vector<std::vector<float>>, std_type<vis::converting::vector<std::vector<float>>>>);

    // Arrays & nested.
    static_assert(std::same_as<std::array<float, 3>, std_type<std::array<float, 3>>>);
    static_assert(std::same_as<std::array<float, 3>, std_type<vis::converting::array<float, 3>>>);
    static_assert(std::same_as<std::array<std::array<float, 2>, 3>, std_type<vis::converting::array<vis::converting::array<float, 2>, 3>>>);
    static_assert(std::same_as<std::array<std::array<float, 2>, 3>, std_type<vis::converting::array<std::array<float, 2>, 3>>>);
    static_assert(std::same_as<std::array<std::array<float, 2>, 3>, std_type<std::array<vis::converting::array<float, 2>, 3>>>);

    // Mixed.
    static_assert(std::same_as<std::vector<std::array<float, 3>>, std_type<vis::converting::vector<vis::converting::array<float, 3>>>>);
    static_assert(std::same_as<std::array<std::vector<float>, 3>, std_type<vis::converting::array<vis::converting::vector<float>, 3>>>);

    // Satisfaction of std::ranges::range<T>.
    static_assert(std::ranges::range<vis::converting::array<float, 3>>);
    static_assert(std::ranges::range<vis::converting::array<vis::converting::array<float, 3>, 3>>);
    static_assert(std::ranges::range<vis::converting::vector<float>>);
    static_assert(std::ranges::range<vis::converting::vector<vis::converting::vector<float>>>);
}
