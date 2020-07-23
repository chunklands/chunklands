#ifndef __CHUNKLANDS_JSEXPORT_HXX__
#define __CHUNKLANDS_JSEXPORT_HXX__

#define JS_EXPORT(CLAZZ) exports[#CLAZZ] = CLAZZ::Initialize(env)

#endif