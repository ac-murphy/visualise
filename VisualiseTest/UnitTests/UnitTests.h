#pragma once
#include "Visualise.h"
#include "gtest/gtest.h"

class UnitTest : public ::testing::Test {};

TEST_F(UnitTest, TEMP)
{
    vis::visualise_3D vis;

    std::array<float, 3> point{1.0f, 1.0f, 1.0f};
    std::vector<std::array<float, 3>> points{
        { -1.0f, -1.0f, -1.0f },
        {  1.0f, -1.0f, -1.0f },
        { -1.0f,  1.0f, -1.0f },
        {  1.0f,  1.0f, -1.0f },
        { -1.0f, -1.0f,  1.0f },
        {  1.0f, -1.0f,  1.0f },
        { -1.0f,  1.0f,  1.0f },
        {  1.0f,  1.0f,  1.0f }
    };

    std::vector<std::array<float, 3>> basis
    {
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f }
    };

    std::vector<std::array<uint32_t, 3>> tri_indices
    {
        { 0, 1, 2 },
        { 0, 1, 4 }
    };

    vis.begin_frame("first-scene");
        vis.set_points(points);
        vis.add_marker(point);
    vis.end_frame();

    vis.begin_frame("second-scene");
        vis.add_marker(point);
        vis.add_plane(point, std::array{ 1.0f, 1.0f, 1.0f });
        vis.add_axes(point, basis);
    vis.end_frame();

    vis.begin_frame("third-scene");
        vis.set_points(points);
        vis.add_mesh(points, tri_indices);
    vis.end_frame();

    vis.run();
}

