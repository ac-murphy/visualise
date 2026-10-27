#pragma once
#include "gtest/gtest.h"
#include "Visualise.h"
#include "Eigen"


class Experimentation : public ::testing::Test {};
TEST_F(Experimentation, Geometric)
{
    vis::visualise_geometry vis;

    std::array<float, 3> point{1.0f, 1.0f, 1.0f};
    std::vector<std::array<float, 3>> points{
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 0.0f },
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 1.0f }
    };

    Eigen::Matrix<float, 3, 3, Eigen::RowMajor> basis { { 0.0f, -1.0f, 0.0f },
                                                        { 1.0f,  0.0f, 1.0f },
                                                        { 0.0f,  0.0f, 1.0f } };

    std::vector<std::array<uint32_t, 3>> tri_indices
    {
        { 0, 1, 2 },
        { 0, 1, 4 },
        { 0, 7, 6 }
    };

    constexpr int N = 100;
    constexpr float dx = 0.1f;
    constexpr float dy = 0.1f;
    Eigen::VectorXf X(N);
    Eigen::VectorXf Y(N);
    Eigen::MatrixXf Z(N, N);

    for (size_t i = 0; i < N; i++)
    {
        X(i) = i * dx;
        Y(i) = i * dy;
    }

    auto f = [](const float& x){ return std::sin(x); };
    auto g = [](const float& x){ return std::cos(x); };
    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < N; j++)
            Z(i, j) = f(i * dx) + g(j * dy);

    vis.begin_frame("surface");
    {
        vis::surface<float> surface(X, Y, Z);
        vis.add(surface);

        vis::box<float> box;
        box.vertices = points;
        vis.add(box);
    }
    vis.end_frame();

    vis.begin_frame("first-scene");
    {
        vis.add_points(points);
        vis.add_marker(point);
        vis::arrow<float> arrow;
        arrow.direction = { 1.0f, 1.0f, 1.0f };
    }
    vis.end_frame();

    vis.begin_frame("second-scene");
    {
        vis.add_marker(point);
        vis.add_plane(point, std::array{ 1.0f, 1.0f, 1.0f });
        vis.add_axes(point, basis);
    }
    vis.end_frame();

    vis.begin_frame("third-scene");
    {
        vis.add_points(points);
        vis.add_mesh(points, tri_indices);
    }
    vis.end_frame();

    vis.run();
}
TEST_F(Experimentation, Plotting)
{
    vis::visualise_plotting vis;

    std::vector<float> x, y1, y2, z;
    float dx = 0.1f;
    constexpr int N = 100;

    Eigen::VectorXf X(N);
    Eigen::MatrixXf Z(N, N);

    auto f = [](const float& x){ return std::sin(x); };
    auto g = [](const float& x){ return std::cos(x); };
    for (size_t i = 0; i < N; i++)
    {
        x.push_back(i * dx);
        X(i) = i * dx;
        y1.push_back(f(i * dx));
        y2.push_back(g(i * dx));
        for (size_t j = 0; j < N; j++)
            Z(i, j) = f(i * dx) + g(j * dx);
    }

    vis.begin_frame("test 2D plot");
    {
        vis.set_axis_labels("$x$", "$y$");
        vis.set_legend_visible(true);

        auto x = X;
        auto y = Z.row(0);
        vis::plot_2D<float> plot;
        plot.x_values = x;
        plot.y_values = y;
        plot.label = "$y = f(x)$";
        vis.add(plot);
    }
    vis.end_frame();

    vis.begin_frame("test 3D plot");
    {
        vis.set_axis_labels("$x$", "$y$", "$z$");
        vis::plot_3D<float> plot;
        plot.x_values = X;
        plot.y_values = X;
        plot.z_values = Z;

        vis.add(plot);
    }
    vis.end_frame();

    vis.begin_frame("test 2D integer plot");
    {
        vis::plot_2D<uint32_t> plot;
        plot.x_values = { 1, 2, 3,                                    4, 5, 6 };
        plot.y_values = { 0, 0, 1, std::numeric_limits<uint32_t>::max(), 2, 2 };
        vis.add(plot);
    }
    vis.end_frame();

    vis.run();
}
// TEST_F(UnitTest, Converting_Array)
// {
//     vis::converting::array<float, 9> matrix_3_3 = {
//         1.0f, 0.0f, 0.0f,
//         0.0f, 1.0f, 0.0f,
//         0.0f, 0.0f, 1.0f
//     };
//     Eigen::Matrix<float, 3, 3> eigen_matrix;
//     eigen_matrix << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f; //, 10.0f, 11.0f, 12.0f;
//     matrix_3_3 = eigen_matrix;
//
// }

class UnitTest : public ::testing::Test {};
TEST_F(UnitTest, ConvertingArray__Reference)
{
    vis::converting::array<float, 3> arr = { 1.0f, 2.0f, 3.0f };
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 3.0f);

    arr[0] = 3.0f;
    arr[1] = 2.0f;
    arr[2] = 1.0f;
    ASSERT_EQ(arr[0], 3.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 1.0f);

    static_assert(std::same_as<decltype(arr[0]), float&>);
}
TEST_F(UnitTest, ConvertingArray__ConstReference)
{
    const vis::converting::array<float, 3> arr = { 1.0f, 2.0f, 3.0f };
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 3.0f);

    static_assert(std::same_as<decltype(arr[0]), const float&>);
}
TEST_F(UnitTest, ConvertingArray__To_StdArray)
{
    vis::converting::array<float, 3> arr = { 1.0f, 2.0f, 3.0f };
    auto std_arr = arr.to_std();

    ASSERT_EQ(std_arr.size(), 3);
    ASSERT_EQ(std_arr[0], 1.0f);
    ASSERT_EQ(std_arr[1], 2.0f);
    ASSERT_EQ(std_arr[2], 3.0f);
    static_assert(std::same_as<decltype(std_arr), std::array<float, 3>>);

    vis::converting::array<vis::converting::array<float, 3>, 2> arr_2_3 = { { 1.0f, 2.0f, 3.0f },
                                                                            { 4.0f, 5.0f, 6.0f } };
    auto std_arr_2_3 = arr_2_3.to_std();
    ASSERT_EQ(std_arr_2_3.size(), 2);
    ASSERT_EQ(std_arr_2_3[0].size(), 3);
    ASSERT_EQ(std_arr_2_3[0][0], 1.0f);
    ASSERT_EQ(std_arr_2_3[0][1], 2.0f);
    ASSERT_EQ(std_arr_2_3[0][2], 3.0f);
    ASSERT_EQ(std_arr_2_3[1][0], 4.0f);
    ASSERT_EQ(std_arr_2_3[1][1], 5.0f);
    ASSERT_EQ(std_arr_2_3[1][2], 6.0f);
    static_assert(std::same_as<decltype(std_arr_2_3), std::array<std::array<float, 3>, 2>>);
}
TEST_F(UnitTest, ConvertingArray__From_StdArray)
{
    vis::converting::array<float, 3> arr;
    std::array<float, 3> std_arr = { 1.0f, 2.0, 3.0f };
    ASSERT_EQ(arr[0], 0.0f);
    ASSERT_EQ(arr[1], 0.0f);
    ASSERT_EQ(arr[2], 0.0f);

    arr = std_arr;
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 3.0f);
}
TEST_F(UnitTest, ConvertingArray__From_StdArray_BadSize)
{
    vis::converting::array<float, 3> arr;
    std::array<float, 2> std_arr_2 = { 1.0f, 2.0f };
    std::array<float, 4> std_arr_4 = { 1.0f, 2.0f, 3.0f, 4.0f };

    ASSERT_ANY_THROW(arr = std_arr_2);
    ASSERT_ANY_THROW(arr = std_arr_4);
}
TEST_F(UnitTest, ConvertingArray__From_StdVector)
{
    vis::converting::array<float, 3> arr;
    std::vector<float> std_arr = { 1.0f, 2.0, 3.0f };
    ASSERT_EQ(arr[0], 0.0f);
    ASSERT_EQ(arr[1], 0.0f);
    ASSERT_EQ(arr[2], 0.0f);

    arr = std_arr;
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 3.0f);
}
TEST_F(UnitTest, ConvertingArray__From_StdVector_BadSize)
{
    vis::converting::array<float, 3> arr;
    std::vector<float> std_arr_2 = { 1.0f, 2.0f };
    std::vector<float> std_arr_4 = { 1.0f, 2.0f, 3.0f, 4.0f };

    ASSERT_ANY_THROW(arr = std_arr_2);
    ASSERT_ANY_THROW(arr = std_arr_4);
}
TEST_F(UnitTest, ConvertingArray__From_EigenVectorStatic)
{
    vis::converting::array<float, 3> arr;
    Eigen::Vector<float, 3> eigen_arr = { 1.0f, 2.0f, 3.0f };
    ASSERT_EQ(arr[0], 0.0f);
    ASSERT_EQ(arr[1], 0.0f);
    ASSERT_EQ(arr[2], 0.0f);

    arr = eigen_arr;
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 3.0f);
}
TEST_F(UnitTest, ConvertingArray__From_EigenVectorStatic_BadSize)
{
    vis::converting::array<float, 3> arr;
    Eigen::Vector<float, 2> eigen_arr_2 = { 1.0f, 2.0f };
    Eigen::Vector<float, 4> eigen_arr_4 = { 1.0f, 2.0f, 3.0f, 4.0f };

    ASSERT_ANY_THROW(arr = eigen_arr_2);
    ASSERT_ANY_THROW(arr = eigen_arr_4);
}
TEST_F(UnitTest, ConvertingArray__From_EigenVectorDynamic)
{
    vis::converting::array<float, 3> arr;
    Eigen::VectorXf eigen_arr(3);
    eigen_arr << 1.0f, 2.0f, 3.0f;
    ASSERT_EQ(arr[0], 0.0f);
    ASSERT_EQ(arr[1], 0.0f);
    ASSERT_EQ(arr[2], 0.0f);

    arr = eigen_arr;
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 3.0f);
}
TEST_F(UnitTest, ConvertingArray__From_EigenVectorDynamic_BadSize)
{
    vis::converting::array<float, 3> arr;
    Eigen::VectorXf eigen_arr_2(2);
    eigen_arr_2 << 1.0f, 2.0f;
    Eigen::VectorXf eigen_arr_4(4);
    eigen_arr_4 << 1.0f, 2.0f, 3.0f, 4.0f;

    ASSERT_ANY_THROW(arr = eigen_arr_2);
    ASSERT_ANY_THROW(arr = eigen_arr_4);
}
TEST_F(UnitTest, ConvertingArray__From_EigenMatrixStatic)
{
    vis::converting::array<float, 6> arr;
    Eigen::Matrix<float, 2, 3, Eigen::RowMajor> eigen_arr_2_3;
    Eigen::Matrix<float, 3, 2, Eigen::RowMajor> eigen_arr_3_2;
    eigen_arr_2_3 <<  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f;
    eigen_arr_3_2 << -1.0f, -2.0f, -3.0f, -4.0f,- 5.0f, -6.0f;
    ASSERT_EQ(arr[0], 0.0f);
    ASSERT_EQ(arr[1], 0.0f);
    ASSERT_EQ(arr[2], 0.0f);
    ASSERT_EQ(arr[3], 0.0f);
    ASSERT_EQ(arr[4], 0.0f);
    ASSERT_EQ(arr[5], 0.0f);

    arr = eigen_arr_2_3;
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 3.0f);
    ASSERT_EQ(arr[3], 4.0f);
    ASSERT_EQ(arr[4], 5.0f);
    ASSERT_EQ(arr[5], 6.0f);

    arr = eigen_arr_3_2;
    ASSERT_EQ(arr[0], -1.0f);
    ASSERT_EQ(arr[1], -2.0f);
    ASSERT_EQ(arr[2], -3.0f);
    ASSERT_EQ(arr[3], -4.0f);
    ASSERT_EQ(arr[4], -5.0f);
    ASSERT_EQ(arr[5], -6.0f);
}
TEST_F(UnitTest, ConvertingArray__From_EigenMatrixStatic_BadSize)
{
    vis::converting::array<float, 4> arr;
    Eigen::Matrix<float, 3, 3, Eigen::RowMajor> eigen_arr_3_3;
    Eigen::Matrix<float, 2, 3, Eigen::RowMajor> eigen_arr_2_3;
    eigen_arr_2_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f;
    eigen_arr_3_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f;

    ASSERT_ANY_THROW(arr = eigen_arr_2_3);
    ASSERT_ANY_THROW(arr = eigen_arr_3_3);
}
TEST_F(UnitTest, ConvertingArray__From_EigenMatrixDynamic)
{
    vis::converting::array<float, 6> arr;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_arr_2_3(2, 3);
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_arr_3_2(3, 2);
    eigen_arr_2_3 <<  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f;
    eigen_arr_3_2 << -1.0f, -2.0f, -3.0f, -4.0f,- 5.0f, -6.0f;
    ASSERT_EQ(arr[0], 0.0f);
    ASSERT_EQ(arr[1], 0.0f);
    ASSERT_EQ(arr[2], 0.0f);
    ASSERT_EQ(arr[3], 0.0f);
    ASSERT_EQ(arr[4], 0.0f);
    ASSERT_EQ(arr[5], 0.0f);

    arr = eigen_arr_2_3;
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 3.0f);
    ASSERT_EQ(arr[3], 4.0f);
    ASSERT_EQ(arr[4], 5.0f);
    ASSERT_EQ(arr[5], 6.0f);

    arr = eigen_arr_3_2;
    ASSERT_EQ(arr[0], -1.0f);
    ASSERT_EQ(arr[1], -2.0f);
    ASSERT_EQ(arr[2], -3.0f);
    ASSERT_EQ(arr[3], -4.0f);
    ASSERT_EQ(arr[4], -5.0f);
    ASSERT_EQ(arr[5], -6.0f);
}
TEST_F(UnitTest, ConvertingArray__From_EigenMatrixDynamic_BadSize)
{
    vis::converting::array<float, 4> arr;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_arr_2_3(2, 3);
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_arr_3_3(3, 3);
    eigen_arr_2_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f;
    eigen_arr_3_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f;

    ASSERT_ANY_THROW(arr = eigen_arr_2_3);
    ASSERT_ANY_THROW(arr = eigen_arr_3_3);
}
TEST_F(UnitTest, ConvertingArray__From_EigenMatrixStatic_RowOrCol)
{
    vis::converting::array<float, 3> arr;
    Eigen::Matrix<float, 3, 3> eigen_mat;
    eigen_mat << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f;
    ASSERT_EQ(arr[0], 0.0f);
    ASSERT_EQ(arr[1], 0.0f);
    ASSERT_EQ(arr[2], 0.0f);

    arr = eigen_mat.row(0);;
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 3.0f);

    arr = eigen_mat.col(0);
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 4.0f);
    ASSERT_EQ(arr[2], 7.0f);
}
TEST_F(UnitTest, ConvertingArray__From_EigenMatrixStatic_RowOrCol_BadSize)
{
    vis::converting::array<float, 4> arr;
    Eigen::Matrix<float, 3, 3, Eigen::RowMajor> eigen_arr_3_3;
    Eigen::Matrix<float, 2, 3, Eigen::RowMajor> eigen_arr_2_3;
    eigen_arr_3_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f;
    eigen_arr_2_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f;

    ASSERT_ANY_THROW(arr = eigen_arr_3_3.col(0));
    ASSERT_ANY_THROW(arr = eigen_arr_2_3.row(1));
}
TEST_F(UnitTest, ConvertingArray__From_EigenMatrixDynamic_RowOrCol)
{
    vis::converting::array<float, 3> arr;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_mat(3, 3);
    eigen_mat << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f;
    ASSERT_EQ(arr[0], 0.0f);
    ASSERT_EQ(arr[1], 0.0f);
    ASSERT_EQ(arr[2], 0.0f);

    arr = eigen_mat.row(0);;
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 3.0f);

    arr = eigen_mat.col(0);
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 4.0f);
    ASSERT_EQ(arr[2], 7.0f);
}
TEST_F(UnitTest, ConvertingArray__From_EigenMatrixDynamic_RowOrCol_BadSize)
{
    vis::converting::array<float, 4> arr;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_arr_3_3(3, 3);
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_arr_2_3(2, 3);
    eigen_arr_3_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f;
    eigen_arr_2_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f;

    ASSERT_ANY_THROW(arr = eigen_arr_3_3.col(0));
    ASSERT_ANY_THROW(arr = eigen_arr_2_3.row(1));
}
TEST_F(UnitTest, ConvertingVector__To_StdVector)
{
    vis::converting::vector<float> vec = { 1.0f, 2.0f, 3.0f };
    auto std_vec = vec.to_std();

    ASSERT_EQ(std_vec.size(), 3);
    ASSERT_EQ(std_vec[0], 1.0f);
    ASSERT_EQ(std_vec[1], 2.0f);
    ASSERT_EQ(std_vec[2], 3.0f);
    static_assert(std::same_as<decltype(std_vec), std::vector<float>>);

    vis::converting::vector<vis::converting::vector<float>> vec_2_3 = { { 1.0f, 2.0f, 3.0f },
                                                                        { 4.0f, 5.0f, 6.0f } };
    auto std_vec_2_3 = vec_2_3.to_std();
    ASSERT_EQ(std_vec_2_3.size(), 2);
    ASSERT_EQ(std_vec_2_3[0].size(), 3);
    ASSERT_EQ(std_vec_2_3[0][0], 1.0f);
    ASSERT_EQ(std_vec_2_3[0][1], 2.0f);
    ASSERT_EQ(std_vec_2_3[0][2], 3.0f);
    ASSERT_EQ(std_vec_2_3[1][0], 4.0f);
    ASSERT_EQ(std_vec_2_3[1][1], 5.0f);
    ASSERT_EQ(std_vec_2_3[1][2], 6.0f);
    static_assert(std::same_as<decltype(std_vec_2_3), std::vector<std::vector<float>>>);
}
TEST_F(UnitTest, ConvertingVector__Reference)
{
    vis::converting::vector<float> arr = { 1.0f, 2.0f, 3.0f };
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 3.0f);

    arr[0] = 3.0f;
    arr[1] = 2.0f;
    arr[2] = 1.0f;
    ASSERT_EQ(arr[0], 3.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 1.0f);

    static_assert(std::same_as<decltype(arr[0]), float&>);
}
TEST_F(UnitTest, ConvertingVector__ConstReference)
{
    const vis::converting::vector<float> arr = { 1.0f, 2.0f, 3.0f };
    ASSERT_EQ(arr[0], 1.0f);
    ASSERT_EQ(arr[1], 2.0f);
    ASSERT_EQ(arr[2], 3.0f);

    static_assert(std::same_as<decltype(arr[0]), const float&>);
}
TEST_F(UnitTest, ConvertingVector__From_StdArray)
{
    vis::converting::vector<float> vec;
    std::array<float, 3> std_arr = { 1.0f, 2.0, 3.0f };
    ASSERT_EQ(vec.size(), 0);

    vec = std_arr;
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
}
TEST_F(UnitTest, ConvertingVector__From_StdArray_DiffSize)
{
    vis::converting::vector<float> vec;
    std::array<float, 2> std_arr_2 = { 1.0f, 2.0f };
    std::array<float, 4> std_arr_4 = { 1.0f, 2.0f, 3.0f, 4.0f };
    ASSERT_EQ(vec.size(), 0);

    vec = std_arr_2;
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);

    vec = std_arr_4;
    ASSERT_EQ(vec.size(), 4);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
    ASSERT_EQ(vec[3], 4.0f);
}
TEST_F(UnitTest, ConvertingVector__From_StdVector)
{
    vis::converting::vector<float> vec;
    std::vector<float> std_vec = { 1.0f, 2.0, 3.0f };
    ASSERT_EQ(vec.size(), 0);

    vec = std_vec;
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
}
TEST_F(UnitTest, ConvertingVector__From_StdVector_DiffSize)
{
    vis::converting::vector<float> vec;
    std::vector<float> std_vec_2 = { 1.0f, 2.0f };
    std::vector<float> std_vec_4 = { 1.0f, 2.0f, 3.0f, 4.0f };
    ASSERT_EQ(vec.size(), 0);

    vec = std_vec_2;
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);

    vec = std_vec_4;
    ASSERT_EQ(vec.size(), 4);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
    ASSERT_EQ(vec[3], 4.0f);
}
TEST_F(UnitTest, ConvertingVector__From_EigenVectorStatic)
{
    vis::converting::vector<float> vec;
    Eigen::Vector<float, 3> eigen_vec = { 1.0f, 2.0f, 3.0f };
    ASSERT_EQ(vec.size(), 0);

    vec = eigen_vec;
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
}
TEST_F(UnitTest, ConvertingVector__From_EigenVectorStatic_DiffSize)
{
    vis::converting::vector<float> vec;
    Eigen::Vector<float, 2> eigen_vec_2 = { 1.0f, 2.0f };
    Eigen::Vector<float, 4> eigen_vec_4 = { 1.0f, 2.0f, 3.0f, 4.0f };
    ASSERT_EQ(vec.size(), 0);

    vec = eigen_vec_2;
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);

    vec = eigen_vec_4;
    ASSERT_EQ(vec.size(), 4);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
    ASSERT_EQ(vec[3], 4.0f);
}
TEST_F(UnitTest, ConvertingVector__From_EigenVectorDynamic)
{
    vis::converting::vector<float> vec;
    Eigen::VectorXf eigen_vec(3);
    eigen_vec << 1.0f, 2.0f, 3.0f;
    ASSERT_EQ(vec.size(), 0);

    vec = eigen_vec;
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
}
TEST_F(UnitTest, ConvertingVector__From_EigenVectorDynamic_DiffSize)
{
    vis::converting::vector<float> vec;
    Eigen::VectorXf eigen_vec_2(2);
    Eigen::VectorXf eigen_vec_4(4);
    eigen_vec_2 << 1.0f, 2.0f;
    eigen_vec_4 << 1.0f, 2.0f, 3.0f, 4.0f;
    ASSERT_EQ(vec.size(), 0);

    vec = eigen_vec_2;
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);

    vec = eigen_vec_4;
    ASSERT_EQ(vec.size(), 4);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
    ASSERT_EQ(vec[3], 4.0f);
}
TEST_F(UnitTest, ConvertingVector__From_EigenMatrixStatic)
{
    vis::converting::vector<float> vec;
    Eigen::Matrix<float, 2, 3, Eigen::RowMajor> eigen_mat_2_3;
    Eigen::Matrix<float, 3, 2, Eigen::RowMajor> eigen_mat_3_2;
    eigen_mat_2_3 <<  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f;
    eigen_mat_3_2 << -1.0f, -2.0f, -3.0f, -4.0f,- 5.0f, -6.0f;
    ASSERT_EQ(vec.size(), 0);

    vec = eigen_mat_2_3;
    ASSERT_EQ(vec.size(), 6);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
    ASSERT_EQ(vec[3], 4.0f);
    ASSERT_EQ(vec[4], 5.0f);
    ASSERT_EQ(vec[5], 6.0f);

    vec = eigen_mat_3_2;
    ASSERT_EQ(vec.size(), 6);
    ASSERT_EQ(vec[0], -1.0f);
    ASSERT_EQ(vec[1], -2.0f);
    ASSERT_EQ(vec[2], -3.0f);
    ASSERT_EQ(vec[3], -4.0f);
    ASSERT_EQ(vec[4], -5.0f);
    ASSERT_EQ(vec[5], -6.0f);
}
TEST_F(UnitTest, ConvertingVector__From_EigenMatrixStatic_DiffSize)
{
    vis::converting::vector<float> vec;
    Eigen::Matrix<float, 3, 3, Eigen::RowMajor> eigen_mat_3_3;
    Eigen::Matrix<float, 2, 3, Eigen::RowMajor> eigen_mat_2_3;
    eigen_mat_3_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f;
    eigen_mat_2_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f;
    ASSERT_EQ(vec.size(), 0);

    vec = eigen_mat_3_3;
    ASSERT_EQ(vec.size(), 9);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
    ASSERT_EQ(vec[3], 4.0f);
    ASSERT_EQ(vec[4], 5.0f);
    ASSERT_EQ(vec[5], 6.0f);
    ASSERT_EQ(vec[6], 7.0f);
    ASSERT_EQ(vec[7], 8.0f);
    ASSERT_EQ(vec[8], 9.0f);

    vec = eigen_mat_2_3;
    ASSERT_EQ(vec.size(), 6);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
    ASSERT_EQ(vec[3], 4.0f);
    ASSERT_EQ(vec[4], 5.0f);
    ASSERT_EQ(vec[5], 6.0f);
}
TEST_F(UnitTest, ConvertingVector__From_EigenMatrixDynamic)
{
    vis::converting::vector<float> vec;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_mat_2_3(2, 3);
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_mat_3_2(3, 2);
    eigen_mat_2_3 <<  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f;
    eigen_mat_3_2 << -1.0f, -2.0f, -3.0f, -4.0f,- 5.0f, -6.0f;
    ASSERT_EQ(vec.size(), 0);

    vec = eigen_mat_2_3;
    ASSERT_EQ(vec.size(), 6);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
    ASSERT_EQ(vec[3], 4.0f);
    ASSERT_EQ(vec[4], 5.0f);
    ASSERT_EQ(vec[5], 6.0f);

    vec = eigen_mat_3_2;
    ASSERT_EQ(vec.size(), 6);
    ASSERT_EQ(vec[0], -1.0f);
    ASSERT_EQ(vec[1], -2.0f);
    ASSERT_EQ(vec[2], -3.0f);
    ASSERT_EQ(vec[3], -4.0f);
    ASSERT_EQ(vec[4], -5.0f);
    ASSERT_EQ(vec[5], -6.0f);
}
TEST_F(UnitTest, ConvertingVector__From_EigenMatrixDynamic_DiffSize)
{
    vis::converting::vector<float> vec;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_mat_3_3(3, 3);
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_mat_2_3(2, 3);
    eigen_mat_3_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f;
    eigen_mat_2_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f;
    ASSERT_EQ(vec.size(), 0);

    vec = eigen_mat_3_3;
    ASSERT_EQ(vec.size(), 9);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
    ASSERT_EQ(vec[3], 4.0f);
    ASSERT_EQ(vec[4], 5.0f);
    ASSERT_EQ(vec[5], 6.0f);
    ASSERT_EQ(vec[6], 7.0f);
    ASSERT_EQ(vec[7], 8.0f);
    ASSERT_EQ(vec[8], 9.0f);

    vec = eigen_mat_2_3;
    ASSERT_EQ(vec.size(), 6);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);
    ASSERT_EQ(vec[3], 4.0f);
    ASSERT_EQ(vec[4], 5.0f);
    ASSERT_EQ(vec[5], 6.0f);
}
TEST_F(UnitTest, ConvertingVector__From_EigenMatrixStatic_RowOrCol)
{
    vis::converting::vector<float> vec;
    Eigen::Matrix<float, 3, 3> eigen_mat_3_3;
    eigen_mat_3_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f;
    ASSERT_EQ(vec.size(), 0);

    vec = eigen_mat_3_3.row(0);
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);

    vec = eigen_mat_3_3.col(0);
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 4.0f);
    ASSERT_EQ(vec[2], 7.0f);
}
TEST_F(UnitTest, ConvertingVector__From_EigenMatrixStatic_RowOrCol_DiffSize)
{
    vis::converting::vector<float> vec;
    Eigen::Matrix<float, 3, 3, Eigen::RowMajor> eigen_mat_3_3;
    Eigen::Matrix<float, 2, 3, Eigen::RowMajor> eigen_mat_2_3;
    eigen_mat_3_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f;
    eigen_mat_2_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f;
    ASSERT_EQ(vec.size(), 0);

    vec = eigen_mat_3_3.row(0);
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);

    vec = eigen_mat_3_3.col(0);
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 4.0f);
    ASSERT_EQ(vec[2], 7.0f);
}
TEST_F(UnitTest, ConvertingVector__From_EigenMatrixDynamic_RowOrCol)
{
    vis::converting::vector<float> vec;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_mat_3_3(3, 3);
    eigen_mat_3_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f;
    ASSERT_EQ(vec.size(), 0);

    vec = eigen_mat_3_3.row(0);
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);

    vec = eigen_mat_3_3.col(0);
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 4.0f);
    ASSERT_EQ(vec[2], 7.0f);
}
TEST_F(UnitTest, ConvertingVector__From_EigenMatrixDynamic_RowOrCol_DiffSize)
{
    vis::converting::vector<float> vec;
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_mat_3_3(3, 3);
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> eigen_mat_2_3(2, 3);
    eigen_mat_3_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f;
    eigen_mat_2_3 << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f;
    ASSERT_EQ(vec.size(), 0);

    vec = eigen_mat_3_3.row(0);
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 2.0f);
    ASSERT_EQ(vec[2], 3.0f);

    vec = eigen_mat_3_3.col(0);
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1.0f);
    ASSERT_EQ(vec[1], 4.0f);
    ASSERT_EQ(vec[2], 7.0f);
}