#include <setjmp.h>
#include <stdio.h>
#include "dmcom/dmlist.h"
#include <cmocka.h>

struct node_test {
	int index;
	struct dmlist list_node;
};

static void dmlist_test_init(void **state)
{
	struct dmlist list;
	DMLIST_INIT(&list);
	assert_true(list.next==&list);
	assert_true(list.prev==&list);
}

static void dmlist_test_new(void **state)
{
	DMLIST_NEW(list_new);
	assert_true(list_new.next==&list_new);
	assert_true(list_new.prev==&list_new);
}

static void dmlist_test_empty(void **state)
{
	DMLIST_NEW(list);
	assert_true(DMLIST_IS_EMPTY(&list));
}

static void dmlist_test_add_tail(void **state)
{
	const int num_nodes = 5;
	struct node_test data_test[num_nodes];
	struct node_test *data_node;
	struct dmlist *node;
	int i;
	DMLIST_NEW(list);

	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		DMLIST_ADD_TAIL(&list, &data_test[i].list_node);
	}

	i = 0;
	DMLIST_FOREACH(&list, node){
		data_node = DMLIST_ENTRY(node, struct node_test, list_node);
		assert_true(data_node->index == i);
		i++;
	}
}

static void dmlist_test_add_head(void **state)
{
	const int num_nodes = 5;
	struct node_test data_test[num_nodes];
	struct node_test *data_node;
	struct dmlist *node;
	int i;
	DMLIST_NEW(list);

	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		DMLIST_ADD_HEAD(&list, &data_test[i].list_node);
	}

	i = num_nodes-1;
	DMLIST_FOREACH(&list, node){
		data_node = DMLIST_ENTRY(node, struct node_test, list_node);
		assert_true(data_node->index == i);
		i--;
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
        cmocka_unit_test(dmlist_test_init),
        cmocka_unit_test(dmlist_test_new),
        cmocka_unit_test(dmlist_test_empty),
        cmocka_unit_test(dmlist_test_add_tail),
        cmocka_unit_test(dmlist_test_add_head),
    };

	return cmocka_run_group_tests(tests, NULL, NULL);
}
