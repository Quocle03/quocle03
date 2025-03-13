#include "bh1750.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x.h"

void I2C_Write(uint8_t address, uint8_t data) {
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, address << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, data);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C1, ENABLE);
}

void I2C_Read(uint8_t address, uint8_t *buffer, uint8_t length) {
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, address << 1, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    while (length--) {
        if (length == 0)
            I2C_AcknowledgeConfig(I2C1, DISABLE);

        while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
        *buffer++ = I2C_ReceiveData(I2C1);
    }

    I2C_GenerateSTOP(I2C1, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);
}

void BH1750_Init(void) {
    I2C_Write(BH1750_ADDRESS, 0x01);  // Power ON
    I2C_Write(BH1750_ADDRESS, 0x07);  // Reset
}

void BH1750_StartMeasurement(void) {
    I2C_Write(BH1750_ADDRESS, 0x10);  // Ch? d? do liên t?c, d? phân gi?i cao
}

uint16_t BH1750_ReadLight(void) {
    uint8_t data[2];
    I2C_Read(BH1750_ADDRESS, data, 2);
    return ((data[0] << 8) | data[1]) / 1.2;
}
