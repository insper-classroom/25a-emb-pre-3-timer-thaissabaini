#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

volatile int g_timer_0 = 0;
volatile int g_timer_1= 0;


void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

bool timer_0_callback(repeating_timer_t *rt) {
    g_timer_0 = 1;
    return true;
}

bool timer_1_callback(repeating_timer_t *rt) {
    g_timer_1 = 1;
    return true;
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    int timer_0_hz = 2;
    int timer_1_hz = 4;
    repeating_timer_t timer_0;
    repeating_timer_t timer_1;

    bool led_flag_r = 0;
    bool led_flag_g = 0;

    while (true) {

        if (flag_r) {
            flag_r = 0;
            led_flag_r = !led_flag_r;
            if (led_flag_r) {
                add_repeating_timer_us(1000000 / timer_0_hz, timer_0_callback, NULL, &timer_0);
            } else {
                cancel_repeating_timer(&timer_0);
                gpio_put(LED_PIN_R, 0);
            }
        }

        if (flag_g) {
            flag_g = 0;
            led_flag_g = !led_flag_g;

            if (led_flag_g) {
                add_repeating_timer_us(1000000 / timer_1_hz, timer_1_callback, NULL, &timer_1);
            } else {
                cancel_repeating_timer(&timer_1);
                gpio_put(LED_PIN_G, 0);
            }
        }

        if (g_timer_0) {
            g_timer_0 = 0;
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
        }else if (g_timer_1) {
            g_timer_1 = 0;
            gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G));
        }
    }
}
