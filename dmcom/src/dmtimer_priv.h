#ifndef __DMTIMER_PRIV_H_
#define __DMTIMER_PRIV_H_

#include <stdint.h>

struct dmtimer_op { //Virtual functions for timer operation
	uint64_t (*gettime_ns)(void);
	void* (*malloc)(int size);
	void (*free)(void *mem);
};

void dmtimer_override_op(struct dmtimer_op *op);
void dmtimer_restore_op(void);

#endif //__DMTIMER_PRIV_H_
