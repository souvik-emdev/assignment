/*
 *  ======== ti_drivers_config.h ========
 *  Configured TI-Drivers module declarations
 *
 *  DO NOT EDIT - This file is generated for the CC26X2R1_LAUNCHXL
 *  by the SysConfig tool.
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_SYSCONFIG_PREVIEW

#define CONFIG_CC26X2R1_LAUNCHXL

#ifndef DeviceFamily_CC26X2
#define DeviceFamily_CC26X2
#endif

#include <ti/devices/DeviceFamily.h>

#include <stdint.h>

/* support C++ sources */
#ifdef __cplusplus
extern "C" {
#endif


/*
 *  ======== GPIO ========
 */

/* DIO13, LaunchPad Button BTN-1 (Left) */
#define CONFIG_GPIO_0               0

/* LEDs are active high */
#define CONFIG_GPIO_LED_ON  (1)
#define CONFIG_GPIO_LED_OFF (0)

#define CONFIG_LED_ON  (CONFIG_GPIO_LED_ON)
#define CONFIG_LED_OFF (CONFIG_GPIO_LED_OFF)


/*
 *  ======== PIN ========
 */

/* Includes */
#include <ti/drivers/PIN.h>

/* Externs */
extern const PIN_Config BoardGpioInitTable[];

/* XDS110 UART, Parent Signal: CONFIG_UART_0 TX, (DIO3) */
#define CONFIG_PIN_2    0x00000003
/* XDS110 UART, Parent Signal: CONFIG_UART_0 RX, (DIO2) */
#define CONFIG_PIN_3    0x00000002
/* LaunchPad LED Red, Parent Signal: CONFIG_GPTIMER_0 PWM Pin, (DIO6) */
#define CONFIG_PIN_0    0x00000006
/* LaunchPad LED Green, Parent Signal: CONFIG_GPTIMER_1 PWM Pin, (DIO7) */
#define CONFIG_PIN_1    0x00000007
/* LaunchPad Button BTN-1 (Left), Parent Signal: CONFIG_GPIO_0 GPIO Pin, (DIO13) */
#define CONFIG_PIN_4    0x0000000d


/*
 *  ======== PWM ========
 */

/* DIO6, LaunchPad LED Red */
#define CONFIG_PWM_0                0
/* DIO7, LaunchPad LED Green */
#define CONFIG_PWM_1                1


/*
 *  ======== Timer ========
 */

#define CONFIG_TIMER_0              0


/*
 *  ======== UART ========
 */

/*
 *  TX: DIO3
 *  RX: DIO2
 *  XDS110 UART
 */
#define CONFIG_UART_0               0


/*
 *  ======== Button ========
 */

/* DIO13, LaunchPad Button BTN-1 (Left) */
#define CONFIG_BUTTON_0             0


/*
 *  ======== GPTimer ========
 */

#define CONFIG_GPTIMER_0            0
#define CONFIG_GPTIMER_1            1
#define CONFIG_GPTIMER_2            2


/*
 *  ======== Board_init ========
 *  Perform all required TI-Drivers initialization
 *
 *  This function should be called once at a point before any use of
 *  TI-Drivers.
 */
extern void Board_init(void);

/*
 *  ======== Board_initGeneral ========
 *  (deprecated)
 *
 *  Board_initGeneral() is defined purely for backward compatibility.
 *
 *  All new code should use Board_init() to do any required TI-Drivers
 *  initialization _and_ use <Driver>_init() for only where specific drivers
 *  are explicitly referenced by the application.  <Driver>_init() functions
 *  are idempotent.
 */
#define Board_initGeneral Board_init

#ifdef __cplusplus
}
#endif

#endif /* include guard */
