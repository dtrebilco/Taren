
#include "../Iterator.h"

#define TAREN_PROFILER_IMPLEMENTATION
#include "../Profiler.h"
#include <iostream>
#include <vector>
#include <thread>
#include <random>

bool Iterator_UnitTests();
void Iterator_RunProfile();
bool EnumMacro_UnitTests();


std::atomic_bool g_threadshutdown = false;

void print_thread_id(int id) {

  while (!g_threadshutdown)
  {
    PROFILE_SCOPE_FORMAT("Thread processing {}", id);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(10, 50);
    int sleep_time = dist(mt);

    for (uint32_t i = 0; i < sleep_time; i++)
    {
      PROFILE_SCOPE("Funny");

      using clock = std::chrono::high_resolution_clock;
      clock::time_point startTime = clock::now();
      while ((clock::now() - startTime) < std::chrono::milliseconds(1))
      {
      }

      //std::this_thread::sleep_for(std::chrono::milliseconds(1));
      //std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    //std::cout << "Thread " << id << " slept for " << sleep_time * 10 << " milliseconds\n";
  }
}

int main()
{
  EnumMacro_UnitTests();
  Iterator_UnitTests();
  Iterator_RunProfile();

/*
  std::vector<std::thread> threads;

  for (int i = 0; i < 10; ++i)
  {
    threads.push_back(std::thread(print_thread_id, i));
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  PROFILE_BEGIN();

  {
    PROFILE_SCOPE("Funny");
    PROFILE_SCOPE("Funny");

    {

      std::string buf("Data");
      PROFILE_SCOPE_COPY(buf.c_str());

      PROFILE_SCOPE_FORMAT("Data Format {}", 675);

      PROFILE_SCOPE_PRINTF("Data %d", 123);

      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      PROFILE_SCOPE_PRINTF("Data %d", 456);

      PROFILE_TAG_VALUE("Test", 123);

      std::this_thread::sleep_for(std::chrono::milliseconds(200));

      PROFILE_TAG_VALUE("FooBar", 678);
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      PROFILE_TAG_VALUE_COPY("Copy str", 675);
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      PROFILE_TAG_VALUE_FORMAT(13, "Format str {0}", 132);
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      PROFILE_TAG_VALUE_PRINTF(14, "Printf %d", 142);

      PROFILE_TAG_BEGIN("Not ending");

      PROFILE_TAG_COPY_BEGIN(std::string("CopyStr").c_str());
      PROFILE_TAG_FORMAT_BEGIN("Format {0}", 376);
      PROFILE_TAG_PRINTF_BEGIN("Printf %d", 567);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      PROFILE_TAG_END();
      PROFILE_TAG_END();
      PROFILE_TAG_END();

      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
  }
  PROFILE_ENDFILEJSON("Profile");

  std::string outString;
  PROFILE_END(outString);

  g_threadshutdown = true;
  for (auto& thread : threads)
  {
    thread.join();
  }
*/
  //getchar();
  return 0;
}

