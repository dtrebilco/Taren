
#ifndef __TAREN_ENUM_MACROS_BASE_H__
#define __TAREN_ENUM_MACROS_BASE_H__

#include "../EnumMacros.h"
#include <cstdint>


#define Test_EnumValues(EV) \
    EV(Foo) \
    EV(Bar) \
    EV(Baz)

#define TestVal_EnumValues(EV) \
    EV(Foo,2) \
    EV(Bar)   \
    EV(Baz,7) \
    EV(Single) //Single incrementing 

#define TestValDup_EnumValues(EV) \
    EV(Foo,2) \
    EV(Bar,6) \
    EV(Baz,7) \
    EV(Baz2,7) \

#define TestFlags_EnumValues(EV) \
    EV(Foo, 1 << 0) \
    EV(Bar, 1 << 1) \
    EV(Baz, 1 << 2) \
    EV(FooBaz, 1 << 3) \
    EV(All, Foo|Bar|Baz|FooBaz) 

SEQUENTIAL_ENUM(Test, uint32_t)

VALUE_ENUM(TestVal, uint32_t)
VALUE_ENUM(TestValDup, uint32_t)

VALUE_ENUM(TestFlags, uint32_t)
ENUM_FLAG_OPS(TestFlags)

class ClassTest
{
public:
  SEQUENTIAL_ENUM(Test, uint32_t)
  VALUE_ENUM(TestVal, uint32_t)
  VALUE_ENUM(TestFlags, uint32_t)
};

ENUM_FLAG_OPS(ClassTest::TestFlags)

namespace NameTest
{
  SEQUENTIAL_ENUM(Test, uint32_t)
  VALUE_ENUM(TestVal, uint32_t)
  VALUE_ENUM(TestFlags, uint32_t)
  ENUM_FLAG_OPS(TestFlags)
};

#endif // __TAREN_ENUM_MACROS_BASE_H__
