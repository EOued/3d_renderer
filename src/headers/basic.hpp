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

template <typename T, size_t N> class Vector : public Matrix<T, N, 1>
{
public:
  using Matrix<T, N, 1>::Matrix;
  template <typename U> Vector(const Matrix<U, N, 1>& m) : Matrix<T, N, 1>(m) {}

  Vector(std::initializer_list<T> _data)
  {
    size_t i = 0;
    for (auto& val : _data) (*this)[i++] = val;
  }

  // Dot product
  template <typename U>
  Vector<T, 3> cross_product(const Vector<U, 3>& rhs) const
    requires(N == 3)
  {
    const linalg::Matrix<T, 3, 3> cp_x = {
        {          0, -(*this)[2],  (*this)[1]},
        { (*this)[2],           0, -(*this)[0]},
        {-(*this)[1],  (*this)[0],           0},
    };
    return Vector<T, N>(cp_x * rhs);
  }

  template <typename U, size_t _N>
  Matrix<T, N, _N> outer_product(const Vector<U, _N>& rhs) const
  {
    return (*this) * rhs.transpose();
  }

  Matrix<T, N, N> outer_product() const
  {
    return (*this) * (*this).transpose();
  }

  template <typename U, size_t _N>
  T dot_product(const Vector<U, _N>& rhs) const
    requires(N == _N)
  {
    T sum = 0;
    for (size_t i = 0; i < N; i++) sum += (*this)[i] * rhs[i];
    return sum;
  }

  auto norm(void) const
  {
    T sum = 0;
    for (T value : this->data) sum += std::pow(value, 2);
    return std::sqrt(sum);
  }

  void normalise(void)
  {
    T norm = this->norm();
    if (!norm) return;
    for (size_t i = 0; i < N; i++) (*this)[i] /= norm;
    return;
  }

  Vector<T, N> normalized(void) const
  {
    T norm = this->norm();
    Vector<T, N> result;
    if (!norm) return result;
    for (size_t i = 0; i < N; i++) result[i] = (*this)[i] / norm;
    return result;
  }

  // Matrix multiplication
  template <typename U, size_t Columns>
  Matrix<T, N, Columns> operator*(const Matrix<U, 1, Columns>& rhs) const
  {
    return static_cast<const Matrix<T, N, 1>&>(*this) * rhs;
  }

  // Print
  friend std::ostream& operator<<(std::ostream& os, const Vector<T, N>& p)
  {
    os << p.transpose();
    return os;
  }
};

template <typename T> class Quaternion
{
public:
  Quaternion() {}

  Quaternion(T _w, T _v) : w(_w)
  {
    this->v[0] = _v;
    this->v[1] = _v;
    this->v[2] = _v;
  }
  Quaternion(Matrix<T, 3, 1> _v) : v(_v) {}
  Quaternion(Vector<T, 3> _v) : v(_v) {}
  Quaternion(T _w, Matrix<T, 3, 1> _v) : w(_w), v(_v) {}
  Quaternion(T _w, Vector<T, 3> _v) : w(_w), v(_v) {}

  const static Quaternion<T> Identity()
  {
    Quaternion<T> result;
    result.w = 1;
    return result;
  }

  T _w(void) { return this->w; }
  Vector<T, 3> _v(void) { return this->v; }

  template <typename U> Quaternion<T> operator*(const Quaternion<U>& rhs) const
  {
    Quaternion<T> result;
    result.w = this->w * rhs.w - this->v.dot_product(rhs.v);
    result.v = this->w * rhs.v + rhs.w * this->v + this->v.cross_product(rhs.v);
    return result;
  }

  template <typename U> void operator*=(const Quaternion<U>& rhs)
  {
    T _w    = this->w * rhs.w - this->v.dot_product(rhs.v);
    this->v = this->w * rhs.v + rhs.w * this->v + this->v.cross_product(rhs.v);
    this->w = _w;
  }

  Quaternion<T> conjugate(void) const
  {
    Quaternion<T> result;
    result.w = this->w;
    result.v = -1 * this->v;
    return result;
  }

  Quaternion<T> inverse(void) const
  {
    T n2 = w * w + v.dot_product(v); // norm squared

    Quaternion<T> result;
    result.w = w / n2;
    result.v = (-1 * v) / n2;

    return result;
  }

  auto norm(void) const
  {
    T sum = std::pow(w, 2);
    for (int i = 0; i < 3; i++) sum += std::pow(this->v[i], 2);
    return std::sqrt(sum);
  }

  void normalize(void)
  {
    auto norm = this->norm();
    this->w /= norm;
    v[0] /= norm;
    v[1] /= norm;
    v[2] /= norm;
  }

  friend std::ostream& operator<<(std::ostream& os, const Quaternion<T>& p)
  {
    os << p.w << ' ' << p.v.transpose();
    return os;
  }

private:
  T w = 0;
  Vector<T, 3> v;
};
} // namespace linalg
#endif
