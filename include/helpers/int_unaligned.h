/*
 * Read and write integers to unaligned memory in either byte order
 */

#ifndef INT_UNALIGNED_H_
#define INT_UNALIGNED_H_

#include <stdint.h>

#define INT_UNALIGNED(bits) \
static inline uint##bits##_t __attribute__((optimize("O3"))) \
unaligned_read_be##bits (const uint8_t *ptr) { \
  uint##bits##_t v = 0; \
  for (int i=0;;i++) { \
    v |= *ptr; \
    if (i==bits/8-1) break; \
    v<<=8; ptr++; \
  } \
  return v; \
} \
static inline void __attribute__((optimize("O3"))) \
unaligned_write_be##bits (uint8_t *ptr, uint##bits##_t v) { \
  for (int i=0;;i++) { \
    *(ptr+bits/8-i-1) = v; \
    if (i==bits/8-1) break; \
    v>>=8; \
  } \
} \
static inline uint##bits##_t __attribute__((optimize("O3"))) \
unaligned_read_le##bits (const uint8_t *ptr) { \
  uint##bits##_t v = 0; \
  for (int i=0;;i++) { \
    v |= (uint##bits##_t)(*(ptr+i)) << (i*8); \
    if (i==bits/8-1) break; \
  } \
  return v; \
} \
static inline void __attribute__((optimize("O3"))) \
unaligned_write_le##bits (uint8_t *ptr, uint##bits##_t v) { \
  for (int i=0;;i++) { \
    *(ptr+i) = v; \
    if (i==bits/8-1) break; \
    v>>=8; \
  } \
}
INT_UNALIGNED(16)
INT_UNALIGNED(32)
INT_UNALIGNED(64)
#undef INT_UNALIGNED

#endif

