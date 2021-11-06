//Lab3_TrentReuben_816015091_Unit Test 2

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

SemaphoreHandle_t xMutex = NULL;

//This task set the gpio low, active delays for 0.5s and then task delays for 1s
static void gpio_off(void* arg){
    for (;;){
        //Try to take semaphore.
        while(true){
            if(xSemaphoreTake(xMutex, (TickType_t) 10) == pdTRUE){
                //if mutex is obtained display the foll. message
                ESP_LOGI("Task: gpio_off", "Setting GPIO2 low\n");
                gpio_set_level(GPIO_OUTPUT_IO,0);
                break;
            }else{
                continue;
            }
        }

        //0.5s active delay
        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);    
        int64_t time_start = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec; //time since last reset in us

        while(true){
            gettimeofday(&tv_now, NULL);    
            int64_t time_now = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;

            //if 0.5s has elapsed
            if((time_now - time_start) >= 500000){
                break;
            }
        }
        //the mutex must be given back
        xSemaphoreGive(xMutex);
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

	//Create mutex type semaphore
    xMutex = xSemaphoreCreateMutex();
    //GIve semaphore
	xSemaphoreGive(xMutex);

    //stub to set gpio pin off
    printf("Setting gpio high...");
    gpio_set_level(GPIO_OUTPUT_IO,1);
    vTaskDelay(3000 / portTICK_RATE_MS);
    //create task to turn gpio pin on
    xTaskCreate(gpio_off, "gpio_off", 2042, NULL, 1, NULL);
}
