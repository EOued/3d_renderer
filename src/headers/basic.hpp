#ifndef BASIC_HPP
#define BASIC_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
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

  Matrix(std::initializer_list<T> _data)
    requires(Lines == 1)
  {
    size_t i = 0;
    for (auto& val : _data) (*this)[0, i++] = val;
  }

  const static Matrix<T, Lines, Columns> Identity()
  {
    Matrix<T, Lines, Columns> m;
    for (size_t i = 0; i < Lines; i++) m[i, i] = 1;
    return m;
  }

  const static Matrix<double, 3, 3>
  IsometricProjection(double alpha = 0.20 * M_PI, double beta = 0.25 * M_PI,
                      double gamma = 0)
  {
    Matrix<double, 3, 3> m_x = {
        {1,               0,                    0},
        {0, std::cos(alpha), -1 * std::sin(alpha)},
        {0, std::sin(alpha),      std::cos(alpha)}
    };

    Matrix<double, 3, 3> m_y = {
        {std::cos(beta), 0, -1 * std::sin(beta)},
        {             0, 1,                   0},
        {std::sin(beta), 0,      std::cos(beta)}
    };

    Matrix<double, 3, 3> m_z = {
        {std::cos(gamma), -1 * std::sin(gamma), 0},
        {std::sin(gamma),      std::cos(gamma), 0},
        {              0,                    0, 1}
    };

    return m_x * m_y * m_z;
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
  template <typename U, size_t _Lines, size_t _Columns>
  Matrix<T, Lines, Columns>
  operator+(const Matrix<U, _Lines, _Columns>& rhs) const
  {
    static_assert(Lines == _Lines && Columns == _Columns,
                  "Summing matrixes is allowed on matrixes of same size");
    Matrix<T, Lines, Columns> result;
    for (size_t i = 0; i < Lines * Columns; i++)
      result[i] = (*this)[i] + rhs[i];
    return result;
  }

  template <typename U, size_t _Lines, size_t _Columns>
  void operator+=(const Matrix<U, _Lines, _Columns>& rhs)
  {
    static_assert(Lines == _Lines && Columns == _Columns,
                  "Summing matrixes is allowed on matrixes of same size");
    for (size_t i = 0; i < Lines * Columns; i++)
      (*this)[i] = (*this)[i] + rhs[i];
  }

  template <typename U, size_t _Lines, size_t _Columns>
  Matrix<T, Lines, Columns>
  operator-(const Matrix<U, _Lines, _Columns>& rhs) const
  {
    static_assert(Lines == _Lines && Columns == _Columns,
                  "Summing matrixes is allowed on matrixes of same size");
    Matrix<T, Lines, Columns> result;
    for (size_t i = 0; i < Lines * Columns; i++)
      result[i] = (*this)[i] - rhs[i];
    return result;
  }

  template <typename U, size_t _Lines, size_t _Columns>
  void operator-=(const Matrix<U, _Lines, _Columns>& rhs)
  {
    static_assert(Lines == _Lines && Columns == _Columns,
                  "Summing matrixes is allowed on matrixes of same size");

    for (size_t i = 0; i < Lines * Columns; i++)
      (*this)[i] = (*this)[i] - rhs[i];
  }

  // Scalar multiplication
  template <typename U>
  Matrix<T, Lines, Columns> operator*(const U scalar) const
  {
    Matrix<T, Lines, Columns> result;
    for (size_t i = 0; i < Lines * Columns; i++)
      result[i] = scalar * (*this)[i];
    return result;
  }

  template <typename U> void operator*=(const U scalar)
  {
    for (size_t i = 0; i < Lines * Columns; i++)
      (*this)[i] = scalar * (*this)[i];
  }

  // Matrix multiplication
  template <typename U, size_t _Lines, size_t _Columns>
  Matrix<T, Lines, _Columns>
  operator*(const Matrix<U, _Lines, _Columns>& rhs) const
  {
    static_assert(Columns == _Lines,
                  "lhs columns number needs to be equal to rhs lines number");

    // Using basic multiplication matrix, as we use only small matrixes
    T sum;
    Matrix<T, Lines, _Columns> result;
    for (size_t i = 0; i < Lines; i++)
      for (size_t j = 0; j < _Columns; j++)
      {
        sum = 0;
        for (size_t k = 0; k < Columns; k++) sum += (*this)[i, k] * rhs[k, j];
        result[i, j] = sum;
      }
    return result;
  }

  // Unoptimised
  template <typename U, size_t _Lines, size_t _Columns>
  void operator*=(const Matrix<U, _Lines, _Columns>& rhs)
  {
    static_assert(Columns == _Lines && Columns == _Columns,
                  "Matrix multiplication assignement is allowed if the result "
                  "matrix is the same size as the left-size operand");

    // Using basic multiplication matrix, as we use only small matrixes
    T sum;
    Matrix<T, Lines, Columns> result;
    for (size_t i = 0; i < Lines; i++)
      for (size_t j = 0; j < _Columns; j++)
      {
        sum = 0;
        for (size_t k = 0; k < Columns; k++) sum += (*this)[i, k] * rhs[k, j];
        result[i, j] = sum;
      }
    (*this) = std::move(result);
  }

  // Dot product
  template <typename U>
  Matrix<T, 3, 1> cross_product(const Matrix<U, 3, 1>& rhs) const
  {
    static_assert(Lines * Columns == 3,
                  "Cross product is allowed on vectors only");

    const linalg::Matrix<T, 3, 3> cp_x = {
        {          0, -(*this)[2],  (*this)[1]},
        { (*this)[2],           0, -(*this)[1]},
        {-(*this)[1],  (*this)[0],           0}
    };
    return cp_x * rhs;
  }

  template <typename U, size_t _Lines>
  Matrix<T, Lines, _Lines> outer_product(const Matrix<U, _Lines, 1>& rhs) const
  {
    static_assert(Columns == 1,
                  "Outer product is only allowed for (mathematical) vectors.");
    return (*this) * rhs.transpose();
  }

  Matrix<T, Lines, Lines> outer_product() const
  {
    static_assert(Columns == 1,
                  "Outer product is only allowed for (mathematical) vectors.");
    return (*this) * (*this).transpose();
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

private:
  std::array<T, Lines * Columns> data;
};

template <typename T, size_t N> using Vector = Matrix<T, 1, N>;

// algebra formulas
template <typename T>
Vector<T, 3> Rodrigues_Formula(double angle, Matrix<T, 3, 1> vector,
                               Matrix<T, 3, 1> axis)
{
  return vector * std::cos(angle) +
         axis.cross_product(vector) * std::sin(angle) +
         axis.outer_product() * vector * (1 - cos(angle));
}

template <typename T>
Vector<T, 3> Rodrigues_Formula(double angle, Matrix<T, 1, 3> vector,
                               Matrix<T, 3, 1> axis)
{
  return Rodrigues_Formula(angle, vector, axis.transpose());
}

template <typename T>
Vector<T, 3> Rodrigues_Formula(double angle, Matrix<T, 3, 1> vector,
                               Matrix<T, 1, 3> axis)
{
  return Rodrigues_Formula(angle, vector.transpose(), axis);
}

template <typename T>
Vector<T, 3> Rodrigues_Formula(double angle, Matrix<T, 1, 3> vector,
                               Matrix<T, 1, 3> axis)
{
  return Rodrigues_Formula(angle, vector.transpose(), axis.transpose());
}

} // namespace linalg
#endif
