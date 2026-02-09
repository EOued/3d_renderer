#ifndef BASIC_HPP
#define BASIC_HPP

#include <array>
#include <cmath>
#include <ostream>

template <typename T, uint Width, uint Height> class Matrix
{
public:
  // Constructors
  Matrix() {}

  static Matrix<T, Width, Height> Identity()
  {
    Matrix<T, Width, Height> m;
    for (uint i = 0; i < Width; i++) m.setValue(i, i, 1);
    return m;
  }

  static Matrix<float, 3, 3> IsometricProjection()
  {
    Matrix<float, 3, 3> projection;
    projection.setValue(0, 0, std::sqrt(3));
    projection.setValue(0, 2, std::sqrt(3));
    projection.setValue(1, 0, -1);
    projection.setValue(1, 1, 2);
    projection.setValue(1, 2, -1);
    projection.setValue(2, 0, std::sqrt(2));
    projection.setValue(2, 1, std::sqrt(2));
    projection.setValue(2, 2, std::sqrt(2));
    return projection * (1 / std::sqrt(6));
  }

  template <typename U, uint W, uint H> Matrix(const Matrix<U, W, H>& other)
  {
    static_assert(W == Width && H == Height,
                  "Typecasting matrix is allowed on matrixes of same size");
    for (uint i = 0; i < W * H; i++) this->setValue(i, other.getValue(i));
  }

  // Getters
  T getValue(const uint pos) const
  {
    if (pos >= Height * Width)
      throw std::runtime_error("Access to element outside of matrix.");
    return this->data[pos];
  }

  T getValue(const uint line, const uint col) const
  {
    if (line >= Height || col >= Width)
      throw std::runtime_error("Access to element outside of matrix.");
    return this->getValue(line * Height + col);
  }

  // Setters
  template <typename U> void setValue(const uint pos, const U value)
  {
    if (pos >= Height * Width)
      throw std::runtime_error("Access to element outside of matrix.");
    this->data[pos] = static_cast<T>(value);
    return;
  }

  template <typename U>
  void setValue(const uint line, const uint col, const U value)
  {
    if (col >= Width || line >= Height)
      throw std::runtime_error("Access to element outside of matrix.");
    setValue(line * Height + col, value);
    return;
  }

  // Print
  friend std::ostream& operator<<(std::ostream& os,
                                  const Matrix<T, Width, Height>& p)
  {
    for (uint i = 0; i < Height * Width; i++)
    {
      if (i != 0 && !(i % Height)) os << '\n';
      os << p.getValue(i) << ' ';
    }

    return os;
  }

  void transpose(void)
  {
    T temp;
    for (uint w = 0; w < Width; w++)
      for (uint h = w + 1; h < Height; h++)
      {
        if (w == h) continue;
        temp = this->getValue(w, h);
        this->setValue(w, h, this->getValue(h, w));
        this->setValue(h, w, temp);
      }
    return;
  }

  // Operations overload

  // Matrix summing
  template <typename U, uint W, uint H>
  Matrix<T, Width, Height> operator+(const Matrix<U, W, H>& rhs) const
  {
    static_assert(W == Width && H == Height,
                  "Summing matrixes is allowed on matrixes of same size");
    Matrix<T, Width, Height> result;
    for (uint i = 0; i < Width * Height; i++)
      result.setValue(i, this->getValue(i) + rhs.getValue(i));
    return result;
  }

  template <typename U, uint W, uint H>
  void operator+=(const Matrix<U, W, H>& rhs)
  {
    static_assert(W == Width && H == Height,
                  "Summing matrixes is allowed on matrixes of same size");
    for (uint i = 0; i < Width * Height; i++)
      this->setValue(i, this->getValue(i) + rhs.getValue(i));
  }

  template <typename U, uint W, uint H>
  Matrix<T, Width, Height> operator-(const Matrix<U, W, H>& rhs) const
  {
    static_assert(W == Width && H == Height,
                  "Summing matrixes is allowed on matrixes of same size");
    Matrix<T, Width, Height> result;
    for (uint i = 0; i < Width * Height; i++)
      result.setValue(i, this->getValue(i) - rhs.getValue(i));
    return result;
  }

  template <typename U, uint W, uint H>
  void operator-=(const Matrix<U, W, H>& rhs)
  {
    static_assert(W == Width && H == Height,
                  "Summing matrixes is allowed on matrixes of same size");
    for (uint i = 0; i < Width * Height; i++)
      this->setValue(i, this->getValue(i) - rhs.getValue(i));
  }

  // Scalar multiplication
  template <typename U> Matrix<T, Width, Height> operator*(const U scalar) const
  {
    Matrix<T, Width, Height> result;
    for (uint i = 0; i < Width * Height; i++)
      result.setValue(i, scalar * this->getValue(i));
    return result;
  }

  template <typename U> void operator*=(const U scalar)
  {
    for (uint i = 0; i < Width * Height; i++)
      this->setValue(i, scalar * this->getValue(i));
  }

  // Matrix multiplication
  template <typename U, uint W, uint H>
  Matrix<T, Width, Height> operator*(const Matrix<U, W, H>& rhs) const
  {
    static_assert(Height == W, "lhs height needs to be equal to rhs width");

    // Using basic multiplication matrix, as we use only small matrixes
    T sum;
    Matrix<T, Width, H> result;
    for (uint i = 0; i < Width; i++)
      for (uint j = 0; j < H; j++)
      {
        sum = 0;
        for (uint k = 0; k < Height; k++)
          sum += this->getValue(i, k) * rhs.getValue(k, j);
        result.setValue(i, j, sum);
      }
    return result;
  }

  // Unoptimised
  template <typename U, uint W, uint H>
  void operator*=(const Matrix<U, W, H>& rhs)
  {
    static_assert(Height == W, "lhs height needs to be equal to rhs width");

    T sum{};
    Matrix<T, Width, H> result;
    for (uint i = 0; i < Width; i++)
      for (uint j = 0; j < H; j++)
      {
        sum = 0;
        for (uint k = 0; k < Height; k++)
          sum += this->getValue(i, k) * rhs.getValue(k, j);
        result.setValue(i, j, sum);
      }

    for (uint i = 0; i < Width * Height; i++)
      this->setValue(i, result.getValue(i));
  }

private:
  std::array<T, Width * Height> data;
};

#endif
