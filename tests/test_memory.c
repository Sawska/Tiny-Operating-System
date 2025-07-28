#include "unity.h"
#include "memory.h"

void setUp(void) {
    
}

void tearDown(void) {
    
}

void test_simple_malloc_allocates(void) {
    void* ptr = simple_malloc(64);
    TEST_ASSERT_NOT_NULL(ptr);
}

void test_simple_malloc_fails_on_oversize(void) {
    void* ptr = simple_malloc(1024 * 100);
    TEST_ASSERT_NULL(ptr);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_simple_malloc_allocates);
    RUN_TEST(test_simple_malloc_fails_on_oversize);
    return UNITY_END();
}
