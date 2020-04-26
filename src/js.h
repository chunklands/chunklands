#ifndef __CHUNKLANDS_JS_H__
#define __CHUNKLANDS_JS_H__

#include <cassert>
#include <sstream>

#include "napi_import.h"


// Wrap
#define JS_IMPL_WRAP(CLASS, CLASS_DEF)                public: \
                                                        static Napi::FunctionReference constructor; \
                                                        CLASS(JSCbi info) \
                                                          : JSObjectWrap<CLASS>(info) {} \
                                                        static void Initialize(Napi::Env env) { \
                                                          using JSCurrentWrap = CLASS; \
                                                          constructor = Napi::Persistent( \
                                                            DefineClass(env, #CLASS, CLASS_DEF) \
                                                          ); \
                                                          constructor.SuppressDestruct(); \
                                                        }

// Wrap - Class definition
#define JS_SETTER(WHAT)                               InstanceMethod("set" #WHAT, &JSCurrentWrap::JSCall_Set##WHAT)
#define JS_CB(WHAT)                                   InstanceMethod(      #WHAT, &JSCurrentWrap::JSCall_   ##WHAT)
#define JS_CB_STATIC(WHAT)                            StaticMethod(        #WHAT, &JSCurrentWrap::JSCall_   ##WHAT)

// Wrap - DECL
#define JS_DECL_SETTER_WRAP(TYPE, WHAT)               _JS_IMPL_SETTER_WRAP(TYPE, WHAT, ;)
#define JS_DECL_SETTER_OBJECT(WHAT)                   _JS_IMPL_SETTER_OBJECTECT(WHAT, ;)
#define JS_DECL_CB(WHAT)                              protected:        JSValue JSCall_##WHAT(JSCbi info);
#define JS_DECL_CB_VOID(WHAT)                         protected:        void    JSCall_##WHAT(JSCbi info);
#define JS_DECL_CB_STATIC(WHAT)                       protected: static JSValue JSCall_##WHAT(JSCbi info);
#define JS_DECL_CB_STATIC_VOID(WHAT)                  protected: static void    JSCall_##WHAT(JSCbi info);

// Wrap - DEF
#define JS_DEF_WRAP(CLASS)                            Napi::FunctionReference CLASS::constructor;
#define JS_DEF_SETTER_WRAP(CLASS, WHAT)               void CLASS::JSCall_Set##WHAT(JSCbi info) { \
                                                        js_##WHAT = info[0]; \
                                                      }
#define JS_DEF_SETTER_OBJECT(CLASS, WHAT)             void CLASS::JSCall_Set##WHAT(JSCbi info) {  \
                                                        js_##WHAT = Napi::Persistent(info[0].ToObject()); \
                                                      }

// Wrap - ATTR
#define JS_ATTR_WRAP(TYPE, WHAT)                      protected: \
                                                        JSWrapRef<TYPE> js_##WHAT;

// Wrap - IMPL
#define JS_IMPL_SETTER_WRAP(TYPE, WHAT)               _JS_IMPL_SETTER_WRAP(TYPE, WHAT, ONE_ARG({ \
                                                        js_##WHAT = info[0]; \
                                                      }))
#define JS_IMPL_SETTER_OBJECT(WHAT)                   _JS_IMPL_SETTER_OBJECT(WHAT, ONE_ARG({ \
                                                        js_##WHAT = Napi::Persistent(info[0].ToObject()); \
                                                      }))
#define _JS_IMPL_SETTER_WRAP(TYPE, WHAT, IMPL)        JS_ATTR_WRAP(TYPE, WHAT) \
                                                      protected: \
                                                        void JSCall_Set##WHAT(JSCbi info) IMPL
#define _JS_IMPL_SETTER_OBJECT(WHAT, IMPL)            protected: \
                                                        JSObjectRef js_##WHAT; \
                                                        void JSCall_Set##WHAT(JSCbi info) IMPL

// Wrap - MODULE
#define JS_MODULE_WRAPEXPORT(ENV, CLASS)              do { \
                                                        CLASS::Initialize(ENV); \
                                                        exports[#CLASS] = CLASS::constructor.Value(); \
                                                      } while (0)

// Assert
#define JS_ASSERT(COND)                               JS_ASSERT_MSG(COND, "failed condition " #COND)
#define JS_ASSERT_MSG(COND, MSG)                      do { \
                                                        if (!(COND)) { \
                                                          JS_THROW(MSG); \
                                                        } \
                                                      } while (0)

// Error
#define JS_THROW(MSG)                                 throw create_error(env, (MSG))


// solves comma problems: e.g. `MACRO({ InstanceMethod(...), ... })` -> `MACRO(ONE_ARG({ ... }))`
#define ONE_ARG(...) __VA_ARGS__

namespace chunklands {
  Napi::Error create_error(Napi::Env env, const std::string& msg);

  template<class BASE>
  using JSObjectWrap = Napi::ObjectWrap<BASE>;
  using JSObjectRef  = Napi::ObjectReference;
  using JSCbi        = const Napi::CallbackInfo&;
  using JSValue      = Napi::Value;
  using JSObject     = Napi::Object;

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