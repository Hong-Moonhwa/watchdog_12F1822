/* 
 * File:   main.h
 * Author: jinseok
 *
 * Created on April 19, 2016, 1:17 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#define default_live_check_time   60  
#define default_lcm_duration_time   5  
#define default_lock_open_time 3

typedef enum {
    WDT = 1,
    LCM = 2,
    LCM_SETTIME = 0x14,
    LCM_REBOOT = 0xA4
} OPERATION_MODE_t;
typedef enum {
    NORMAL = 0,
    OPEN_ONCE = 0xA1,
    OPEN_CONTINUE = 0xA2,
    CLOSE = 0xA3,
    REBOOT = 0xA4
} LCM_MODE_t;
typedef enum {
    CHECK_ALIVE = 0,
    WDT_DISABLE = 0x12,
    WDT_ENABLE = 0x13
} WDT_MODE_t;

OPERATION_MODE_t OPERATION_MODE;
LCM_MODE_t LCM_MODE;
WDT_MODE_t WDT_MODE;

uint8_t live_check_time;
uint8_t lcm_duration_time;
uint8_t lock_open_time;

uint8_t command[3];
uint8_t tick_counter;
uint8_t boot_time;

#endif	/* MAIN_H */

