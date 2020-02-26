#include <setjmp.h>
#include <stdio.h>
#include "dmcom/dmlist.h"
#include <cmocka.h>

struct node_test {
	int index;
	struct dmnode node;
};

static void dmlist_fill_data_increase(struct dmlist *list,
									   struct node_test data_test[], int num_nodes){
	int i;
	
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		dmlist_add_tail(list, &data_test[i].node);
	}
}

static void dmlist_test_init(void **state)
{
	struct dmlist list;
	dmlist_init(&list);
	assert_true(list.node.next==&list.node);
	assert_true(list.node.prev==&list.node);
	assert_int_equal(list.nelems, 0);
}

static void dmlist_test_empty(void **state)
{
	struct dmlist list;
	dmlist_init(&list);
	assert_true(dmlist_is_empty(&list));
}

static void dmlist_assert_link(struct dmlist *list)
{
	struct dmnode *node, *last;
	
	last = &list->node;
	//Verify if link is correct
	DMLIST_FOREACH(list, node){
		assert_true(last->next == node);
		assert_true(node->prev == last);
		last = node;
	}

	assert_true(list->node.prev == last);
	assert_true(last->next == &list->node);
}

static void dmlist_assert_increase(struct dmlist *list,
									struct node_test data_test[], int num_nodes)
{
	int count_nodes = 0;
	struct node_test *test_tmp;
	struct dmnode *node;
	
	DMLIST_FOREACH(list, node){
		test_tmp = DMLIST_ENTRY(node, struct node_test, node);
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
	struct dmlist list;
	dmlist_init(&list);

	dmlist_fill_data_increase(&list, data_test, num_nodes);
	dmlist_assert_increase(&list, data_test, num_nodes);
}

static void dmlist_test_add_head(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	struct node_test *data_node;
	struct dmnode *node;
	int i;
	struct dmlist list;
	dmlist_init(&list);

	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		dmlist_add_head(&list, &data_test[i].node);
	}

	i = num_nodes-1;
	DMLIST_FOREACH(&list, node){
		data_node = DMLIST_ENTRY(node, struct node_test, node);
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
	struct dmnode *node;
	int num_node_tmp;
	int middle_index;
	struct dmlist list;
	dmlist_init(&list);

	dmlist_fill_data_increase(&list, data_test, num_nodes);
	
	//1. Remove at the middle
	remove_node = &data_test[num_nodes/2];
	dmlist_remove(&list, &remove_node->node);
	num_node_tmp = 0;
	middle_index = remove_node->index;
	//Check if it was removed actually and did not affect to other nodes
	DMLIST_FOREACH(&list, node){
		struct node_test *test_tmp;
		test_tmp = DMLIST_ENTRY(node, struct node_test, node);
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
	dmlist_remove(&list, &remove_node->node);
	num_node_tmp = 1; //Ignore the first removed node
	//Check if it was removed actually and did not affect to other nodes
	DMLIST_FOREACH(&list, node){
		struct node_test *test_tmp;
		test_tmp = DMLIST_ENTRY(node, struct node_test, node);
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
	dmlist_remove(&list, &remove_node->node);
	num_node_tmp = 1; //Ignore first
	//Check if it was removed actually and did not affect to other nodes
	DMLIST_FOREACH(&list, node){
		struct node_test *test_tmp;
		test_tmp = DMLIST_ENTRY(node, struct node_test, node);
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
	struct dmlist list;
	dmlist_init(&list);

	dmlist_fill_data_increase(&list, data_test, num_nodes);

	assert_true(dmlist_get_head(&list)== &data_test[0].node);
	assert_true(dmlist_get_tail(&list)== &data_test[num_nodes-1].node);
}

static void dmlist_test_remove_head_tail_empty(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	struct node_test *node_tmp;
	int count = 0;
	struct dmlist list;
	dmlist_init(&list);
	
	dmlist_fill_data_increase(&list, data_test, num_nodes);

	while(!dmlist_is_empty(&list)){
		node_tmp = DMLIST_ENTRY(dmlist_get_head(&list), struct node_test, node);
		if(count == num_nodes-1){
			assert_true(&node_tmp->node == dmlist_get_tail(&list));
		}
		dmlist_remove(&list, &node_tmp->node);
		assert_true(node_tmp->index == count);
		count++;
	}
	assert_true(count == num_nodes);
	assert_true(dmlist_is_empty(&list));
}

static void dmlist_test_empty_foreach_safe(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	struct dmnode *node, *tmp;
	struct node_test *test_tmp;
	int i = 0;
	struct dmlist list;
	dmlist_init(&list);

	dmlist_fill_data_increase(&list, data_test, num_nodes);
	assert_false(dmlist_is_empty(&list));
	DMLIST_FOREACH_SAFE(&list, node, tmp){
		test_tmp = DMLIST_ENTRY(node, struct node_test, node);
		assert_true(test_tmp->index == i);
		dmlist_remove(&list, node);
		i++;
	}
	assert_true(dmlist_is_empty(&list));
}

static void dmlist_test_add_before(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	int i;
	struct dmlist list;
	dmlist_init(&list);

	//1. Add to first element 
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		if(i == 0) //Add later on 
			  continue;
		dmlist_add_tail(&list, &data_test[i].node);
	}
	
	dmlist_add_before(&list, &data_test[1].node, &data_test[0].node);
	//Verify
	dmlist_assert_increase(&list, data_test, num_nodes);

	dmlist_init(&list);
	//2. Add to end element
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		if(i == num_nodes-1) //Add later on 
			continue;
		dmlist_add_tail(&list, &data_test[i].node);
	}
	dmlist_add_before(&list, &list.node, &data_test[num_nodes - 1].node);
	//Verify
	dmlist_assert_increase(&list, data_test, num_nodes);
	
	//3. Add to middle
	dmlist_init(&list);
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		if(i == num_nodes/2)
			continue;
		dmlist_add_tail(&list, &data_test[i].node);
	}
	dmlist_add_before(&list, &data_test[num_nodes/2 + 1].node, &data_test[num_nodes/2].node);
	//Verify
	dmlist_assert_increase(&list, data_test, num_nodes);
}

static void dmlist_test_add_after(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	int i;
	struct dmlist list;
	dmlist_init(&list);

	//1. Add to first element 
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		if(i == 0) //Add later on 
			continue;
		dmlist_add_tail(&list, &data_test[i].node);
	}
	
	dmlist_add_after(&list, &list.node, &data_test[0].node);
	//Verify
	dmlist_assert_increase(&list, data_test, num_nodes);

	dmlist_init(&list);
	//2. Add to end element
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		if(i == num_nodes-1) //Add later on 
			continue;
		dmlist_add_tail(&list, &data_test[i].node);
	}
	dmlist_add_after(&list, &data_test[num_nodes - 2].node, &data_test[num_nodes - 1].node);
	//Verify
	dmlist_assert_increase(&list, data_test, num_nodes);
	
	//3. Add to middle
	dmlist_init(&list);
	for(i=0; i<num_nodes; i++){
		data_test[i].index = i;
		if(i == num_nodes/2)
			continue;
		dmlist_add_tail(&list, &data_test[i].node);
	}
	dmlist_add_after(&list, &data_test[num_nodes/2 - 1].node, &data_test[num_nodes/2].node);
	//Verify
	dmlist_assert_increase(&list, data_test, num_nodes);
}

static void dmlist_test_count(void **state)
{
	const int num_nodes = 50;
	struct node_test data_test[num_nodes];
	struct dmnode *node;
	int count;
	struct dmlist list;
	dmlist_init(&list);
	
	count=dmlist_get_nelems(&list);
	assert_int_equal(count, 0);
	dmlist_fill_data_increase(&list, data_test, num_nodes);
	count = dmlist_get_nelems(&list);
	assert_int_equal(count, num_nodes);
	while(!dmlist_is_empty(&list)){
		node = dmlist_get_head(&list);
		dmlist_remove(&list, node);
	}
	count = dmlist_get_nelems(&list);
	assert_int_equal(count, 0);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
        cmocka_unit_test(dmlist_test_init),
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
