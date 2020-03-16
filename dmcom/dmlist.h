#ifndef __DMLIST_H_
#define __DMLIST_H_

#include <stdbool.h>
#include <stdlib.h>

struct dmnode {
	struct dmnode *next;
	struct dmnode *prev;
};

struct dmlist {
	struct dmnode node;
	int nelems;
};

static inline void dmlist_init(struct dmlist *list)
{
	list->node.next = &list->node;
	list->node.prev = &list->node;
	list->nelems = 0;
}

static inline void dmlist_add_tail(struct dmlist *list, struct dmnode *node)
{
	node->prev = list->node.prev;
	list->node.prev->next = node;
	node->next = &list->node;
	list->node.prev = node;
	list->nelems++;
}

static inline void dmlist_add_head(struct dmlist *list, struct dmnode *node)
{
	node->next = list->node.next;
	node->prev = &list->node;
	list->node.next->prev = node;
	list->node.next = node;
	list->nelems++;
}

static inline void dmlist_add_before(struct dmlist *list, struct dmnode *where, struct dmnode *node)
{
	node->prev = where->prev;
	node->next = where;
	where->prev->next = node;
	where->prev = node;
	list->nelems++;
}

static inline void dmlist_add_after(struct dmlist *list, struct dmnode *where, struct dmnode *node)
{
	node->prev = where;
	node->next = where->next;
	where->next->prev = node;
	where->next = node;
	list->nelems++;
}

static inline bool dmlist_is_empty(struct dmlist *list)
{
	return (list->nelems == 0);
}

static inline struct dmnode *dmlist_get_head(struct dmlist *list)
{
	if(dmlist_is_empty(list))
		return NULL;
	return list->node.next;
}

static inline struct dmnode *dmlist_get_tail(struct dmlist *list)
{
	if(dmlist_is_empty(list))
		return NULL;
	return list->node.prev;
}

static inline int dmlist_get_nelems(struct dmlist *list)
{
	return list->nelems;
}

static inline void dmlist_merge(struct dmlist *list, struct dmlist *merge)
{
	if(dmlist_is_empty(merge))
		return;
	
	list->node.prev->next = merge->node.next;
	merge->node.next->prev = list->node.prev;
	merge->node.prev->next = &list->node;
	list->node.prev = merge->node.next;
	list->nelems += merge->nelems;
	dmlist_init(merge);
}

static inline void dmlist_remove(struct dmlist *list, struct dmnode *node)
{
	node->next->prev = node->prev;
	node->prev->next = node->next;
	node->next = node->prev = NULL;
	list->nelems--;
}

#define DMLIST_ENTRY(ptr, type, member)\
	((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define DMLIST_FOREACH(list, node)\
	for(node=(list)->node.next; node!=&(list)->node; node=node->next)

#define DMLIST_FOREACH_SAFE(list, node, tmp)\
	for(node=(list)->node.next, tmp=node->next; node!=&(list)->node; node=tmp,tmp=node->next)

#endif /*__DMLIST_H_*/
