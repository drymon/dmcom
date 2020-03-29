#ifndef __DMMEM_H_
#define __DMMEM_H_

#include <stdlib.h>

#ifndef DMEM_ALLOC
#define DMEM_ALLOC malloc
#endif

#ifndef DMEM_FREE
#define DMEM_FREE free
#endif

#if 0
#define DMMEM_HEAD_SIZE 8

struct dmmem_op {
	void (*alloc)(int size);
	void (*free)(void *mem);
	int alloc_size; //allocated size
};

void dmmem_init(struct dmmem_op *mop,
				void (*alloc)(int size), void (*free)(void *))
{
	mop->alloc = alloc;
	mop->free = free;
}

void *dmmem_alloc(struct dmmem_op *mop, int size)
{
	void *mem = NULL;
	
	if(mop->alloc){
		mem = mop->alloc(size+DMMEM_HEAD_SIZE);
		if(mem)
			mop->alloc_size += size;
	}
	return mem;
}

void dmmem_free(struct dmmem_op *mop, void *mem)
{
	if(!mem)
		return;
	if(mop->free){
		mop->free(mem);
		
	}
}
#endif

#endif //__DMMEM_H_
