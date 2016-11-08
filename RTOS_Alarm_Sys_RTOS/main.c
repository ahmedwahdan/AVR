/*
 * FreeRTOS_TaskCreation.c
 *
 * Created: 10/19/2016 12:12:58 AM
 * Author : Master
 */ 
 #define F_CPU (8000000UL)
 #include <util/delay.h>
#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "keypad.h"
#include "LCD_Lib.h"
#include <string.h>

typedef enum
{
	Welcome,
	Recieving,
	Submit
}LCD_State_t;

#define NOTPRESSED  (0xFF)


xQueueHandle LCD_Queue = NULL;
xSemaphoreHandle CheckPasswordSemaphore = NULL;
uint8_t g_Password[10]={0};
uint8_t g_CurrentPassword[]="12345678";
void vTask_KeyPad(void  *vpParamter)
{
	uint8_t KeypadLastState = NOTPRESSED;
	uint8_t KeypadKey = 0;
	Keypad_Init();
	LCD_Write_Command(LCD_CLEAR_CMD);
	LCD_Write_String("keypadTask");
	_delay_ms(1000);
	while(1)
	{
		KeypadKey = Keypad_getkey();
		if (KeypadKey !=NOTPRESSED && KeypadLastState == NOTPRESSED)
		{
		
			xQueueSend(LCD_Queue,(void *)&KeypadKey,0); 
		    
		} 
		KeypadLastState = KeypadKey;
			
		vTaskDelay(50);
	}
	
}

void vTask_LCD(void  *vpParamter)
{
	uint8_t KeypadKey = 0;
	uint8_t passwordIndex = 0;
	BaseType_t queueState=pdFALSE;
	LCD_State_t LCD_CurrentState = Welcome;
	LCD_Initialization();
	LCD_Write_Command(LCD_CLEAR_CMD);
	LCD_Write_String("Welcome");
	_delay_ms(1000);
	
	while(1)
	{
		// block on queue for ever
		queueState = xQueueReceive(LCD_Queue,&KeypadKey,5000);
		if (queueState==pdPASS)
		{
			switch(LCD_CurrentState)
			{
				case Welcome:
				if (KeypadKey=='#')
				{
					LCD_CurrentState = Recieving;
					LCD_Write_Command(LCD_CLEAR_CMD);
					LCD_Write_String("Password:");
					LCD_Go_To_X_Y(LCD_ROW_2,LCD_POSITION_2);
				}
				break;
				case Recieving:
				switch(KeypadKey)
				{
					
					case 'C':
					//clear one character from lcd and string
					if (passwordIndex>0)
					{
						passwordIndex--;
						LCD_Write_Command(LCD_CURSOR_SHIFT_LIFT_CMD);
						LCD_Write_Character(' ');
						LCD_Write_Command(LCD_CURSOR_SHIFT_LIFT_CMD);
					}
					
					break;
					case '#':
					
					// add null to the end of the string
					g_Password[passwordIndex]='\0';
					LCD_CurrentState = Submit;
					// reset index
					passwordIndex = 0;
					//release semaphore to run check Task
					xSemaphoreGive(CheckPasswordSemaphore);
					break;
					default:
					// add to string and show on lcd
					// check if index over flow and don't accept other character
					if (passwordIndex<9)
					{
						g_Password[passwordIndex]=KeypadKey;
						passwordIndex++;
						LCD_Write_Character(KeypadKey);
						vTaskDelay(200);
						LCD_Write_Command(LCD_CURSOR_SHIFT_LIFT_CMD);
						LCD_Write_Character('*');

					}
					
					
					break;

				}
				break;
				case Submit:
				LCD_CurrentState = Welcome;
				passwordIndex=0;
				LCD_Write_Command(LCD_CLEAR_CMD);
				LCD_Write_String("Welcome");
				break;
			}
		} 
		else
		{
			if (LCD_CurrentState != Welcome)
			{
				LCD_CurrentState = Welcome;
				passwordIndex=0;
				LCD_Write_Command(LCD_CLEAR_CMD);
				LCD_Write_String("Welcome");
			}
			
		}
		// check recieved data
		
		#if 0
		switch(KeypadKey)
		{
			case 'A':
			LCD_Write_Command(LCD_CLEAR_CMD);
			LCD_Write_String("Password:");
			LCD_Go_To_X_Y(LCD_ROW_2,LCD_POSITION_2);
				// start recieveing password
			break;
			case 'C':
			//clear one character from lcd and string
			if (passwordIndex>0)
			{
				passwordIndex--;
				LCD_Write_Command(LCD_CURSOR_SHIFT_LIFT_CMD);
				LCD_Write_Character(' ');
				LCD_Write_Command(LCD_CURSOR_SHIFT_LIFT_CMD);
			}
			
			break;
			case '#':
				
			// add null to the end of the string
			g_Password[passwordIndex]='\0';
			// reset index
			passwordIndex = 0;
			LCD_CurrentState = Submit;
			//release semaphore to run check Task
			xSemaphoreGive(CheckPasswordSemaphore);
			break;
			default:
				// add to string and show on lcd
				// check if index over flow and don't accept other character
				if (passwordIndex<9)
				{
					g_Password[passwordIndex]=KeypadKey;
					passwordIndex++;
					LCD_Write_Character(KeypadKey);
					vTaskDelay(200);
					LCD_Write_Command(LCD_CURSOR_SHIFT_LIFT_CMD);
					LCD_Write_Character('*');

				}
				
				
			break;

		}
	
		#endif

	}
}

void vTask_Password_Check(void  *vpParamter)
{
	xSemaphoreTake(CheckPasswordSemaphore,0);
	LCD_Write_Command(LCD_CLEAR_CMD);
	LCD_Write_String("check");
	_delay_ms(1000);
	while(1)
	{
		
	  // block on semaphore
	  xSemaphoreTake(CheckPasswordSemaphore,portMAX_DELAY);
	  // check the password with saved password
	  if (strcmp(g_Password,g_CurrentPassword)==0)
	  {
		LCD_Write_Command(LCD_CLEAR_CMD);
		LCD_Write_String("Access granted");
	  } 
	  else
	  {
		LCD_Write_Command(LCD_CLEAR_CMD);
		LCD_Write_String("Access denied");
	  }
		// write access granted to LCD or rejected
	  	 
	}
}




int main()
{
	LCD_Initialization();
	LCD_Write_String("hello");
	
	
	BaseType_t x=0;
		LCD_Queue = xQueueCreate(10,sizeof(char));
		CheckPasswordSemaphore = xSemaphoreCreateBinary();
	
		
		x=xTaskCreate( vTask_LCD, "LCD",200,NULL, 1, NULL );
		
		x=xTaskCreate( vTask_KeyPad, "Keypad",200,NULL, 2, NULL );
		
		x=xTaskCreate( vTask_Password_Check, "check",200,NULL, 3, NULL );
		
		
		
		
		vTaskStartScheduler();
	
	while(1);
	

}
