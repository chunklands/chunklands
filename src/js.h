#ifndef __CHUNKLANDS_JS_H__
#define __CHUNKLANDS_JS_H__

#include <cassert>
#include <sstream>

#include "napi_import.h"

// solves comma problems: e.g. `MACRO({ InstanceMethod(...), ... })` -> `MACRO(ONE_ARG({ ... }))`
#define ONE_ARG(...) __VA_ARGS__


// Wrap

#define JS_DECL_WRAP(CLASS)                           public: \
                                                        static Napi::FunctionReference constructor; \
                                                        static void Initialize(Napi::Env env); \
                                                      public: \
                                                        CLASS(const Napi::CallbackInfo& info);

#define JS_DEF_INIT(CLASS, CLASS_DEF)                 Napi::FunctionReference CLASS::constructor; \
                                                      void CLASS::Initialize(Napi::Env env) { \
                                                        using JSCurrentWrap = CLASS; \
                                                        constructor = Napi::Persistent( \
                                                          DefineClass(env, #CLASS, CLASS_DEF) \
                                                        ); \
                                                        constructor.SuppressDestruct(); \
                                                      }

#define JS_DEF_WRAP(CLASS, CLASS_DEF)                 JS_DEF_INIT(CLASS, ONE_ARG(CLASS_DEF)) \
                                                      CLASS::CLASS(const Napi::CallbackInfo& info) \
                                                        : JSWrap<CLASS>(info) {}

#define JS_DECL_CB(WHAT)                              _JS_DECL_CB_TYPE(Napi::Value, WHAT)
#define JS_DECL_CB_VOID(WHAT)                         _JS_DECL_CB_TYPE(void, WHAT)
#define JS_DECL_CB_STATIC(WHAT)                       _JS_DECL_CB_TYPE_STATIC(Napi::Value, WHAT)
#define JS_DECL_CB_STATIC_VOID(WHAT)                  _JS_DECL_CB_TYPE_STATIC(void, WHAT)

#define JS_MODULE_WRAPEXPORT(ENV, CLASS)              do { \
                                                        CLASS::Initialize(ENV); \
                                                        exports[#CLASS] = CLASS::constructor.Value(); \
                                                      } while (0)

#define JS_DECL_SETTER_REF(TYPE, WHAT)                JS_DECL_CB_VOID(Set_##WHAT) \
                                                      private: JSWrapRef<TYPE> js_##WHAT;

#define JS_DECL_SETTER_OBJ(WHAT)                      JS_DECL_CB_VOID(Set_##WHAT) \
                                                      private: JSObjRef js_##WHAT;

#define JS_SETTER(WHAT)                               InstanceMethod("set" #WHAT, \
                                                        &JSCurrentWrap::_JS_CB_NAME(Set_##WHAT))

#define JS_CB(WHAT)                                   InstanceMethod(#WHAT, \
                                                        &JSCurrentWrap::_JS_CB_NAME(WHAT))

#define JS_CB_STATIC(WHAT)                            StaticMethod(#WHAT, \
                                                        &JSCurrentWrap::_JS_CB_NAME(WHAT))

#define JS_DEF_SETTER_JSREF(CLASS, WHAT)              void CLASS::_JS_CB_NAME(Set_##WHAT)\
                                                        (const Napi::CallbackInfo& info) { \
                                                          js_##WHAT = info[0]; \
                                                        }

#define JS_DEF_SETTER_JSOBJ(CLASS, WHAT)              void CLASS::_JS_CB_NAME(Set_##WHAT)\
                                                        (const Napi::CallbackInfo& info) {  \
                                                          js_##WHAT = Napi::Persistent(info[0].ToObject()); \
                                                        }

// Assert

#define JS_ASSERT(COND)                               JS_ASSERT_MSG(COND, "failed condition " #COND)

#define JS_ASSERT_MSG(COND, MSG)                      do { \
                                                        if (!(COND)) { \
                                                          JS_THROW(MSG); \
                                                        } \
                                                      } while (0)

// Error

#define JS_THROW(MSG)                                 throw create_error(env, (MSG))

// internal macros

#define _JS_CB_NAME(WHAT)                             JSCall_##WHAT

#define _JS_DECL_CB_TYPE(RETTYPE, WHAT)               private: RETTYPE _JS_CB_NAME(WHAT)\
                                                        (const Napi::CallbackInfo& info);

#define _JS_DECL_CB_TYPE_STATIC(RETTYPE, WHAT)        private: static RETTYPE _JS_CB_NAME(WHAT)\
                                                        (const Napi::CallbackInfo& info);


namespace chunklands {
  Napi::Error create_error(Napi::Env env, const std::string& msg);

  template<class BASE>
  using JSWrap = Napi::ObjectWrap<BASE>;

  using JSObjRef = Napi::ObjectReference;

  template <typename T>
  class JSWrapRef {
  public:
    JSWrapRef() {}

    JSWrapRef(Napi::Object object) {
      (*this) = object;
    }

    JSWrapRef(Napi::Value value) {
      (*this) = value;
    }

    ~JSWrapRef() {
      object_ = nullptr;
    }

    JSWrapRef& operator=(Napi::Value value) {
      return (*this) = value.ToObject();
    }

    JSWrapRef& operator=(Napi::Object object) {
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