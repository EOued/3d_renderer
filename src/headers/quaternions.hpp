#ifndef QUATERNIONS_HPP
#define QUATERNIONS_HPP

#include <ostream>
#include "vectors.hpp"

namespace linalg
{

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
