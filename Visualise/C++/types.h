#pragma once
#include <array>

#include "include_guard.h"

namespace vis::concepts
{
    template <typename T> concept container = std::ranges::range<T> &&
        requires(T& container)
        {
            { container.size() } -> std::convertible_to<size_t>;
            typename T::value_type;
        };

    // Indexing concepts.
    template <typename T> concept supports_linear_parentheses_index =
        requires(T& container, const T& const_container, size_t i)
        {
            { container(i) } -> std::convertible_to<std::remove_cvref_t<typename T::value_type>>;
            // { const_container(i) } -> std::same_as<const typename T::value_type&>;
        };
    template <typename T> concept supports_linear_bracket_index =
        requires(T& container, const T& const_container, size_t i)
        {
            { container[i] } -> std::convertible_to<std::remove_cvref_t<typename T::value_type>>;
            // { const_container[i] } -> std::same_as<const typename T::value_type&>;
        };
    template <typename T> concept supports_multidimensional_parentheses_index =
        requires(T& container, const T& const_container, size_t i, size_t j)
        {
            { container(i, j) } -> std::convertible_to<std::remove_cvref_t<typename T::value_type>>;
            // { const_container(i, j) } -> std::same_as<const typename T::value_type&>;
        };
    template <typename T> concept supports_multidimensional_bracket_index =
        requires(T& container, const T& const_container, size_t i, size_t j)
        {
            { container[i][j] } -> std::convertible_to<std::remove_cvref_t<typename T::value_type>>;
            // { const_container[i][j] } -> std::same_as<const typename T::value_type&>;
        };
    template <typename T> concept supports_linear_index = supports_linear_parentheses_index<T> || supports_linear_bracket_index<T>;
    template <typename T> concept supports_multidimensional_index = supports_multidimensional_parentheses_index<T> || supports_multidimensional_bracket_index<T>;
    template <typename T> concept supports_index = supports_linear_index<T> || supports_multidimensional_index<T>;
    template <typename T> concept supports_iteration = std::ranges::range<T>;

    // Dimension concepts.
    namespace dimension_interface
    {
        template <typename T> concept rows_cols =
            requires(const T& container)
            {
                { container.rows() } -> std::convertible_to<size_t>;
                { container.cols() } -> std::convertible_to<size_t>;
            };

        template <typename T> concept size_i_size_j =
            requires(const T& container)
            {
                { container.size_i() } -> std::convertible_to<size_t>;
                { container.size_j() } -> std::convertible_to<size_t>;
            };

        // Note: assumes support for bracket indexing.
        template <typename T> concept nested =
            requires(const T& container)
            {
                { container.size()    } -> std::convertible_to<size_t>;
                { container[0].size() } -> std::convertible_to<size_t>;
            };
    }
}

namespace vis::types
{
    using point_3D = std::array<float, 3>;
    template <typename value_type, size_t dimension>
    using point = std::array<value_type, dimension>;
    template <typename value_type, size_t rows, size_t columns>
    using matrix = std::array<std::array<value_type, columns>, rows>;
    using id = uint32_t;
}

namespace vis
{
    template <typename container_type>
    std::pair<int32_t, int32_t> dimensions(const container_type& container)
    {
        if constexpr (vis::concepts::dimension_interface::nested<container_type>)
            return { container.size(), container[0].size() };
        if constexpr (vis::concepts::dimension_interface::rows_cols<container_type>)
            return { container.rows(), container.cols() };
        if constexpr (vis::concepts::dimension_interface::size_i_size_j<container_type>)
            return { container.size_i(), container.size_j() };

        std::cerr << std::format("dimensions \"could not determine dimensions of container with type \'{}\'\"", typeid(container_type).name()) << std::endl;
        return { 0, 0 };
    }

    template <typename value_type, typename container_type>
    std::vector<value_type> vector_cast(const container_type& container)
    {
        if constexpr (std::same_as<container_type, std::vector<value_type>>)
        {
            return container;
        }

        std::vector<value_type> result;

        if constexpr (vis::concepts::supports_linear_parentheses_index<container_type>)
        {
            for (int32_t i = 0; i < container.size(); ++i)
                result.push_back(container(i));

            return result;
        }

        else if constexpr (vis::concepts::supports_linear_bracket_index<container_type>)
        {
            for (int32_t i = 0; i < container.size(); ++i)
                result.push_back(container[i]);

            return result;
        }

        else if constexpr (vis::concepts::supports_multidimensional_parentheses_index<container_type>)
        {
            const auto& [I, J] = dimensions(container);
            for (int32_t i = 0; i < I; ++i)
                for (int32_t j = 0; j < J; ++j)
                    result.push_back(container(i, j));

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

        else if constexpr (vis::concepts::supports_iteration<container_type>)
        {
            for (const auto& value : container)
                result.push_back(value);

            return result;
        }

        throw std::runtime_error(std::format("vector_cast \"could not convert \'{}\' to \'std::vector<{}>\'\"",
                                             typeid(container_type).name(),
                                             typeid(value_type).name()));

        // result.reserve(container.size());
        // for (int32_t i = 0; i < container.size(); ++i)
        //     result.push_back(static_cast<value_type>(container.data()[i]));
        // return result;
    }

    template <typename container_type>
    requires (vis::concepts::supports_linear_index<container_type>)
    auto index(container_type& container, const size_t& i)
    {
        if constexpr (vis::concepts::supports_linear_bracket_index<container_type>)
        {
            return container[i];
        }
        else if constexpr (vis::concepts::supports_linear_parentheses_index<container_type>)
        {
            return container(i);
        }
    }

    template <typename container_type>
    requires (vis::concepts::supports_multidimensional_index<container_type>)
    auto index(container_type& container, const size_t& i, const size_t& j)
    {
        if constexpr (vis::concepts::supports_multidimensional_bracket_index<container_type>)
        {
            return container[i][j];
        }
        else if constexpr (vis::concepts::supports_multidimensional_parentheses_index<container_type>)
        {
            return container(i, j);
        }
    }
}