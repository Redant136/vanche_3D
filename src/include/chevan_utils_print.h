#ifndef CHEVAN_UTILS_PRINT_H
#define CHEVAN_UTILS_PRINT_H
#include <chevan_utils_min.h>

#define printVec2(v) printf("{%f, %f}\n", v.x, v.y)
#define printVec3(v) printf("{%f, %f, %f}\n", v.x, v.y, v.z)
#define printVec4(v) printf("{%f, %f, %f, %f}\n", v.x, v.y, v.z, v.w)

static void ch_printMat4(float mat[16])
{
  for (uint i = 0; i < 4; i++)
  {
    printf("%f, %f, %f, %f\n", mat[0 + i * 4], mat[1 + i * 4], mat[2 + i * 4], mat[3 + i * 4]);
  }
}
static void ch_printSep()
{
  printf("---------------------------------\n");
}
static void ch_printBits_ptr(void *e, size_t _elementSize)
{
  for (uint i = 0; i < _elementSize * 8; i++)
  {
    printf("%d", (((uchar *)e)[i / 8] & (1 << i % 8)) > 0 ? 1 : 0);
  }
  printf("\n");
}
#define ch_printBits(e) ch_printBits_ptr(&(e), sizeof((e)))

static void chevanut_print_str(const char *s)
{
  printf("%s", s);
}
static void chevanut_print_char(const char c) { printf("%c", c); }
static void chevanut_print_uchar(const uchar c) { printf("%c", c); }
#define _chevanut_print_macro(type)                                    \
  static void chevanut_print_##type(const type t) { printf("%d", t); } \
  static void chevanut_print_u##type(const type t) { printf("%d", t); }
_chevanut_print_macro(short);
_chevanut_print_macro(int);
#undef _chevanut_print_macro
static void chevanut_print_long(const long t)
{
  printf("%ld", t);
}
static void chevanut_print_ulong(const long t) { printf("%ld", t); }
static void chevanut_print_llong(const llong t) { printf("%lld", t); }
static void chevanut_print_ullong(const llong t) { printf("%lld", t); }

static void chevanut_print_float(const float f)
{
  printf("%f", f);
}
static void chevanut_print_double(const double f) { printf("%f", f); }
static void chevanut_print_ptr(const void *p) { printf("%p", p); }
static void chevanut_print_8dir(enum Cardinal8dir dir)
{
  switch (dir)
  {
  case CENTER:
    printf("CENTER\n");
    break;
  case NORTH:
    printf("NORTH\n");
    break;
  case SOUTH:
    printf("SOUTH\n");
    break;
  case EAST:
    printf("EAST\n");
    break;
  case WEST:
    printf("WEST\n");
    break;
  case NORTH_EAST:
    printf("NORTH_EAST\n");
    break;
  case NORTH_WEST:
    printf("NORTH_WEST\n");
    break;
  case SOUTH_EAST:
    printf("SOUTH_EAST\n");
    break;
  case SOUTH_WEST:
    printf("SOUTH_WEST\n");
    break;
  default:
    break;
  }
}
#ifdef CHEVAN_UTILS_VEC2
static void chevanut_print_vec2(CHEVAN_UTILS_VEC2 v) { printf("{%f, %f}", v.x, v.y); }
#endif
#ifdef CHEVAN_UTILS_VEC3
static void chevanut_print_vec3(CHEVAN_UTILS_VEC3 v) { printf("{%f, %f, %f}", v.x, v.y, v.z); }
#endif
#ifdef CHEVAN_UTILS_VEC3
static void chevanut_print_vec4(CHEVAN_UTILS_VEC4 v) { printf("{%f, %f, %f, %f}", v.x, v.y, v.z, v.w); }
#endif
#ifdef CHEVAN_UTILS_MATH_H
static void chevanut_print_chevanutVec2(struct vec2 v) { printf("{%f, %f}", v.x, v.y); }
static void chevanut_print_chevanutVec3(struct vec3 v) { printf("{%f, %f, %f}", v.x, v.y, v.z); }
static void chevanut_print_chevanutVec4(struct vec4 v) { printf("{%f, %f, %f, %f}", v.x, v.y, v.z, v.w); }
static void chevanut_print_ivec2(struct ivec2 v) { printf("{%d, %d}", v.x, v.y); }
static void chevanut_print_ivec3(struct ivec3 v) { printf("{%d, %d, %d}", v.x, v.y, v.z); }
static void chevanut_print_ivec4(struct ivec4 v) { printf("{%d, %d, %d, %d}", v.x, v.y, v.z, v.w); }
#define CHEVANUT_VEC_PRINT                                                                                       \
  CHEVAN_UTILS_VEC2:                                                                                             \
  chevanut_print_vec2,                                                                                           \
      CHEVAN_UTILS_VEC3 : chevanut_print_vec3,                                                                   \
                          CHEVAN_UTILS_VEC4 : chevanut_print_vec4,                                               \
                                              struct ivec2 : chevanut_print_ivec2,                               \
                                                             struct ivec3 : chevanut_print_ivec3,                \
                                                                            struct ivec4 : chevanut_print_ivec4,
#endif
#ifdef CHEVAN_UTILS_ARR_H
#define ch_println_struct_Array(type, arr)        \
  if (1)                                          \
  {                                               \
    ch_print("{");                                \
    for (uint _i = 0; _i < arr.length; _i++)      \
    {                                             \
      ch_print(array_get(type, arr, _i));         \
      ch_print((_i < arr.length - 1) ? "," : ""); \
    }                                             \
    ch_println("}");                              \
  }
#define ch_println_array(length, arr)         \
  if (1)                                      \
  {                                           \
    ch_print("{");                            \
    for (uint _i = 0; _i < length; _i++)      \
    {                                         \
      ch_print(arr[_i]);                      \
      ch_print((_i < length - 1) ? "," : ""); \
    }                                         \
    ch_println("}");                          \
  }
#endif
#ifdef CHEVAN_UTILS_MACRO_MAGIC
#define _chevanut_println_recurse_MAP(x) ch_print(x);
#define ch_print_recurse(...) EVAL(MAP(_chevanut_println_recurse_MAP, __VA_ARGS__))
#define ch_println_recurse(...) EVAL(MAP(_chevanut_println_recurse_MAP, __VA_ARGS__, "\n"))
#endif

#define chprint(x) _Generic((x),  \
    char *: chevanut_print_str,    \
    uchar: chevanut_print_uchar,   \
    char: chevanut_print_char,     \
    ushort: chevanut_print_ushort, \
    short: chevanut_print_short,   \
    uint: chevanut_print_uint,     \
    int: chevanut_print_int,       \
    ulong: chevanut_print_ulong,   \
    long: chevanut_print_long,     \
    ullong: chevanut_print_ullong, \
    llong: chevanut_print_llong,   \
    float: chevanut_print_float,   \
    double: chevanut_print_double, \
    void *: chevanut_print_ptr,    \
    CHEVANUT_VEC_PRINT\
    default: chevanut_print_ptr)(x)

#define chprintln(x) \
  ch_print(x);        \
  printf("\n")
#endif