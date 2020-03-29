#ifndef __DMTIMER_PRIV_H_
#define __DMTIMER_PRIV_H_

#include <stdint.h>
#include "dmtypes.h"

void dmtimer_set_clock_getts64(dmclock_getts64_t clock_getts64);
void dmtimer_set_malloc(dmem_alloc_t malloc);
void dmtimer_set_free(dmem_free_t free);
void dmtimer_set_clock_getts64_default(void);
void dmtimer_set_malloc_default(void);
void dmtimer_set_free_default(void);

#endif //__DMTIMER_PRIV_H_

