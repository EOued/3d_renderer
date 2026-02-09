#include "basic.hpp"
#include <iostream>

int main()
{
  Matrix<float, 3, 3> m;

  std::cout << m.getValue(0, 0) << '\n';
  std::cout << m << "\n\n";

  m.setValue(1, 1, 2.5);
  std::cout << m << "\n\n";

  Matrix<int, 3, 3> m2 = m;
  std::cout << m2 << "\n\n";

  m2 += m;
  std::cout << m2 << "\n\n";

  m2 *= 3;
  std::cout << m2 << "\n\n";

  m2 -= m;
  std::cout << m2 << "\n\n";

  m2 *= m;
  std::cout << m << "\n\n";
  std::cout << m2 << "\n\n";

  return 0;
}
