#ifndef SUPERSTRING_NUMBER_CONVERSION_H
#define SUPERSTRING_NUMBER_CONVERSION_H

#include "nan.h"
#include "optional.h"

namespace number_conversion {
  template<typename T>
  optional<T> number_from_js(v8::Local<v8::Value> js_value) {
    v8::Local<v8::Number> js_number;
    if (Nan::To<v8::Number>(js_value).ToLocal(&js_number)) {
      return js_number->NumberValue();
    } else {
      return optional<T>{};
    }
  }
}

#endif // SUPERSTRING_NUMBER_CONVERSION_H
