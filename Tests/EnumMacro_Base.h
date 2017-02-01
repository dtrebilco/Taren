
#include "../EnumMacros.h"
#include <cstdint>


#define Test_EnumValues(EV) \
    EV(Foo) \
    EV(Bar) \
    EV(Baz)


#define TestVal_EnumValues(EV) \
    EV(Foo,2) \
    EV(Bar,6) \
    EV(Baz,7)


#define TestValDup_EnumValues(EV) \
    EV(Foo,2) \
    EV(Bar,6) \
    EV(Baz,7) \
    EV(Baz2,7) \


SEQUENTIAL_ENUM(Test, uint32_t)

VALUE_ENUM(TestVal, uint32_t)
VALUE_ENUM(TestValDup, uint32_t)


class ClassTest
{
public:
  SEQUENTIAL_ENUM(Test, uint32_t)
  VALUE_ENUM(TestVal, uint32_t)
};

namespace NameTest
{
  SEQUENTIAL_ENUM(Test, uint32_t)
  VALUE_ENUM(TestVal, uint32_t)
};





