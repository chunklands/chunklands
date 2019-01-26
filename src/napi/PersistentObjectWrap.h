#ifndef __CHUNKLANDS_NAPI_PERSISTENTOBJECTWRAP_H__
#define __CHUNKLANDS_NAPI_PERSISTENTOBJECTWRAP_H__

#include <napi.h>

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
      return object_;
    }

    T* operator->() {
      return object_;
    }

    T& operator*() {
      return *object_;
    }

  private:
    Napi::ObjectReference ref_;
    T* object_;
  };
}

#endif