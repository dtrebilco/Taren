//=============================================================================
//  Copyright (C) 2016  Damian Trebilco
//  Licensed under the MIT license - See LICENSE.txt for details.
//=============================================================================
#ifndef __TAREN_ENUM_MACROS_H__
#define __TAREN_ENUM_MACROS_H__

#include  <type_traits>

template<typename T>
class SequentialEnum
{
public:

  using ValueType = typename std::underlying_type<T>::type;
  static const ValueType COUNT;      //!< The count of how many enum values there are
  static const char * s_strValues[];

  struct Value
  {
  public:

    inline T value() { return T(m_item); }
    inline const char* c_str() { return s_strValues[m_item]; }

  protected:
    ValueType m_item = 0;
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
    ValueType value = (ValueType)a_type;
    if (value < COUNT)
    {
      return s_strValues[value];
    }
    return "";
  }

  inline Iterator begin() { return Iterator(); }
  inline Iterator end() { return Iterator(); }

};


template<typename T>
class ValueEnum
{
public:

  using ValueType = typename std::underlying_type<T>::type;
  static const ValueType COUNT;       //!< The count of how many enum values there are
  static const ValueType s_values[];
  static const char * s_strValues[];

  struct Value
  {
  public:

    inline T value() { return T(s_values[m_index]); }
    inline const char* c_str() { return s_strValues[m_index]; }

  protected:
    ValueType m_index = 0;
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
    for (ValueType i = 0; i < COUNT; i++)
    {
      if (a_type == T(s_values[i]))
      {
        return s_strValues[i];
      }
    }
    return "";
  }

  inline Iterator begin() { return Iterator(); }
  inline Iterator end() { return Iterator(); }

};


#define TAREN_MAKE_ENUM(VAR) VAR,
#define TAREN_MAKE_STRINGS(VAR) #VAR,

#define TAREN_MAKE_ENUM_VALUE(VAR,VAL) VAR = (VAL),
#define TAREN_MAKE_ENUM_SET_VALUE(VAR,VAL) (VAL),
#define TAREN_MAKE_STRINGS_VALUE(VAR,VAL) #VAR,

#define SEQUENTIAL_ENUM(NAME, TYPE)           \
  enum class NAME : TYPE{                     \
    NAME##_EnumValues(TAREN_MAKE_ENUM)        \
  };                                          \
  using NAME##_Values = SequentialEnum<NAME>; \

#define TAREN_SEQUENTIAL_ENUM_INTERNAL(NAME, VALUES)  \
  template<> const char* SequentialEnum<NAME>::s_strValues[] = { \
    VALUES(TAREN_MAKE_STRINGS)                        \
  };                                                  \
  template<> const std::underlying_type<NAME>::type SequentialEnum<NAME>::COUNT = sizeof(SequentialEnum<NAME>::s_strValues) / sizeof(SequentialEnum<NAME>::s_strValues[0]);\

#define SEQUENTIAL_ENUM_BODY(NAME)  TAREN_SEQUENTIAL_ENUM_INTERNAL(NAME, NAME##_EnumValues)
#define SEQUENTIAL_ENUM_BODYSCOPE(NAME, SCOPE) TAREN_SEQUENTIAL_ENUM_INTERNAL(SCOPE::NAME, NAME##_EnumValues) 


#define VALUE_ENUM(NAME, TYPE)               \
  enum class NAME : TYPE{                    \
    NAME##_EnumValues(TAREN_MAKE_ENUM_VALUE) \
  };                                         \
  using NAME##_Values = ValueEnum<NAME>;     \

#define TAREN_VALUE_ENUM_BODY_INTERNAL(NAME, VALUES)                     \
  template<> const std::underlying_type<NAME>::type ValueEnum<NAME>::s_values[] = { \
    VALUES(TAREN_MAKE_ENUM_SET_VALUE)                                    \
  };                                                                     \
  template<> const char* ValueEnum<NAME>::s_strValues[] = {              \
    VALUES(TAREN_MAKE_STRINGS_VALUE)                                     \
  };                                                                     \
  template<> const std::underlying_type<NAME>::type ValueEnum<NAME>::COUNT = sizeof(ValueEnum<NAME>::s_strValues) / sizeof(ValueEnum<NAME>::s_strValues[0]);\

#define VALUE_ENUM_BODY(NAME)  TAREN_VALUE_ENUM_BODY_INTERNAL(NAME, NAME##_EnumValues)
#define VALUE_ENUM_BODYSCOPE(NAME, SCOPE) TAREN_VALUE_ENUM_BODY_INTERNAL(SCOPE::NAME, NAME##_EnumValues) 

#endif // !__TAREN_ENUM_MACROS_H__

