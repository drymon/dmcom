#ifndef __DMLIST_H_
#define __DMLIST_H_

#include <stdbool.h>
#include <stdlib.h>

struct dmnode {
	struct dmnode *next;
	struct dmnode *prev;
};

struct dmlist {
	struct dmnode anchor;
	int num_nodes;
};

static inline void dmlist_init(struct dmlist *list)
{
	list->anchor.next = &list->anchor;
	list->anchor.prev = &list->anchor;
	list->num_nodes = 0;
}

static inline void dmlist_add_tail(struct dmlist *list, struct dmnode *node)
{
	node->prev = list->anchor.prev;
	list->anchor.prev->next = node;
	node->next = &list->anchor;
	list->anchor.prev = node;
	list->num_nodes++;
}

static inline void dmlist_add_head(struct dmlist *list, struct dmnode *node)
{
	node->next = list->anchor.next;
	node->prev = &list->anchor;
	list->anchor.next->prev = node;
	list->anchor.next = node;
	list->num_nodes++;
}

static inline void dmlist_add_before(struct dmlist *list, struct dmnode *where, struct dmnode *node)
{
	node->prev = where->prev;
	node->next = where;
	where->prev->next = node;
	where->prev = node;
	list->num_nodes++;
}

static inline void dmlist_add_after(struct dmlist *list, struct dmnode *where, struct dmnode *node)
{
	node->prev = where;
	node->next = where->next;
	where->next->prev = node;
	where->next = node;
	list->num_nodes++;
}

static inline bool dmlist_is_empty(struct dmlist *list)
{
	return (list->num_nodes == 0);
}

static inline struct dmnode *dmlist_get_head(struct dmlist *list)
{
	if(dmlist_is_empty(list))
		return NULL;
	return list->anchor.next;
}

static inline struct dmnode *dmlist_get_tail(struct dmlist *list)
{
	if(dmlist_is_empty(list))
		return NULL;
	return list->anchor.prev;
}

static inline int dmlist_get_num_nodes(struct dmlist *list)
{
	return list->num_nodes;
}

static inline void dmlist_merge(struct dmlist *list, struct dmlist *merge)
{
	if(dmlist_is_empty(merge))
		return;
	
	list->anchor.prev->next = merge->anchor.next;
	merge->anchor.next->prev = list->anchor.prev;
	merge->anchor.prev->next = &list->anchor;
	list->anchor.prev = merge->anchor.next;
	list->num_nodes += merge->num_nodes;
	dmlist_init(merge);
}

static inline void dmlist_remove(struct dmlist *list, struct dmnode *node)
{
	node->next->prev = node->prev;
	node->prev->next = node->next;
	node->next = node->prev = NULL;
	list->num_nodes--;
}

#define DMLIST_ENTRY(ptr, type, member)\
	((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define DMLIST_FOREACH(list, node)\
	for(node=(list)->anchor.next; node!=&(list)->anchor; node=node->next)

#define DMLIST_FOREACH_SAFE(list, node, tmp)\
	for(node=(list)->anchor.next, tmp=node->next; node!=&(list)->anchor; node=tmp,tmp=node->next)

#endif /*__DMLIST_H_*/
