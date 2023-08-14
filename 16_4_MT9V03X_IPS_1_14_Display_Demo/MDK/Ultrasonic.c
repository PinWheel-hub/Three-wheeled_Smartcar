#include "Ultrasonic.h"
// **************************** �������� ****************************
uint16 ranging_counter = 0;															// �������
uint16 ranging_counter_return = 0;															// �������
uint8 ranging_flage = 0x00;															// ���״̬
uint8 uart_data;
uint8 dat[3];
uint8 num;
// **************************** �������� ****************************


void uart_handler (void)
{
	uart_query(SPLIT_ULTRASONIC_UART, &uart_data);
	dat[num] = uart_data;
	if(dat[0] != 0xa5)	num = 0;													//����һ�������Ƿ�Ϊ0xa5
	else				num++;

	if(num == 3)																	//������� ��ʼ��������
	{
		num = 0;
		ranging_counter = dat[1]<<8 | dat[2];										//�õ�������ģ�����ľ���
		ranging_flage = 0x01;
	}
}

void Ultrasonic_init()  //������ȥģ���ʼ��
{
	gpio_init(SEND_PORT_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);							// ��ʼ����ȥģ��ʹ������
	uart_init(SPLIT_ULTRASONIC_UART, SPLIT_ULTRASONIC_BAUD, SPLIT_ULTRASONIC_TX, SPLIT_ULTRASONIC_RX);
	uart_rx_irq(SPLIT_ULTRASONIC_UART, 2);
}

void Ultrasonic_send()  //��ȥģ�鷢��
{
	gpio_set(SEND_PORT_PIN, GPIO_HIGH);												// ����ʹ�� ��ȥģ�鷢�� ʵ�ʽ��ϵ������Ϳ�ʼ���� ������Բ��ÿ���
}

int Ultrasonic_receive() //���ؾ�����Ϣ
{
	if(ranging_flage)
	{
		ranging_flage=0x00;
		ranging_counter_return=ranging_counter;
	}
	return ranging_counter_return;
}
	