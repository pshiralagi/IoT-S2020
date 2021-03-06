/*
 * @filename	: i2c.c
 * @description	: This file contains the source code
 * @author 		: Pavan Shiralagi, Sarayu Managoli
 * @course      : Internet of Things Embedded Firmware
 * 				  https://siliconlabs.github.io/Gecko_SDK_Doc/efr32bg13/html/index.html
 * 				  Health thermometer demo project
 */

#include "i2c.h"

uint8_t read_data[2];
I2C_TransferSeq_TypeDef seq_write;
I2C_TransferSeq_TypeDef seq_read;
uint8_t write_data = 0xE5; //No Master Hold Mode HUMIDITY

uint32_t i2c_interrupt;
float Received_Data;



void I2C_Initialize()
{
	I2CSPM_Init_TypeDef i2c_init =
	{
			I2C0,
			gpioPortC,                  /* SCL port */                                 \
			10,                         /* SCL pin */                                  \
			gpioPortC,                  /* SDA port */                                 \
			11,                         /* SDA pin */                                  \
			14,                         /* Location of SCL */                          \
			16,                         /* Location of SDA */                          \
			0,                          /* Use currently configured reference clock */ \
			I2C_FREQ_STANDARD_MAX,      /* Set to standard rate  */                    \
			i2cClockHLRStandard,        /* Set to use 4:4 low/high duty cycle */       \
	};
	I2CSPM_Init(&i2c_init);
	NVIC_EnableIRQ(I2C0_IRQn);
}

void I2C_Write()
{
//	LOG_INFO("In I2C write");
	I2C_TransferReturn_TypeDef ret;

	seq_write.addr = SLAVE_ADDRESS << 1; //Left shifting the slave address
	seq_write.flags = I2C_FLAG_WRITE; //Flag for I2C write
	seq_write.buf[0].data = &write_data; //
	seq_write.buf[0].len = 1;
	ret = I2C_TransferInit(I2C0, &seq_write);

	if(ret != i2cTransferInProgress){
		LOG_ERROR("I2C write failed");
	}
}

void I2C_Read()
{
//	LOG_INFO("In I2C read");
	I2C_TransferReturn_TypeDef ret;

	seq_read.addr = SLAVE_ADDRESS << 1; //Left shifting the slave address
	seq_read.flags = I2C_FLAG_READ; //Flag for I2C read
	seq_read.buf[0].data = read_data;
	seq_read.buf[0].len = sizeof(read_data);

	ret = I2C_TransferInit(I2C0,&seq_read);

	if(ret != i2cTransferInProgress){
		LOG_ERROR("I2C read failed");
	}

}

void Get_Humidity()
{
//	LOG_INFO("read_data[0] = %d",read_data[0]);
//	LOG_INFO("read_data[1] = %d",read_data[1]);
	Received_Data = (read_data[0]<<8) + read_data[1]; //To store the temperature sensed in one Word
//	LOG_INFO("Received data = %f",Received_Data);
	Received_Data = (((125 * Received_Data)/65536) - 6); //Calculation for temperature in degree Celsius
	LOG_INFO("Humidity = %f",Received_Data);
}


void Hum_Buffer()
{
	char HumBufferChar[32]={0};
	snprintf(HumBufferChar, sizeof (HumBufferChar), "%.2f", Received_Data);
	displayPrintf(DISPLAY_ROW_HUMIDITY,HumBufferChar);

}

void I2C0_IRQHandler()
{
	I2C_TransferReturn_TypeDef interrupt = I2C_Transfer(I2C0);
	/* Transfer failure */
	if(interrupt != i2cTransferInProgress)
	{

		if(eNextState == READ_COMPLETE)
		{
			gecko_external_signal(0x06); //Setting signal event for next state
		}
		else if(eNextState == WRITE_COMPLETE)
		{
			gecko_external_signal(0x04); //Setting signal event for next state
		}

//		LOG_INFO("WRITE SUCCESS");
	}
}




