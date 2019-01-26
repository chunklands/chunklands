#ifndef __CHUNKLANDS_NAPI_OBJECT_WRAP_UTIL_H__
#define __CHUNKLANDS_NAPI_OBJECT_WRAP_UTIL_H__

// solves comma problems: e.g. `MACRO({ InstanceMethod(...), ... })` -> `MACRO(ONE_ARG({ ... }))`
#define ONE_ARG(...) __VA_ARGS__

#define DECLARE_OBJECT_WRAP_CB(SIGNATURE_WITHOUT_PARAMS) \
  private: SIGNATURE_WITHOUT_PARAMS(const Napi::CallbackInfo& info);

#define DECLARE_OBJECT_WRAP(CLASS)              \
  public:                                       \
    static Napi::FunctionReference constructor; \
    static void Initialize(Napi::Env env);      \
                                                \
  public:                                       \
    CLASS(const Napi::CallbackInfo& info);

#define DEFINE_OBJECT_WRAP(CLASS, CLASS_DEF)                                            \
  Napi::FunctionReference CLASS::constructor;                                           \
                                                                                        \
  void CLASS::Initialize(Napi::Env env) {                                               \
    constructor = Napi::Persistent(DefineClass(env, #CLASS, CLASS_DEF));                \
    constructor.SuppressDestruct();                                                     \
  }

#define DEFINE_OBJECT_WRAP_DEFAULT_CTOR(CLASS, CLASS_DEF)                         \
  DEFINE_OBJECT_WRAP(CLASS, ONE_ARG(CLASS_DEF))                                   \
  CLASS::CLASS(const Napi::CallbackInfo& info) : Napi::ObjectWrap<CLASS>(info) {}

#define EXPORTS_OBJECT_WRAP(ENV, CLASS)           \
  do {                                            \
    CLASS::Initialize(ENV);                       \
    exports[#CLASS] = CLASS::constructor.Value(); \
  } while (0)

#endif