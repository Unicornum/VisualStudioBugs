
#include "stdafx.h"

inline void Function1(void)
{
# pragma message("Pragma message: ������� ������� 1")
  __pragma(message("Pragma message: ������� ������� 1"));
}

template<class T>
inline void Function2(void)
{
# pragma message("Pragma message: ��������� ������� 2")
  __pragma(message("Pragma message: ��������� ������� 2"));
}

int main()
{
  Function1();
  Function2<int>();

  return 0;
}
