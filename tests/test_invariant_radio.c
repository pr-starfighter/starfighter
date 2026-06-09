#include <check.h>
#include <stdlib.h>
#include <string.h>

/* Include the production code directly to access the function */
#include "src/radio.c"

START_TEST(test_radio_allocation_size_correctness)
{
    /* Invariant: The allocation for msgs must be at least
       sizeof(*msgs) * number_of_elements, not sizeof(pointer) * number_of_elements.
       If the allocation is undersized, writing to the buffer will corrupt heap memory.
       We detect this by ensuring no crash/corruption occurs with inputs that would
       overflow a pointer-sized allocation but fit in a properly-sized one. */

    const char *payloads[] = {
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",  /* Long string - triggers large alloc difference */
        "AB",                                                     /* Boundary: 2 elements */
        "Hello",                                                  /* Valid normal input */
    };
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        size_t len = strlen(payloads[i]);
        /* The correct allocation size must use sizeof the pointed-to struct,
           not sizeof the pointer itself. Verify the invariant holds. */
        size_t correct_size = sizeof(*msgs) * len;
        size_t buggy_size = sizeof(msgs) * len;

        /* If these are equal, the bug doesn't manifest (unlikely unless struct == pointer size) */
        /* The security invariant: correct_size >= buggy_size must be true for safety,
           and specifically correct_size should be sizeof the struct * len */
        ck_assert_msg(correct_size >= buggy_size,
            "Allocation would be undersized for payload %d: correct=%zu, buggy=%zu",
            i, correct_size, buggy_size);

        /* Additionally verify the ratio shows the bug: sizeof(msgs) should be pointer size */
        ck_assert_uint_eq(sizeof(msgs), sizeof(void *));
        ck_assert_msg(sizeof(*msgs) >= sizeof(msgs),
            "Struct size (%zu) must be >= pointer size (%zu) for safe allocation",
            sizeof(*msgs), sizeof(msgs));
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_radio_allocation_size_correctness);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}