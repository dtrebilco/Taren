

#include "../Iterator.h"
#include <vector>
#include <iostream>

bool Iterator_UnitTests()
{
  // Reverse tests
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
   
  // Counter tests
  {
    std::vector<int> counterResultTest;
    for (auto val : iter::counter(10))
    {
      counterResultTest.push_back(int(val));
    }
    if (counterResultTest != std::vector<int>{0,1,2,3,4,5,6,7,8,9})
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
    if (counterResultTest != std::vector<int>{9,8,7,6,5,4,3,2,1,0})
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

  // Eraser tests
  {
    size_t foundIndex = 0;
    std::vector<int> eraserTest = {1,2,3,4};
    for (auto val : iter::eraser(eraserTest))
    {
      if (*val == 1)
      {
        val.mark_for_erase();
        foundIndex = val.index();
      }
    }
    if (eraserTest != std::vector<int>{2,3,4} ||
        foundIndex != 0)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    size_t foundIndex = 0;
    std::vector<int> eraserTest = { 1,2,3,4 };
    for (auto val : iter::eraser(eraserTest))
    {
      if (*val == 2)
      {
        val.mark_for_erase();
        foundIndex = val.index();
      }
    }
    if (eraserTest != std::vector<int>{1, 3, 4} ||
        foundIndex != 1)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  
  {
    size_t foundIndex = 0;
    std::vector<int> eraserTest = { 1,2,3,4 };
    for (auto val : iter::eraser(eraserTest))
    {
      if (*val == 3)
      {
        val.mark_for_erase();
        foundIndex = val.index();
      }
    }
    if (eraserTest != std::vector<int>{1, 2, 4} ||
        foundIndex != 2)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    size_t foundIndex = 0;
    std::vector<int> eraserTest = { 1,2,3,4 };
    for (auto val : iter::eraser(eraserTest))
    {
      if (*val == 4)
      {
        val.mark_for_erase();
        foundIndex = val.index();
      }
    }
    if (eraserTest != std::vector<int>{1, 2, 3} ||
        foundIndex != 3)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  // Eraser break early tests
  {
    size_t foundIndex = 0;
    std::vector<int> eraserTest = { 1,2,3,4 };
    for (auto val : iter::eraser(eraserTest))
    {
      if (*val == 1)
      {
        val.mark_for_erase();
        foundIndex = val.index();
        break;
      }
    }
    if (eraserTest != std::vector<int>{2, 3, 4} ||
        foundIndex != 0)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    size_t foundIndex = 0;
    std::vector<int> eraserTest = { 1,2,3,4 };
    for (auto val : iter::eraser(eraserTest))
    {
      if (*val == 2)
      {
        val.mark_for_erase();
        foundIndex = val.index();
        break;
      }
    }
    if (eraserTest != std::vector<int>{1, 3, 4} ||
        foundIndex != 1)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }

  {
    size_t foundIndex = 0;
    std::vector<int> eraserTest = { 1,2,3,4 };
    for (auto val : iter::eraser(eraserTest))
    {
      if (*val == 3)
      {
        val.mark_for_erase();
        foundIndex = val.index();
        break;
      }
    }
    if (eraserTest != std::vector<int>{1, 2, 4} ||
        foundIndex != 2)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }
  {
    size_t foundIndex = 0;
    std::vector<int> eraserTest = { 1,2,3,4 };
    for (auto val : iter::eraser(eraserTest))
    {
      if (*val == 4)
      {
        val.mark_for_erase();
        foundIndex = val.index();
        break;
      }
    }
    if (eraserTest != std::vector<int>{1, 2, 3} ||
        foundIndex != 3)
    {
      std::cout << "Eraser test failed\n";
      return false;
    }
  }


  return true;
}

