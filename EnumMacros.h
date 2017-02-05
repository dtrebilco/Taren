//=============================================================================
//  Copyright (C) 2016  Damian Trebilco
//  Licensed under the MIT license - See LICENSE.txt for details.
//=============================================================================
#ifndef __TAREN_ENUM_MACROS_H__
#define __TAREN_ENUM_MACROS_H__

#include <type_traits>
#include <limits>

/// \brief This header contains helper enum macros to all iterating over enum values and the string representation of them.
///        These enums provide similar functionality to Better Enums (https://github.com/aantron/better-enums) or C++ proposial N4428. 
///        Usage - First define the enum values with a define:
///               #define MyEnum_EnumValues(EV) \
///                         EV(Value1) \
///                         EV(Value2) \
///                         EV(value3)
///
///               Values can also be assigned to each enum:
///               #define MyEnumVal_EnumValues(EV) \
///                         EV(Value1) \
///                         EV(Value2, 1 << 8) \
///                         EV(value3, Value2 | Value1)
///
///         Then in a header file use the define like:
///                VALUE_ENUM(MyEnum, uint32_t) // second parameter is the base type of the enum
///         This expands to:
///                enum class MyEnum : uint32_t { ...Enum values... };
///         
///         If the enum is purely sequential, some optimizations can be made by using;
///                SEQUENTIAL_ENUM(MyEnum, uint32_t);
///
///         In a .cpp file a call to VALUE_ENUM_BODY(MyEnum) or SEQUENTIAL_ENUM_BODY(MyEnum) should be made.
///         If the enum is defined in a class or a namespace, use VALUE_ENUM_BODYSCOPE(MyEnum, ClassName) or SEQUENTIAL_ENUM_BODYSCOPE(MyEnum, ClassName)
///
///         Once defined, a MyEnumVal_Values type is available.
///         This can be used in code to:
///             - Convert values to strings: MyEnumVal_Values::to_string(Value1)
///             - Get the count of enum values MyEnumVal_Values::COUNT
///             - Iterate over the values/strings:
///                   for (auto val : MyEnumVal_Values()) {
///                       val.c_str() // Get string of enum
///                       val.value() // Get enum value
///          Using this code you can easily write code to convert from a string to an enum. 
///          (Not provided as every code base has different error handling and string requirements)
///
///     Finally, a macro is provided ENUM_FLAG_OPS(MyEnum) to generate the bitwise operations generally needed if the enum is a flag type. 
///     (Can be a plain enum, does not need to be defined by VALUE_ENUM)
///     Eg.   MyEnum bitOp1 = MyEnum::Bar | MyEnum::Baz;
///           MyEnum bitOp2 = MyEnum::Bar & MyEnum::Baz;
///           MyEnum bitOp3 = MyEnum::Bar ^ MyEnum::Baz;
///           MyEnum bitOp4 = ~MyEnum::Bar;
/// 

/// \brief Helper template for enums that are sequential
template<typename T>
class SequentialEnum
{
public:

  using ValueType = typename std::underlying_type<T>::type;

  static const size_t COUNT;      //!< The count of how many enum values there are
  static const char * const s_strValues[]; //!< The string values of each enum

  struct Value
  {
  public:

    inline T value() { return static_cast<T>(m_item); }
    inline const char* c_str() { return s_strValues[m_item]; }

  protected:
    ValueType m_item = 0; //!< The current iterator item type
  };
  
  struct Iterator : public Value
  {
    inline Iterator& operator++() { this->m_item++; return *this; }
    template <typename V>
    inline bool operator != (const V&) const { return this->m_item != COUNT; }
    inline Value& operator *() { return *this; }
  };

  static const char* to_string(T a_type) 
  {
    ValueType value = static_cast<ValueType>(a_type);
    if (value < COUNT)
    {
      return s_strValues[value];
    }
    return "";
  }

  inline Iterator begin() { return Iterator(); }
  inline Iterator end() { return Iterator(); }
};

/// \brief Helper template for enums that have assigned values
template<typename T>
class ValueEnum
{
public:

  using EnumType = T;
  using ValueType = typename std::underlying_type<T>::type;

  static const size_t COUNT;       //!< The count of how many enum values there are
  static const T s_values[];       //!< The array of enum values
  static const char * const s_strValues[]; //!< The string values of each enum

  struct Value
  {
  public:

    inline T value() { return s_values[m_index]; }
    inline const char* c_str() { return s_strValues[m_index]; }

  protected:
    size_t m_index = 0; //!< The current iterator index
  };

  struct Iterator : public Value
  {
    inline Iterator& operator++() { this->m_index++; return *this; }
    template <typename V>
    inline bool operator != (const V&) const { return this->m_index != COUNT; }
    inline Value& operator *() { return *this; }
  };

  static const char* to_string(T a_type)
  {
    for (size_t i = 0; i < COUNT; i++)
    {
      if (a_type == s_values[i])
      {
        return s_strValues[i];
      }
    }
    return "";
  }

  inline Iterator begin() { return Iterator(); }
  inline Iterator end() { return Iterator(); }
};

// Check that the enum values do not have any equals signs in them
constexpr bool TAREN_ENUM_BAD_STR_VALUE(const char *a_str)
{
  return (*a_str == 0) ? false : 
         (*a_str == '=') ? true :
         TAREN_ENUM_BAD_STR_VALUE(a_str + 1);
}

// Sequential enum definition
#define TAREN_MAKE_ENUM(VAR) VAR,
#define TAREN_MAKE_STRINGS(VAR) #VAR,
#define TAREN_MAKE_ENUM_CHECK(VAR, ...) #VAR

#define SEQUENTIAL_ENUM(NAME, TYPE)           \
  enum class NAME : TYPE{                     \
    NAME##_EnumValues(TAREN_MAKE_ENUM)        \
  };                                          \
  static_assert(!TAREN_ENUM_BAD_STR_VALUE(NAME##_EnumValues(TAREN_MAKE_ENUM_CHECK)), "Bad sequential enum value"); \
  using NAME##_Values = SequentialEnum<NAME>; \

#define TAREN_SEQUENTIAL_ENUM_INTERNAL(NAME, VALUES)  \
  template<> const char* const SequentialEnum<NAME>::s_strValues[] = { \
    VALUES(TAREN_MAKE_STRINGS)                        \
  };                                                  \
  template<> const size_t SequentialEnum<NAME>::COUNT = sizeof(SequentialEnum<NAME>::s_strValues) / sizeof(SequentialEnum<NAME>::s_strValues[0]);\

#define SEQUENTIAL_ENUM_BODY(NAME)  TAREN_SEQUENTIAL_ENUM_INTERNAL(NAME, NAME##_EnumValues)
#define SEQUENTIAL_ENUM_BODYSCOPE(NAME, SCOPE) TAREN_SEQUENTIAL_ENUM_INTERNAL(SCOPE::NAME, NAME##_EnumValues) 

// Value assigned enum definition
#define TAREN_MAKE_ENUM_MACRO_VAL(X) X
#define TAREN_MAKE_ENUM_FUNC_APPLY(FUNC, ...) TAREN_MAKE_ENUM_MACRO_VAL(FUNC(__VA_ARGS__))

#define TAREN_MAKE_ENUM_ARG_COUNT_IMPL(_1, _2, COUNT, ...) COUNT
#define TAREN_MAKE_ENUM_ARG_COUNTER(...) TAREN_MAKE_ENUM_MACRO_VAL(TAREN_MAKE_ENUM_ARG_COUNT_IMPL(__VA_ARGS__, 2, 1))

#define TAREN_MAKE_ENUM_VALUE1(VAR) VAR,
#define TAREN_MAKE_ENUM_VALUE2(VAR,VAL) VAR = (VAL),
#define TAREN_MAKE_ENUM_VALUE_COUNT(COUNT) TAREN_MAKE_ENUM_VALUE##COUNT

#define TAREN_MAKE_ENUM_VALUE(...) TAREN_MAKE_ENUM_MACRO_VAL(TAREN_MAKE_ENUM_FUNC_APPLY(TAREN_MAKE_ENUM_VALUE_COUNT, TAREN_MAKE_ENUM_ARG_COUNTER(__VA_ARGS__)) ( __VA_ARGS__))
#define TAREN_MAKE_ENUM_SET_VALUE(VAR,...) EnumType::VAR,
#define TAREN_MAKE_STRINGS_VALUE(VAR,...) #VAR,

#define VALUE_ENUM(NAME, TYPE)               \
  enum class NAME : TYPE{                    \
    NAME##_EnumValues(TAREN_MAKE_ENUM_VALUE) \
  };                                         \
  using NAME##_Values = ValueEnum<NAME>;     \
  static_assert(!TAREN_ENUM_BAD_STR_VALUE(NAME##_EnumValues(TAREN_MAKE_ENUM_CHECK)), "Bad enum value"); 

#define TAREN_VALUE_ENUM_BODY_INTERNAL(NAME, VALUES)                     \
  template<> const NAME ValueEnum<NAME>::s_values[] = {                  \
    VALUES(TAREN_MAKE_ENUM_SET_VALUE)                                    \
  };                                                                     \
  template<> const char* const ValueEnum<NAME>::s_strValues[] = {        \
    VALUES(TAREN_MAKE_STRINGS_VALUE)                                     \
  };                                                                     \
  template<> const size_t ValueEnum<NAME>::COUNT = sizeof(ValueEnum<NAME>::s_strValues) / sizeof(ValueEnum<NAME>::s_strValues[0]); \

#define VALUE_ENUM_BODY(NAME)  TAREN_VALUE_ENUM_BODY_INTERNAL(NAME, NAME##_EnumValues)
#define VALUE_ENUM_BODYSCOPE(NAME, SCOPE) TAREN_VALUE_ENUM_BODY_INTERNAL(SCOPE::NAME, NAME##_EnumValues) 

// Bit wise operations for enums that are flag values
#define ENUM_FLAG_OPS(NAME)                                   \
  static_assert(!std::numeric_limits<std::underlying_type<NAME>::type>::is_signed, "Unsigned enums only"); \
  constexpr inline NAME operator|(NAME a_lhs, NAME a_rhs)     \
  {                                                           \
    return NAME(std::underlying_type<NAME>::type(a_lhs) |     \
                std::underlying_type<NAME>::type(a_rhs));     \
  }                                                           \
                                                              \
  constexpr inline NAME operator&(NAME a_lhs, NAME a_rhs)     \
  {                                                           \
    return NAME(std::underlying_type<NAME>::type(a_lhs) &     \
                std::underlying_type<NAME>::type(a_rhs));     \
  }                                                           \
                                                              \
  constexpr inline NAME operator^(NAME a_lhs, NAME a_rhs)     \
  {                                                           \
    return NAME(std::underlying_type<NAME>::type(a_lhs) ^     \
                std::underlying_type<NAME>::type(a_rhs));     \
  }                                                           \
                                                              \
  constexpr inline NAME operator~(NAME a_lhs)                 \
  {                                                           \
    return NAME(~std::underlying_type<NAME>::type(a_lhs));    \
  }


#endif // !__TAREN_ENUM_MACROS_H__

