

#include "EnumMacro_Base.h"
#include <iostream>
#include <vector>
#include <string>

static_assert(uint32_t(Test::Foo) == 0, "Bad enum");
static_assert(uint32_t(Test::Bar) == 1, "Bad enum");
static_assert(uint32_t(Test::Baz) == 2, "Bad enum");

static_assert(uint32_t(TestVal::Foo) == 2, "Bad enum");
static_assert(uint32_t(TestVal::Bar) == 3, "Bad enum");
static_assert(uint32_t(TestVal::Baz) == 7, "Bad enum");
static_assert(uint32_t(TestVal::Single) == 8, "Bad enum");

static_assert(uint32_t(TestValDup::Foo) == 2, "Bad enum");
static_assert(uint32_t(TestValDup::Bar) == 6, "Bad enum");
static_assert(uint32_t(TestValDup::Baz) == 7, "Bad enum");
static_assert(uint32_t(TestValDup::Baz2) == 7, "Bad enum");

static_assert(uint32_t(TestFlags::Foo) == (1 << 0), "Bad enum");
static_assert(uint32_t(TestFlags::Bar) == (1 << 1), "Bad enum");
static_assert(uint32_t(TestFlags::Baz) == (1 << 2), "Bad enum");
static_assert(uint32_t(TestFlags::FooBaz) == (1 << 3), "Bad enum");
static_assert(uint32_t(TestFlags::All) == uint32_t(TestFlags::Foo | TestFlags::Bar | TestFlags::Baz | TestFlags::FooBaz), "Bad enum");


bool EnumMacro_UnitTests()
{
  {
    std::vector<std::string> strValues;
    for (auto val : Test_Values())
    {
      std::string strValue = val.c_str();
      if (strValue != Test_Values::to_string(val.value()))
      {
        std::cout << "Bad string";
        return false;
      }

      strValues.push_back(strValue);
    }

    if (Test_Values::COUNT != 3 ||
      strValues !=std::vector<std::string>({ "Foo", "Bar", "Baz"}))
    {
      std::cout << "Bad enum array";
      return false;
    }
  }

  {
    std::vector<std::string> strValues;
    for (auto val : ClassTest::Test_Values())
    {
      std::string strValue = val.c_str();
      if (strValue != ClassTest::Test_Values::to_string(val.value()))
      {
        std::cout << "Bad string";
        return false;
      }

      strValues.push_back(strValue);
    }

    if (ClassTest::Test_Values::COUNT != 3 ||
      strValues != std::vector<std::string>({ "Foo", "Bar", "Baz" }))
    {
      std::cout << "Bad enum array";
      return false;
    }
  }

  {
    std::vector<std::string> strValues;
    for (auto val : NameTest::Test_Values())
    {
      std::string strValue = val.c_str();
      if (strValue != NameTest::Test_Values::to_string(val.value()))
      {
        std::cout << "Bad string";
        return false;
      }

      strValues.push_back(strValue);
    }

    if (NameTest::Test_Values::COUNT != 3 ||
      strValues != std::vector<std::string>({ "Foo", "Bar", "Baz" }))
    {
      std::cout << "Bad enum array";
      return false;
    }
  }

  {
    std::vector<std::string> strValues;
    for (auto val : TestVal_Values())
    {
      std::string strValue = val.c_str();
      if (strValue != TestVal_Values::to_string(val.value()))
      {
        std::cout << "Bad string";
        return false;
      }

      strValues.push_back(strValue);
    }

    if (TestVal_Values::COUNT != 4 ||
      strValues != std::vector<std::string>({ "Foo", "Bar", "Baz", "Single" }))
    {
      std::cout << "Bad enum array";
      return false;
    }
  }

  {
    std::vector<std::string> strValues;
    for (auto val : ClassTest::TestVal_Values())
    {
      std::string strValue = val.c_str();
      if (strValue != ClassTest::TestVal_Values::to_string(val.value()))
      {
        std::cout << "Bad string";
        return false;
      }

      strValues.push_back(strValue);
    }

    if (ClassTest::TestVal_Values::COUNT != 4 ||
      strValues != std::vector<std::string>({ "Foo", "Bar", "Baz", "Single" }))
    {
      std::cout << "Bad enum array";
      return false;
    }
  }

  {
    std::vector<std::string> strValues;
    for (auto val : NameTest::TestVal_Values())
    {
      std::string strValue = val.c_str();
      if (strValue != NameTest::TestVal_Values::to_string(val.value()))
      {
        std::cout << "Bad string";
        return false;
      }

      strValues.push_back(strValue);
    }

    if (NameTest::TestVal_Values::COUNT != 4 ||
      strValues != std::vector<std::string>({ "Foo", "Bar", "Baz", "Single" }))
    {
      std::cout << "Bad enum array";
      return false;
    }
  }

  {
    std::vector<std::string> strValues;
    for (auto val : TestValDup_Values())
    {
      std::string strValue = val.c_str();
      if (strValue != TestValDup_Values::to_string(val.value()))
      {
        // This test has a duplicate value
        std::string serachStr = TestValDup_Values::to_string(val.value());
        bool foundVal = false;
        for (auto serach : TestValDup_Values())
        {
          if (serachStr == serach.c_str())
          {
            if (val.value() != serach.value())
            {
              std::cout << "Bad string";
              return false;
            }
            foundVal = true;
            break;
          }
        }

        if (!foundVal)
        {
          std::cout << "Bad string";
          return false;
        }
      }

      strValues.push_back(strValue);
    }

    if (TestValDup_Values::COUNT != 4 ||
      strValues != std::vector<std::string>({ "Foo", "Bar", "Baz", "Baz2" }))
    {
      std::cout << "Bad enum array";
      return false;
    }
  }


  // Test flags
  {
    std::vector<std::string> strValues;
    for (auto val : TestFlags_Values())
    {
      std::string strValue = val.c_str();
      if (strValue != TestFlags_Values::to_string(val.value()))
      {
        std::cout << "Bad string";
        return false;
      }

      strValues.push_back(strValue);
    }

    if (TestFlags_Values::COUNT != 5 ||
      strValues != std::vector<std::string>({ "Foo", "Bar", "Baz", "FooBaz", "All" }))
    {
      std::cout << "Bad enum array";
      return false;
    }
  }

  {
    std::vector<std::string> strValues;
    for (auto val : ClassTest::TestFlags_Values())
    {
      std::string strValue = val.c_str();
      if (strValue != ClassTest::TestFlags_Values::to_string(val.value()))
      {
        std::cout << "Bad string";
        return false;
      }

      strValues.push_back(strValue);
    }

    if (ClassTest::TestFlags_Values::COUNT != 5 ||
      strValues != std::vector<std::string>({ "Foo", "Bar", "Baz", "FooBaz", "All" }))
    {
      std::cout << "Bad enum array";
      return false;
    }
  }

  {
    std::vector<std::string> strValues;
    for (auto val : NameTest::TestFlags_Values())
    {
      std::string strValue = val.c_str();
      if (strValue != NameTest::TestFlags_Values::to_string(val.value()))
      {
        std::cout << "Bad string";
        return false;
      }

      strValues.push_back(strValue);
    }

    if (NameTest::TestFlags_Values::COUNT != 5 ||
      strValues != std::vector<std::string>({ "Foo", "Bar", "Baz", "FooBaz", "All" }))
    {
      std::cout << "Bad enum array";
      return false;
    }
  }

  {
    TestFlags bitOp1 = TestFlags::Bar | TestFlags::Baz;
    TestFlags bitOp2 = TestFlags::Bar & TestFlags::Baz;
    TestFlags bitOp3 = TestFlags::Bar ^ TestFlags::Baz;
    TestFlags bitOp4 = ~TestFlags::Bar;
    if (int(bitOp1) != 6 ||
        int(bitOp2) != 0 ||
        int(bitOp3) != 6 ||
        int64_t(bitOp4) != 4294967293ULL)
    {
      std::cout << "Bad bit ops";
      return false;
    }
  }

  {
    ClassTest::TestFlags bitOp1 = ClassTest::TestFlags::Bar | ClassTest::TestFlags::Baz;
    ClassTest::TestFlags bitOp2 = ClassTest::TestFlags::Bar & ClassTest::TestFlags::Baz;
    ClassTest::TestFlags bitOp3 = ClassTest::TestFlags::Bar ^ ClassTest::TestFlags::Baz;
    ClassTest::TestFlags bitOp4 = ~ClassTest::TestFlags::Bar;
    if (int(bitOp1) != 6 ||
      int(bitOp2) != 0 ||
      int(bitOp3) != 6 ||
      int64_t(bitOp4) != 4294967293ULL)
    {
      std::cout << "Bad bit ops";
      return false;
    }
  }

  {
    NameTest::TestFlags bitOp1 = NameTest::TestFlags::Bar | NameTest::TestFlags::Baz;
    NameTest::TestFlags bitOp2 = NameTest::TestFlags::Bar & NameTest::TestFlags::Baz;
    NameTest::TestFlags bitOp3 = NameTest::TestFlags::Bar ^ NameTest::TestFlags::Baz;
    NameTest::TestFlags bitOp4 = ~NameTest::TestFlags::Bar;
    if (int(bitOp1) != 6 ||
      int(bitOp2) != 0 ||
      int(bitOp3) != 6 ||
      int64_t(bitOp4) != 4294967293ULL)
    {
      std::cout << "Bad bit ops";
      return false;
    }
  }

  return true;
}

