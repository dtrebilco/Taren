

#include "EnumMacro_Base.h"
#include <iostream>

void EnumMacro_UnitTests()
{
  std::cout << Test_Values::COUNT << std::endl;
  for (auto val : Test_Values())
  {
    std::cout << val.c_str() << "," << int(val.value()) << "," << Test_Values::to_string(val.value()) << std::endl;
  }

  std::cout << "======" << std::endl;

  std::cout << ClassTest::Test_Values::COUNT << std::endl;
  for (auto val : ClassTest::Test_Values())
  {
    std::cout << val.c_str() << "," << int(val.value()) << "," << ClassTest::Test_Values::to_string(val.value()) << std::endl;
  }

  std::cout << "======" << std::endl;

  std::cout << NameTest::Test_Values::COUNT << std::endl;
  for (auto val : NameTest::Test_Values())
  {
    std::cout << val.c_str() << "," << int(val.value()) << "," << NameTest::Test_Values::to_string(val.value()) << std::endl;
  }

  std::cout << "======" << std::endl;


  std::cout << TestVal_Values::COUNT << std::endl;
  for (auto val : TestVal_Values())
  {
    std::cout << val.c_str() << "," << int(val.value()) << "," << TestVal_Values::to_string(val.value()) << std::endl;
  }

  std::cout << "======" << std::endl;


  std::cout << TestValDup_Values::COUNT << std::endl;
  for (auto val : TestValDup_Values())
  {
    std::cout << val.c_str() << "," << int(val.value()) << "," << TestValDup_Values::to_string(val.value()) << std::endl;
  }

  std::cout << "======" << std::endl;


  std::cout << ClassTest::TestVal_Values::COUNT << std::endl;
  for (auto val : ClassTest::TestVal_Values())
  {
    std::cout << val.c_str() << "," << int(val.value()) << "," << ClassTest::TestVal_Values::to_string(val.value()) << std::endl;
  }

  std::cout << "======" << std::endl;

  std::cout << NameTest::TestVal_Values::COUNT << std::endl;
  for (auto val : NameTest::TestVal_Values())
  {
    std::cout << val.c_str() << "," << int(val.value()) << "," << NameTest::TestVal_Values::to_string(val.value()) << std::endl;
  }

  std::cout << "======" << std::endl;

}

