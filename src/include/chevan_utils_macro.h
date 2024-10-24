#ifndef CHEVAN_UTILS_MACRO_H
#define CHEVAN_UTILS_MACRO_H

#ifdef _WIN32
#error MSVC does not expand macros properly
#endif
// Some c pre processor fuckery that allows to have loops & recursion in the pre processor

#define FIRST(a, ...) a
#define SECOND(a, b, ...) b
#define CONCAT(a, b) a##b
#define PASS(...) __VA_ARGS__
#define CLEAR(...)
#define EMPTY()

#define _DEFER1(m) m EMPTY()
#define _DEFER2(m) m EMPTY EMPTY()()
#define _DEFER3(m) m EMPTY EMPTY EMPTY()()()
#define _DEFER4(m) m EMPTY EMPTY EMPTY EMPTY()()()()

#define _PROBE(...) SECOND(__VA_ARGS__, 1)
#define _CHECK_PROBE() ~, 0
#define _CHECK_PROBE0() ~, 0

#define _IF_ELSE(C) CONCAT(_IF_ELSE, C)
#define _IF_ELSE1(...) __VA_ARGS__ CLEAR
#define _IF_ELSE0(...) PASS

#define _EVAL1024(...) _EVAL512(_EVAL512(__VA_ARGS__))
#define _EVAL512(...) _EVAL256(_EVAL256(__VA_ARGS__))
#define _EVAL256(...) _EVAL128(_EVAL128(__VA_ARGS__))
#define _EVAL128(...) _EVAL64(_EVAL64(__VA_ARGS__))
#define _EVAL64(...) _EVAL32(_EVAL32(__VA_ARGS__))
#define _EVAL32(...) _EVAL16(_EVAL16(__VA_ARGS__))
#define _EVAL16(...) _EVAL8(_EVAL8(__VA_ARGS__))
#define _EVAL8(...) _EVAL4(_EVAL4(__VA_ARGS__))
#define _EVAL4(...) _EVAL2(_EVAL2(__VA_ARGS__))
#define _EVAL2(...) _EVAL1(_EVAL1(__VA_ARGS__))
#define _EVAL1(...) __VA_ARGS__

#define _END_OF_ARGUMENTS_() 0

// returns 0 if 0 or empty, 1 otherwise
#define BOOL(A,...) _PROBE(CONCAT(_CHECK_PROBE,A)())
#define EVAL(...) _EVAL1024(__VA_ARGS__)
// how to use: IF_ELSE(1)(smt)(smt)
#define IF_ELSE(C) _IF_ELSE(BOOL(C))
#define HAS_ARGS(...) BOOL(FIRST(_END_OF_ARGUMENTS_ __VA_ARGS__)())

#define _FOR_EACH1(F, A, ...) F(A) _IF_ELSE(HAS_ARGS(__VA_ARGS__))(_DEFER2(_FOR_EACH_)(1)(F, __VA_ARGS__))(EMPTY())
#define _FOR_EACH2(F, A, B, ...) F(A, B) _IF_ELSE(HAS_ARGS(__VA_ARGS__))(_DEFER2(_FOR_EACH_)(2)(F, __VA_ARGS__))(EMPTY())
#define _FOR_EACH_(N) _FOR_EACH##N

// applies F for each element. i.e. F(a) F(b) ...
#define FOR_EACH(F, ...) EVAL(_FOR_EACH_(1)(F, __VA_ARGS__))
// n is the number of arguments consumed i.e. F(a,b) F(c,d)
#define FOR_EACH_N(N, F, ...) EVAL(_FOR_EACH_(N)(F, __VA_ARGS__))

#define _ENUM_PARSE_N(val, loc, A, ...) \
  _IF_ELSE(BOOL(__VA_ARGS__))    \
  ((!strcmp(val, #A) ?\
     loc A :\
     _DEFER2(_ENUM_PARSE_N_)()(val, loc, __VA_ARGS__)))\
  (loc A)
#define _ENUM_PARSE_N_() _ENUM_PARSE_N

#define _ENUM_TOSTR_N(val, loc, A, ...) \
  _IF_ELSE(BOOL(FIRST(__VA_ARGS__)))    \
  ((val == loc A ? #A : _DEFER2(_ENUM_TOSTR_N_)()(val, loc, __VA_ARGS__)))(#A)
#define _ENUM_TOSTR_N_() _ENUM_TOSTR_N

#define CH_ENUM_PARSE(val,loc, ...) (EVAL(_ENUM_PARSE_N(val,loc, __VA_ARGS__,0)))
#define CH_ENUM_TOSTR(val, loc, ...) (EVAL(_ENUM_TOSTR_N(val, loc, __VA_ARGS__,0)))

#endif