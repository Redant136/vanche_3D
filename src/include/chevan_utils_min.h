#pragma once
#ifndef CHEVAN_UTILS_MIN_H
#define CHEVAN_UTILS_MIN_H
#define CHEVAN_UTILS_VERSION "3.0.0"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define CHEVAN_UTILS_INLINE inline
#define CHEVAN_VAL 0x86AC
#define membuild(type, name, data) \
  type name;                       \
  memcpy(&name, data, sizeof(name));
#define mallocArr(type, size) malloc(sizeof(type) * (size))
#define sizeofArr(arr) (sizeof(arr) / sizeof(arr[0]))
#define mallocArr(type, size) malloc(sizeof(type) * (size)) // calloc technically slower as sets all bytes to 0
#define fcompare(a, b) ((a - b) < 0.0001 && (b - a) < 0.0001)
#define USER_NOT_IMPLEMENTED_ERROR(usr) assert(0 && "this has yet to be implemented. Please ask " #usr " to create it")
#define AC_NOT_IMPLEMENTED_ERROR assert(0 && "this has yet to be implemented. Please kindly ask Antoine Chevalier to get off his ass and get to work")
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832
#endif

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;
typedef long long llong;

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
enum Cardinal8dir
{
  CENTER,
  NORTH,
  SOUTH,
  EAST,
  WEST,
  NORTH_EAST,
  NORTH_WEST,
  SOUTH_EAST,
  SOUTH_WEST
};


typedef struct color3_t
{
  union
  {
    struct
    {
      union
      {
        uint8_t x, r;
      };
      union
      {
        uint8_t y, g;
      };
      union
      {
        uint8_t z, b;
      };
    };
    struct
    {
      uint8_t r, g, b;
    } rgb;
    struct
    { 
      uint8_t y, cb, cr;
    } ycc;
  };
} color3_t;
typedef struct color4_t
{
  union
  {
    uint8_t x, r;
  };
  union
  {
    uint8_t y, g;
  };
  union
  {
    uint8_t z, b;
  };
  union
  {
    uint8_t w, a;
  };
} color4_t;
static color3_t initColor3(uint8_t x, uint8_t y, uint8_t z)
{
  color3_t c = {x, y, z};
  return c;
}
#define color3_t(x, y, z) initColor3(x, y, z)
static color4_t initColor4(uint8_t x, uint8_t y, uint8_t z, uint8_t w)
{
  color4_t c = {x, y, z, w};
  return c;
}
#define color4_t(x, y, z, w) initColor4(x, y, z, w)

#ifdef CHEVAN_UTILS_BYTE_TYPEDEF
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
#endif

#endif