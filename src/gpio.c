/*
 * gpio.c
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 *      Co-author :Pshiralagi
 */
#include "gpio.h"
#include "em_gpio.h"
#include <string.h>




/*
 * @brief : Function to initialize required GPIO pins in required mode
 *
 */

void gpioInit()
{
	/* LED initializations */
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
//	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, false);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
//	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, false);
	/*	I2C enables	*/
	GPIO_DriveStrengthSet(I2C0_SCL_PORT, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(I2C0_SCL_PORT, I2C0_SCL_PIN, gpioModePushPull, false);
	GPIO_DriveStrengthSet(I2C0_SDA_PORT, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(I2C0_SDA_PORT, I2C0_SDA_PIN, gpioModePushPull, false);
	GPIO_DriveStrengthSet(I2C0_ENABLE_PORT, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(I2C0_ENABLE_PORT, I2C0_ENABLE_PIN, gpioModePushPull, false);
	GPIO_PinOutSet(I2C0_ENABLE_PORT,I2C0_ENABLE_PIN);
	/*	PB0 Button initialization */
	GPIO_PinModeSet(PB0_Port, PB0_Pin, gpioModeInputPull, true);
	/*	PB1 Button initialization */
	GPIO_PinModeSet(PB1_Port, PB1_Pin, gpioModeInputPull, true);
}

void gpioLed0SetOn()
{
	GPIO_PinOutSet(LED0_port,LED0_pin);
}
void gpioLed0SetOff()
{
	GPIO_PinOutClear(LED0_port,LED0_pin);
}
void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED1_port,LED1_pin);
}
void gpioLed1SetOff()
{
	GPIO_PinOutClear(LED1_port,LED1_pin);
}

/*	@brief : Function to toggle LED based on flag set in interrupt (Assignment 2)	*/
void toggleLed(void)
{
	static uint8_t irq_flg = 0;
	  if (irq_flg == 0)
	  {
		  gpioLed0SetOn();
		  irq_flg = 1;
	  }
	  else if (irq_flg == 1)
	  {
		  gpioLed0SetOff();
		  irq_flg = 0;
	  }
}


void gpioEnableDisplay(void)
{
	GPIO_PinOutSet(LCD_Port,LCD_ENABLE);
}
void gpioSetDisplayExtcomin(bool state)
{
	if(state == true)
		GPIO_PinOutSet(LCD_Port,LCD_EXTCOMIN);
	else
		GPIO_PinOutClear(LCD_Port,LCD_EXTCOMIN);
}

/***************************************************************************//**
 * This is a callback function that is invoked each time a GPIO interrupt
 * in one of the pushbutton inputs occurs. Pin number is passed as parameter.
 *
 * @param[in] pin  Pin number where interrupt occurs
 *
 * @note This function is called from ISR context and therefore it is
 *       not possible to call any BGAPI functions directly. The button state
 *       change is signaled to the application using gecko_external_signal()
 *       that will generate an event gecko_evt_system_external_signal_id
 *       which is then handled in the main loop.
 ******************************************************************************/
void gpioint(uint8_t pin)
{
  if (pin == PB0_Pin)
  {
    gecko_external_signal(0x40);
  }
}

/***************************************************************************//**
 * Enable button interrupts for PB0. Both GPIOs are configured to trigger
 * an interrupt on the rising edge (button released).
 ******************************************************************************/
void enable_button_interrupts(void)
{
  GPIOINT_Init();

  /* configure interrupt for PB0 and PB1, both falling and rising edges */
  GPIO_ExtIntConfig(PB0_Port, PB0_Pin, PB0_Pin, true, true, true);

  /* register the callback function that is invoked when interrupt occurs */
  GPIOINT_CallbackRegister(PB0_Pin, gpioint);
}

void redAlert(void)
{
	gpioLed0SetOn();
	gpioLed1SetOn();
}

void clearAlert(void)
{
	gpioLed0SetOff();
	gpioLed1SetOff();
}


void pirInit(void)
{
	//Pin D 13 is used as input
	GPIO_PinModeSet(MOTION_PORT, MOTION_PIN, gpioModeInput, 0);
	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIOINT_Init();
	GPIO_ExtIntConfig(MOTION_PORT, MOTION_PIN, MOTION_PIN, true, true, true);
	GPIOINT_CallbackRegister(MOTION_PIN, motionDetected);
//	LOG_ERROR("PIR Initialized");
}


void motionDetected(uint8_t pin)
{
	CORE_DECLARE_IRQ_STATE;
	if(pin == MOTION_PIN)
	{
		if(GPIO_PinInGet(MOTION_PORT, MOTION_PIN) == 1)
		{
			CORE_ENTER_CRITICAL();
			gecko_external_signal(0x50);
			CORE_EXIT_CRITICAL();
		}
	}

}

void LPM_On(void)
{
	//Enables all the pins to turn ON Load Power
//	GPIO_PinOutSet(I2C0_ENABLE_PORT,I2C0_ENABLE_PIN);
	GPIO_PinOutSet(I2C0_SCL_PORT, I2C0_SCL_PIN);
	GPIO_PinOutSet(I2C0_SDA_PORT, I2C0_SDA_PIN);

	timerWaitMs(80); //Time needed for power to stabilize = 80ms
}

void LPM_Off(void)
{
	//Disables all the pins to turn OFF Load Power
//	GPIO_PinOutClear(I2C0_ENABLE_PORT, I2C0_ENABLE_PIN);
	GPIO_PinOutClear(I2C0_SCL_PORT, I2C0_SCL_PIN);
	GPIO_PinOutClear(I2C0_SDA_PORT, I2C0_SDA_PIN);
}
