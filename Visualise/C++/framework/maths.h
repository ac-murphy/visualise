#pragma once

#include <Eigen>

namespace vis::maths
{
    template <typename vector_type, typename value_type, size_t N = 0>
    auto to_eigen_vector(const vector_type& vector)
    {
        if constexpr (N == 0)
        {
            const size_t size = vector.size();
            Eigen::VectorX<value_type> eigen_vector(size);
            std::copy(vector.begin(), vector.end(), eigen_vector.data());
            return eigen_vector;
        }
        else
        {
            Eigen::Vector<value_type, N> eigen_vector;
            std::copy(vector.begin(), vector.end(), eigen_vector.data());
            return eigen_vector;
        }
    }

    template <typename matrix_type, typename value_type, size_t M = 0, size_t N = 0>
    auto to_eigen_matrix(const matrix_type& matrix, const size_t rows = 0, const size_t cols = 0)
    {
        if constexpr (M == 0 && N == 0)
        {
            if (rows == 0 || cols == 0)
                throw std::runtime_error("maths::to_eigen_matrix \"dynamically sized matrix rows and cols must be non-zero\"");

            Eigen::MatrixXf eigen_matrix(rows, cols);
            for (size_t i = 0; i < rows; i++)
                for (size_t j = 0; j < cols; j++)
                    eigen_matrix(i, j) = matrix(i, j);

            return eigen_matrix;
        }
        else
        {
            Eigen::Matrix<value_type, M, N> eigen_matrix;
            for (size_t i = 0; i < M; i++)
                for (size_t j = 0; j < N; j++)
                    eigen_matrix(i, j) = matrix(i, j);

            return eigen_matrix;
        }
    }
}