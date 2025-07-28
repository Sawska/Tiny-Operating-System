#include "unity.h"
#include "scheduler.h"

void setUp(void) {
    scheduler_init();
}

void tearDown(void) {
}

void test_task_creation(void) {
    int before = total_tasks;
    task_create((void(*)(void))0xdeadbeef, 1, CAP_UART);
    TEST_ASSERT_EQUAL_INT(before + 1, total_tasks);
}

void test_get_next_task(void) {
    task_create((void(*)(void))0xdeadbeef, 1, CAP_UART);
    int next = get_next_task();
    TEST_ASSERT_TRUE(next >= 0 && next < total_tasks);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_task_creation);
    RUN_TEST(test_get_next_task);
    return UNITY_END();
}
