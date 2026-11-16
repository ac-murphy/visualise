#pragma once
#include <algorithm>

namespace vis
{
    template <typename T, typename visualise_class>
    class parameter
    {
    public:
        parameter(visualise_class& vis_instance)             : _value{}            , _vis_instance(vis_instance)        {}
        parameter(visualise_class& vis_instance, const T& t) : _value(t)           , _vis_instance(vis_instance)        {}
        parameter(visualise_class& vis_instance, T&& t)      : _value(std::move(t)), _vis_instance(vis_instance)        {}
        parameter(parameter&& other) noexcept                : _value(other._value), _vis_instance(other._vis_instance) {}
        parameter(const parameter& other)                    : _value(other._value), _vis_instance(other._vis_instance) {}
        parameter& operator=(parameter&& other) noexcept
        {
            _value = std::move(other._value);
            _vis_instance = std::move(other._vis_instance);
            return *this;
        }
        parameter& operator=(const parameter& other)
        {
            _value = other._value;
            _vis_instance = other._vis_instance;
            return *this;
        }

    public:
        parameter& operator=(T&& t) noexcept
        {
            _value = std::move(t);
            return *this;
        }
        parameter& operator=(const T& t)
        {
            _value = t;
            return *this;
        }

    private:
        T _value;
        visualise_class& _vis_instance;
    };
}


