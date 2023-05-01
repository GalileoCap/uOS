#ifndef __TEST_H__
#define __TEST_H__

#include <utils.h>

namespace std {

extern u64 FOOTEST;

template<typename T>
struct Test {
  Test(T value);

  T getValue() const;
  void setValue(T newValue);

  T _value;
};

}

#include "../test.ipp"

#endif // __TEST_H__
