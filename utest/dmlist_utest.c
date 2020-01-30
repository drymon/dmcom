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

static void dmlist_test_add_tail_data(void **state)
{
	const int num_nodes = 50;
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

static void dmlist_test_add_tail_link(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	struct dmlist *node, *last;
	int i;
	DMLIST_NEW(list);

	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		DMLIST_ADD_TAIL(&list, &data_test[i].list_node);
	}

	last = &list;
	//Verify if link is correct
	for(i=0; i<num_nodes; i++){
		node = &data_test[i].list_node;
		assert_true(last->next == node);
		assert_true(node->prev == last);
		last = node;
	}
	assert_true(list.prev == node);
	assert_true(node->next == &list);
}

static void dmlist_test_add_head(void **state)
{
	const int num_nodes = 50;
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

static void dmlist_test_add_head_link(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	struct dmlist *node, *last;
	int i;
	DMLIST_NEW(list);

	for(i=num_nodes-1; i>=0; i--){
		data_test[i].index = i;
		DMLIST_ADD_HEAD(&list, &data_test[i].list_node);
	}

	last = &list;
	//Verify if link is correct
	for(i=0; i<num_nodes; i++){
		node = &data_test[i].list_node;
		assert_true(last->next == node);
		assert_true(node->prev == last);
		last = node;
	}
	assert_true(list.prev == node);
	assert_true(node->next == &list);
}

static void dmlist_test_remove(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	struct node_test *remove_node;
	struct dmlist *node;
	int num_node_tmp;
	int middle_index;
	int i;
	DMLIST_NEW(list);

	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		DMLIST_ADD_TAIL(&list, &data_test[i].list_node);
	}
	
	//1. Remove at the middle
	remove_node = &data_test[num_nodes/2];
	DMLIST_REMOVE(&remove_node->list_node);
	num_node_tmp = 0;
	middle_index = remove_node->index;
	//Check if it was removed actually and did not affect to other nodes
	DMLIST_FOREACH(&list, node){
		struct node_test *test_tmp;
		test_tmp = DMLIST_ENTRY(node, struct node_test, list_node);
		assert_true(num_node_tmp < num_nodes); //Limit case loop more than actual node
		assert_true(remove_node != test_tmp); //Make sure it was removed
		if(num_node_tmp == middle_index) //Ignore removed node
			num_node_tmp++;
		assert_true(test_tmp->index == num_node_tmp);
		num_node_tmp++;
	}
	assert_true(num_node_tmp == num_nodes);

	//2. Remove at the head
	remove_node = &data_test[0];
	DMLIST_REMOVE(&remove_node->list_node);
	num_node_tmp = 1; //Ignore the first removed node
	//Check if it was removed actually and did not affect to other nodes
	DMLIST_FOREACH(&list, node){
		struct node_test *test_tmp;
		test_tmp = DMLIST_ENTRY(node, struct node_test, list_node);
		assert_true(num_node_tmp < num_nodes);
		assert_true(remove_node != test_tmp);
		if(num_node_tmp == middle_index) //Ignore the middle removed node
			num_node_tmp++;
		assert_true(test_tmp->index == num_node_tmp);
		num_node_tmp++;
	}
	assert_true(num_node_tmp == num_nodes);

	//3. Remove at the end
	remove_node = &data_test[num_nodes-1];
	DMLIST_REMOVE(&remove_node->list_node);
	num_node_tmp = 1; //Ignore first
	//Check if it was removed actually and did not affect to other nodes
	DMLIST_FOREACH(&list, node){
		struct node_test *test_tmp;
		test_tmp = DMLIST_ENTRY(node, struct node_test, list_node);
		assert_true(num_node_tmp < num_nodes);
		assert_true(remove_node != test_tmp);
		if(num_node_tmp == middle_index) //Ignore the middle removed node
			num_node_tmp++;
		assert_true(test_tmp->index == num_node_tmp);
		num_node_tmp++;
	}
	num_node_tmp++; //Ignore the last node
	assert_true(num_node_tmp == num_nodes);
}

static void dmlist_test_head_tail(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	int i;
	DMLIST_NEW(list);

	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		DMLIST_ADD_TAIL(&list, &data_test[i].list_node);
	}
	assert_true(DMLIST_HEAD(&list)== &data_test[0].list_node);
	assert_true(DMLIST_TAIL(&list)== &data_test[num_nodes-1].list_node);
}

static void dmlist_test_empty_foreach_safe(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	struct dmlist *node, *tmp;
	struct node_test *test_tmp;
	int i;
	DMLIST_NEW(list);

	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		DMLIST_ADD_TAIL(&list, &data_test[i].list_node);
	}
	assert_false(DMLIST_IS_EMPTY(&list));
	i = 0;
	DMLIST_FOREACH_SAFE(&list, node, tmp){
		test_tmp = DMLIST_ENTRY(node, struct node_test, list_node);
		assert_true(test_tmp->index == i);
		DMLIST_REMOVE(node);
		i++;
	}
	assert_true(DMLIST_IS_EMPTY(&list));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
        cmocka_unit_test(dmlist_test_init),
        cmocka_unit_test(dmlist_test_new),
        cmocka_unit_test(dmlist_test_empty),
        cmocka_unit_test(dmlist_test_add_tail_data),
        cmocka_unit_test(dmlist_test_add_tail_link),
        cmocka_unit_test(dmlist_test_add_head),
        cmocka_unit_test(dmlist_test_add_head_link),
        cmocka_unit_test(dmlist_test_remove),
        cmocka_unit_test(dmlist_test_head_tail),
        cmocka_unit_test(dmlist_test_empty_foreach_safe),
    };

	return cmocka_run_group_tests(tests, NULL, NULL);
}
