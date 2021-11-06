//Lab3_TrentReuben_816015091 (Integration/Verification Test)

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

//This task set the gpio high, active delays for 0.5s and then task delays for 1s
static void gpio_on(void* arg)
{
    for (;;){
        //Try to take semaphore.
        while(true){
            if(xSemaphoreTake(xMutex, (TickType_t) 10) == pdTRUE){
                //if mutex is obtained display the foll. message
                ESP_LOGI("Task: gpio_on", "Setting GPIO2 high\n");
                gpio_set_level(GPIO_OUTPUT_IO,1);
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
	
	//Create mutex type semaphore
    xMutex = xSemaphoreCreateMutex();
	
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

    //Check the semaphore was created successfully before creating the tasks
	if( xMutex != NULL){
		//Create instance of the task to turn set the gpio high with priority 1
		xTaskCreate(gpio_on, "gpio_on", 2048, NULL, 1, NULL);

		//Create instance of the task to turn set the gpio low with priority 2
		xTaskCreate(gpio_off, "gpio_off", 2048, NULL, 2, NULL);
	} 
	//Create instance of the task to display the status of the gpio2 pin with highest priority
    xTaskCreate(status_msg, "status_msg", 2048, NULL, 3, NULL);	
   
    //for heap memory management
	for(;;);
}


