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

// TODO:
//  - Add defines to start and end profiling?
//  - Allow tags to exceed limit? 
//  - Have a buffer of chars to allow custom formatting - falls back to "CustomTag"
//  - Have single memory alloc tag

#ifdef TAREN_PROFILE_ENABLE

#define PROFILE_BEGIN(...) taren_profiler::Begin(__VA_ARGS__)
#define PROFILE_END(...) taren_profiler::End(__VA_ARGS__)
#define PROFILE_ENDFILEJSON(...) taren_profiler::EndFileJson(__VA_ARGS__)

#define PROFILE_TAG_BEGIN(str) static_assert(str[0] != 0, "Only literal strings - Use PROFILE_TAGCOPY_BEGIN"); taren_profiler::ProfileTagBegin(str)
#define PROFILE_TAG_COPY_BEGIN(str) taren_profiler::ProfileTagBegin(taren_profiler::CopyTag(str))
#define PROFILE_TAG_FORMAT_BEGIN(...) taren_profiler::ProfileTagBegin(taren_profiler::FormatTag(__VA_ARGS__))
#define PROFILE_TAG_END() taren_profiler::ProfileTagEnd()

#define PROFILE_SCOPE_INTERNAL2(X,Y) X ## Y
#define PROFILE_SCOPE_INTERNAL(a,b) PROFILE_SCOPE_INTERNAL2(a,b)
#define PROFILE_SCOPE(str) static_assert(str[0] != 0, "Only literal strings - Use PROFILE_SCOPECOPY"); taren_profiler::ProfileScope PROFILE_SCOPE_INTERNAL(taren_profile_scope,__LINE__) (str)
#define PROFILE_SCOPE_COPY(str) taren_profiler::ProfileScope PROFILE_SCOPE_INTERNAL(taren_profile_scope,__LINE__) (taren_profiler::CopyTag(str))
#define PROFILE_SCOPE_FORMAT(...) taren_profiler::ProfileScope PROFILE_SCOPE_INTERNAL(taren_profile_scope,__LINE__) (taren_profiler::FormatTag(__VA_ARGS__))

#define PROFILE_TAG_VALUE(str, value) static_assert(str[0] != 0, "Only literal strings - Use PROFILE_TAG_VALUE_COPY"); taren_profiler::ProfileTagValue(str, value)
#define PROFILE_TAG_VALUE_COPY(str, value) taren_profiler::ProfileTagValue(taren_profiler::CopyTag(str), value)

// https://en.cppreference.com/w/cpp/utility/format/format_to_n

#else // !TAREN_PROFILE_ENABLE

#define PROFILE_BEGIN(...)
#define PROFILE_END(...)
#define PROFILE_ENDFILEJSON(...)

#define PROFILE_TAG_BEGIN(...)
#define PROFILE_TAG_BEGIN_COPY(...)
#define PROFILE_TAG_BEGIN_FORMAT(...)
#define PROFILE_TAG_END()

#define PROFILE_SCOPE(...)
#define PROFILE_SCOPE_COPY(...)
#define PROFILE_SCOPE_FORMAT(...)

#define PROFILE_TAG_VALUE(...)
#define PROFILE_TAG_VALUE_COPY(...)

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
  /// \return Returns true if profiling was started
  bool Begin();
  
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
  /// \param i_str The tag name, must be a literal string or safe copy
  void ProfileTagBegin(const char* i_str);

  /// \brief End a profile tag
  void ProfileTagEnd();

  /// \brief Sets a profile tag and value at the current time
  /// \param i_str The tag name, must be a literal string or safe copy
  /// \param i_value The value to supply with the tag
  void ProfileTagValue(const char* i_str, int32_t i_value);

  struct ProfileScope
  {
    ProfileScope(const char* i_str) { ProfileTagBegin(i_str); }
    ~ProfileScope() { ProfileTagEnd(); }
  };

}

#ifdef TAREN_PROFILER_IMPLEMENTATION

#ifndef TAREN_PROFILER_TAG_COUNT 
#define TAREN_PROFILER_TAG_COUNT 10000000
#endif //!TAREN_PROFILER_TAG_COUNT

#ifndef TAREN_PROFILER_TAG_NAME_BUFFER_SIZE
#define TAREN_PROFILER_TAG_NAME_BUFFER_SIZE 1000000
#endif //!TAREN_PROFILER_TAG_NAME_BUFFER_SIZE

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

  enum class TagType
  {
    Begin,
    End,
    Value,
  };

  struct ProfileRecord
  {
    clock::time_point m_time;    // The time of the profile data
    const char* m_tag = nullptr; // The tag used in profiling - if empty is an end event
    std::thread::id m_threadID;  // The id of the thread

    TagType m_type;              // The tag type
    int32_t m_value;             // Misc value used with the tag
  };

  struct Tags
  {
    int32_t m_index = -1;            // The index of the thread
    std::vector<const char*> m_tags; // The tag stack
  };

  struct ProfileData
  {
    clock::time_point m_startTime;        // The start time of the profile

    std::atomic_bool m_enabled = false;                // If profiling is enabled
    std::atomic_uint32_t m_recordCount = 0;            // The current record count
    ProfileRecord m_records[TAREN_PROFILER_TAG_COUNT]; // The profiling records
  };
  static ProfileData g_data; // The global profile data

  void ProfileTagBegin(const char* i_str)
  {
    if (!g_data.m_enabled)
    {
      return;
    }

    // There is a race condition where a record could be added after the profiling has ended (m_enabled changed)- this is benign however
    uint32_t recordIndex = g_data.m_recordCount.fetch_add(1);
    if (recordIndex < TAREN_PROFILER_TAG_COUNT)
    {
      ProfileRecord& newData = g_data.m_records[recordIndex];
      newData.m_type = TagType::Begin;
      newData.m_threadID = std::this_thread::get_id();
      newData.m_tag = i_str;
      newData.m_time = clock::now();  // Assign the time as the last possible thing
    }
    else
    {
      g_data.m_recordCount--; // Only hit if exceeded the record count, reverse the add
    }
  }

  void ProfileTagEnd()
  {
    if (!g_data.m_enabled)
    {
      return;
    }

    // There is a race condition where a record could be added after the profiling has ended (m_enabled changed)- this is benign however
    uint32_t recordIndex = g_data.m_recordCount.fetch_add(1);
    if (recordIndex < TAREN_PROFILER_TAG_COUNT)
    {
      ProfileRecord& newData = g_data.m_records[recordIndex];
      newData.m_time = clock::now(); // Always get time as soon as possible
      newData.m_type = TagType::End;
      newData.m_threadID = std::this_thread::get_id();
    }
    else
    {
      g_data.m_recordCount--; // Only hit if exceeded the record count, reverse the add
    }
  }

  void ProfileTagValue(const char* i_str, int32_t i_value)
  {
    if (!g_data.m_enabled)
    {
      return;
    }

    // There is a race condition where a record could be added after the profiling has ended (m_enabled changed)- this is benign however
    uint32_t recordIndex = g_data.m_recordCount.fetch_add(1);
    if (recordIndex < TAREN_PROFILER_TAG_COUNT)
    {
      ProfileRecord& newData = g_data.m_records[recordIndex];
      newData.m_type = TagType::Value;
      newData.m_threadID = std::this_thread::get_id();
      newData.m_tag = i_str;
      newData.m_value = i_value;
      newData.m_time = clock::now(); // Assign the time as the last possible thing
    }
    else
    {
      g_data.m_recordCount--; // Only hit if exceeded the record count, reverse the add
    }
  }

  bool IsProfiling()
  {
    return g_data.m_enabled;
  }

  bool Begin()
  {
    if (g_data.m_enabled)
    {
      return false;
    }

    // Clear all data (may have been some extra in buffers from previous enable)
    g_data.m_recordCount = 0;
    g_data.m_startTime = clock::now();
    g_data.m_enabled = true;
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
    if (!g_data.m_enabled)
    {
      return false;
    }
    g_data.m_enabled = false;

    // Init this thread as the primary thread
    std::unordered_map<std::thread::id, Tags> threadStack;
    threadStack[std::this_thread::get_id()].m_index = 0;

    int32_t threadCounter = 1;

    std::string cleanTag;
    o_outStream << "{\"traceEvents\":[\n";

    // Save off count in case any other threads are still running
    size_t recordCount = g_data.m_recordCount;
    if (recordCount > TAREN_PROFILER_TAG_COUNT)
    {
      recordCount = TAREN_PROFILER_TAG_COUNT;
    }
    for (size_t i = 0; i < recordCount; i++)
    {
      const ProfileRecord& entry = g_data.m_records[i];

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
      if (entry.m_type == TagType::Begin)
      {
        stack.m_tags.push_back(tag);
      }
      else if (entry.m_type == TagType::End)
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
      else if (entry.m_type == TagType::Value)
      {
        typeTag = "O";
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
      long long msCount = std::chrono::duration_cast<std::chrono::microseconds>(entry.m_time - g_data.m_startTime).count();

      if (i != 0)
      {
        o_outStream << ",\n";
      }

      // Format the string
      o_outStream <<
        "{\"name\":\"" << tag << "\",\"ph\":\"" << typeTag << "\",\"ts\":" << msCount << ",\"tid\":" << stack.m_index << ",\"cat\":\"\",\"pid\":0,";
        
      if (entry.m_type == TagType::Value)
      {
        o_outStream << "\"id\":\"" << tag << "\", \"args\":{\"snapshot\":{\"Value\": " << entry.m_value << "}}}";
      }
      else
      {
        o_outStream << "\"args\":{}}";
      }
    }

    // Write thread "names"
    if (recordCount > 0)
    {
      for (auto& t : threadStack)
      {
        // Sort thread listing by the time that they appear in the profile (tool sorts by name)
        char indexSpaceString[64];
        snprintf(indexSpaceString, sizeof(indexSpaceString), "%02d", t.second.m_index);

        // Ensure a clean json string
        std::stringstream ss;
        ss << t.first;
        std::string threadName = ss.str();
        CleanJsonStr(threadName);

        o_outStream <<
          ",\n{\"name\":\"thread_name\",\"ph\":\"M\",\"pid\":0,\"tid\":" << t.second.m_index <<
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
