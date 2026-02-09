#ifndef BASIC_HPP
#define BASIC_HPP

// 3d Forms
#include <array>
#include <ostream>

template <typename T, uint Width, uint Height> class Matrix
{
public:
  // Constructors
  Matrix() {}

  template <typename U, uint W, uint H> Matrix(const Matrix<U, W, H>& other)
  {
    if (W != Width || H != Height)
      throw std::runtime_error(
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

  T getValue(const uint w, const uint h) const
  {
    if (w >= Width || h >= Height)
      throw std::runtime_error("Access to element outside of matrix.");

    return this->getValue(h * Height + w);
  }

  // Setters
  template <typename U> void setValue(const uint pos, const U value)
  {
    if (pos >= Height * Width)
      throw std::runtime_error("Access to element outside of matrix.");
    this->data[pos] = static_cast<T>(value);
    return;
  }

  template <typename U> void setValue(const uint w, const uint h, const U value)
  {
    if (w >= Width || h >= Height)
      throw std::runtime_error("Access to element outside of matrix.");
    setValue(h * Height + w, value);
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

  // Operations

  // Matrix summing
  template <typename U, uint W, uint H>
  Matrix<T, Width, Height> operator+(const Matrix<U, W, H>& rhs) const
  {
    if (W != Width || H != Height)
      throw std::runtime_error(
          "Summing matrixes is allowed on matrixes of same size");

    Matrix<T, Width, Height> result;
    for (uint i = 0; i < Width * Height; i++)
      result.setValue(i, this->getValue(i) + rhs.getValue(i));
    return result;
  }

  template <typename U, uint W, uint H>
  void operator+=(const Matrix<U, W, H> rhs)
  {
    if (W != Width || H != Height)
      throw std::runtime_error(
          "Summing matrixes is allowed on matrixes of same size");

    for (uint i = 0; i < Width * Height; i++)
      this->setValue(i, this->getValue(i) + rhs.getValue(i));
  }

  template <typename U, uint W, uint H>
  Matrix<T, Width, Height> operator-(const Matrix<U, W, H>& rhs) const
  {
    if (W != Width || H != Height)
      throw std::runtime_error(
          "Summing matrixes is allowed on matrixes of same size");

    Matrix<T, Width, Height> result;
    for (uint i = 0; i < Width * Height; i++)
      result.setValue(i, this->getValue(i) - rhs.getValue(i));
    return result;
  }

  template <typename U, uint W, uint H>
  void operator-=(const Matrix<U, W, H> rhs)
  {
    if (W != Width || H != Height)
      throw std::runtime_error(
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
  Matrix<T, Width, Height> operator*(const Matrix<U, W, H> rhs) const
  {
    if (Height != W)
      throw std::runtime_error("lhs height needs to be equal to rhs width");

    // Using basic multiplication matrix, as we use only small matrixes
    T sum;
    Matrix<T, Width, Height> result;
    for (uint i = 0; i < Width; i++)
      for (uint j = 0; j < H; j++)
      {
        sum = 0;
        for (uint k = 0; k < Height; k++)
          sum += this->getValue(i, k) + rhs.getValue(k, j);
        result.setValue(i, j, sum);
      }
    return result;
  }

  // Unoptimised
  template <typename U, uint W, uint H>
  void operator*=(const Matrix<U, W, H> rhs)
  {
    if (Height != W)
      throw std::runtime_error("lhs height needs to be equal to rhs width");

    T sum;
    Matrix<T, Width, Height> result;
    for (uint i = 0; i < Width; i++)
      for (uint j = 0; j < H; j++)
      {
        sum = 0;
        for (uint k = 0; k < Height; k++)
          sum += this->getValue(i, k) + rhs.getValue(k, j);
        result.setValue(i, j, sum);
      }

    for (uint i = 0; i < Width * Height; i++)
      this->setValue(i, result.getValue(i));
  }

private:
  std::array<T, Width * Height> data;
};

#endif
