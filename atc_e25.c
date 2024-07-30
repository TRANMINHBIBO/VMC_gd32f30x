#include "atc_e25.h"
#include "gd32f30x.h"
#include "gd32f30x_gpio.h"
#include "gd32f30x_rcu.h"
#include "systick.h"
#include "gd32f30x_exti.h"
#include "string.h"
#include "gd32f30x_adc.h"
#include "stdio.h"
uint8_t received_data[100];
uint8_t rcv_index=0;
uint8_t received_data2[100];
MQTTTopic topics[MAX_TOPICS];
//init led RGB on board
void atc25_led_init(void)
{
	rcu_periph_clock_enable(LED_BLUE_CLK);
	gpio_init(LED_BLUE_PORT,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,LED_BLUE_PIN);
	gpio_bit_set(LED_BLUE_PORT,LED_BLUE_PIN); //turn off led blue
	
	rcu_periph_clock_enable(LED_GREEN_CLK);
	gpio_init(LED_GREEN_PORT,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,LED_GREEN_PIN|LED_RED_PIN);
	gpio_bit_set(LED_GREEN_PORT,LED_GREEN_PIN|LED_RED_PIN); //turn off led green & red
}

void atc25_btn1_init(keymode_typedef_enum btn_mode)
{
	if(btn_mode!=KEY_MODE_EXTI)
		{
			rcu_periph_clock_enable(BTN1_GPIO_CLK);
			gpio_init(BTN1_GPIO_PORT,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,BTN1_PIN);
		}
	else
		{
			rcu_periph_clock_enable(BTN1_GPIO_CLK);
			rcu_periph_clock_enable(RCU_AF);
			gpio_init(BTN1_GPIO_PORT,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,BTN1_PIN);
			
			nvic_irq_enable(BTN1_EXTI_IRQn,1U,0U);
			/* connect key EXTI line to key GPIO pin */
      gpio_exti_source_select(BTN1_EXTI_PORT_SOURCE,BTN1_EXTI_PIN_SOURCE);
			/* configure key EXTI line */
			exti_init(BTN1_EXTI_LINE,EXTI_INTERRUPT,EXTI_TRIG_BOTH);
			//exti_interrupt_enable(BTN1_EXTI_LINE);
			exti_interrupt_flag_clear(BTN1_EXTI_LINE);
		}
}

void atc25_btn2_init(keymode_typedef_enum btn_mode)
{
	if(btn_mode!=KEY_MODE_EXTI)
		{
			rcu_periph_clock_enable(BTN2_GPIO_CLK);
			gpio_init(BTN2_GPIO_PORT,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,BTN2_PIN);
		}
	else
		{
			rcu_periph_clock_enable(BTN2_GPIO_CLK);
			rcu_periph_clock_enable(RCU_AF);
			gpio_init(BTN2_GPIO_PORT,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,BTN2_PIN);
			
			nvic_irq_enable(BTN2_EXTI_IRQn,2U,0U);
			/* connect key EXTI line to key GPIO pin */
      gpio_exti_source_select(BTN2_EXTI_PORT_SOURCE,BTN2_EXTI_PIN_SOURCE);
			/* configure key EXTI line */
			exti_init(BTN2_EXTI_LINE,EXTI_INTERRUPT,EXTI_TRIG_BOTH);
			//exti_interrupt_enable(BTN2_EXTI_LINE);
			exti_interrupt_flag_clear(BTN2_EXTI_LINE);
		}
}

//turn on specific LED (RED/GREEN/BLUE) or All LED on the board
void atc25_led_on(ledlist_typedef_enum led_num)
{
	if(led_num==RED)
		gpio_bit_reset(LED_RED_PORT,LED_RED_PIN);
	else if(led_num==GREEN)
		gpio_bit_reset(LED_GREEN_PORT,LED_GREEN_PIN);
	else if(led_num==BLUE)
		gpio_bit_reset(LED_BLUE_PORT,LED_BLUE_PIN);
	else
	{
		gpio_bit_reset(LED_GREEN_PORT,LED_GREEN_PIN|LED_RED_PIN);
		gpio_bit_reset(LED_BLUE_PORT,LED_BLUE_PIN);
	}
}

//turn off LED
void atc25_led_off(ledlist_typedef_enum led_num)
{
	if(led_num==RED)
		gpio_bit_set(LED_RED_PORT,LED_RED_PIN);
	else if(led_num==GREEN)
		gpio_bit_set(LED_GREEN_PORT,LED_GREEN_PIN);
	else if(led_num==BLUE)
		gpio_bit_set(LED_BLUE_PORT,LED_BLUE_PIN);
	else
	{
		gpio_bit_set(LED_GREEN_PORT,LED_GREEN_PIN|LED_RED_PIN);
		gpio_bit_set(LED_BLUE_PORT,LED_BLUE_PIN);
	}
}
//LED running program
void atc25_led_running(uint32_t time_delay_ms)
{
	atc25_led_on(RED);
	delay_1ms(time_delay_ms);
	atc25_led_off(ALL);
	atc25_led_on(GREEN);
	delay_1ms(time_delay_ms);
	atc25_led_off(ALL);
	atc25_led_on(BLUE);
	delay_1ms(time_delay_ms);
	atc25_led_off(ALL);
	atc25_led_on(RED);
	delay_1ms(time_delay_ms);
	atc25_led_off(ALL);
	atc25_led_on(GREEN);
	delay_1ms(time_delay_ms);
	atc25_led_off(ALL);
	atc25_led_on(BLUE);
	delay_1ms(time_delay_ms);
	atc25_led_off(ALL);
}

void uart3_init(void){
	rcu_periph_clock_enable(RCU_UART3);
	gpio_init(GPIOC,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);
	gpio_init(GPIOC,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_11);
	usart_enable(UART3);
	usart_word_length_set(UART3,USART_WL_8BIT);
	usart_stop_bit_set(UART3,USART_STB_1BIT);
	usart_baudrate_set(UART3,115200U);
	usart_receive_config(UART3,USART_RECEIVE_ENABLE);
	usart_transmit_config(UART3,USART_TRANSMIT_ENABLE);
	usart_parity_config(UART3,USART_PM_NONE);
	usart_hardware_flow_cts_config(UART3,USART_CTS_DISABLE);
	usart_hardware_flow_rts_config(UART3,USART_RTS_DISABLE);
}

void usart1_init(void){
	rcu_periph_clock_enable(RCU_USART1);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
	usart_enable(USART1);
	usart_word_length_set(USART1, USART_WL_8BIT);
	usart_stop_bit_set(USART1, USART_STB_1BIT);
	usart_baudrate_set(USART1, 115200U);
	usart_receive_config(USART1, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
	usart_parity_config(USART1, USART_PM_NONE);
	usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);
	usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);
}

void usart_string_transmit(uint32_t usart_periph, uint8_t data[])
{
	for(uint8_t i=0;i<strlen(data); i++)
	{
		usart_data_transmit(usart_periph,data[i]);
		while(RESET == usart_flag_get(usart_periph, USART_FLAG_TBE));
	}
}

void atc25_led_flash(ledlist_typedef_enum led_num, uint32_t time_ms)
{
	atc25_led_off(ALL);
	atc25_led_on(led_num);
	delay_1ms(time_ms);
	atc25_led_off(led_num);
	delay_1ms(time_ms);
	atc25_led_on(led_num);
	delay_1ms(time_ms);
	atc25_led_off(led_num);
	delay_1ms(time_ms);
}

bool check = FALSE;
void UART3_IRQHandler(void){
	if(usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE) == SET){		
		received_data[rcv_index++] = usart_data_receive(UART3);		
		if(rcv_index == 100) rcv_index = 0;
		if(received_data[rcv_index - 1] == 13){
			received_data[rcv_index] = 10;
			for(uint8_t i = 0; i <= rcv_index; i++) received_data2[i] = received_data[i];
			
			for(uint8_t i = 0; i <= rcv_index; i++) received_data[i] = 0;
			rcv_index = 0;
		}
		check = TRUE;
		usart_interrupt_flag_clear(UART3, USART_INT_FLAG_RBNE);
	}
	else{
		check = FALSE;
	}
}

bool atc25_check_string(char* str){
	if(check && (strstr((char*)received_data2, str) != NULL)){
		for(uint8_t i = 0; i <= 99; i++) received_data2[i] = 0;	
		return TRUE;
	}
	else{
		return FALSE;
	}
}
void neoway_init(void){
	while(!atc25_check_string("OK")){
		usart_string_transmit(UART3,(uint8_t*)"AT\r\n");
		delay_1ms(200);
	}
	atc25_led_flash(RED, 200);
	while(!atc25_check_string("READY")){
		usart_string_transmit(UART3, (uint8_t*)"AT+CPIN?\r\n");
		delay_1ms(200);
	}
	atc25_led_flash(GREEN, 200);
	while(atc25_check_string("+CSQ: 99,99")){
		usart_string_transmit(UART3, (uint8_t*)"AT+CSQ\r\n");
		delay_1ms(200);
	}
	atc25_led_flash(BLUE, 200);
	while(!atc25_check_string("0,1") && !atc25_check_string("0,5")){
		usart_string_transmit(UART3, (uint8_t*)"AT+CEREG?\r\n");
		delay_1ms(200);
	}
	atc25_led_flash(RED, 200);
	while(!atc25_check_string("OK")){
		usart_string_transmit(UART3, (uint8_t*)"AT+XIIC=1\r\n");
		delay_1ms(200);
	}
	atc25_led_flash(GREEN, 200);
	while(!atc25_check_string("OK")){
		usart_string_transmit(UART3, (uint8_t*)"AT+XIIC?\r\n");
		delay_1ms(200);
	}
	atc25_led_flash(BLUE, 200);
	while(!atc25_check_string("OK")){
		neoway_set_param("mqttx_1819203438", "", "");
		delay_1ms(200);
	}
	atc25_led_flash(RED, 200);
	while(!atc25_check_string("OK")){
		neoway_connect("91.121.93.94:1883", 0, 60);
		delay_1ms(300);
	}
	atc25_led_flash(GREEN, 200);	
}

void neoway_set_param(char* clientID, char* username, char* password){
	char command[100];
	sprintf(command, "AT+MQTTCONNPARAM=\"%s\",\"%s\",\"%s\"\r\n", clientID, username, password);
	usart_string_transmit(UART3, (uint8_t*)command);
}
void neoway_connect(char* host, uint8_t clean, uint8_t keep_alive){
	char command[100];
	sprintf(command, "AT+MQTTCONN=\"%s\",%d,%d\r\n", host, clean, keep_alive);
	usart_string_transmit(UART3, (uint8_t*)command);
}

void neoway_subscribe(char* topic_name, uint8_t qos){
	while(!atc25_check_string("OK")){
		char command[100];
		sprintf(command, "AT+MQTTSUB=\"%s\", %d\r\n", topic_name, qos);
		usart_string_transmit(UART3, (uint8_t*)command);
		delay_1ms(200);
	}
	atc25_led_on(ALL);
	delay_1ms(1000);
	atc25_led_off(ALL);
}

void neoway_pub(uint8_t retained, uint8_t qos, char* topicname, char* message){
	char command[100];
	sprintf(command, "AT+MQTTPUB=%d,%d,\"%s\",\"%s\"\r\n", retained, qos, topicname, message);
	usart_string_transmit(UART3, (uint8_t*)command);
}

void neoway_received_string(char* topic_name) {
  if(strstr((char*)received_data2, topic_name)){
		char* start = NULL;
		char* end = NULL;
		char* start_topic = NULL;
		char* end_topic = NULL;
		char command[100];
		char topic[100];
		start_topic = strchr((char*)received_data2, '\"');
		start_topic = start_topic + 1;
		if(start_topic != NULL){
			end_topic = strchr(start_topic, '\"');
			if(end_topic != NULL){
				strncpy(topic, start_topic, end_topic-start_topic);
				topic[end_topic-start_topic] = '\0';
			}
		}
		start = strstr((char*)received_data2, "\"msg\": ");
		start = start + 8;
		if(start != NULL){
			end = strchr(start, '\"');
			if(end != NULL){
				strncpy(command, start, end-start);
				command[end-start] = '\0';
				neoway_pub(0, 1, "lixytopic123", command);
			}
		}
		store_topic(topic, command);		
	}
	memset(received_data2, 0, sizeof(received_data2));
}

void init_topic(void){
	for(uint8_t i = 0; i < MAX_TOPICS; i++){
		topics[i].topic[0] = '\0';
		topics[i].message_count = 0;
	}
}

void store_topic(char* topic_name, char* message){
	for(uint8_t i = 0; i < MAX_TOPICS; i++) {
		if(strncmp(topics[i].topic, topic_name, MAX_TOPIC_NAME_LENGTH) == 0) {
			if(topics[i].message_count < MAX_MESSAGES_PER_TOPIC) {
				strncpy(topics[i].messages[topics[i].message_count].message, message, MAX_MESSAGE_LENGTH);
				topics[i].message_count++;
			}
			return;
		}	
	}
	for (uint8_t i = 0; i < MAX_TOPICS; i++) {
		if (topics[i].topic[0] == '\0') { 
			strncpy(topics[i].topic, topic_name, MAX_TOPIC_NAME_LENGTH);
			strncpy(topics[i].messages[0].message, message, MAX_MESSAGE_LENGTH);
			topics[i].message_count = 1;
			return;
		}
	}
}

MQTTMessage* get_messages_for_topic(char* topic_name, uint8_t* message_count) {
    for(uint8_t i = 0; i < MAX_TOPICS; i++){
			if(strncmp(topics[i].topic, topic_name, MAX_TOPIC_NAME_LENGTH) == 0){
				*message_count = topics[i].message_count;
				return topics[i].messages;
			}
    }
    *message_count = 0;
    return NULL;
}
