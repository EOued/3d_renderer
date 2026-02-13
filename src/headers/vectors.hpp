#ifndef VECTORS_H
#define VECTORS_H

#include <ostream>
#include "matrix.hpp"

namespace linalg
{

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

} // namespace linalg
#endif
