#ifndef __DMLIST_H_
#define __DMLIST_H_

struct dmlist {
	struct dmlist *next;
	struct dmlist *prev;
};

#define DMLIST_NEW(list)\
	struct dmlist list = {\
		.next = &list,\
		.prev = &list\
	}

#define DMLIST_INIT(list){\
	(list)->next = list;\
	(list)->prev = list;\
}

#define DMLIST_ADD_TAIL(list, node){\
	(node)->prev = (list)->prev;\
	(list)->prev->next = node;\
	(node)->next = list;\
	(list)->prev = node;\
}

#define DMLIST_ADD_HEAD(list, node){\
	(node)->next = (list)->next;\
	(node)->prev = list;\
	(list)->next->prev = node;\
	(list)->next = node;\
}

#define DMLIST_ADD_BEFORE(where, node){\
	(node)->prev = (where)->prev;\
	(node)->next = where;\
	(where)->prev->next = node;\
	(where)->prev = node;\
}

#define DMLIST_ADD_AFTER(where, node){\
	(node)->prev = where;\
	(node)->next = (where)->next;\
	(where)->next->prev = node;\
	(where)->next = node;\
}

#define DMLIST_MERGE(list, merge){\
	if(!DMLIST_IS_EMPTY(merge)){\
		(list)->prev->next = (merge)->next;\
		(merge)->next->prev = (list)->prev;\
		(merge)->prev->next = list;\
		(list)->prev = (merge)->next;\
		DMLIST_INIT(merge);\
	}\
}

#define DMLIST_REMOVE(node){\
	(node)->next->prev = (node)->prev;\
	(node)->prev->next = (node)->next;\
	(node)->next = (node)->prev = NULL;\
}

#define DMLIST_HEAD(list) ((list)->next)

#define DMLIST_TAIL(list) ((list)->prev)

#define DMLIST_IS_EMPTY(list) ((list)->next==(list))

#define DMLIST_ENTRY(ptr, type, member)\
	((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define DMLIST_FOREACH(list, node)\
	for(node=(list)->next; node!=(list); node=node->next)

#define DMLIST_FOREACH_SAFE(list, node, tmp)\
	for(node=(list)->next, tmp=node->next; node!=(list); node=tmp, tmp=node->next)

#endif /*__DMLIST_H_*/
