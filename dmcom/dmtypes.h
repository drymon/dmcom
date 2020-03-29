#ifndef __DMTYPES_H_
#define __DMTYPES_H_

#include <stdint.h>

typedef uint64_t (*dmclock_getts64_t)(void);
typedef void* (*dmem_alloc_t)(int size);
typedef void (*dmem_free_t)(void *mem);

#endif //__DMTYPES_H_
