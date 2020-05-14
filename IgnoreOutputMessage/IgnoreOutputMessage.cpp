
#include <iostream>

#define STRING2(x) #x
#define STRING(x) STRING2(x)

int main()
{
  __pragma(message(__FILE__ "(" STRING(__LINE__) "): message : выполняется компиляция ..."))

  std::cout << "Hello World!\n";
}
