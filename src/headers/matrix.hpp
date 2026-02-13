#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <array>
#include <cmath>
#include <ostream>
#include <vector>

namespace linalg
{

template <typename T, size_t Lines, size_t Columns> class Matrix
{
public:
  // Constructors
  Matrix() {}
  Matrix(std::initializer_list<std::initializer_list<T>> _data)
  {
    size_t j = 0;
    for (auto& row : _data)
    {
      size_t i = 0;
      for (auto& val : row) (*this)[j, i++] = val;
      j++;
    }
  }

  const static Matrix<T, Lines, Columns> Identity()
  {
    Matrix<T, Lines, Columns> m;
    for (size_t i = 0; i < Lines; i++) m[i, i] = 1;
    return m;
  }

  const static Matrix<T, Lines, Columns>
  ScalingMatrix(std::vector<T> scale_factors)
  {
    Matrix<T, Lines, Columns> m;
    static_assert(Lines == Columns, "Scaling Matrix should be square.");
    for (size_t i = 0; i < std::min(scale_factors.size(), Lines); i++)
      m[i, i] = scale_factors[i];
    return m;
  }

  template <typename U, size_t _Lines, size_t _Columns>
  Matrix(const Matrix<U, _Lines, _Columns>& other)
  {
    // Transposition
    if constexpr (Lines == _Columns && Columns == _Lines)
    {
      static_assert(
          std::is_same_v<T, U>,
          "Auto-transposition is only allowed on matrix of same type.");

      (*this) = other.transpose();
    }
    else
    {
      static_assert(Lines == _Lines && Columns == _Columns,
                    "Typecasting matrix is allowed on matrixes of same size");
      for (size_t i = 0; i < Lines * Columns; i++) (*this)[i] = other[i];
    }
  }

  // Print
  friend std::ostream& operator<<(std::ostream& os,
                                  const Matrix<T, Lines, Columns>& p)
  {
    for (size_t i = 0; i < Lines * Columns; i++)
    {
      if (i != 0 && !(i % Columns)) os << '\n';
      os << p[i] << ' ';
    }

    return os;
  }

  Matrix<T, Columns, Lines> transpose(void) const
  {
    Matrix<T, Columns, Lines> result;
    for (size_t col = 0; col < Columns; col++)
      for (size_t lin = 0; lin < Lines; lin++)
        result[col, lin] = (*this)[lin, col];
    return result;
  }

  // ============================== OPERATIONS ==============================

  // Operations overload

  // Matrix summing
  template <typename U>
  auto operator+(const Matrix<U, Lines, Columns>& rhs) const
  {
    using R = std::common_type_t<T, U>;
    Matrix<R, Lines, Columns> result;
    for (size_t i = 0; i < Lines * Columns; i++)
      result[i] = (*this)[i] + rhs[i];
    return result;
  }

  template <typename U> void operator+=(const Matrix<U, Lines, Columns>& rhs)
  {
    for (size_t i = 0; i < Lines * Columns; i++)
      (*this)[i] = (*this)[i] + rhs[i];
  }

  template <typename U>
  auto operator-(const Matrix<U, Lines, Columns>& rhs) const
  {
    Matrix<std::common_type_t<T, U>, Lines, Columns> result;
    for (size_t i = 0; i < Lines * Columns; i++)
      result[i] = (*this)[i] - rhs[i];
    return result;
  }

  template <typename U> void operator-=(const Matrix<U, Lines, Columns>& rhs)
  {
    for (size_t i = 0; i < Lines * Columns; i++)
      (*this)[i] = (*this)[i] - rhs[i];
  }

  // Scalar multiplication
  template <typename U>
    requires std::is_arithmetic_v<U>
  Matrix<T, Lines, Columns> operator*(const U scalar) const
  {
    Matrix<T, Lines, Columns> result;
    for (size_t i = 0; i < Lines * Columns; i++)
      result[i] = scalar * (*this)[i];
    return result;
  }

  template <typename U>
    requires std::is_arithmetic_v<U>
  void operator*=(const U scalar)
  {
    for (size_t i = 0; i < Lines * Columns; i++)
      (*this)[i] = scalar * (*this)[i];
  }

  // Matrix multiplication
  template <typename U, size_t _Columns>
  Matrix<T, Lines, _Columns>
  operator*(const Matrix<U, Columns, _Columns>& rhs) const
  {
    // Using basic multiplication matrix, as we use only small matrixes
    using R = std::common_type_t<T, U>;
    R sum;
    Matrix<R, Lines, _Columns> result;
    for (size_t i = 0; i < Lines; i++)
      for (size_t j = 0; j < _Columns; j++)
      {
        sum = 0;
        for (size_t k = 0; k < Columns; k++) sum += (*this)[i, k] * rhs[k, j];
        result[i, j] = sum;
      }
    return result;
  }

  // Scalar multiplication
  template <typename U>
    requires std::is_arithmetic_v<U>
  Matrix<T, Lines, Columns> operator/(const U scalar) const
  {
    return (*this) * (1 / scalar);
  }

  template <typename U>
    requires std::is_arithmetic_v<U>
  void operator/=(const U scalar)
  {
    for (size_t i = 0; i < Lines * Columns; i++)
      (*this)[i] = scalar / (*this)[i];
  }

  // Unoptimised
  template <typename U> void operator*=(const Matrix<U, Columns, Columns>& rhs)
  {
    // Using basic multiplication matrix, as we use only small matrixes
    using R = std::common_type<T, U>;
    R sum;
    Matrix<R, Lines, Columns> result;
    for (size_t i = 0; i < Lines; i++)
      for (size_t j = 0; j < Columns; j++)
      {
        sum = 0;
        for (size_t k = 0; k < Columns; k++) sum += (*this)[i, k] * rhs[k, j];
        result[i, j] = sum;
      }
    (*this) = std::move(result);
  }

  // Data accessing
  T& operator[](std::size_t idx)
  {
    if (idx >= Lines * Columns)
      throw std::runtime_error("Access to element outside of matrix.");
    return this->data[idx];
  }
  const T& operator[](std::size_t idx) const
  {
    if (idx >= Lines * Columns)
      throw std::runtime_error("Access to element outside of matrix.");
    return this->data[idx];
  }

  T& operator[](std::size_t idx1, std::size_t idx2)
  {
    if (idx1 >= Lines || idx2 >= Columns)
      throw std::runtime_error("Access to element outside of matrix.");
    return this->data[idx1 * Columns + idx2];
  }
  const T& operator[](std::size_t idx1, std::size_t idx2) const
  {
    if (idx1 >= Lines || idx2 >= Columns)
      throw std::runtime_error("Access to element outside of matrix.");
    return this->data[idx1 * Columns + idx2];
  }

protected:
  std::array<T, Lines * Columns> data;
};

template <typename T, size_t Lines, size_t Columns, typename U>
Matrix<T, Lines, Columns> operator*(const U scalar,
                                    const Matrix<T, Lines, Columns>& rhs)
{
  Matrix<T, Lines, Columns> result;
  for (size_t i = 0; i < Lines * Columns; i++) result[i] = scalar * rhs[i];
  return result;
}
} // namespace linalg
#endif
