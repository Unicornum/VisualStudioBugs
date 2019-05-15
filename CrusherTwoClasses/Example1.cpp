
#include "stdafx.h"
#include "Example1.hpp"

class Crusher
{
public:
  int GetInt(void) const { return m_Value; }

private:
  const int m_Value = 77;
};

void Example1::Execute(void)
{
  const auto Result = Crusher().GetInt();
}
