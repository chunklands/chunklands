#ifndef __CHUNKLANDS_JS_H__
#define __CHUNKLANDS_JS_H__

#include <cassert>
#include <napi.h>
#include <boost/current_function.hpp>
#include <boost/preprocessor/stringize.hpp>

// solves comma problems: e.g. `MACRO({ InstanceMethod(...), ... })` -> `MACRO(ONE_ARG({ ... }))`
#define ONE_ARG(...) __VA_ARGS__

#define _JS_CB_NAME(WHAT) JSCall_##WHAT

#define JS_DECL_CB(SIGNATURE_WITHOUT_PARAMS) \
  private: SIGNATURE_WITHOUT_PARAMS(const Napi::CallbackInfo& info);

#define JS_DECL_WRAP(CLASS)                     \
  public:                                       \
    static Napi::FunctionReference constructor; \
    static void Initialize(Napi::Env env);      \
                                                \
  public:                                       \
    CLASS(const Napi::CallbackInfo& info);

#define JS_DEF_INIT(CLASS, CLASS_DEF)                                                   \
  Napi::FunctionReference CLASS::constructor;                                           \
                                                                                        \
  void CLASS::Initialize(Napi::Env env) {                                               \
    using JSCurrentWrap = CLASS;                                                        \
    constructor = Napi::Persistent(DefineClass(env, #CLASS, CLASS_DEF));                \
    constructor.SuppressDestruct();                                                     \
  }

#define JS_DEF_WRAP(CLASS, CLASS_DEF)                                             \
  JS_DEF_INIT(CLASS, ONE_ARG(CLASS_DEF))                                                 \
  CLASS::CLASS(const Napi::CallbackInfo& info) : JSWrap<CLASS>(info) {}

#define JS_INIT_WRAP(ENV, CLASS)                  \
  do {                                            \
    CLASS::Initialize(ENV);                       \
    exports[#CLASS] = CLASS::constructor.Value(); \
  } while (0)

#define JS_DECL_SETTER(TYPE, WHAT)                                \
  JS_CB_DECL(Set_##WHAT)                                      \
  private: JSRef<TYPE> ref_##WHAT##_;

#define JS_SETTER(WHAT) InstanceMethod("set" #WHAT, &JSCurrentWrap::_JS_CB_NAME(Set_##WHAT))

#define JS_CB_DECL(WHAT) \
  private: Napi::Value _JS_CB_NAME(WHAT)(const Napi::CallbackInfo& info);
#define JS_CB(WHAT) InstanceMethod(#WHAT, &JSCurrentWrap::_JS_CB_NAME(WHAT))


#define JS_DEF_SETTER(CLASS, WHAT)                           \
  Napi::Value CLASS::_JS_CB_NAME(Set_##WHAT)(const Napi::CallbackInfo& info) {  \
    ref_##WHAT##_ = info[0];                                \
  }

#define JS_THROW(ENV, MSG)                \
  do {                                    \
    std::string msg;                      \
    msg += __FILE__;                      \
    msg += ":";                           \
    msg += BOOST_PP_STRINGIZE(__LINE__);  \
    msg += " at ";                        \
    msg += BOOST_CURRENT_FUNCTION;        \
    msg += ": ";                          \
    msg += (MSG);                         \
    throw Napi::Error::New((ENV), msg);   \
  } while (0)

namespace chunklands {
  template<class BASE>
  using JSWrap = Napi::ObjectWrap<BASE>;

  template <typename T>
  class JSRef {
  public:
    JSRef() {}

    JSRef(Napi::Object object) {
      (*this) = object;
    }

    JSRef(Napi::Value value) {
      (*this) = value;
    }

    ~JSRef() {
      object_ = nullptr;
    }

    JSRef& operator=(Napi::Value value) {
      return (*this) = value.ToObject();
    }

    JSRef& operator=(Napi::Object object) {
      ref_    = Napi::Persistent(object);
      object_ = Napi::ObjectWrap<T>::Unwrap(object);
      return *this;
    }

    operator napi_ref() {
      return ref_;
    }

    operator T*() {
      assert(object_ != nullptr);
      return object_;
    }

    const T* operator->() const {
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

    const T& operator*() const {
      assert(object_ != nullptr);
      return *object_;
    }

    bool IsEmpty() const {
      return object_ == nullptr;
    }

  private:
    Napi::ObjectReference ref_;
    T* object_ = nullptr;
  };
}

#endif