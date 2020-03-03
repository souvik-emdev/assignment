
/*
 *  ======== empty.c ========
 */

/* For usleep() */
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>
#include <ti/drivers/timer/TimerCC26XX.h>
#include <ti/drivers/Timer.h>
// #include <ti/drivers/Watchdog.h>

/* Driver configuration */
#include "ti_drivers_config.h"

uint8_t rxBuf[10];   // Receive buffer
uint8_t txBuf[] = {0x28, 0x02, 0x0, 0x0, 0x29};   // Transmit buffer
uint32_t wantedRxBytes;            // Number of bytes received so far
uint32_t timeoutUs = 5000;

UART_Handle handle;
UART_Params params;

Timer_Handle    timerhandle;
Timer_Params    timerparams;

uint8_t flag = 0;

static void readCallback(UART_Handle handle, void *rxBuf, size_t size)
{
    if (((uint8_t*)rxBuf)[0] == 0x28 && ((uint8_t*)rxBuf)[((uint8_t*)rxBuf)[1]+2] == 0x29)
    {
        if (((uint8_t*)rxBuf)[2] == 0x5)
        {
            Timer_stop(timerhandle);
        }
        else {
        uint8_t ackBuf[] = {0x28, 0x1, 0x5, 0x29};
       // Echo the bytes received back to transmitter

       // Start another read, with size the same as it was during first call to
       // UART_read()
       GPIO_write(CONFIG_GPIO_LED_0, ((uint8_t*)rxBuf)[3]);
       UART_write(handle, ackBuf, sizeof(ackBuf));
        }
    }
       UART_read(handle, rxBuf, wantedRxBytes);
//    }
//    else {
//        // Handle error or call to UART_readCancel()
//    }
}

// Write callback function
  static void writeCallback(UART_Handle handle, void *rxBuf, size_t size)
  {
      // Do nothing
      flag = 1;
  }

void UartTransmit( )
{
    txBuf[3] = GPIO_read(CONFIG_GPIO_LED_0);
    UART_write(handle, txBuf, sizeof(txBuf));
}

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_0.
 */
void gpioButtonFxn0(uint_least8_t index)
{
    /* Clear the GPIO interrupt and toggle an LED */
    GPIO_toggle(CONFIG_GPIO_LED_0);
    UartTransmit();
    Timer_start(timerhandle);
}

void timerCallbackFunction(Timer_Handle timerhandle)
{
    UART_write(handle, txBuf, sizeof(txBuf));
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{


    /* Call driver init functions */
    GPIO_init();
    // I2C_init();
    // SPI_init();
    UART_init();
    // Watchdog_init();
    Timer_init();

    UART_Params_init(&params);
    params.baudRate      = 115200;
    params.writeMode     = UART_MODE_CALLBACK;
    params.writeDataMode = UART_DATA_BINARY;
    params.readMode      = UART_MODE_CALLBACK;
    params.readDataMode  = UART_DATA_BINARY;
    params.readCallback  = readCallback;
    params.writeCallback  = writeCallback;
    //params.readTimeout   = timeoutUs / ClockP_tickPeriod;


    // Open the UART and initiate the first read
    handle = UART_open(CONFIG_UART_0, &params);

    UART_control(handle, UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE, NULL);

    //UART_write(handle, "(Before)", sizeof("(Before)"));
    UART_write(handle, "(Before)", 8);



    while(!flag);
    flag = 0;
    wantedRxBytes = 10;
    uint8_t rxBytes = UART_read(handle, rxBuf, wantedRxBytes);

    //UART_write(handle, rxBytes , sizeof(rxBytes));
    //UART_write(handle, "(After)", sizeof("(After)"));

    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    GPIO_setConfig(CONFIG_GPIO_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    /* install Button callback */
    GPIO_setCallback(CONFIG_GPIO_0, gpioButtonFxn0);

    /* Enable interrupts */
    GPIO_enableInt(CONFIG_GPIO_0);


    // Initialize Timer parameters
    Timer_Params_init(&timerparams);
    timerparams.periodUnits = Timer_PERIOD_US;
    timerparams.period = 800000;
    timerparams.timerMode  = Timer_CONTINUOUS_CALLBACK;
    timerparams.timerCallback = timerCallbackFunction;

    timerhandle = Timer_open(CONFIG_TIMER_0, &timerparams);

    //Timer_start(timerhandle);

    while (1);
}
