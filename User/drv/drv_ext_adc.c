#include "bsp_soft_spi.h"
#include "main.h"

#include <stdint.h>

#define RTD_READ_TIMES              (2)

#define AD7124_REG_STATUS           (0x00)
#define AD7124_REG_ADC_CONTROL      (0x01)
#define AD7124_REG_DATA             (0x02)
#define AD7124_REG_IO_CONTROL1      (0x03)
#define AD7124_REG_ID               (0x05)

extern osSemaphoreId g_EXTADC_StartupHandle;
extern osSemaphoreId g_EXTADC_DoneHandle;

// #define TEST_BUF_TX_SIZE            (8192)
// static char s_TxBuf[TEST_BUF_TX_SIZE] = {0};

typedef enum {
    AD7124_REG_CHANNEL0 = 0x09,
    AD7124_REG_CHANNEL1,
    AD7124_REG_CHANNEL2,
    AD7124_REG_CHANNEL3,
    AD7124_REG_CHANNEL4,
    AD7124_REG_CHANNEL5,
    AD7124_REG_CHANNEL6,
    AD7124_REG_CHANNEL7,
    AD7124_REG_CHANNEL8,
    AD7124_REG_CHANNEL9,
    AD7124_REG_CHANNEL10,
    AD7124_REG_CHANNEL11,
    AD7124_REG_CHANNEL12,
    AD7124_REG_CHANNEL13,
    AD7124_REG_CHANNEL14,
    AD7124_REG_CHANNEL15,
}AD7124ChannelReg;

typedef enum {
    AD7124_REG_CONFIG0 = 0x19,
    AD7124_REG_CONFIG1,
    AD7124_REG_CONFIG2,
    AD7124_REG_CONFIG3,
    AD7124_REG_CONFIG4,
    AD7124_REG_CONFIG5,
    AD7124_REG_CONFIG6,
    AD7124_REG_CONFIG7,
}AD7124ConfigReg;

typedef enum {
    AD7124_REG_FILTER0 = 0x21,
    AD7124_REG_FILTER1,
    AD7124_REG_FILTER2,
    AD7124_REG_FILTER3,
    AD7124_REG_FILTER4,
    AD7124_REG_FILTER5,
    AD7124_REG_FILTER6,
    AD7124_REG_FILTER7,
}AD7124FilterReg;

typedef enum {
    AIN0 = 0,
    AIN1,
    AIN2,
    AIN3,
    AIN4,
    AIN5,
    AIN6,
    AIN7,
    AIN8,
    AIN9,
    AIN10,
    AIN11,
    AIN12,
    AIN13,
    AIN14,
    AIN15,
    TEMP_SENSOR,
    AVSS,
    INT_REF_P,
}AD7124In;

typedef enum {
    SETUP0 = 0,
    SETUP1,
    SETUP2,
    SETUP3,
    SETUP4,
    SETUP5,
    SETUP6,
    SETUP7,
}AD7124Setup;

typedef enum {
    INT_REF_DISENABLE = 0,
    INT_REF_ENABLE,
}AD7124IntRef;

typedef enum {
    LOW_POWER = 0,
    MID_POWER,
    FULL_POWER,
}AD7124PowerMode;

typedef enum {
    CONTINUOUS_CONVERT = 0,
    SINGLE_CONVERT,
    STANDBY,
    POWER_DOWN,
    IDLE_MODE,
}AD7124Mode;

typedef enum {
    REFIN1 = 0,
    REFIN2,
    INT_REF,
    AVDD_AVSS,
}AD7124RefSel;

typedef enum {
    GAIN_1 = 0,
    GAIN_2,
    GAIN_4,
    GAIN_8,
    GAIN_16,
    GAIN_32,
    GAIN_64,
    GAIN_128,
}AD7124PGA;

#define PT1000_PGA4                 (2796.2027f)

#define BATTERY_COEFFICIENT         (610080.58f)
#define TEMPERATURE_COEFFICIENT     (13584.0f)

// char prntstr[100];


static inline void DoNoting(void) {}

typedef struct {
    bool RefBufP;
    bool RefBufM;
    bool InBufP;
    bool InBufM;
    AD7124RefSel refSel;
    AD7124PGA    gain;
}AD7124Config;

typedef struct {
    uint16_t fs;
}AD7124Filter;

typedef struct {
    AD7124In P;
    AD7124In M;
}AD7124Channel;

typedef struct {
    AD7124IntRef    intRef;
    AD7124PowerMode powerMode;
    AD7124Mode      runMode;
}AD7124Control;

typedef struct {
    AD7124Config  cfg;
    AD7124Filter  filter;
    AD7124Channel channel;
    AD7124Control ctrl;
    void (*funIout)(void);
    void (*funIoutOff)(void);
    uint8_t readTimes;
    float   div;
}AD7124Handle;


static inline void Plug1Iout150uA(void);
static inline void Plug2Iout150uA(void);
static inline void IoutOff(void);
static inline void ExtAdcSpiInit(void);

static AD7124Handle s_Plug1PT1000 = {
    .cfg = {
        .RefBufP = true,
        .RefBufM = true,
        .InBufP  = true,
        .InBufM  = true,
        .refSel  = REFIN2,
        .gain    = GAIN_4,
    },
    .filter = {
        .fs = 96,
    },
    .channel = {
        .P = AIN5,
        .M = AIN6,
    },
    .ctrl = {
        .intRef    = INT_REF_DISENABLE,
        .powerMode = MID_POWER,
        .runMode   = CONTINUOUS_CONVERT,
    },
    .funIout    = Plug1Iout150uA,
    .funIoutOff = IoutOff,
    .readTimes  = 2,
    .div        = PT1000_PGA4,
};

static AD7124Handle s_Plug2PT1000 = {
    .cfg = {
        .RefBufP = true,
        .RefBufM = true,
        .InBufP  = true,
        .InBufM  = true,
        .refSel  = REFIN2,
        .gain    = GAIN_4,
    },
    .filter = {
        .fs = 96,
    },
    .channel = {
        .P = AIN9,
        .M = AIN10,
    },
    .ctrl = {
        .intRef    = INT_REF_DISENABLE,
        .powerMode = MID_POWER,
        .runMode   = CONTINUOUS_CONVERT,
    },
    .funIout    = Plug2Iout150uA,
    .funIoutOff = IoutOff,
    .readTimes  = 2,
    .div        = PT1000_PGA4,
};

static AD7124Handle s_Temperature = {
    .cfg = {
        .RefBufP = false,
        .RefBufM = false,
        .InBufP  = true,
        .InBufM  = true,
        .refSel  = INT_REF,
        .gain    = GAIN_1,
    },
    .filter = {
        .fs = 2,
    },
    .channel = {
        .P = TEMP_SENSOR,
        .M = AVSS,
    },
    .ctrl = {
        .intRef    = INT_REF_ENABLE,
        .powerMode = MID_POWER,
        .runMode   = SINGLE_CONVERT,
    },
    .funIout    = DoNoting,
    .funIoutOff = DoNoting,
    .readTimes  = 1,
    .div        = TEMPERATURE_COEFFICIENT,
};

static AD7124Handle s_Battery = {
    .cfg = {
        .RefBufP = false,
        .RefBufM = false,
        .InBufP  = true,
        .InBufM  = true,
        .refSel  = INT_REF,
        .gain    = GAIN_2,
    },
    .filter = {
        .fs = 2,
    },
    .channel = {
        .P = AIN1,
        .M = AVSS,
    },
    .ctrl = {
        .intRef    = INT_REF_ENABLE,
        .powerMode = MID_POWER,
        .runMode   = SINGLE_CONVERT,
    },
    .funIout    = DoNoting,
    .funIoutOff = DoNoting,
    .readTimes  = 1,
    .div        = BATTERY_COEFFICIENT,
};

typedef struct {
    AD7124Handle* plug1Handle;//如果为NULL则不测量
    AD7124Handle* plug2Handle;//如果为NULL则不测量
    uint8_t delay1;//上电或者唤醒后延时时间(ms)
    uint8_t delay2;//开启Iout后延时时间(ms)
    bool    isPrePwrOff;//上一次测量完是关闭电源则为true
    bool    isPwrOff;//测量完关闭电源则为true，否则进入deepSleep
}ExtUart1Cmd;

ExtUart1Cmd s_Uart1Cmd = {
    .plug1Handle = NULL,
    .plug2Handle = NULL, 
    .delay1 = 3,
    .delay2 = 3,
    .isPrePwrOff = false,
    .isPwrOff    = false,
};

static SoftSpiHandle s_AdcSpi;

static inline void AD7124MisoInit(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ADC_MISO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIOInitHelper(ADC_MISO_GPIO_Port, &GPIO_InitStruct);
}

static inline void AD7124MisoDeInit(void) {
    GPIODeInitHelper(ADC_MISO_GPIO_Port, ADC_MISO_Pin);
}

static inline void AD7124RdyPinInit(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ADC_MISO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIOInitHelper(ADC_MISO_GPIO_Port, &GPIO_InitStruct);
    __HAL_GPIO_EXTI_CLEAR_IT(ADC_MISO_Pin);
    HAL_NVIC_SetPriority(ADC_MISO_EXTI_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(ADC_MISO_EXTI_IRQn);
}

static inline void AD7124RdyPinDeInit(void) {
    HAL_NVIC_DisableIRQ(ADC_MISO_EXTI_IRQn);
    AD7124MisoDeInit();
    AD7124MisoInit();
}

static inline void AD7124Read( uint8_t reg, uint8_t* buf, uint8_t size ) {
    uint8_t comms = reg | 0x40;     //0x40 read register
    SoftSpiTransmit(&s_AdcSpi, &comms, 1);
    SoftSpiReceive(&s_AdcSpi, buf, size);
}

static inline void AD7124Write( uint8_t reg, uint8_t* buf, uint8_t size ) {
    SoftSpiTransmit(&s_AdcSpi, &reg, 1);
    SoftSpiTransmit(&s_AdcSpi, buf, size);
}

static inline void AD7124PwrOn(void) {
    HAL_GPIO_WritePin(ADC_PWR_GPIO_Port, ADC_PWR_Pin, GPIO_PIN_RESET);
}

void AD7124PwrOff(void) {
    GPIODeInitHelper( ADC_MISO_GPIO_Port, ADC_MISO_Pin);
    HAL_GPIO_WritePin( ADC_PWR_GPIO_Port, ADC_PWR_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin( ADC_SCK_GPIO_Port,  ADC_SCK_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin( ADC_MOSI_GPIO_Port,  ADC_MOSI_Pin, GPIO_PIN_RESET);
}

static inline void WakeUp(void) {
    if( s_Uart1Cmd.isPrePwrOff ) {
        AD7124PwrOn();
        ExtAdcSpiInit();
    }
    else {
        AD7124MisoInit();
        uint8_t buf[8] = {0};
        memset(buf, 0xFF, sizeof(buf));
        SoftSpiTransmit(&s_AdcSpi, buf, sizeof(buf));
    }
    osDelay(s_Uart1Cmd.delay1);
}

static inline void ControlSet(AD7124IntRef ref, AD7124PowerMode power, AD7124Mode mode ) {
    uint8_t cmd[2] = {0x04, 0x00};//set Data + status, Internal clock & output off
    cmd[0] |= (uint8_t)ref;
    cmd[1] = (uint8_t)power << 6 | (uint8_t)mode << 2;
    AD7124Write(AD7124_REG_ADC_CONTROL, cmd, sizeof(cmd));
}

static inline void StandbyMode(void) {
    ControlSet(INT_REF_DISENABLE, LOW_POWER, STANDBY);
}

static inline void IdleMode(void) {
    ControlSet(INT_REF_DISENABLE, LOW_POWER, IDLE_MODE);
}

static inline void PowerDown(void) {
    ControlSet(INT_REF_DISENABLE, LOW_POWER, POWER_DOWN);
}

static inline void DeepSleep(void) { //必须先standby 再 power down
    if( s_Uart1Cmd.isPwrOff ) {
        AD7124PwrOff();
    }
    else {
        StandbyMode();
        StandbyMode();
        PowerDown();
        PowerDown();
        AD7124MisoDeInit();
    }
    s_Uart1Cmd.isPrePwrOff = s_Uart1Cmd.isPwrOff;
}

static inline void ConfigSet(AD7124ConfigReg reg, const AD7124Config* cfg) {
    uint8_t bufP = (cfg->RefBufP ? 0x01 : 0x00);
    uint8_t bufM = (cfg->RefBufM ? 0x01 : 0x00);
    uint8_t inP  = (cfg->InBufP ? 0x01 : 0x00);
    uint8_t inM  = (cfg->InBufM ? 0x01 : 0x00);
    uint8_t cmd[2] = { 0x08 | bufP, 
                       0x00 | bufM << 7 | inP << 6 | inM << 5 | (uint8_t)cfg->refSel << 3 | (uint8_t)cfg->gain };
    AD7124Write((uint8_t)reg, cmd, sizeof(cmd));
}

static inline void FilterSet(AD7124FilterReg reg, uint16_t fsValue) {
    uint8_t cmd[3] = {0x16, (fsValue >> 8) & 0x07, fsValue};
    AD7124Write((uint8_t)reg, cmd, sizeof(cmd));
}

static inline void Plug1Iout150uA(void) {
    uint8_t cmd[3] = {0x00, 0x11, 0x34};
    AD7124Write(AD7124_REG_IO_CONTROL1, cmd, sizeof(cmd));
    vTaskDelay(s_Uart1Cmd.delay2);
}

static inline void Plug2Iout150uA(void) {
    uint8_t cmd[3] = {0x00, 0x11, 0xBC};
    AD7124Write(AD7124_REG_IO_CONTROL1, cmd, sizeof(cmd));
    vTaskDelay(s_Uart1Cmd.delay2);
}

static inline void IoutOff(void) {
    uint8_t cmd[3] = {0x00, 0x00, 0x00};
    AD7124Write(AD7124_REG_IO_CONTROL1, cmd, sizeof(cmd));
}

static inline void ChannelSet(AD7124ChannelReg reg, AD7124Setup setup, AD7124In inP, AD7124In inM) {
    uint16_t tmp = 0x8000 | (uint8_t)setup << 12 | (uint8_t)inP << 5 | (uint8_t)inM;
    uint8_t cmd[2] = {tmp >> 8, tmp};
    AD7124Write((uint8_t)reg, cmd, sizeof(cmd));
}

static inline void ExtAdcSpiInit(void) {
    GpioHandle clk = { 
        .port = ADC_SCK_GPIO_Port,
        .pin  = ADC_SCK_Pin,
    };

    GpioHandle miso = { 
        .port = ADC_MISO_GPIO_Port,
        .pin  = ADC_MISO_Pin,
    };

    GpioHandle mosi = { 
        .port = ADC_MOSI_GPIO_Port,
        .pin  = ADC_MOSI_Pin,
    };

    SoftSpiInit( &s_AdcSpi,
                 SOFT_SPI_POLARITY_HIGH,
                 SOFT_SPI_PHASE_1EDGE,
                 true,// bool isMSBFirst,
                 NULL,// GpioHandle* nss,
                 &clk,
                 &miso,
                 &mosi,
                 GPIO_SPEED_FREQ_LOW,
                 1 );
}

static inline void ExtAdcInit(void) {
    ExtAdcSpiInit();

    AD7124PwrOn();
    osDelay(5);

    WakeUp();
    uint8_t id = 0;
    AD7124Read(AD7124_REG_ID, &id, 1);
    DbgTrace("AD7124 ID = %02x\r\n", id);

    DeepSleep();
}

static inline float ConvertHelper(const AD7124Handle* h) {
    
    char prntstr[30];
    uint32_t start = osKernelSysTick();

    IdleMode();
    ConfigSet(AD7124_REG_CONFIG0, &h->cfg);
    FilterSet(AD7124_REG_FILTER0, h->filter.fs);
    ChannelSet(AD7124_REG_CHANNEL0, SETUP0, h->channel.P, h->channel.M);

    h->funIout();

    uint32_t notUsed = 0;
    //while(pdTRUE == xTaskNotifyWait( 0, 0xFFFFFFFFuL, &notUsed, 0 ));//清除可能的已发送事件
    /* clear semaphore*/
    xSemaphoreTake(g_EXTADC_DoneHandle, 0);
    ControlSet(h->ctrl.intRef, h->ctrl.powerMode, h->ctrl.runMode);
    
    float res = 0.0f;
    uint8_t buf[4] = {0};

    for(int i = 0; i < h->readTimes; ++i) {
        AD7124RdyPinInit();
        if(pdTRUE != xSemaphoreTake( g_EXTADC_DoneHandle, 200) ) {
            AD7124RdyPinDeInit();
            DbgTrace("time out!\r\n");
        }
        else {
            AD7124RdyPinDeInit();
            AD7124Read(AD7124_REG_DATA, buf, 4);
            int32_t tmp = (int32_t)( (buf[0] << 16 | buf[1] << 8 | buf[2]) - 0x800000uL);
            res = (float)tmp / h->div;
            if( h == &s_Temperature ) {
                res -= 272.5f;
            }

            if(res >= 0)
            {
                if(h == &s_Battery)
                {
                    sprintf(prntstr, "Battery voltage is %.2f\r\n", res);
                    DbgTrace(prntstr);
                    //DbgTrace("Battery voltage is %d.%02dV\r\n",(int)res,(int)(res * 100) % 100);
                }
                else if(h == &s_Temperature)
                {
                    sprintf(prntstr, "ADC die temperature is %.2f\r\n", res);
                    DbgTrace(prntstr);
                    // DbgTrace("ADC die temperature is %d.%02ddeg\r\n",(int)res,(int)(res * 100) % 100);
                }
                else if(h == &s_Plug1PT1000)
                {
                    sprintf(prntstr, "Ch1 resister is %.4f\r\n", res);
                    DbgTrace(prntstr);
                    // DbgTrace("CH1 resister is %d.%04d ohm\r\n",(int)res,(int)(res * 10000) % 10000);
                }
                else if(h == &s_Plug2PT1000)
                {
                    sprintf(prntstr, "Ch2 resister is %.4f\r\n", res);
                    DbgTrace(prntstr);
                    // DbgTrace("CH2 resister is %d.%04d ohm\r\n",(int)res,(int)(res * 10000) % 10000);
                }
                else
                {
                    DbgTrace("Unknown measurement!\r\n");
                }
            }
            else
            {
                res = -res;
                if(h == &s_Battery)
                {
                    DbgTrace("Battery voltage is %d.%02d\r\n",(int)res,(int)(res * 100) % 100);
                }
                else if(h == &s_Temperature)
                {
                    DbgTrace("ADC die temperature is %d.%02d\r\n",(int)res,(int)(res * 100) % 100);
                }
                else if(h == &s_Plug1PT1000)
                {
                    DbgTrace("CH1 resister is %d.%04d ohm\r\n",(int)res,(int)(res * 10000) % 10000);
                }
                else if(h == &s_Plug2PT1000)
                {
                    DbgTrace("CH2 resister is %d.%04d ohm\r\n",(int)res,(int)(res * 10000) % 10000);
                }
                else
                {
                    DbgTrace("Unknown measurement!\r\n");
                }
            }
        }
    }
   
    h->funIoutOff();

    AD7124Config cfg = h->cfg;
    cfg.gain = GAIN_1;
    ConfigSet(AD7124_REG_CONFIG0, &cfg);

    DbgTrace("t:%d\r\n",osKernelSysTick()-start);

    return res;
}

void ADC_ReadID(void) 
{
    ExtAdcSpiInit();

    AD7124PwrOn();
    osDelay(5);

    uint8_t id = 0;
    AD7124Read(AD7124_REG_ID, &id, 1);
    if(0x16 == id)
    {
        DbgTrace("AD7124-8 B Grade!\r\n");
    }
    else if(0x14 == id)
    {
        DbgTrace("AD7124-8!\r\n");
    }
    else
    {
        DbgTrace("Unknown device! ID = %02x\r\n", id);
    }
    AD7124PwrOff();
}

void xHandleAdcProc(void const * argument)
{
    for(;;)
    {
        xSemaphoreTake(g_EXTADC_StartupHandle, portMAX_DELAY);
        AD7124PwrOn();
        osDelay(5);
        ConvertHelper(&s_Battery);
        ConvertHelper(&s_Temperature);
        ConvertHelper(&s_Plug1PT1000);
        ConvertHelper(&s_Plug2PT1000);
        AD7124PwrOff();
    }
}