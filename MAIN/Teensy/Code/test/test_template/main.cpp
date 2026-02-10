#include <Arduino.h>
#include <unity.h>

const int LED_PIN = 13;

void setUp(void) {
    digitalWrite(LED_PIN, LOW);
}

void tearDown(void) {
}

void test_led_pin_number(void) {
    TEST_ASSERT_EQUAL(13, LED_PIN);
}

void test_multiplication(void) {
    int a = 5;
    int b = 5;
    TEST_ASSERT_EQUAL_INT(25, a * b);
}

void setup() {
    delay(2000); 

    UNITY_BEGIN();

    RUN_TEST(test_led_pin_number);
    RUN_TEST(test_multiplication);

    UNITY_END();
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(500);
}