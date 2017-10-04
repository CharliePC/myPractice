#ifndef MUDUO_BASE_TYPES_H
#define MUDUO_BASE_TYPES_H

#include <stdint.h>
#ifdef MUDUO_STD_STRING
//使用标准string库
#include <string>
#else  // !MUDUO_STD_STRING
//扩展版string实现，目标是成为新std::string，提供优化后的计数引用和无计数引用
//并提供算法实现的优化版本，但由于ABI接口兼容性问题，没能成为std::string
#include <ext/vstring.h>
#include <ext/vstring_fwd.h>
#endif

//debug模式，使用断言
#ifndef NDEBUG
#include <assert.h>
#endif

namespace muduo
{

//标准string和C++扩展string类，两者不兼容!
#ifdef MUDUO_STD_STRING
using std::string;
#else  // !MUDUO_STD_STRING
typedef __gnu_cxx::__sso_string string;
#endif

// Use implicit_cast as a safe version of static_cast or const_cast
// for upcasting in the type hierarchy (i.e. casting a pointer to Foo
// to a pointer to SuperclassOfFoo or casting a pointer to Foo to
// a const pointer to Foo).
// When you use implicit_cast, the compiler checks that the cast is safe.
// Such explicit implicit_casts are necessary in surprisingly many
// situations where C++ demands an exact type match instead of an
// argument type convertable to a target type.
//
// The From type can be inferred, so the preferred syntax for using
// implicit_cast is the same as for static_cast etc.:
//
//   implicit_cast<ToType>(expr)
//
// implicit_cast would have been part of the C++ standard library,
// but the proposal was submitted too late.  It will probably make
// its way into the language in the future.
//父类指针引用转子类指针或引用，或者const转非const内容
template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

// When you upcast (that is, cast a pointer from type Foo to type
// SuperclassOfFoo), it's fine to use implicit_cast<>, since upcasts
// always succeed.  When you downcast (that is, cast a pointer from
// type Foo to type SubclassOfFoo), static_cast<> isn't safe, because
// how do you know the pointer is really of type SubclassOfFoo?  It
// could be a bare Foo, or of type DifferentSubclassOfFoo.  Thus,
// when you downcast, you should use this macro.  In debug mode, we
// use dynamic_cast<> to double-check the downcast is legal (we die
// if it's not).  In normal mode, we do the efficient static_cast<>
// instead.  Thus, it's important to test in debug mode to make sure
// the cast is legal!
//    This is the only place in the code we should use dynamic_cast<>.
// In particular, you SHOULDN'T be using dynamic_cast<> in order to
// do RTTI (eg code like this:
//    if (dynamic_cast<Subclass1>(foo)) HandleASubclass1Object(foo);
//    if (dynamic_cast<Subclass2>(foo)) HandleASubclass2Object(foo);
// You should design the code some other way not to need this.

//子类指针转父类
//1.dynamic_cast是运行时安全转换，可以保证安全
//2.但是dynamic_cast效率不高，在测试确认没问题后，可以改为static_cast
//3.使用模版来保证类型，这点可以考虑借鉴
template<typename To, typename From>     // use like this: down_cast<T*>(foo);
inline To down_cast(From* f)                     // so we only accept pointers
{
  // Ensures that To is a sub-type of From *.  This test is here only
  // for compile-time type checking, and has no overhead in an
  // optimized build at run-time, as it will be optimized away
  // completely.
  if (false)
  {
    implicit_cast<From*, To>(0);
  }

//对于dynamic_cast直接下断言!
//ifdef ifndef 后面只能跟一个macro,扩展性可能不如下面的实现
//#ifdef NDEBUG
//#ifdef xxxx
//#endif
//#endif
#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
  assert(f == NULL || dynamic_cast<To>(f) != NULL);  // RTTI: debug mode only!
#endif
//注意这里，直接无elif ，因为不需要，也是一种很风骚的实现
  return static_cast<To>(f);
}

}

#endif
