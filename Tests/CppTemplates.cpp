
#include "../Iterator.h"
#include <iostream>
#include <vector>

bool Iterator_UnitTests();


int main()
{
  Iterator_UnitTests();


  // Test
  // - int types
  // - string types
  // - test vector list and other types
  // - remove front,middle,end
  // - remove and break on found (front, middle,end)
  // - remove multiple non-sequential
  // - remove multiple sequential
  

  // Profile iterators
  // - remove first matching element (font/middle/end)
  // - remove 1 element (font middle end)
  // - remove 1/4
  // - remove  1/2
  // - remove 3/4
  // - remove all

  // Array sizes (10,100,1000,10000, 100000, 1000000)


  std::vector<std::string> vecbuf = { "a", "b", "c", "d" };
  const auto& vecbuf_const = vecbuf;
  for (auto it : iter::counter(vecbuf.size())) 
  {
    std::cout << vecbuf[it].c_str();
  }
  std::cout << "\n";

  for (auto it : iter::counter_reverse(vecbuf.size()))
  {
    std::cout << vecbuf[it].c_str();
  }
  std::cout << "\n";

  for (auto it : iter::reverse(vecbuf_const))
  {
    std::cout << it.c_str();
  }
  std::cout << "\n";

  // Ordered
  vecbuf = { "a", "b", "c", "d" };
  for (const auto& it : iter::eraser(vecbuf))
  {
    if (*it == "b" ||
        *it == "d")
    {
      //*it = "test";
      it.mark_for_erase();
      //break;
    }

    std::cout << it->c_str() << " - " << it.index() << ", ";
  }
  std::cout << "\n";
  
  std::cout << "Del items:";
  for (auto it : vecbuf) 
  {
    std::cout << it.c_str();
  }
  std::cout << "\n";


  // Unordered 
  vecbuf = { "a", "b", "c", "d" };
  for (const auto& it : iter::unordered_eraser(vecbuf))
  {
    if (*it == "b" ||
      *it == "d")
    {
      //*it = "test";
      it.mark_for_erase();
      //break;
    }

    std::cout << it->c_str() << " - " << it.loop_index() << ", ";
  }
  std::cout << "\n";

  std::cout << "Del items:";
  for (auto it : vecbuf)
  {
    std::cout << it.c_str();
  }
  std::cout << "\n";




  getchar();


  return 0;
}

