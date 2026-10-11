#pragma once
#include <array>

namespace vis::concepts
{

    template <typename T> concept container = std::ranges::range<T> &&
        requires
        {
            typename T::value_type;
        };
}

namespace vis::types
{
    using point_3D = std::array<float, 3>;
    using id = uint32_t;
}

namespace vis
{
    struct operation
    {
        std::string name;

        std::string type;
    };
}