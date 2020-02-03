#include <setjmp.h>
#include <stdio.h>
#include "dmcom/dmlist.h"
#include <cmocka.h>

struct node_test {
	int index;
	struct dmlist list_node;
};

static void dmlist_fill_data_increase(struct dmlist *list,
									   struct node_test data_test[], int num_nodes){
	int i;
	
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		DMLIST_ADD_TAIL(list, &data_test[i].list_node);
	}
}

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

static void dmlist_assert_link(struct dmlist *list)
{
	struct dmlist *node, *last;
	
	last = list;
	//Verify if link is correct
	DMLIST_FOREACH(list, node){
		assert_true(last->next == node);
		assert_true(node->prev == last);
		last = node;
	}

	assert_true(list->prev == last);
	assert_true(last->next == list);
}

static void dmlist_assert_increase(struct dmlist *list,
									struct node_test data_test[], int num_nodes)
{
	int count_nodes = 0;
	struct node_test *test_tmp;
	struct dmlist *node;
	
	DMLIST_FOREACH(list, node){
		test_tmp = DMLIST_ENTRY(node, struct node_test, list_node);
		assert_true(test_tmp->index == count_nodes);
		count_nodes++;
	}
	assert_true(num_nodes == count_nodes);
	dmlist_assert_link(list);
}

static void dmlist_test_add_tail_data(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	DMLIST_NEW(list);

	dmlist_fill_data_increase(&list, data_test, num_nodes);
	dmlist_assert_increase(&list, data_test, num_nodes);
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
	dmlist_assert_link(&list);
}

static void dmlist_test_remove(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	struct node_test *remove_node;
	struct dmlist *node;
	int num_node_tmp;
	int middle_index;
	DMLIST_NEW(list);

	dmlist_fill_data_increase(&list, data_test, num_nodes);
	
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
	DMLIST_NEW(list);

	dmlist_fill_data_increase(&list, data_test, num_nodes);

	assert_true(DMLIST_HEAD(&list)== &data_test[0].list_node);
	assert_true(DMLIST_TAIL(&list)== &data_test[num_nodes-1].list_node);
}

static void dmlist_test_remove_head_tail_empty(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	struct node_test *node_tmp;
	int count = 0;
	DMLIST_NEW(list);
	
	dmlist_fill_data_increase(&list, data_test, num_nodes);

	while(!DMLIST_IS_EMPTY(&list)){
		node_tmp = DMLIST_ENTRY(DMLIST_HEAD(&list), struct node_test, list_node);
		if(count == num_nodes-1){
			assert_true(&node_tmp->list_node == DMLIST_TAIL(&list));
		}
		DMLIST_REMOVE(&node_tmp->list_node);
		assert_true(node_tmp->index == count);
		count++;
	}
	assert_true(count == num_nodes);
	assert_true(DMLIST_IS_EMPTY(&list));
}

static void dmlist_test_empty_foreach_safe(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	struct dmlist *node, *tmp;
	struct node_test *test_tmp;
	int i = 0;
	DMLIST_NEW(list);

	dmlist_fill_data_increase(&list, data_test, num_nodes);
	assert_false(DMLIST_IS_EMPTY(&list));
	DMLIST_FOREACH_SAFE(&list, node, tmp){
		test_tmp = DMLIST_ENTRY(node, struct node_test, list_node);
		assert_true(test_tmp->index == i);
		DMLIST_REMOVE(node);
		i++;
	}
	assert_true(DMLIST_IS_EMPTY(&list));
}

static void dmlist_test_add_before(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	int i;
	DMLIST_NEW(list);

	//1. Add to first element 
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		if(i == 0) //Add later on 
			  continue;
		DMLIST_ADD_TAIL(&list, &data_test[i].list_node);
	}
	
	DMLIST_ADD_BEFORE(&data_test[1].list_node, &data_test[0].list_node);
	//Verify
	dmlist_assert_increase(&list, data_test, num_nodes);

	DMLIST_INIT(&list);
	//2. Add to end element
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		if(i == num_nodes-1) //Add later on 
			continue;
		DMLIST_ADD_TAIL(&list, &data_test[i].list_node);
	}
	DMLIST_ADD_BEFORE(&list, &data_test[num_nodes - 1].list_node);
	//Verify
	dmlist_assert_increase(&list, data_test, num_nodes);
	
	//3. Add to middle
	DMLIST_INIT(&list);
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		if(i == num_nodes/2)
			continue;
		DMLIST_ADD_TAIL(&list, &data_test[i].list_node);
	}
	DMLIST_ADD_BEFORE(&data_test[num_nodes/2 + 1].list_node, &data_test[num_nodes/2].list_node);
	//Verify
	dmlist_assert_increase(&list, data_test, num_nodes);
}

static void dmlist_test_add_after(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	int i;
	DMLIST_NEW(list);

	//1. Add to first element 
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		if(i == 0) //Add later on 
			continue;
		DMLIST_ADD_TAIL(&list, &data_test[i].list_node);
	}
	
	DMLIST_ADD_AFTER(&list, &data_test[0].list_node);
	//Verify
	dmlist_assert_increase(&list, data_test, num_nodes);

	DMLIST_INIT(&list);
	//2. Add to end element
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		if(i == num_nodes-1) //Add later on 
			continue;
		DMLIST_ADD_TAIL(&list, &data_test[i].list_node);
	}
	DMLIST_ADD_AFTER(&data_test[num_nodes - 2].list_node, &data_test[num_nodes - 1].list_node);
	//Verify
	dmlist_assert_increase(&list, data_test, num_nodes);
	
	//3. Add to middle
	DMLIST_INIT(&list);
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		if(i == num_nodes/2)
			continue;
		DMLIST_ADD_TAIL(&list, &data_test[i].list_node);
	}
	DMLIST_ADD_AFTER(&data_test[num_nodes/2 - 1].list_node, &data_test[num_nodes/2].list_node);
	//Verify
	dmlist_assert_increase(&list, data_test, num_nodes);
}

static void dmlist_test_count(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	struct dmlist *node;
	int count;
	DMLIST_NEW(list);
	
	DMLIST_COUNT(&list, node, count);
	assert_int_equal(count, 0);
	dmlist_fill_data_increase(&list, data_test, num_nodes);
	DMLIST_COUNT(&list, node, count);
	assert_int_equal(count, num_nodes);
	while(!DMLIST_IS_EMPTY(&list)){
		node = DMLIST_HEAD(&list);
		DMLIST_REMOVE(node);
	}
	DMLIST_COUNT(&list, node, count);
	assert_int_equal(count, 0);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
        cmocka_unit_test(dmlist_test_init),
        cmocka_unit_test(dmlist_test_new),
        cmocka_unit_test(dmlist_test_empty),
        cmocka_unit_test(dmlist_test_add_tail_data),
        cmocka_unit_test(dmlist_test_add_head),
        cmocka_unit_test(dmlist_test_remove),
        cmocka_unit_test(dmlist_test_head_tail),
        cmocka_unit_test(dmlist_test_empty_foreach_safe),
        cmocka_unit_test(dmlist_test_remove_head_tail_empty),
        cmocka_unit_test(dmlist_test_add_before),
        cmocka_unit_test(dmlist_test_add_after),
        cmocka_unit_test(dmlist_test_count),
    };

	return cmocka_run_group_tests(tests, NULL, NULL);
}
