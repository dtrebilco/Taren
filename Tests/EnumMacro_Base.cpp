#include "EnumMacro_Base.h"



SEQUENTIAL_ENUM_BODY(Test)
VALUE_ENUM_BODY(TestVal)
VALUE_ENUM_BODY(TestValDup)
VALUE_ENUM_BODY(TestFlags)

SEQUENTIAL_ENUM_BODYSCOPE(Test, ClassTest)
VALUE_ENUM_BODYSCOPE(TestVal, ClassTest)
VALUE_ENUM_BODYSCOPE(TestFlags, ClassTest)

SEQUENTIAL_ENUM_BODYSCOPE(Test, NameTest)
VALUE_ENUM_BODYSCOPE(TestVal, NameTest)
VALUE_ENUM_BODYSCOPE(TestFlags, NameTest)


