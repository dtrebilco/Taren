/// \brief A simple profiler that generates json that can be loaded into chrome://tracing
///       It is implemented with no platform specific code. 
///       Limitations compared to the real trace generator:
///        * No correct thread id's or thread names logged
///        * Only simple meta data recorded
/// See:  http://www.gamasutra.com/view/news/176420/Indepth_Using_Chrometracing_to_view_your_inline_profiling_data.php
///       https://aras-p.info/blog/2017/01/23/Chrome-Tracing-as-Profiler-Frontend/
///       https://github.com/mainroach/sandbox/tree/master/chrome-event-trace
///       https://github.com/catapult-project/catapult
///       https://src.chromium.org/viewvc/chrome/trunk/src/base/debug/trace_event.h?view=markup
///       https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit
///
///  To enable, define TAREN_PROFILE_ENABLE in the project builds that need profiling, then in one .cpp file
///  define TAREN_PROFILER_IMPLEMENTATION before including this file. 
///    // i.e. it should look like this:
///    #define TAREN_PROFILER_IMPLEMENTATION
///    #include "Profiler.h"
/// 
#pragma once

// TODO:
//  - Note on no allocations and limits TAREN_PROFILER_FORMAT_COUNT TAREN_PROFILER_TAG_MAX_COUNT TAREN_PROFILER_TAG_NAME_BUFFER_SIZE
//  - Note on thread safety - only during profiling is thread safe, Begin/End are not thread safe - use a wrapper mutex if this is neeed
//  - Test thread safety in End() code

#ifdef TAREN_PROFILE_ENABLE

#ifndef TAREN_PROFILER_FORMAT_COUNT
#define TAREN_PROFILER_FORMAT_COUNT 30
#endif //!TAREN_PROFILER_TAG_COUNT

#define PROFILE_FORMAT_INTERNAL(...) char buf[TAREN_PROFILER_FORMAT_COUNT]; const auto out = std::format_to_n(buf, TAREN_PROFILER_FORMAT_COUNT - 1, __VA_ARGS__); *out.out = '\0'
#define PROFILE_PRINTF_INTERNAL(...) char buf[TAREN_PROFILER_FORMAT_COUNT]; std::snprintf(buf, TAREN_PROFILER_FORMAT_COUNT, __VA_ARGS__)

#define PROFILE_BEGIN(...) taren_profiler::Begin(__VA_ARGS__)
#define PROFILE_END(...) taren_profiler::End(__VA_ARGS__)
#define PROFILE_ENDFILEJSON(...) taren_profiler::EndFileJson(__VA_ARGS__)

#define PROFILE_TAG_BEGIN(str) static_assert(str[0] != 0, "Only literal strings - Use PROFILE_TAGCOPY_BEGIN"); taren_profiler::ProfileTag(taren_profiler::TagType::Begin, str)
#define PROFILE_TAG_COPY_BEGIN(str) taren_profiler::ProfileTag(taren_profiler::TagType::Begin, str, true)
#define PROFILE_TAG_FORMAT_BEGIN(...) if(taren_profiler::IsProfiling()) { PROFILE_FORMAT_INTERNAL(__VA_ARGS__); PROFILE_TAG_COPY_BEGIN(buf); }
#define PROFILE_TAG_PRINTF_BEGIN(...) if(taren_profiler::IsProfiling()) { PROFILE_PRINTF_INTERNAL(__VA_ARGS__); PROFILE_TAG_COPY_BEGIN(buf); }
#define PROFILE_TAG_END() taren_profiler::ProfileTag(taren_profiler::TagType::End, nullptr)

#define PROFILE_SCOPE_INTERNAL2(X,Y) X ## Y
#define PROFILE_SCOPE_INTERNAL(a,b) PROFILE_SCOPE_INTERNAL2(a,b)
#define PROFILE_SCOPE(str) PROFILE_TAG_BEGIN(str); taren_profiler::ProfileScope PROFILE_SCOPE_INTERNAL(taren_profile_scope,__LINE__)
#define PROFILE_SCOPE_COPY(str) PROFILE_TAG_COPY_BEGIN(str); taren_profiler::ProfileScope PROFILE_SCOPE_INTERNAL(taren_profile_scope,__LINE__)
#define PROFILE_SCOPE_FORMAT(...) PROFILE_TAG_FORMAT_BEGIN(__VA_ARGS__); taren_profiler::ProfileScope PROFILE_SCOPE_INTERNAL(taren_profile_scope,__LINE__)
#define PROFILE_SCOPE_PRINTF(...) PROFILE_TAG_PRINTF_BEGIN(__VA_ARGS__); taren_profiler::ProfileScope PROFILE_SCOPE_INTERNAL(taren_profile_scope,__LINE__)

#define PROFILE_TAG_VALUE(str, value) static_assert(str[0] != 0, "Only literal strings - Use PROFILE_TAG_VALUE_COPY"); taren_profiler::ProfileTag(taren_profiler::TagType::Value, str, false, value)
#define PROFILE_TAG_VALUE_COPY(str, value) taren_profiler::ProfileTag(taren_profiler::TagType::Value, str, true, value)
#define PROFILE_TAG_VALUE_FORMAT(...) if(taren_profiler::IsProfiling()) { PROFILE_FORMAT_INTERNAL(__VA_ARGS__); PROFILE_TAG_VALUE_COPY(buf, value); }
#define PROFILE_TAG_VALUE_PRINTF(...) if(taren_profiler::IsProfiling()) { PROFILE_PRINTF_INTERNAL(__VA_ARGS__); PROFILE_TAG_VALUE_COPY(buf, value); }

#else // !TAREN_PROFILE_ENABLE

#define PROFILE_BEGIN(...)
#define PROFILE_END(...)
#define PROFILE_ENDFILEJSON(...)

#define PROFILE_TAG_BEGIN(...)
#define PROFILE_TAG_BEGIN_COPY(...)
#define PROFILE_TAG_BEGIN_FORMAT(...)
#define PROFILE_TAG_BEGIN_PRINTF(...)
#define PROFILE_TAG_END()

#define PROFILE_SCOPE(...)
#define PROFILE_SCOPE_COPY(...)
#define PROFILE_SCOPE_FORMAT(...)
#define PROFILE_SCOPE_PRINTF(...)

#define PROFILE_TAG_VALUE(...)
#define PROFILE_TAG_VALUE_COPY(...)
#define PROFILE_TAG_VALUE_FORMAT(...)
#define PROFILE_TAG_VALUE_PRINTF(...)

#endif // !TAREN_PROFILE_ENABLE

#ifdef TAREN_PROFILE_ENABLE

#include <string>
#include <ostream>

#if (__cplusplus >= 202002L)
#include <format>
#endif

namespace taren_profiler
{
  enum class TagType
  {
    Begin,
    End,
    Value,
  };

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
  /// \param i_appendDateExtension If true, the current date/time and the extension .json is appened to the filename before opening.
  /// \return Returns true on success
  bool EndFileJson(const char* i_fileName, bool i_appendDateExtension = true);

  /// \brief Set a profiling tag
  /// \param i_type The type of tag
  /// \param i_str The tag name, must be a literal string or i_copyTag set to true
  /// \param i_copyStr If the i_str value should be copied internally
  /// \param i_value The value to supply with the tag
  void ProfileTag(TagType i_type, const char* i_str, bool i_copyStr = false, int32_t i_value = 0);

  struct ProfileScope
  {
    ~ProfileScope() { ProfileTag(TagType::End, nullptr); }
  };
}

#ifdef TAREN_PROFILER_IMPLEMENTATION

#ifndef TAREN_PROFILER_TAG_MAX_COUNT 
#define TAREN_PROFILER_TAG_MAX_COUNT 10000000
#endif //!TAREN_PROFILER_TAG_COUNT

#ifndef TAREN_PROFILER_TAG_NAME_BUFFER_SIZE
#define TAREN_PROFILER_TAG_NAME_BUFFER_SIZE 1000000
#endif //!TAREN_PROFILER_TAG_NAME_BUFFER_SIZE

#include <chrono>
#include <thread>
#include <atomic>

#include <ctime>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>

namespace
{
  using clock = std::chrono::high_resolution_clock;

  struct ProfileRecord
  {
    clock::time_point m_time;    // The time of the profile data
    const char* m_tag = nullptr; // The tag used in profiling - if empty is an end event
    std::thread::id m_threadID;  // The id of the thread

    taren_profiler::TagType m_type; // The tag type
    int32_t m_value = 0;            // Misc value used with the tag
  };

  std::atomic_bool g_enabled = false; // If profiling is enabled
  clock::time_point g_startTime;      // The start time of the profile

  std::atomic_uint32_t g_slotCount = 0;                  // The current slot counter
  std::atomic_uint32_t g_recordCount = 0;                // The current record count
  ProfileRecord g_records[TAREN_PROFILER_TAG_MAX_COUNT]; // The profiling records

  std::atomic_uint32_t g_copyBufferSize = 0;              // The current copy buffer usage count
  char g_copyBuffer[TAREN_PROFILER_TAG_NAME_BUFFER_SIZE]; // The buffer to store copied tag names

  const char* CopyStr(const char* i_str)
  {
    if (i_str == nullptr)
    {
      return nullptr;
    }

    // Allocate space to copy into
    uint32_t len = (uint32_t)strlen(i_str) + 1;
    uint32_t startOffset = g_copyBufferSize.fetch_add(len);
    if ((startOffset + len) <= TAREN_PROFILER_TAG_NAME_BUFFER_SIZE)
    {
      char* outBuffer = &g_copyBuffer[startOffset];
      memcpy(outBuffer, i_str, len);
      return outBuffer;
    }
    else
    {
      g_copyBufferSize -= len; // Undo the add to make room for a smaller tag
    }

    return "OutOfTagBufferSpace";
  }
}

namespace taren_profiler
{
  void ProfileTag(TagType i_type, const char* i_str, bool i_copyStr, int32_t i_value)
  {
    if (!g_enabled)
    {
      return;
    }

    // Get the slot to put the record
    uint32_t recordIndex = g_slotCount.fetch_add(1);
    if (recordIndex < TAREN_PROFILER_TAG_MAX_COUNT)
    {
      ProfileRecord& newData = g_records[recordIndex];
      newData.m_type = i_type;
      newData.m_threadID = std::this_thread::get_id();
      newData.m_tag = i_copyStr ? CopyStr(i_str) : i_str;
      newData.m_value = i_value;
      newData.m_time = clock::now();  // Assign the time as the last possible thing

      g_recordCount++; // Flag that the record is complete
    }
    else
    {
      g_slotCount--; // Only hit if exceeded the record count or end of profiling, reverse the add
    }
  }

  bool IsProfiling()
  {
    return g_enabled;
  }

  bool Begin()
  {
    if (g_enabled)
    {
      return false;
    }

    // Clear all data (may have been some extra in buffers from previous enable)
    g_recordCount = 0;
    g_slotCount = 0;
    g_copyBufferSize = 0;
    g_startTime = clock::now();
    g_enabled = true;
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
    if (!g_enabled)
    {
      return false;
    }
    g_enabled = false;

    struct Tags
    {
      int32_t m_index = -1;            // The index of the thread
      std::vector<const char*> m_tags; // The tag stack
    };

    // Init this thread as the primary thread
    std::unordered_map<std::thread::id, Tags> threadStack;
    threadStack[std::this_thread::get_id()].m_index = 0;

    int32_t threadCounter = 1;

    std::string cleanTag;
    o_outStream << "{\"traceEvents\":[\n";

    // Flag that records should no longer be written by setting the slot count to TAREN_PROFILER_TAG_COUNT
    uint32_t slotCount = g_slotCount;
    do
    {
      // Check if already at the limit (can exceed the limit for a short duration)
      if (slotCount >= TAREN_PROFILER_TAG_MAX_COUNT)
      {
        slotCount = TAREN_PROFILER_TAG_MAX_COUNT;
        break;
      }

    } while (!g_slotCount.compare_exchange_weak(slotCount, (uint32_t)TAREN_PROFILER_TAG_MAX_COUNT));

    // Wait for all threads to finish writing tags
    uint32_t recordCount = g_recordCount;
    while (recordCount != slotCount)
    {
      std::this_thread::yield();
      recordCount = g_recordCount;
    }

    for (size_t i = 0; i < recordCount; i++)
    {
      const ProfileRecord& entry = g_records[i];

      // Assign a unique index to each thread
      Tags& stack = threadStack[entry.m_threadID];
      if (stack.m_index < 0)
      {
        stack.m_index = threadCounter;
        threadCounter++;
      }

      // Get the name tags
      const char* tag = entry.m_tag;
      if (tag == nullptr)
      {
        tag = "Unknown";
      }
      const char* typeTag = "B";
      if (entry.m_type == TagType::Begin)
      {
        stack.m_tags.push_back(tag);
      }
      else if (entry.m_type == TagType::End)
      {
        typeTag = "E";
        if (stack.m_tags.size() > 0)
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
      long long msCount = std::chrono::duration_cast<std::chrono::microseconds>(entry.m_time - g_startTime).count();

      if (i != 0)
      {
        o_outStream << ",\n";
      }

      // Format the string (Note using process ID for threads as that gives a better formatting in the output tool for value tags)
      o_outStream <<
        "{\"name\":\"" << tag << "\",\"ph\":\"" << typeTag << "\",\"ts\":" << msCount << ",\"pid\":" << stack.m_index << ",\"cat\":\"\",\"tid\":0,";
        
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
        std::snprintf(indexSpaceString, sizeof(indexSpaceString), "%02d", t.second.m_index);

        // Ensure a clean json string (undefined what thread::id is)
        std::ostringstream ss;
        ss << t.first;
        std::string threadName = ss.str();
        CleanJsonStr(threadName);

        o_outStream << // (Note using process ID for threads as that gives a better formatting in the output tool for value tags)
          ",\n{\"name\":\"thread_name\",\"ph\":\"M\",\"tid\":0,\"pid\":" << t.second.m_index <<
          ",\"args\":{\"name\":\"Thread" << indexSpaceString << "_" << threadName << "\"}}";
      }
    }

    o_outStream << "\n]\n}\n";
    return true;
  }

  bool End(std::string& o_outString)
  {
    std::ostringstream ss;
    bool retval = End(ss);
    o_outString = ss.str();
    return retval;
  }

  bool EndFileJson(const char* i_fileName, bool i_appendDateExtension)
  {
    std::ofstream file;
    if (i_appendDateExtension)
    {
      // Create a filename with the current date in it with extension
      std::time_t t = std::time(nullptr);
      tm timeBuf;
      localtime_s(&timeBuf, &t);
      char extStr[120];
      if (std::strftime(extStr, sizeof(extStr), "_%Y%m%d-%H%M%S.json", &timeBuf) == 0)
      {
        return false;
      }

      file.open(std::string(i_fileName) + extStr);
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
