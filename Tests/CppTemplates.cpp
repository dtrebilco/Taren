
#include "../Iterator.h"
#include <iostream>
#include <vector>

bool Iterator_UnitTests();
void Iterator_RunProfile();
bool EnumMacro_UnitTests();


int main()
{
  EnumMacro_UnitTests();
  Iterator_UnitTests();
  Iterator_RunProfile();
  //getchar();
  return 0;
}

