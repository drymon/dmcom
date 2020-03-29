#include "utest_header.h"
#include "dmtimer.h"
#include "dmtimer_priv.h"

static void dmtimer_test_group_create(void **state)
{
	struct dmtimer_group * group = dmtimer_group_create();
	assert_non_null(group);
	assert_int_equal(dmtimer_group_ntimers(group), 0);
	dmtimer_group_destroy(group);
}

static void *null_malloc(int size)
{
	return NULL;
}

static void dmtimer_test_group_create_null_memory(void **state)
{
	struct dmtimer_group *group;

	dmtimer_set_malloc(null_malloc);
	group = dmtimer_group_create();
	assert_null(group);
	dmtimer_set_malloc_default();
}

static void dmtimer_test_create(void **state)
{
	struct dmtimer *timer = dmtimer_create();
	
	assert_non_null(timer);
	dmtimer_destroy(timer);
}

static void dmtimer_test_create_null_memory(void **state)
{
	struct dmtimer *timer;
	
	dmtimer_set_malloc(null_malloc);
	timer = dmtimer_create();
	assert_null(timer);
	dmtimer_set_malloc_default();
}

static void dmtimer_test_group_add_create(struct dmtimer_group **group, struct dmtimer **timer)
{
	*group = dmtimer_group_create();
	assert_non_null(*group);
	*timer = dmtimer_create();
	assert_non_null(*timer);
}

static void dmtimer_test_group_add_destroy(struct dmtimer_group *group, struct dmtimer *timer)
{
	dmtimer_destroy(timer);
	dmtimer_group_destroy(group);
}

static void dmtimer_test_group_add_remove(void **state)
{
	struct dmtimer_group *group;
	struct dmtimer *timer;
	
	dmtimer_test_group_add_create(&group, &timer);

	assert_int_equal(dmtimer_add_group(timer, group), DM_E_OK);
	assert_int_equal(dmtimer_group_ntimers(group), 1);
	assert_ptr_equal(dmtimer_get_group(timer), group);
	dmtimer_remove_group(timer);
	assert_null(dmtimer_get_group(timer));
	assert_int_equal(dmtimer_group_ntimers(group), 0);
	dmtimer_test_group_add_destroy(group, timer);
}

static void dmtimer_test_group_add_remove_multi_times(void **state)
{
	struct dmtimer_group *group;
	struct dmtimer *timer;
	int times = 5;
	int i;
	
	dmtimer_test_group_add_create(&group, &timer);
	
	assert_int_equal(dmtimer_add_group(timer, group), DM_E_OK);
	assert_int_equal(dmtimer_group_ntimers(group), 1);

	//Add when existed
	for(i=0; i<times; i++){
		assert_int_equal(dmtimer_add_group(timer, group), DM_E_EXIST);
		assert_int_equal(dmtimer_group_ntimers(group), 1);
	}

	for(i=0; i<times; i++){
		dmtimer_remove_group(timer);
		assert_int_equal(dmtimer_group_ntimers(group), 0);
	}

	//Add remove many times
	for(i=0; i<times; i++){
		assert_int_equal(dmtimer_add_group(timer, group), DM_E_OK);
		assert_int_equal(dmtimer_group_ntimers(group), 1);
		dmtimer_remove_group(timer);
		assert_int_equal(dmtimer_group_ntimers(group), 0);
	}
	
	dmtimer_test_group_add_destroy(group, timer);
}

static void dmtimer_test_group_add_remove_multi_timers(void **state)
{
	struct dmtimer_group *group;
	int ntimers = 10;
	int i;
	struct dmtimer *timer[ntimers];

	group = dmtimer_group_create();
	assert_non_null(group);

	for(i=0; i<ntimers; i++){
		timer[i] = dmtimer_create();
		assert_non_null(timer[i]);
		assert_int_equal(dmtimer_add_group(timer[i], group), DM_E_OK);
		assert_int_equal(dmtimer_group_ntimers(group), i+1);
	}
	
	for(i=0; i<ntimers; i++){
		dmtimer_remove_group(timer[i]);
		assert_int_equal(dmtimer_group_ntimers(group), ntimers-(i+1));
		dmtimer_destroy(timer[i]);
	}
	dmtimer_group_destroy(group);
}

static void dmtimer_test_multi_group_switch_timer(void **state)
{
	struct dmtimer_group *group1, *group2;
	struct dmtimer *timer;

	group1 = dmtimer_group_create();
	assert_non_null(group1);

	group2 = dmtimer_group_create();
	assert_non_null(group2);

	timer = dmtimer_create();
	assert_non_null(timer);

	assert_int_equal(dmtimer_add_group(timer, group1), DM_E_OK);
	assert_int_equal(dmtimer_add_group(timer, group2), DM_E_EXIST);
	assert_int_equal(dmtimer_group_ntimers(group1), 1);
	assert_ptr_equal(dmtimer_get_group(timer), group1);
	
	dmtimer_remove_group(timer);

	assert_int_equal(dmtimer_add_group(timer, group2), DM_E_OK);
	assert_ptr_equal(dmtimer_get_group(timer), group2);
	
	assert_int_equal(dmtimer_group_ntimers(group1), 0);
	assert_int_equal(dmtimer_group_ntimers(group2), 1);
	
	dmtimer_group_destroy(group1);
	dmtimer_group_destroy(group2);
	dmtimer_destroy(timer);
}

static void expire_param_invalid(struct dmtimer *timer, void *cbarg){}

static void dmtimer_test_invalid_param(void **state)
{
	struct dmtimer_group *group = dmtimer_group_create();
	struct dmtimer *timer = dmtimer_create();
	int remain_us;
	int next_timout;

	assert_int_equal(dmtimer_group_schedule(group, NULL), DM_E_PARAM);
	assert_int_equal(dmtimer_group_schedule(NULL, &next_timout), DM_E_PARAM);
	assert_int_equal(dmtimer_group_schedule(NULL, NULL), DM_E_PARAM);

	assert_int_equal(dmtimer_group_ntimers(NULL), DM_E_PARAM);

	assert_int_equal(dmtimer_register_expire_cb(timer, NULL, NULL), DM_E_PARAM);
	assert_int_equal(dmtimer_register_expire_cb(NULL, expire_param_invalid, NULL), DM_E_PARAM);
	assert_int_equal(dmtimer_register_expire_cb(NULL, NULL, NULL), DM_E_PARAM);

	assert_int_equal(dmtimer_add_group(NULL, group), DM_E_PARAM);
	assert_int_equal(dmtimer_add_group(timer, NULL), DM_E_PARAM);
	assert_int_equal(dmtimer_add_group(NULL, NULL), DM_E_PARAM);

	assert_int_equal(dmtimer_start(NULL, 0), DM_E_PARAM);
	
	assert_int_equal(dmtimer_restart(NULL, 0), DM_E_PARAM);

	assert_false(dmtimer_is_running(NULL));
	
	assert_false(dmtimer_is_repeat(NULL));
	
	assert_int_equal(dmtimer_get_remain(NULL, &remain_us), DM_E_PARAM);
	assert_int_equal(dmtimer_get_remain(timer, NULL), DM_E_PARAM);
	assert_int_equal(dmtimer_get_remain(NULL, NULL), DM_E_PARAM);
	
	dmtimer_group_destroy(group);
	dmtimer_destroy(timer);
}

static void dmtimer_test_repeat_enable_disable(void **state)
{
	struct dmtimer *timer = dmtimer_create();
	assert_false(dmtimer_is_repeat(timer));
	dmtimer_enable_repeat(timer);
	assert_true(dmtimer_is_repeat(timer));
	dmtimer_disable_repeat(timer);
	assert_false(dmtimer_is_repeat(timer));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
        cmocka_unit_test(dmtimer_test_group_create),
        cmocka_unit_test(dmtimer_test_group_create_null_memory),
		cmocka_unit_test(dmtimer_test_create),
		cmocka_unit_test(dmtimer_test_create_null_memory),
		cmocka_unit_test(dmtimer_test_group_add_remove),
		cmocka_unit_test(dmtimer_test_group_add_remove_multi_times),
		cmocka_unit_test(dmtimer_test_group_add_remove_multi_timers),
		cmocka_unit_test(dmtimer_test_multi_group_switch_timer),
		cmocka_unit_test(dmtimer_test_invalid_param),
		cmocka_unit_test(dmtimer_test_repeat_enable_disable),
    };

	return cmocka_run_group_tests(tests, NULL, NULL);
}
