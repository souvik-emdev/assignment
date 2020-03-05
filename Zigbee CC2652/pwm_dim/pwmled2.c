/* For usleep() */
#include <unistd.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/PWM.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>
#include <ti/drivers/timer/TimerCC26XX.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/GPIO.h>
#include "currection_curve.h"

/* Driver configuration */
#include "ti_drivers_config.h"

//Global Variables
uint8_t rxBuf[10];      // Receive buffer
uint32_t wantedRxBytes; // Number of bytes received so far
UART_Handle uartHandle;
UART_Params uartParams;

uint8_t flag = 0;

Timer_Handle timerHandle;
Timer_Params timerParams;

PWM_Handle pwm1 = NULL;
PWM_Handle pwm2 = NULL;
PWM_Params pwmParams;

uint32_t lightState, currentDuty, expectedDuty, dutySteps = 10;
uint32_t dutyValue;

uint32_t fadeTime, timerCounter = 0;

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//////////////////////////////////////////////

void configSmoothDimTime(uint8_t timingValue)
{
    if (timingValue <= 0x0A)
    {
        fadeTime = 1000 * timingValue;
    }
}

void expectedDutyCalculator(uint8_t dimValue)
{
    // expectedDuty = map(dimValue, 0, 255, 0, fadeTime);
    expectedDuty = dimValue;
}

static void readCallback(UART_Handle handle, void *rxBuf, size_t size)
{
    if (((uint8_t *)rxBuf)[0] == 0x28 && ((uint8_t *)rxBuf)[((uint8_t *)rxBuf)[1] + 2] == 0x29)
    {
        if (((uint8_t *)rxBuf)[2] == 0x1)
        {
            configSmoothDimTime(((uint8_t *)rxBuf)[3]);
        }
        else if (((uint8_t *)rxBuf)[2] == 0x2)
        {
            expectedDutyCalculator(((uint8_t *)rxBuf)[3]);
        }
        else
        {
            uint8_t ackBuf[] = {0x28, 0x1, 0x5, 0x29};
            // Echo the bytes received back to transmitter

            // Start another read, with size the same as it was during first call to
            // UART_read()
            //GPIO_write(CONFIG_GPIO_LED_0, ((uint8_t *)rxBuf)[3]);
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

void timerCallbackFunction(Timer_Handle timerHandle)
{
    // UART_write(uartHandle, "timer", sizeof("timer"));
    if (expectedDuty > currentDuty)
    {
        timerCounter += dutySteps;
        currentDuty = map(timerCounter, 0, fadeTime, 0, 255);

        dutyValue = (uint32_t)(((uint64_t)PWM_DUTY_FRACTION_MAX * arr_currectionCurve[currentDuty]) / 255);

        PWM_setDuty(pwm1, dutyValue);

        dutyValue = (uint32_t)(((uint64_t)PWM_DUTY_FRACTION_MAX * arr_currectionCurve2[currentDuty]) / 255);

        PWM_setDuty(pwm2, dutyValue);
    }
    else if (expectedDuty < currentDuty)
    {
        timerCounter -= dutySteps;
        currentDuty = map(timerCounter, 0, fadeTime, 0, 255);
        dutyValue = (uint32_t)(((uint64_t)PWM_DUTY_FRACTION_MAX * arr_currectionCurve[currentDuty]) / 255);

        PWM_setDuty(pwm1, dutyValue);

        dutyValue = (uint32_t)(((uint64_t)PWM_DUTY_FRACTION_MAX * arr_currectionCurve2[currentDuty]) / 255);

        PWM_setDuty(pwm2, dutyValue);
    }
    else
    {
        //timerCounter = 0;
        if (lightState == 0x0)
        {
            dutyValue = (uint32_t)(((uint64_t)PWM_DUTY_FRACTION_MAX * 0) / 255);

            PWM_setDuty(pwm1, dutyValue);

            PWM_setDuty(pwm2, dutyValue);
        }
        else if (lightState == 0xFF)
        {
            dutyValue = (uint32_t)(((uint64_t)PWM_DUTY_FRACTION_MAX * 255) / 255);

            PWM_setDuty(pwm1, dutyValue);

            PWM_setDuty(pwm2, dutyValue);
        }
    }
}

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_0.
 */
void gpioButtonFxn0(uint_least8_t index)
{
    if (lightState)
    {
        expectedDuty = 0;
        lightState = 0;
    }
    else
    {
        expectedDutyCalculator(0xFF);
        lightState = 0xFF;
    }
}

/*
 *  ======== mainThread ========
 *  Task periodically increments the PWM duty for the on board LED.
 */
void *mainThread(void *arg0)
{
    /* Period and duty in microseconds */
    uint16_t pwmPeriod = 1000;
    uint16_t duty = 0;
    uint16_t dutyInc = 5;

    currentDuty = 0, expectedDuty = 0;
    configSmoothDimTime(6);

    /* Sleep time in microseconds */
    uint32_t time = 50000;

    /* Call driver init functions. */
    PWM_init();
    UART_init();
    Timer_init();
    GPIO_init();

    ////////////////////////////////////////////////PWM Functions sysinit////////////////
    PWM_Params_init(&pwmParams);
    // params.dutyUnits = PWM_DUTY_US;
    // params.dutyValue = 0;
    // params.periodUnits = PWM_PERIOD_US;
    // params.periodValue = pwmPeriod;
    // pwm1 = PWM_open(CONFIG_PWM_0, &params);
    pwmParams.dutyUnits = PWM_DUTY_FRACTION;
    pwmParams.dutyValue = 0;
    pwmParams.periodUnits = PWM_PERIOD_HZ;
    pwmParams.periodValue = 1000;
    pwm1 = PWM_open(CONFIG_PWM_0, &pwmParams);
    if (pwm1 == NULL)
    {
        /* CONFIG_PWM_0 did not open */
        while (1)
            ;
    }

    PWM_start(pwm1);

    pwmParams.dutyUnits = PWM_DUTY_FRACTION;
    pwmParams.dutyValue = 0;
    pwmParams.periodUnits = PWM_PERIOD_HZ;
    pwmParams.periodValue = 5000;
    pwm2 = PWM_open(CONFIG_PWM_1, &pwmParams);

    if (pwm2 == NULL)
    {
        /* CONFIG_PWM_0 did not open */
        while (1)
            ;
    }

    PWM_start(pwm2);

    ////////////////////////////////////////////////UART Functions sysinit////////////////
    UART_Params_init(&uartParams);
    uartParams.baudRate = 115200;
    uartParams.writeMode = UART_MODE_CALLBACK;
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readCallback = readCallback;
    uartParams.writeCallback = writeCallback;

    // Open the UART and initiate the first read
    uartHandle = UART_open(CONFIG_UART_0, &uartParams);
    UART_control(uartHandle, UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE, NULL);
    UART_write(uartHandle, "UART_INIT_Complete", sizeof("UART_INIT_Complete"));

    while (!flag)
        ;
    flag = 0;
    wantedRxBytes = 10;
    uint8_t rxBytes = UART_read(uartHandle, rxBuf, wantedRxBytes);

    ////////////////////////////////////////////////TIMER Functions sysinit////////////////
    // Initialize Timer parameters
    Timer_Params_init(&timerParams);
    timerParams.periodUnits = Timer_PERIOD_US;
    timerParams.period = 10000;
    timerParams.timerMode = Timer_CONTINUOUS_CALLBACK;
    timerParams.timerCallback = timerCallbackFunction;

    timerHandle = Timer_open(CONFIG_TIMER_0, &timerParams);

    Timer_start(timerHandle);

    ////////////////////////////////////////////////GPIO Functions sysinit////////////////

    /* install Button callback */
    GPIO_setCallback(CONFIG_BUTTON_0, gpioButtonFxn0);

    /* Enable interrupts */
    GPIO_enableInt(CONFIG_BUTTON_0);

    /* Loop forever incrementing the PWM duty */
    while (1)
    {
        // dutyValue = (uint32_t)(((uint64_t)PWM_DUTY_FRACTION_MAX * duty) / 1000);

        // PWM_setDuty(pwm1, dutyValue);

        // PWM_setDuty(pwm2, dutyValue);

        // duty = (duty + dutyInc);

        // if (duty == pwmPeriod || (!duty))
        // {
        //     dutyInc = -dutyInc;
        // }

        // usleep(time);
    }
}
