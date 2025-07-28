
#include "unity.h"
#include "ipc.h"


void setUp(void) {
    
}

void tearDown(void) {
}

void test_ipc_basic(void) {
    
    int status = ipc_send(1, (void*)"test");
    TEST_ASSERT_EQUAL_INT(0, status);

}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ipc_basic);
    return UNITY_END();
}
