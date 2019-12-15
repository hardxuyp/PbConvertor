#ifndef TEST_H
#define TEST_H

#include <stdint.h>

#pragma pack(1)

namespace testNamespace
{

enum TestEnum : int
{
  enum0 = 0,
  enum1 = 1,
  enum2 = 2,
  enum3 = 3
};

struct TestGrandchildMessage
{
  double doubleField = 0.0;
  uint64_t repeatedStringLengthField = 0;
  char **repeatedStringField = NULL;
};

struct TestChildMessage
{
  int32_t int32Field = 0;
  char *stringField = NULL;
  TestGrandchildMessage testGrandchildMessageField;
};

struct TestMessage
{
  double doubleField = 0.0;
  float floatField = 0.0;
  int32_t int32Field = 0;
  int64_t int64Field = 0;
  uint32_t uint32Field = 0;
  uint64_t uint64Field = 0;
  bool boolField = false;
  char *stringField = NULL;
  TestEnum testEnumField = enum0;
  TestChildMessage testChildMessageField;
  uint64_t repeatedDoubleLengthField = 0;
  double *repeatedDoubleField = NULL;
  uint64_t repeatedStringLengthField = 0;
  char **repeatedStringField = NULL;
  uint64_t repeatedTestChildMessageLengthField = 0;
  TestChildMessage *repeatedTestChildMessageField = NULL;
};

}

#pragma pack()

#endif // TEST_H
