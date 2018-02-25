#include <iostream>

extern int foo (volatile unsigned);
extern int goo (volatile unsigned);

int main() {
  foo(0);
  std::cerr << "==================== \n";
  foo(100);
  std::cerr << "-------------------- \n";
  goo(0);
  std::cerr << "-------------------- \n";
  goo(10);
  return 0;
}
