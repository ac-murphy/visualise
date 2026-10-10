#pragma once

namespace vis::concepts
{

    template <typename T> concept container = std::ranges::range<T> &&
        requires
        {
            typename T::value_type;
        };

    template <typename T> concept point_3D = container<T> &&
                                             std::is_arithmetic_v<typename T::value_type>;
    template <typename T> concept matrix_3D = container<T> &&
                                              point_3D<typename T::value_type>;
}

namespace vis::types
{
    using point_3D = std::array<float, 3>;
    using id = uint32_t;
}