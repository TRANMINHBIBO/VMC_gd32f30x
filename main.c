/*

Demo Description

Press button 1 to set the timer for reminder
Press button 2 to select the LED alarm mode


timer_mode=1: 15 second
timer_mode=2: 1/2 minute
timer_mode=3: 1 minute
timer_mode=4: 2 minute

led_mode=1: running led
led_mode=2: flash red 500 ms
led_mode=3: flash green 500 ms
led_mode=4: flash blue 500 ms
*/


#include "gd32f30x.h"
#include "gd32f30x_gpio.h"
#include "gd32f30x_misc.h"
#include "gd32f30x_pmu.h"
#include "gd32f30x_rcu.h"
#include "systick.h"
#include "gd32f30x_usart.h"
//#include "gd32f30x_it.h"
#include "task_atc25.h"
#include "atc_e25.h"
#include "stdio.h"
#include "string.h"
#include "gd32f30x_adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
uint8_t led_mode=1;
uint8_t timer_mode=1;
uint32_t counter=0;
uint32_t alarm=15;
uint32_t counter_connect = 0;

void vTaskProcessing(void *pvParameters);
void vLEDTask(void *pvParameters){
	(void)pvParameters;
	for(;;){
		atc25_led_running(500);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
int main(void)
{
	//initial clock
	systick_config();
	//init LED and Button
	atc25_led_init();
	/*
	atc25_btn1_init(KEY_MODE_EXTI);
	atc25_btn2_init(KEY_MODE_EXTI);

	//init USART0
	uart3_init();
	usart1_init();
	
	nvic_irq_enable(UART3_IRQn,0,0);
	//allow interruption by USART0 when a word is ready to received
	usart_interrupt_enable(UART3,USART_INT_RBNE);
	//running led to welcome
	//atc25_led_running(200);
	
	*/
	xTaskCreate(vLEDTask, "LED TASK", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	vTaskStartScheduler();
	for(;;);
}
