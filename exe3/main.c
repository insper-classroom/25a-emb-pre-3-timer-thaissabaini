#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>


const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;
volatile int state = 0;
volatile bool timer_fired = false;

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    timer_fired = true;

    return 0;
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        timer_fired = 0;
        add_alarm_in_ms(500, alarm_callback, NULL, false);
        flag_f_r = 1;

    } else if (events == 0x8) { // rise edge
        if (flag_f_r && timer_fired) {
            state = !state;
        }
        flag_f_r = 0;
    }
}



int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0); 

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    while (true) {

        gpio_put(LED_PIN_R, state);
    }
}
