

#include "../Iterator.h"
#include <iostream>

#include <vector>
#include <list>


bool ReverseTests()
{
  {
    std::vector<int> reverseTest = { 1,2,3,4 };
    std::vector<int> reverseResultTest;
    for (auto& val : iter::reverse(reverseTest))
    {
      reverseResultTest.push_back(val);
    }
    if (reverseResultTest != std::vector<int>{4, 3, 2, 1})
    {
      std::cout << "Reverse failed\n";
      return false;
    }
  }

  {
    const std::vector<int> reverseTest = { 1,2,3,4 };
    std::vector<int> reverseResultTest;
    for (auto& val : iter::reverse(reverseTest))
    {
      reverseResultTest.push_back(val);
    }
    if (reverseResultTest != std::vector<int>{4, 3, 2, 1})
    {
      std::cout << "Reverse failed\n";
      return false;
    }
  }

  {
    const std::vector<std::string> reverseTest = { "1","2","3","4" };
    std::vector<std::string> reverseResultTest;
    for (auto& val : iter::reverse(reverseTest))
    {
      reverseResultTest.push_back(val);
    }
    if (reverseResultTest != std::vector<std::string>{"4", "3", "2", "1"})
    {
      std::cout << "Reverse failed\n";
      return false;
    }
  }

  return true;
}

static bool CounterTests()
{
  // Counter tests
  {
    std::vector<int> counterResultTest;
    for (auto val : iter::counter(10))
    {
      counterResultTest.push_back(int(val));
    }
    if (counterResultTest != std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9})
    {
      std::cout << "Counter failed\n";
      return false;
    }
  }

  {
    std::vector<int> counterResultTest;
    for (auto val : iter::counter(0))
    {
      counterResultTest.push_back(int(val));
    }
    if (counterResultTest != std::vector<int>{})
    {
      std::cout << "Counter failed\n";
      return false;
    }
  }

  {
    std::vector<int> counterResultTest;
    for (auto val : iter::counter(1))
    {
      counterResultTest.push_back(int(val));
    }
    if (counterResultTest != std::vector<int>{0})
    {
      std::cout << "Counter failed\n";
      return false;
    }
  }


  // Counter reverse tests
  {
    std::vector<int> counterResultTest;
    for (auto val : iter::counter_reverse(10))
    {
      counterResultTest.push_back(int(val));
    }
    if (counterResultTest != std::vector<int>{9, 8, 7, 6, 5, 4, 3, 2, 1, 0})
    {
      std::cout << "Counter Reverse failed\n";
      return false;
    }
  }

  {
    std::vector<int> counterResultTest;
    for (auto val : iter::counter_reverse(0))
    {
      counterResultTest.push_back(int(val));
    }
    if (counterResultTest != std::vector<int>{})
    {
      std::cout << "Counter Reverse failed\n";
      return false;
    }
  }

  {
    std::vector<int> counterResultTest;
    for (auto val : iter::counter_reverse(1))
    {
      counterResultTest.push_back(int(val));
    }
    if (counterResultTest != std::vector<int>{0})
    {
      std::cout << "Counter Reverse failed\n";
      return false;
    }
  }
  return true;
}

template <typename A, typename B>
bool IsSame(const A& a_a, const B& a_b)
{
  if (a_a.size() != a_b.size())
  {
    return false;
  }

  auto aStart = a_a.begin();
  auto bStart = a_b.begin();

  for (; aStart != a_a.end() &&
       bStart != a_b.end(); )
  {
    if (*aStart != *bStart)
    {
      return false;
    }
    ++aStart;
    ++bStart;
  }
  return true;
}

template <typename A, typename B>
bool ContainsSame(const A& a_a, const B& a_b)
{
  if (a_a.size() != a_b.size())
  {
    return false;
  }

  for (const auto& item : a_a)
  {
    if (std::count(a_a.begin(), a_a.end(), item) !=
        std::count(a_b.begin(), a_b.end(), item))
    {
      return false;
    }
  }

  return true;
}

template <typename T>
bool TypeIntTests()
{
  // Eraser tests
  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      *val;
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 1)
      {
        foundIndex = val.index();
      }
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }) ||
      foundIndex != 0)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 2)
      {
        foundIndex = val.index();
      }
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }) ||
      foundIndex != 1)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 3)
      {
        foundIndex = val.index();
      }
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }) ||
      foundIndex != 2)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 4)
      {
        foundIndex = val.index();
      }
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }) ||
      foundIndex != 3)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }


  ////

  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 1)
      {
        foundIndex = val.index();
        break;
      }
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }) ||
      foundIndex != 0)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 2)
      {
        foundIndex = val.index();
        break;
      }
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }) ||
      foundIndex != 1)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 3)
      {
        foundIndex = val.index();
        break;
      }
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }) ||
      foundIndex != 2)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 4)
      {
        foundIndex = val.index();
        break;
      }
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }) ||
      foundIndex != 3)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }


  ////

  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 1)
      {
        val.mark_for_erase();
        foundIndex = val.index();
      }
    }
    if (!IsSame(eraserTest, T{2, 3, 4}) ||
      foundIndex != 0)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 2)
      {
        val.mark_for_erase();
        foundIndex = val.index();
      }
    }
    if (!IsSame(eraserTest, T{1, 3, 4}) ||
      foundIndex != 1)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 3)
      {
        val.mark_for_erase();
        foundIndex = val.index();
      }
    }
    if (!IsSame(eraserTest, T{1, 2, 4}) ||
      foundIndex != 2)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 4)
      {
        val.mark_for_erase();
        foundIndex = val.index();
      }
    }
    if (!IsSame(eraserTest, T{1, 2, 3}) ||
      foundIndex != 3)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  // Eraser break early tests
  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 1)
      {
        val.mark_for_erase();
        foundIndex = val.index();
        break;
      }
    }
    if (!IsSame(eraserTest, T{2, 3, 4}) ||
      foundIndex != 0)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 2)
      {
        val.mark_for_erase();
        foundIndex = val.index();
        break;
      }
    }
    if (!IsSame(eraserTest, T{1, 3, 4}) ||
      foundIndex != 1)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 3)
      {
        val.mark_for_erase();
        foundIndex = val.index();
        break;
      }
    }
    if (!IsSame(eraserTest, T{1, 2, 4}) ||
      foundIndex != 2)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    size_t foundIndex = 0;
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == 4)
      {
        val.mark_for_erase();
        foundIndex = val.index();
        break;
      }
    }
    if (!IsSame(eraserTest, T{1, 2, 3}) ||
      foundIndex != 3)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  // Eraser unordered tests
  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      *val;
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  ////

  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == 1)
      {
        break;
      }
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == 2)
      {
        break;
      }
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }) )
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == 3)
      {
        break;
      }
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == 4)
      {
        break;
      }
    }
    if (!IsSame(eraserTest, T{ 1, 2, 3, 4 }))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  ////

  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == 1)
      {
        val.mark_for_erase();
      }
    }
    if (!ContainsSame(eraserTest, T{2, 3, 4}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == 2)
      {
        val.mark_for_erase();
      }
    }
    if (!ContainsSame(eraserTest, T{1, 3, 4}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == 3)
      {
        val.mark_for_erase();
      }
    }
    if (!ContainsSame(eraserTest, T{1, 2, 4}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == 4)
      {
        val.mark_for_erase();
      }
    }
    if (!ContainsSame(eraserTest, T{1, 2, 3}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  // Eraser unordered break early tests
  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == 1)
      {
        val.mark_for_erase();
        break;
      }
    }
    if (!ContainsSame(eraserTest, T{2, 3, 4}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == 2)
      {
        val.mark_for_erase();
        break;
      }
    }
    if (!ContainsSame(eraserTest, T{1, 3, 4}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == 3)
      {
        val.mark_for_erase();
        break;
      }
    }
    if (!ContainsSame(eraserTest, T{1, 2, 4}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    T eraserTest = { 1,2,3,4 };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == 4)
      {
        val.mark_for_erase();
        break;
      }
    }
    if (!ContainsSame(eraserTest, T{1, 2, 3}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  return true;
}


template <typename T>
bool TypeStringTests()
{
  // Eraser tests
  {
    size_t foundIndex = 0;
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == "1")
      {
        val.mark_for_erase();
        foundIndex = val.index();
      }
    }
    if (!IsSame(eraserTest, T{"2", "3", "4"}) ||
      foundIndex != 0)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    size_t foundIndex = 0;
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == "2")
      {
        val.mark_for_erase();
        foundIndex = val.index();
      }
    }
    if (!IsSame(eraserTest, T{"1", "3", "4"}) ||
      foundIndex != 1)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    size_t foundIndex = 0;
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == "3")
      {
        val.mark_for_erase();
        foundIndex = val.index();
      }
    }
    if (!IsSame(eraserTest, T{"1", "2", "4"}) ||
      foundIndex != 2)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    size_t foundIndex = 0;
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == "4")
      {
        val.mark_for_erase();
        foundIndex = val.index();
      }
    }
    if (!IsSame(eraserTest, T{"1", "2", "3"}) ||
      foundIndex != 3)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  // Eraser break early tests
  {
    size_t foundIndex = 0;
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == "1")
      {
        val.mark_for_erase();
        foundIndex = val.index();
        break;
      }
    }
    if (!IsSame(eraserTest, T{"2", "3", "4"}) ||
      foundIndex != 0)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    size_t foundIndex = 0;
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == "2")
      {
        val.mark_for_erase();
        foundIndex = val.index();
        break;
      }
    }
    if (!IsSame(eraserTest, T{"1", "3", "4"}) ||
      foundIndex != 1)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    size_t foundIndex = 0;
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == "3")
      {
        val.mark_for_erase();
        foundIndex = val.index();
        break;
      }
    }
    if (!IsSame(eraserTest, T{"1", "2", "4"}) ||
      foundIndex != 2)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    size_t foundIndex = 0;
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::eraser(eraserTest))
    {
      if (*val == "4")
      {
        val.mark_for_erase();
        foundIndex = val.index();
        break;
      }
    }
    if (!IsSame(eraserTest, T{"1", "2", "3"}) ||
      foundIndex != 3)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  // Eraser unordered tests
  {
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == "1")
      {
        val.mark_for_erase();
      }
    }
    if (!ContainsSame(eraserTest, T{"2", "3", "4"}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == "2")
      {
        val.mark_for_erase();
      }
    }
    if (!ContainsSame(eraserTest, T{"1", "3", "4"}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == "3")
      {
        val.mark_for_erase();
      }
    }
    if (!ContainsSame(eraserTest, T{"1", "2", "4"}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == "4")
      {
        val.mark_for_erase();
      }
    }
    if (!ContainsSame(eraserTest, T{"1", "2", "3"}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  // Eraser unordered break early tests
  {
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == "1")
      {
        val.mark_for_erase();
        break;
      }
    }
    if (!ContainsSame(eraserTest, T{"2", "3", "4"}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == "2")
      {
        val.mark_for_erase();
        break;
      }
    }
    if (!ContainsSame(eraserTest, T{"1", "3", "4"}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == "3")
      {
        val.mark_for_erase();
        break;
      }
    }
    if (!ContainsSame(eraserTest, T{"1", "2", "4"}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    T eraserTest = { "1","2","3","4" };
    for (auto& val : iter::unordered_eraser(eraserTest))
    {
      if (*val == "4")
      {
        val.mark_for_erase();
        break;
      }
    }
    if (!ContainsSame(eraserTest, T{"1", "2", "3"}))
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  return true;
}

bool Iterator_UnitTests()
{
  // Reverse and counter tests
  if (!ReverseTests() ||
      !CounterTests())
  {
    return false;
  }

  // Eraser type Tests
  if (!TypeIntTests<std::vector<int>>() ||
      !TypeIntTests<std::list<int>>())
  {
    return false;
  }

  if (!TypeStringTests<std::vector<std::string>>() ||
      !TypeStringTests<std::list<std::string>>())
  {
    return false;
  }

  return true;
}

