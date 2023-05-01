#include "test.h"

namespace std {

u64 FOOTEST = 0x600DB007F337;

template<typename T>
Test<T>::Test(T value) : _value(value) {}

template<typename T>
T Test<T>::getValue() const {
  return _value + 1;
}

template<typename T>
void Test<T>::setValue(T newValue) {
  _value = newValue;
}

}
