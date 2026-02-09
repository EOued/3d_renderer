#ifndef BASIC_HPP
#define BASIC_HPP

#include <array>
#include <cmath>
#include <ostream>
#include <vector>

namespace linalg
{

template <typename T, uint Lines, uint Columns> class Matrix
{
public:
  // Constructors
  Matrix() {}
  Matrix(std::initializer_list<std::initializer_list<T>> _data)
  {
    uint j = 0;
    for (auto& row : _data)
    {
      uint i = 0;
      for (auto& val : row) this->setValue(j, i++, val);
      j++;
    }
  }

  Matrix(std::initializer_list<T> _data)
  {

    static_assert(Lines == 1, "Initialisation with non-nested initialiser list "
                              "is allowed for vectors only");
    uint i = 0;
    for (auto& val : _data) this->setValue(0, i++, val);
  }

  const static Matrix<T, Lines, Columns> Identity()
  {
    Matrix<T, Lines, Columns> m;
    for (uint i = 0; i < Lines; i++) m.setValue(i, i, 1);
    return m;
  }

  const static Matrix<float, 3, 3> IsometricProjection()
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

  template <typename U, uint _Lines, uint _Columns>
  Matrix(const Matrix<U, _Lines, _Columns>& other)
  {
    static_assert(Lines == _Lines && Columns == _Columns,
                  "Typecasting matrix is allowed on matrixes of same size");
    for (uint i = 0; i < Lines * Columns; i++)
      this->setValue(i, other.getValue(i));
  }

  // Getters
  T getValue(const uint pos) const
  {
    if (pos >= Lines * Columns)
      throw std::runtime_error("Access to element outside of matrix.");
    return this->data[pos];
  }

  T getValue(const uint line, const uint col) const
  {
    if (line >= Lines || col >= Columns)
      throw std::runtime_error("Access to element outside of matrix.");
    return this->getValue(line * Lines + col);
  }

  // Setters
  template <typename U> void setValue(const uint pos, const U value)
  {
    if (pos >= Lines * Columns)
      throw std::runtime_error("Access to element outside of matrix.");
    this->data[pos] = static_cast<T>(value);
    return;
  }

  template <typename U>
  void setValue(const uint line, const uint col, const U value)
  {

    if (col >= Columns || line >= Lines)
      throw std::runtime_error("Access to element outside of matrix.");
    setValue(line * Lines + col, value);
    return;
  }

  // Print
  friend std::ostream& operator<<(std::ostream& os,
                                  const Matrix<T, Lines, Columns>& p)
  {
    for (uint i = 0; i < Lines * Columns; i++)
    {
      if (i != 0 && !(i % Columns)) os << '\n';
      os << p.getValue(i) << ' ';
    }

    return os;
  }

  void transpose(void)
  {
    T temp;
    for (uint col = 0; col < Columns; col++)
      for (uint lin = col + 1; lin < Lines; lin++)
      {
        if (lin == col) continue;
        temp = this->getValue(lin, col);
        this->setValue(lin, col, this->getValue(col, lin));
        this->setValue(col, lin, temp);
      }
    return;
  }

  // Operations overload

  // Matrix summing
  template <typename U, uint _Lines, uint _Columns>
  Matrix<T, Lines, Columns>
  operator+(const Matrix<U, _Lines, _Columns>& rhs) const
  {
    static_assert(Lines == _Lines && Columns == _Columns,
                  "Summing matrixes is allowed on matrixes of same size");
    Matrix<T, Lines, Columns> result;
    for (uint i = 0; i < Lines * Columns; i++)
      result.setValue(i, this->getValue(i) + rhs.getValue(i));
    return result;
  }

  template <typename U, uint _Lines, uint _Columns>
  void operator+=(const Matrix<U, _Lines, _Columns>& rhs)
  {
    static_assert(Lines == _Lines && Columns == _Columns,
                  "Summing matrixes is allowed on matrixes of same size");
    for (uint i = 0; i < Lines * Columns; i++)
      this->setValue(i, this->getValue(i) + rhs.getValue(i));
  }

  template <typename U, uint _Lines, uint _Columns>
  Matrix<T, Lines, Columns>
  operator-(const Matrix<U, _Lines, _Columns>& rhs) const
  {
    static_assert(Lines == _Lines && Columns == _Columns,
                  "Summing matrixes is allowed on matrixes of same size");
    Matrix<T, Lines, Columns> result;
    for (uint i = 0; i < Lines * Columns; i++)
      result.setValue(i, this->getValue(i) - rhs.getValue(i));
    return result;
  }

  template <typename U, uint _Lines, uint _Columns>
  void operator-=(const Matrix<U, _Lines, _Columns>& rhs)
  {
    static_assert(Lines == _Lines && Columns == _Columns,
                  "Summing matrixes is allowed on matrixes of same size");
    for (uint i = 0; i < Lines * Columns; i++)
      this->setValue(i, this->getValue(i) - rhs.getValue(i));
  }

  // Scalar multiplication
  template <typename U>
  Matrix<T, Lines, Columns> operator*(const U scalar) const
  {
    Matrix<T, Lines, Columns> result;
    for (uint i = 0; i < Lines * Columns; i++)
      result.setValue(i, scalar * this->getValue(i));
    return result;
  }

  template <typename U> void operator*=(const U scalar)
  {
    for (uint i = 0; i < Lines * Columns; i++)
      this->setValue(i, scalar * this->getValue(i));
  }

  // Matrix multiplication
  template <typename U, uint _Lines, uint _Columns>
  Matrix<T, Lines, Columns>
  operator*(const Matrix<U, _Lines, _Columns>& rhs) const
  {
    static_assert(Columns == _Lines,
                  "lhs columns number needs to be equal to rhs lines number");

    // Using basic multiplication matrix, as we use only small matrixes
    T sum;
    Matrix<T, Lines, Columns> result;
    for (uint i = 0; i < Lines; i++)
      for (uint j = 0; j < _Columns; j++)
      {
        sum = 0;
        for (uint k = 0; k < Columns; k++)
          sum += this->getValue(i, k) * rhs.getValue(k, j);
        result.setValue(i, j, sum);
      }
    return result;
  }

  // Unoptimised
  template <typename U, uint _Lines, uint _Columns>
  void operator*=(const Matrix<U, _Lines, _Columns>& rhs)
  {
    static_assert(Columns == _Lines,
                  "lhs columns number needs to be equal to rhs lines number");

    // Using basic multiplication matrix, as we use only small matrixes
    T sum;
    Matrix<T, Lines, Columns> result;
    for (uint i = 0; i < Lines; i++)
      for (uint j = 0; j < _Columns; j++)
      {
        sum = 0;
        for (uint k = 0; k < Columns; k++)
          sum += this->getValue(i, k) * rhs.getValue(k, j);
        result.setValue(i, j, sum);
      }

    for (uint i = 0; i < Lines * Columns; i++)
      this->setValue(i, result.getValue(i));
  }

private:
  std::array<T, Lines * Columns> data;
};

template <typename T, uint N> using Vector = Matrix<T, 1, N>;

} // namespace linalg
#endif
