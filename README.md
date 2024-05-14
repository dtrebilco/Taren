# Taren
Taren is a collection of C++ code that provide simple but useful functionality. 
Taren code is standalone and only makes use of some STL.

# Taren articles
Along with the documentation is a series of articles explaining why certain functions exist.
- [Eraser iterators.](./Articles/EraserProfile.md)
- [Eraser append.](./Articles/EraserSafeAppend.md)
- [Enum Macros.](./Articles/EnumMacros.md)

# Taren functions
## Iterator: eraser() and unordered_eraser()
Helpers to remove elements from a container while iterating it.

Read about the performance of these iterators in [this article.](./Articles/EraserProfile.md)

```c++   
  for(auto& item : iter::eraser(array))
  {
    if(*item == value) // Access item via deref
    {
      item.mark_for_erase();  // Item is marked for deletion, but is still valid until end of loop iteration
      item.index(); // Get the original index of the item in the array 
    }
  }
```
If preserving order is not important:
```c++   
  for(auto& item : iter::unordered_eraser(array))
  {
    if(*item == value) // Access item via deref
    {
      item.mark_for_erase();  // Item is marked for deletion, but is still valid until end of loop iteration
    }
  }
```

## IteratorExt: eraser_safe_append()
Helper to remove elements and append to a container while iterating it.

Read about the usage of this iterator in [this article.](./Articles/EraserSafeAppend.md)

```c++   
  for(auto& item : iter::eraser_safe_append(vector))
  {
    auto& data = *item;          // Can safely store reference to item under iteration
    vector.push_back(newValue);  // Can append new data to vector (even if vector resizes)

    if(data == value)
    {
      item.mark_for_erase();  // Item is marked for deletion, but is still valid until end of loop iteration
      item.index(); // Get the original index of the item in the array 
    }
  }
```

## Iterator: reverse()
This helper simply reverses the iteration of the container
```c++
  for(auto& item : iter::reverse(array))
```


## Iterator: counter() and counter_reverse()
This helper provides a counter iterator in the cases where you still need a index.
```c++
  // Produces -> 0,1,2,3,4,5...
  for(auto& index : iter::counter(array.size()))
```

```c++
  // Produces -> ...5,4,3,2,1,0 
  for(auto& index : iter::counter_reverse(array.size()))
```

If used throughout a code base, it can be easily modified to account for different platforms preferences in loop counter types.


## Iterator: indexer()
This helper simply provides a way to get the index of the iteration item (eraser() also provides this functionality)
```c++
  for(const auto& item : iter::indexer(array))
  {
    if(*item == blah)
    {
      return item.index();
    }
  }
```

## EnumMacros VALUE_ENUM() / SEQUENTIAL_ENUM()
These macros provide a way to iterate enum values and associated strings.
See the [article](./Articles/EnumMacros.md) for more details.
```c++
#define MyEnum_EnumValues(EV) \
            EV(Value1) \
            EV(Value2, 1 << 8) \
            EV(value3, Value2 | Value1) 
VALUE_ENUM(MyEnum, uint32_t)
VALUE_ENUM_BODY(MyEnum)

// Can now do the below code
const char* enumStr = MyEnum_Values::to_string(Value1);

for (auto val : MyEnum_Values()) {
    val.c_str() // Get string of enum
    val.value() // Get enum value
```

## EnumMacros ENUM_FLAG_OPS()
This macro is used to generate the bitwise operations generally needed if the enum is a flag type. 
See the [article](./Articles/EnumMacros.md) for more details.
```c++
enum class BitFlags
{
  Flag1 =  1 << 0,
  Flag2 =  1 << 1,
  Flag3 =  1 << 2,
};
ENUM_FLAG_OPS(BitFlags)

// Allows the below bit operations:
BitFlags bitOp1 = BitFlags::Flag1 | BitFlags::Flag2;
BitFlags bitOp2 = BitFlags::Flag1 & BitFlags::Flag2;
BitFlags bitOp3 = BitFlags::Flag1 ^ BitFlags::Flag2;
BitFlags bitOp4 = ~BitFlags::Flag1;

```

## Profiler

This header generates profile json that can be loaded into: 

<chrome://tracing>

[Blog on Chrome tracing](https://aras-p.info/blog/2017/01/23/Chrome-Tracing-as-Profiler-Frontend/)

It is implemented lock free and allocation free (during profiling) with no platform specific code. 

To enable, define **TAREN_PROFILE_ENABLE** in the project builds that need profiling, then in one .cpp file define **TAREN_PROFILER_IMPLEMENTATION** before including this file. 
i.e. it should look like this:
```c++
#define TAREN_PROFILER_IMPLEMENTATION
#include "Profiler.h"
```

```c++
PROFILE_BEGIN(); // Enables profiling

PROFILE_TAG_BEGIN("TagName");  // Starts a tag
PROFILE_TAG_END();             // Ends a tag

PROFILE_SCOPE("TagName");      // Begin / End tag scope
 
PROFILE_TAG_VALUE("TagName", 123); // Add an instant tag with a value

PROFILE_END(string)  // Writes tags to a string
PROFILE_ENDFILEJSON("filename") // Writes tags to a file
```
 
Default tags must be a string literal or it will fail to compile. If you need a dynamic string, there is a limited scratch buffer that is used with the COPY / FORMAT / PRINTF variants of the tag types.

```c++
PROFILE_TAG_PRINTF_BEGIN("Value %d", 1234);
PROFILE_TAG_FORMAT_BEGIN("Value {}", 1234);
PROFILE_TAG_COPY_BEGIN(dynamicString.c_str());
```
