# Enum Macros
This article describes why and how to use the provided enum macros.


## Introduction

Typically when using enums you also want a string representation which involes maintaining a seperate list:

```c++
  enum class MyEnum
  {
    Value1,
    Value2,
    Value3
  };

  const char * s_myEnumStr[]
  {
    "Value1",
    "Value2",
    "Value3"
  };
```


This is obviously error prone if the enum ever changes. One way of helping keep in sync is to add a static_assert():

```c++
  enum class MyEnum
  {
    Value1,
    Value2,
    Value3,
    COUNT
  };

  const char * s_myEnumStr[]
  {
    "Value1",
    "Value2",
    "Value3"
  };
  static_assert(MyEnum::COUNT == (sizeof(s_myEnumStr)/ sizeof(s_myEnumStr[0])), "Update lookup table");
```

But this is still error prone if enums change naming or ordering. This will also not work well if the enums are assigned non-sequential values.


## VALUE_ENUM() and SEQUENTIAL_ENUM() Macros


Usage - First define the enum values with a define:
```c++
#define MyEnum_EnumValues(EV) \
            EV(Value1) \
            EV(Value2) \
            EV(value3)
```

 Values can also be assigned to each enum:
 ```c++
#define MyEnum_EnumValues(EV) \
            EV(Value1) \
            EV(Value2, 1 << 8) \
            EV(value3, Value2 | Value1)
```

Then in a header file use the define like:

 ```c++
VALUE_ENUM(MyEnum, uint32_t) // second parameter is the base type of the enum
```

This expands to:

```c++
enum class MyEnum : uint32_t { ...Enum values... };
```

If the enum is purely sequential, some optimizations can be made by using;

```c++
SEQUENTIAL_ENUM(MyEnum, uint32_t);
```

In a .cpp file call:

```c++
VALUE_ENUM_BODY(MyEnum) // For value enum
SEQUENTIAL_ENUM_BODY(MyEnum) // For sequential enum

// Use the below if the enum is in a class/namespace
VALUE_ENUM_BODYSCOPE(MyEnum, ClassName)
SEQUENTIAL_ENUM_BODYSCOPE(MyEnum, ClassName)
```

Once defined, a **MyEnum_Values** type is available.
This can be used in code to:

- Convert values to strings: **MyEnum_Values::to_string(Value1)**
- Get the count of enum values **MyEnum_Values::COUNT**
- Iterate over the values/strings:
```c++ 
    for (auto val : MyEnum_Values()) {
        val.c_str() // Get string of enum
        val.value() // Get enum value
```

Using this code you can easily write code to convert from a string to an enum. 
(Not provided as every code base has different error handling and string requirements)


Some other solutons to this enum issue can be found at:
 - Better Enums (https://github.com/aantron/better-enums)
 - C++ proposial N4428 (possibly for C++20?)


## ENUM_FLAG_OPS() Macro

A macro is also provided to generate the bitwise operations generally needed if the enum is a flag type. 
This can be a plain enum - does not need to be defined by VALUE_ENUM().

```c++
    // Define this below an enum 
    ENUM_FLAG_OPS(MyEnum)

    // Allows the below bit operations:
    MyEnum bitOp1 = MyEnum::Value1 | MyEnum::Value2;
    MyEnum bitOp2 = MyEnum::Value1 & MyEnum::Value2;
    MyEnum bitOp3 = MyEnum::Value1 ^ MyEnum::Value2;
    MyEnum bitOp4 = ~MyEnum::Value1;
```

If the enum is inside a class, use the macro outside the class definition
```c++
    class ClassName
    {
    public:
      enum MyEnum { ...}
    };
    
    ENUM_FLAG_OPS(ClassName::MyEnum)
```



