#include "dmevent_reactor.h"
#include "dmlist.h"
#include "dmevent_priv.h"

static dmem_alloc_t g_malloc = malloc;
static dmem_free_t g_free = free;

struct dmevent_reactor {
	dmlist pend_add_list;
	dmlist pend_rm_list;
	dmlist ready_list;
};

struct dmevent_reactor *dmevent_reactor_create(void)
{
	struct dmevent_reactor * reactor = g_malloc(sizeof *reactor);
	if(!reactor)
		return NULL;
	memset(reactor, 0, sizeof *reactor);
	dmlist_init(&reactor->pend_add_list);
	dmlist_init(&reactor->pend_rm_list);
	dmlist_init(&reactor->ready_list);

	return reactor;
}

void dmevent_reactor_destroy(struct dmevent_reactor *reactor)
{

}

int dmevent_reactor_handle_events(struct dmevent_reactor *reactor, int timeout_us)
{
	int num_events;
	
	if(!reactor)
		return DM_E_PARAM;

	if(!dmlist_is_empty(&reactor->pend_add_list)){
		dmlist_merge(&reactor->ready_list, &reactor->pend_add_list);
	}
	if(!dmlist_is_empty(&reactor->pend_rm_list)){
		//TODO: remove
	}
	num_events = wait_events_ready(&reactor->ready_list, timeout_us);
	if(num_events <= 0)
		return num_events;
	
	return DM_E_OK;
}
