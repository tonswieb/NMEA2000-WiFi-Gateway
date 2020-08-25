#include <Arduino.h>
#include <nmea/N183ToN2k.h>
#include "nmea/N183ToN2k.cpp"
#include <unity.h>

void test_calc_eta_isnan_return_ulong_max() {
    tETA eta = calcETA(NAN,6.0,0);
    TEST_ASSERT_EQUAL_INT(ULONG_MAX, eta.etaDays);

    eta = calcETA(6.0,NAN,0);
    TEST_ASSERT_EQUAL_INT(ULONG_MAX, eta.etaDays);
}

void test_calc_eta_speed_to_low_returns_ulong_max() {

    tETA eta = calcETA(6.0,0.001,0);
    TEST_ASSERT_EQUAL_INT(ULONG_MAX, eta.etaDays);
}

void test_calc_eta_dtw_is_0() {
    tETA eta = calcETA(0.0,6.0,0);
    TEST_ASSERT_EQUAL_INT(0,eta.etaDays);
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 0.0, eta.etaTime);
}

void test_calc_eta_one_hour(void) {
    tETA eta = calcETA(6.0,6.0,0);
    TEST_ASSERT_EQUAL_INT(0,eta.etaDays);
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 3600, eta.etaTime);
}

void test_calc_eta_around_10_minutes(void) {
    tETA eta = calcETA(1.0001,6.0,0);
    TEST_ASSERT_EQUAL_INT(0,eta.etaDays);
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 600, eta.etaTime);
}

void test_calc_23H_59M_59S(void) {
    tETA eta = calcETA(1.0,1.0002,0);
    TEST_ASSERT_EQUAL_INT(0,eta.etaDays);
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 3600-1, eta.etaTime);
}

void test_calc_one_day(void) {
    tETA eta = calcETA(48.0,2.0,0);
    TEST_ASSERT_EQUAL_INT(1,eta.etaDays);
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 0, eta.etaTime);
}

void test_calc_one_day_one_hour(void) {
    tETA eta = calcETA(50.0,2.0,0);
    TEST_ASSERT_EQUAL_INT(1,eta.etaDays);
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 3600, eta.etaTime);
}

void test_calc_time_offset_usa_east_coast(void) {
    tETA eta = calcETA(2.0,2.0,-6.0);
    TEST_ASSERT_EQUAL_INT(0,eta.etaDays);
    TEST_ASSERT_FLOAT_WITHIN(0.000001, 7*3600, eta.etaTime);
}

void test_calc_time_offset_europe_summer_time(void) {
    tETA eta = calcETA(2.0,2.0,2.0);
    TEST_ASSERT_EQUAL_INT(0,eta.etaDays);
    TEST_ASSERT_FLOAT_WITHIN(0.000001, -1*3600, eta.etaTime);
}

// int main(int argc, char **argv) {
void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_calc_eta_isnan_return_ulong_max);
    RUN_TEST(test_calc_eta_speed_to_low_returns_ulong_max);
    RUN_TEST(test_calc_eta_dtw_is_0);
    RUN_TEST(test_calc_eta_around_10_minutes);
    RUN_TEST(test_calc_eta_one_hour);
    RUN_TEST(test_calc_23H_59M_59S);
    RUN_TEST(test_calc_one_day);
    RUN_TEST(test_calc_one_day_one_hour);
    RUN_TEST(test_calc_time_offset_usa_east_coast);
    RUN_TEST(test_calc_time_offset_europe_summer_time);
    UNITY_END();
    // return 0;
}

void loop() {

}