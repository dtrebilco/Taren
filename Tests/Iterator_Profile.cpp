
#include "../Iterator.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <numeric>

#include <vector>
#include <list>

static int32_t LOOPCOUNT = 1000;
static uint32_t TEST_LOOPCOUNT = 100;

#define TIMER_START  std::chrono::microseconds shortDuration; \
                     for (uint32_t t = 0; t < TEST_LOOPCOUNT; t++) \
                     { \
                       std::vector<T> process(LOOPCOUNT); \
                       for (auto& i : process) \
                       { \
                         i = a_data; \
                       } \
                       auto start_time = chrono::high_resolution_clock::now();

#define TIMER_END      auto duration_time = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start_time); \
                       if (t == 0 || duration_time < shortDuration) \
                       { \
                         shortDuration = duration_time; \
                       } \
                     } \
                     cout << chrono::duration_cast<chrono::microseconds>(shortDuration).count() << "us\n";


using namespace std;

template <typename T, typename V>
void ProfileRemove(const T & a_data, const V a_value, bool a_unorderd = false)
{
  TIMER_START

  if (a_unorderd)
  {
    for (auto& d : process)
    {
      for (auto& a : iter::unordered_eraser(d))
      {
        if (*a == a_value)
        {
          a.mark_for_erase();
        }
      }
    }
  }
  else
  {
    for (auto& d : process)
    {
      for (auto& a : iter::eraser(d))
      {
        if (*a == a_value)
        {
          a.mark_for_erase();
        }
      }
    }
  }
  TIMER_END
}


template <typename T, typename V>
void ProfileRemoveFirst(const T & a_data, const V a_value, bool a_unorderd = false)
{
  TIMER_START
    if (a_unorderd)
    {
      for (auto& d : process)
      {
        for (auto& a : iter::unordered_eraser(d))
        {
          if (*a == a_value)
          {
            a.mark_for_erase();
            break;
          }
        }
      }
    }
    else
    {
      for (auto& d : process)
      {
        for (auto& a : iter::eraser(d))
        {
          if (*a == a_value)
          {
            a.mark_for_erase();
            break;
          }
        }
      }
    }
  TIMER_END
}


template <typename T, typename V>
void GenericRemove(const T & a_data, const V a_value)
{
  TIMER_START
    for (auto& d : process)
    {
      d.erase(std::remove(d.begin(), d.end(), a_value), d.end());
    }
  TIMER_END
}

template <typename T, typename V>
void GenericRemoveFirst(const T & a_data, const V a_value)
{

  TIMER_START
    for (auto& d : process)
    {
      auto found = std::find(d.begin(), d.end(), a_value);
      if (found != d.end())
      {
        d.erase(found);
      }
    }
  TIMER_END
}

template <typename T>
void ProfileDirectRemoveFirstBreak(const T & a_data)
{

  TIMER_START
    for (auto& d : process)
    {
      d.erase(d.begin());
    }
  TIMER_END
}


template <typename T>
void ProfileDirectMemRemoveFirstBreak(const T & a_data)
{

  TIMER_START
    for (auto& d : process)
    {
      memmove(d.data(), d.data() + 1, (d.size() - 1) *sizeof(int));
      d.resize(d.size() - 1);
    }
  TIMER_END
}


template <typename T, typename V>
void RunTest(const T & a_data, const V & a_value)
{
  cout << "======================\n";
  cout << "  Remove : "; ProfileRemove(a_data, a_value);
  cout << "  Remove(Unordered) : "; ProfileRemove(a_data, a_value, true);
  cout << "  Remove(Generic) : "; GenericRemove(a_data, a_value);

  cout << "======================\n";
  cout << "  Remove First : "; ProfileRemoveFirst(a_data, a_value);
  cout << "  Remove(Unordered) First : "; ProfileRemoveFirst(a_data, a_value, true);
  cout << "  Remove(Generic) First : "; GenericRemoveFirst(a_data, a_value);
}

template <typename T, typename V>
void RunTests(const T & a_data, const V & a_value)
{
  {
    T newData = a_data;

    cout << "======================\n";
    cout << "  Base Iterate\n";
    RunTest(a_data, a_value);
  }

  {
    T newData = a_data;
    newData[0] = a_value;

    cout << "======================\n";
    cout << "  First value\n";
    RunTest(newData, a_value);
  }

  {
    T newData = a_data;
    newData[newData.size() / 2] = a_value;

    cout << "======================\n";
    cout << "  Middle value\n";
    RunTest(newData, a_value);  }

  {
    T newData = a_data;
    newData[newData.size() - 1] = a_value;

    cout << "======================\n";
    cout << "  Last value\n";
    RunTest(newData, a_value);
  }

  {
    T newData = a_data;
    for (auto i : iter::counter(newData.size() / 4)) { newData[i] = a_value; }

    cout << "======================\n";
    cout << "  Remove 1/4\n";
    RunTest(newData, a_value);
  }

  {
    T newData = a_data;
      for (auto i : iter::counter(newData.size() / 2) ){ newData[i] = a_value; }

    cout << "======================\n";
      cout << "  Remove 1/2\n";
      RunTest(newData, a_value);
  }

  {
    T newData = a_data;
      for (auto i : iter::counter(newData.size()  * 3 / 4)) { newData[i] = a_value; }

    cout << "======================\n";
      cout << "  Remove 3/4\n";
      RunTest(newData, a_value);
  }

  {
    T newData = a_data;
      for (auto& i : newData) { i = a_value; }

    cout << "======================\n";
      cout << "  Remove all\n";
      RunTest(newData, a_value);
  }

  //cout << "  Direct First Break: "; ProfileDirectRemoveFirstBreak(a_data);
 // cout << "  Direct Mem First Break: "; ProfileDirectMemRemoveFirstBreak(a_data);
}

void Iterator_RunProfile()
{
  //std::vector<int> testData(1000000);
  //std::list<int> testData(1000000);
  //std::list<int> testData(1000);

  for(int i = 10; i <= 10000; i*=10)
  {
    std::vector<int> testData(i);
    cout << "===================================================================\n";
    cout << "\n\nstd:vector<int>: (" << testData.size() << ") x (" << LOOPCOUNT << ")\n";
    RunTests(testData, 1);
  }

  for (int i = 10; i <= 10000; i *= 10)
  {
    std::vector<std::string> testData(i);
    for (auto& v : testData) { v = "0"; }

    cout << "===================================================================\n";
    cout << "\n\nstd:vector<std::string>: (" << testData.size() << ") x (" << LOOPCOUNT << ")\n";
    RunTests(testData, "1");
  }

}