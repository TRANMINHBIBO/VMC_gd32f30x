
#include "gd32f30x_it.h"
#include "systick.h"
#include "atc_e25.h"
#include "string.h"
#include "stdio.h"
//uint8_t received_data[100];
//uint8_t rcv_index=0;

uint8_t message_count;

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    delay_decrement();
}

void EXTI10_15_IRQHandler(void)
{
	if(exti_interrupt_flag_get(EXTI_15)==SET)
	{
		
		char topic[100];
		sprintf(topic, "AT+MQTTPUB=0,1,\"lixytopic\",\"led_mod = %d, timer_mode = %d, alarm = %d, counter = %d\"\r\n", led_mode, timer_mode, alarm, counter);
		usart_string_transmit(UART3, (uint8_t*)topic);
		
		while(gpio_input_bit_get(BTN1_GPIO_PORT,BTN1_PIN)==RESET);
		//code here
		led_mode++;
		//if(mode>=4) mode=0;
		switch (led_mode)
		{
    
    case 1:
      atc25_led_flash(RED,500);
      break;
    case 2:
      atc25_led_flash(GREEN,500);
      break;
		case 3:
      atc25_led_flash(BLUE,500);
      break;
		case 4:
			atc25_led_running(200);
      break;
		case 5:
			led_mode=1;
			atc25_led_flash(RED,500);
			break;
    default:
			atc25_led_off(ALL);
		}
		//end here
		
		
		exti_interrupt_flag_clear(EXTI_15);
		
	}
	if(exti_interrupt_flag_get(EXTI_12)==SET)
	{
		neoway_connect("91.121.93.94:1883", 0, 60);
		
		MQTTMessage* messages = get_messages_for_topic("lixytopic", &message_count);
		for(uint8_t i = 0; i < message_count; i++){
			usart_string_transmit(UART3, (uint8_t*)messages[i].message);
			usart_string_transmit(UART3, (uint8_t*)"\r\n");
		}
		while(gpio_input_bit_get(BTN2_GPIO_PORT,BTN2_PIN)==RESET);
		//code here
		timer_mode++;
		switch (timer_mode)
		{
    case 1:
			atc25_led_on(RED);
			delay_1ms(500);
			atc25_led_off(RED);
			alarm=15;
			//counter=0;
      break;
    case 2:
      atc25_led_on(GREEN);
			delay_1ms(500);
			atc25_led_off(GREEN);
			alarm=30;
			//counter=0;
      break;
    case 3:
      atc25_led_on(BLUE);
			delay_1ms(500);
			atc25_led_off(BLUE);
			alarm=60;
			//counter=0;
      break;
		case 4:
      atc25_led_on(ALL);
			delay_1ms(500);
			atc25_led_off(ALL);
			alarm=120;
			//counter=0;
      break;
		case 5:
			timer_mode=1;
			atc25_led_on(RED);
			delay_1ms(500);
			atc25_led_off(RED);
			alarm=15;
			break;
    default:
			atc25_led_off(ALL);
		}
		//end here
		exti_interrupt_flag_clear(EXTI_12);
	}
	
}

