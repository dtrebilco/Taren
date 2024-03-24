/// \brief A simple profiler that generates json that can be loaded into chrome://tracing
///       It is implemented with no platform specific code. 
///       Limitations compared to the real trace generator:
///        * No string copying
///        * No correct thread id's or thread names logged
///        * No meta data or log arguments recorded
/// See:  http://www.gamasutra.com/view/news/176420/Indepth_Using_Chrometracing_to_view_your_inline_profiling_data.php
///       https://aras-p.info/blog/2017/01/23/Chrome-Tracing-as-Profiler-Frontend/
///       https://github.com/mainroach/sandbox/tree/master/chrome-event-trace
///       https://github.com/catapult-project/catapult
///       https://src.chromium.org/viewvc/chrome/trunk/src/base/debug/trace_event.h?view=markup
///       https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit
///
///  To enable, define TAREN_PROFILE_ENABLE in the project in the builds that need profiling, then in one C++ file
///  define TAREN_PROFILER_IMPLEMENTATION before including this file. 
///    // i.e. it should look like this:
///    #include ...
///    #include ...
///    #define TAREN_PROFILER_IMPLEMENTATION
///    #include "Profiler.h"
/// 
#pragma once

#ifdef TAREN_PROFILE_ENABLE

#define PROFILE_BEGIN(x) static_assert(x[0] != 0, "Only static strings"); taren_profiler::ProfileTagBegin(x)
#define PROFILE_END() taren_profiler::ProfileTagEnd()
#define PROFILE_SCOPE_INTERNAL2(X,Y) X ## Y
#define PROFILE_SCOPE_INTERNAL(a,b) PROFILE_SCOPE_INTERNAL2(a,b)
#define PROFILE_SCOPE(x) static_assert(x[0] != 0, "Only static strings"); taren_profiler::ProfileScope PROFILE_SCOPE_INTERNAL(p,__LINE__) (x)

#else // !TAREN_PROFILE_ENABLE

#define PROFILE_BEGIN(x)
#define PROFILE_END()
#define PROFILE_SCOPE(x)

#endif // !TAREN_PROFILE_ENABLE

#ifdef TAREN_PROFILE_ENABLE

#include <string>
#include <ostream>

namespace taren_profiler
{
  /// \brief Get if the profiler is currently running
  /// \return Returns true if profiling is current running
  bool IsProfiling();

  /// \brief Start profiling recording
  /// \param i_bufferSize How large the buffer size for recording should be (excess is discarded)
  /// \return Returns true if profiling was started
  bool Begin(size_t i_bufferSize = 10000000);
  
  /// \brief Ends the profiling
  /// \param o_outStream The stream to write the json to
  /// \param o_outString The string to write the json to
  /// \return Returns true on success
  bool End(std::ostream& o_outStream);
  bool End(std::string& o_outString);

  /// \brief Ends the profiling and writes the json results to a file
  /// \param i_fileName The file name to write to.
  /// \param i_appendDate If true, the current date/time and the extension .json is appened to the filename before opening.
  /// \return Returns true on success
  bool EndFileJson(const char* i_fileName, bool i_appendDate = true);

  /// \brief Starts a profile tag
  /// \param i_str The tag name, must be a static string
  void ProfileTagBegin(const char* i_str);

  /// \brief End a profile tag
  void ProfileTagEnd();

  struct ProfileScope
  {
    ProfileScope(const char* i_str) { ProfileTagBegin(i_str); }
    ~ProfileScope() { ProfileTagEnd(); }
  };

}

#ifdef TAREN_PROFILER_IMPLEMENTATION

#include <memory>
#include <chrono>
#include <ctime>

#include <thread>
#include <atomic>
#include <mutex>

#include <vector>
#include <unordered_map>

#include <sstream>
#include <fstream>

namespace taren_profiler
{
  using clock = std::chrono::high_resolution_clock;

  struct ProfileRecord
  {
    clock::time_point m_time;    // The time of the profile data
    std::thread::id m_threadID;  // The id of the thread
    const char* m_tag = nullptr; // The tag used in profiling - if empty is an end event
  };

  struct Tags
  {
    int32_t m_index = -1;            // The index of the thread
    std::vector<const char*> m_tags; // The tag stack
  };

  struct ProfileData
  {
    clock::time_point m_startTime;        // The start time of the profile

    std::atomic_bool m_enabled = false;   // If profiling is enabled
    std::mutex m_access;                  // Access mutex for changing data

    size_t m_maxRecords = 0;              // The maximum number of records
    std::vector<ProfileRecord> m_records; // The profiling records
  };
  static ProfileData g_pData; // The global profile data

  void ProfileTagBegin(const char* i_str)
  {
    if (!g_pData->m_enabled)
    {
      return;
    }

    // Ensure no null pointers on begin
    if (i_str == nullptr)
    {
      i_str = "Unknown";
    }

    // Create the profile record
    ProfileRecord newData = {};
    newData.m_threadID = std::this_thread::get_id();
    newData.m_tag = i_str;

    // There is a race condition where a record could be added after the profiling has ended (m_enabled changed)- this is benign however
    std::lock_guard<std::mutex> lock(g_pData->m_access);
    if (g_pData->m_records.size() < g_pData->m_maxRecords)
    {
      g_pData->m_records.push_back(newData);

      // Assign the time as the last possible thing
      g_pData->m_records.back().m_time = clock::now();
    }
  }

  void ProfileTagEnd()
  {
    if (!g_pData->m_enabled)
    {
      return;
    }

    ProfileRecord newData = {};
    newData.m_time = clock::now(); // Always get time as soon as possible
    newData.m_threadID = std::this_thread::get_id();
    newData.m_tag = nullptr;

    // There is a race condition where a record could be added after the profiling has ended (m_enabled changed)- this is benign however
    std::lock_guard<std::mutex> lock(g_pData->m_access);
    if (g_pData->m_records.size() < g_pData->m_maxRecords)
    {
      g_pData->m_records.push_back(newData);
    }
  }

  bool IsProfiling()
  {
    if (!g_pData)
    {
      return false;
    }

    return g_pData->m_enabled;
  }

  bool Begin(size_t i_bufferSize)
  {
    if (!g_pData)
    {
      return false;
    }

    std::lock_guard<std::mutex> lock(g_pData->m_access);
    if (g_pData->m_enabled)
    {
      return false;
    }

    // Clear all data (may have been some extra in buffers from previous enable)
    g_pData->m_maxRecords = i_bufferSize / sizeof(ProfileRecord);
    g_pData->m_records.resize(0);
    g_pData->m_records.reserve(g_pData->m_maxRecords);
    g_pData->m_startTime = clock::now();

    g_pData->m_enabled = true;
    return true;
  }

  static void CleanJsonStr(std::string& io_str)
  {
    size_t startPos = 0;
    while ((startPos = io_str.find_first_of("\\\"", startPos)) != std::string::npos)
    {
      io_str.insert(startPos, 1, '\\');
      startPos += 2;
    }
  }

  bool End(std::ostream& o_outStream)
  {
    if (!g_pData)
    {
      return false;
    }

    std::lock_guard<std::mutex> lock(g_pData->m_access);
    if (!g_pData->m_enabled)
    {
      return false;
    }
    g_pData->m_enabled = false;

    // Init this thread as the primary thread
    std::unordered_map<std::thread::id, Tags> threadStack;
    threadStack[std::this_thread::get_id()].m_index = 0;

    bool first = true;
    int32_t threadCounter = 1;

    std::string cleanTag;
    o_outStream << "{\"traceEvents\":[\n";
    for (const ProfileRecord& entry : g_pData->m_records)
    {
      auto& stack = threadStack[entry.m_threadID];

      // Assign a unique index to each thread
      if (stack.m_index < 0)
      {
        stack.m_index = threadCounter;
        threadCounter++;
      }

      // Get the name tags
      const char* tag = entry.m_tag;
      const char* typeTag = "B";
      if (tag != nullptr)
      {
        stack.m_tags.push_back(tag);
      }
      else
      {
        typeTag = "E";
        if (stack.m_tags.size() == 0)
        {
          tag = "Unknown";
        }
        else
        {
          tag = stack.m_tags.back();
          stack.m_tags.pop_back();
        }
      }

      // Markup invalid json characters
      if (strchr(tag, '"') != nullptr ||
        strchr(tag, '\\') != nullptr)
      {
        cleanTag = tag;
        CleanJsonStr(cleanTag);
        tag = cleanTag.c_str();
      }

      // Get the microsecond count
      long long msCount = std::chrono::duration_cast<std::chrono::microseconds>(entry.m_time - g_pData->m_startTime).count();

      if (!first)
      {
        o_outStream << ",\n";
      }
      first = false;

      char msString[64];
      snprintf(msString, sizeof(msString), "%lld", msCount);

      char indexString[64];
      snprintf(indexString, sizeof(indexString), "%d", stack.m_index);

      // Format the string
      o_outStream <<
        "{\"name\":\"" << tag <<
        "\",\"ph\":\"" << typeTag <<
        "\",\"ts\":" << msString <<
        ",\"tid\":" << indexString <<
        ",\"cat\":\"\",\"pid\":0,\"args\":{}}";
    }

    // Write thread "names"
    if (!first)
    {
      for (auto& t : threadStack)
      {
        char indexString[64];
        snprintf(indexString, sizeof(indexString), "%d", t.second.m_index);

        // Sort thread listing by the time that they appear in the profile (tool sorts by name)
        char indexSpaceString[64];
        snprintf(indexSpaceString, sizeof(indexSpaceString), "%02d", t.second.m_index);

        // Ensure a clean json string
        std::stringstream ss;
        ss << t.first;
        std::string threadName = ss.str();
        CleanJsonStr(threadName);

        o_outStream <<
          ",\n{\"name\":\"thread_name\",\"ph\":\"M\",\"pid\":0,\"tid\":" << indexString <<
          ",\"args\":{\"name\":\"Thread" << indexSpaceString << "_" << threadName << "\"}}";
      }
    }

    o_outStream << "\n]\n}\n";
    return true;
  }

  bool End(std::string& o_outString)
  {
    std::stringstream ss;
    bool retval = End(ss);
    o_outString = ss.str();
    return retval;
  }

  bool EndFileJson(const char* i_fileName, bool i_appendDate)
  {
    std::ofstream file;
    if (i_appendDate)
    {
      // Create a filename with the current date in it
      std::time_t t = std::time(nullptr);
      tm timeBuf;
      localtime_s(&timeBuf, &t);
      char timeStr[100];
      if (std::strftime(timeStr, sizeof(timeStr), "%Y%m%d-%H%M%S", &timeBuf) == 0)
      {
        return false;
      }

      // Append date and file extension
      char newFilename[1024];
      snprintf(newFilename, sizeof(newFilename), "%s_%s.json", i_fileName, timeStr);
      file.open(newFilename);
    }
    else
    {
      file.open(i_fileName);
    }

    if (!file.is_open())
    {
      return false;
    }
    return End(file);
  }

}

#endif // TAREN_PROFILER_IMPLEMENTATION

#endif // TAREN_PROFILE_ENABLE
