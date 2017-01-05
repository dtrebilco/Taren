
#include "../Iterator.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <numeric>

#include <vector>
#include <list>

static int32_t LOOPCOUNT = 1000;
static uint32_t TEST_LOOPCOUNT = 100;

static std::ofstream s_csvFile;

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
                     cout << chrono::duration_cast<chrono::microseconds>(shortDuration).count() << "us\n"; \
                     s_csvFile << chrono::duration_cast<chrono::microseconds>(shortDuration).count() << ",";


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
void GenericRemoveIf(const T & a_data, const V a_value)
{
  using IterType = decltype(std::declval<T>().begin());
  using ValueRef = decltype(*std::declval<IterType>());
  using ValueType = typename std::remove_reference<ValueRef>::type;

  TIMER_START
    for (auto& d : process)
    {
      d.erase(std::remove_if(d.begin(), d.end(), [a_value](ValueType& a) { return (a == a_value); }), d.end());
    }
  TIMER_END
}

template <typename T, typename V>
void RemoveArrayCopy(const T & a_data, const V a_value)
{
  T copyData;
  copyData.reserve(a_data.size());

  TIMER_START
    for (auto& d : process)
    {
      // This code is about 2x slower than what is below
      //copyData.resize(0);
      //for (auto& v : d)
      //{
      //  if (v != a_value)
      //  {
      //    copyData.push_back(std::move(v));
      //  }
      //}
      //d.swap(copyData);

      copyData.resize(d.size());
      int32_t size = 0;
      for (auto& v : d)
      {
        if (v != a_value)
        {
          copyData[size] = std::move(v);
          size++;
        }
      }
      copyData.resize(size);
      d.swap(copyData);
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


template <typename T, typename V>
void ProfileIndex(const T & a_data, const V a_value)
{
  TIMER_START
    for (auto& d : process)
    {

      for (int i = 0; i < d.size(); )
      {
        if (d[i] == a_value)
        {
          d.erase(d.begin() + i);
        }
        else
        {
          i++;// Increment i++ if not deleting 
        }
      }
    }
  TIMER_END
}

template <typename T, typename V>
void ProfileIndexFirst(const T & a_data, const V a_value)
{
  TIMER_START
    for (auto& d : process)
    {

      for (int i = 0; i < d.size(); )
      {
        if (d[i] == a_value)
        {
          d.erase(d.begin() + i);
          break;
        }
        else
        {
          i++;// Increment i++ if not deleting 
        }
      }
    }
  TIMER_END
}


template <typename T, typename V>
void ProfileIterator(const T & a_data, const V a_value)
{
  TIMER_START
    for (auto& d : process)
    {
      for (auto it = d.begin(); it != d.end();)
      {
        if (*it == a_value)
        {
          it = d.erase(it);
        }
        else
        {
          it++;
        }
      }
    }
  TIMER_END
}


template <typename T, typename V>
void ProfileIteratorFirst(const T & a_data, const V a_value)
{
  TIMER_START
    for (auto& d : process)
    {
      for (auto it = d.begin(); it != d.end();)
      {
        if (*it == a_value)
        {
          it = d.erase(it);
          break;
        }
        else
        {
          it++;
        }
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
void RunTest(const T & a_data, const V & a_value, bool a_firstOnlyTests = false)
{ 
  cout << "======================\n";
  cout << "  eraser : "; ProfileRemove(a_data, a_value);
  cout << "  unordered_eraser : "; ProfileRemove(a_data, a_value, true);
  cout << "  std::remove : "; GenericRemove(a_data, a_value);
  cout << "  std::remove_if : "; GenericRemoveIf(a_data, a_value);
  cout << "  array_copy : "; RemoveArrayCopy(a_data, a_value);
  cout << "  index : "; ProfileIndex(a_data, a_value);
  cout << "  iterator : "; ProfileIterator(a_data, a_value);

  if (a_firstOnlyTests)
  {
    cout << "======================\n";
    cout << "  eraser First : "; ProfileRemoveFirst(a_data, a_value);
    cout << "  unordered_eraser First : "; ProfileRemoveFirst(a_data, a_value, true);
    cout << "  std::find First : "; GenericRemoveFirst(a_data, a_value);
    cout << "  index First : "; ProfileIndexFirst(a_data, a_value);
    cout << "  iterator First: "; ProfileIteratorFirst(a_data, a_value);
  }

  s_csvFile << "\n";
}

void WriteHeaders()
{
  s_csvFile << "eraser,";
  s_csvFile << "unordered_eraser,";
  s_csvFile << "std::remove,";
  s_csvFile << "std::remove_if,";
  s_csvFile << "array_copy,";
  s_csvFile << "index,"; 
  s_csvFile << "iterator,"; 

  s_csvFile << "eraser First,"; 
  s_csvFile << "unordered_eraser First,"; 
  s_csvFile << "std::find First,"; 
  s_csvFile << "index First,"; 
  s_csvFile << "iterator First,"; 

  s_csvFile << "\n";
}

template <typename T, typename V>
void RunTests(const T & a_data, const V & a_value)
{
  {
    T newData = a_data;
    s_csvFile << "Base Iterate,";
    RunTest(a_data, a_value, true);
  }

  {
    T newData = a_data;
    newData[0] = a_value;
    s_csvFile << "First value,";
    RunTest(newData, a_value, true);
  }

  {
    T newData = a_data;
    newData[newData.size() / 2] = a_value;
    s_csvFile << "Middle value,";
    RunTest(newData, a_value, true); 
  }

  {
    T newData = a_data;
    newData[newData.size() - 1] = a_value;
    s_csvFile << "Last value,";
    RunTest(newData, a_value, true);
  }

  {
    T newData = a_data;
    for (auto i : iter::counter(newData.size() / 4)) { newData[i] = a_value; }
    s_csvFile << "Remove 1/4,";
    RunTest(newData, a_value);
  }

  {
    T newData = a_data;
    for (auto i : iter::counter(newData.size() / 2) ){ newData[i] = a_value; }

    s_csvFile << "Remove 1/2,";
    RunTest(newData, a_value);
  }

  {
    T newData = a_data;
    for (auto i : iter::counter(newData.size()  * 3 / 4)) { newData[i] = a_value; }
    s_csvFile << "Remove 3/4,";
    RunTest(newData, a_value);
  }

  {
    T newData = a_data;
    for (auto& i : newData) { i = a_value; }
    s_csvFile << "Remove all,";
    RunTest(newData, a_value);
  }
}

void Iterator_RunProfile()
{
  s_csvFile.open("iter_profile.csv");

  for(int i = 10; i <= 1000; i*=10)
  {
    std::vector<int> testData(i);
    cout << "\n\nstd:vector<int>: (" << testData.size() << ") x (" << LOOPCOUNT << ")\n";
    s_csvFile << "\n\nstd:vector<int>: (" << testData.size() << ") x (" << LOOPCOUNT << "),";
    WriteHeaders();
    RunTests(testData, 1);
  }

  for (int i = 10; i <= 1000; i *= 10)
  {
    std::vector<std::string> testData(i);
    for (auto& v : testData) { v = "0"; }

    cout << "===================================================================\n";
    cout << "\n\nstd:vector<std::string>: (" << testData.size() << ") x (" << LOOPCOUNT << ")\n";
    s_csvFile << "\n\nstd:vector<std::string>: (" << testData.size() << ") x (" << LOOPCOUNT << "),";
    WriteHeaders();
    RunTests(testData, "1");
  }


  s_csvFile.close();
}