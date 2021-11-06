//Lab3_TrentReuben_816015091_Unit Test 3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "freertos/semphr.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"
#include "sys/time.h"

static const char *TAG = "main";

#define GPIO_OUTPUT_IO    2
#define GPIO_OUTPUT_PIN_SEL  (1ULL<<GPIO_OUTPUT_IO)

//this task display the status of the gpio pin
static void status_msg(void* arg)
{
    for(;;){
        //display message for status
        ESP_LOGI(TAG,"GPIO_2 Pin STATUS: %d\n", gpio_get_level(GPIO_OUTPUT_IO));
        //1s delay
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void app_main(void){
	
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //stub to set gpio pin on
    gpio_set_level(GPIO_OUTPUT_IO,1);  
    //tasks to call status message
    xTaskCreate(status_msg, "status_msg", 2042, NULL, 1, NULL);
    //stub to set gpio pin on
    gpio_set_level(GPIO_OUTPUT_IO,0);
    //task to call status message
    xTaskCreate(status_msg, "status_msg", 2042, NULL, 1, NULL);
}

