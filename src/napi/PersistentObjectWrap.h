#ifndef __CHUNKLANDS_NAPI_PERSISTENTOBJECTWRAP_H__
#define __CHUNKLANDS_NAPI_PERSISTENTOBJECTWRAP_H__

#include <cassert>
#include "../napi_import.h"

namespace NapiExt {
  /**
   * To simply store and persist an object with no overhead on accessing wrapped object (no napi_unwrap calls).
   */
  template <typename T>
  class PersistentObjectWrap {
  public:
    PersistentObjectWrap() : object_(nullptr) {
    }

    PersistentObjectWrap(Napi::Object wrap) {
      ref_ = Napi::Persistent(wrap);
      object_ = Napi::ObjectWrap<T>::Unwrap(wrap);
    }

    operator napi_ref() {
      return ref_;
    }

    operator T*() {
      assert(object_ != nullptr);
      return object_;
    }

    T* operator->() {
      assert(object_ != nullptr);
      return object_;
    }

    T& operator*() {
      assert(object_ != nullptr);
      return *object_;
    }

    bool IsEmpty() const {
      return object_ == nullptr;
    }

  private:
    Napi::ObjectReference ref_;
    T* object_;
  };
}

#endif