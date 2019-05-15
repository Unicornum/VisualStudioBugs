
#include "stdafx.h"
#include "Example2.hpp"
#include <string>

class Crusher
{
public:
  ::std::string GetString(void) const { return m_String; }

private:
  const ::std::string m_String = "string";
};

void Example2::Execute(void)
{
  const auto Result = Crusher().GetString(); // crushed here!!!!
}
