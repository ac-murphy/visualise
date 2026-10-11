#pragma once
#include "Visualise.h"
#include "gtest/gtest.h"
#include "Eigen"

class UnitTest : public ::testing::Test {};

TEST_F(UnitTest, Test_3D)
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

    Eigen::Matrix3f basis { { 0.0f, -1.0f, 0.0f },
                            { 1.0f,  0.0f, 1.0f },
                            { 0.0f,  0.0f, 1.0f } };

    std::vector<std::array<uint32_t, 3>> tri_indices
    {
        { 0, 1, 2 },
        { 0, 1, 4 },
        { 0, 7, 6 }
    };

    vis.begin_frame("first-scene");
        vis.add_points(points);
        vis.add_marker(point);
    vis.end_frame();

    vis.begin_frame("second-scene");
        vis.add_marker(point);
        vis.add_plane(point, std::array{ 1.0f, 1.0f, 1.0f });
        vis.add_axes(point, basis);
    vis.end_frame();

    vis.begin_frame("third-scene");
        vis.add_points(points);
        vis.add_mesh(points, tri_indices);
    vis.end_frame();

    vis.run();
}
TEST_F(UnitTest, Test_2D)
{
    vis::visualise_2D vis;

    std::vector<float> x, y1, y2, z;
    float dx = 0.1f;
    constexpr int N = 100;

    auto f = [](const float& x){ return std::sin(x); };
    auto g = [](const float& x){ return std::cos(x); };
    for (size_t i = 0; i < N; i++)
    {
        x.push_back(i * dx);
        y1.push_back(f(i * dx));
        y2.push_back(g(i * dx));
        for (size_t j = 0; j < N; j++)
            z.push_back(f(i * dx) + g(j * dx));
    }

    vis.begin_frame("test 2D plot");
    {
        vis.set_axis_labels("$x$", "$y$");
        vis.set_legend_visible(true);

        vis::plot_2D<float> plot1(x, y1);
        plot1.style = "--";
        plot1.label = "$y = sin(x)$";
        vis.add_plot(plot1);

        vis::plot_2D<float> plot2(x, y2);
        plot2.label = "$y = cos(x)$";
        vis.add_plot(plot2);
    }
    vis.end_frame();
    vis.begin_frame("test 3D plot");
    {
        vis::plot_3D<float> plot(x, x, z);
        plot.label = "$z = f(x) + g(y)$";
        plot.data_storage_order = vis::storage_order::column_major;
        vis.add_plot(plot);
    }
    vis.end_frame();

    vis.run();
}
