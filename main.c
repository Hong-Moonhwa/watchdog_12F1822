/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using MPLAB(c) Code Configurator

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 3.15.0
        Device            :  PIC12F1822
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.20
 */

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
 */

#include "mcc_generated_files/mcc.h"
#include "main.h"

void SELECTOR_LCM() 
{
    PORTAbits.RA0 = 1;
    OPERATION_MODE = LCM;
    __delay_ms(1);              // delay for switch trigger
}
void SELECTOR_WDT() 
{
    PORTAbits.RA0 = 0;
    OPERATION_MODE = WDT;
    __delay_ms(1);              // delay for switch trigger
}
void LOCK_CNTL_HIGH() 
{
    PORTAbits.RA1 = 1;
}
void LOCK_CNTL_LOW() 
{
    PORTAbits.RA1 = 0;
}
void POWER_ON() 
{
    PORTAbits.RA2 = 1;
}
void POWER_OFF() 
{
    PORTAbits.RA2 = 0;
}
bool lock_cntl_state()
{
    if(PORTAbits.RA1)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void lock_open(uint8_t seconds)
{
    uint8_t i;
    uint8_t iteration;
    iteration = seconds * 2;
    
    if(seconds == 0)
    {
        __delay_ms(500);
    }
    else 
    {
        for(i = 0; i < iteration; i++)
        {
            __delay_ms(500);        
        }      
    }
}

void request_wdt() 
{
    EUSART_Write(0x02);
    EUSART_Write(0x30);
    EUSART_Write(0x00);
}

void RESET_POWER() 
{
    LOCK_CNTL_LOW();
    POWER_OFF();
    __delay_ms(100);
    SELECTOR_WDT();
    __delay_ms(300);
    SELECTOR_LCM();
    __delay_ms(100);
    POWER_ON();
    WDT_MODE = CHECK_ALIVE;
    tick_counter = 0;
    boot_time = 0;
}

void PIN_MANAGER_IOC(void) 
{
    // interrupt on change for group IOCAF
    if (IOCAFbits.IOCAF3 == 1) 
    {
        IOCAFbits.IOCAF3 = 0;
        // Add handler code here for Pin - RA3
        if(OPERATION_MODE == LCM)
        {
            LCM_MODE = OPEN_ONCE;
        }        
    }
}

/*
                         Main application
 */
void main(void) 
{
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    /*
     *  WDT & LCM Logic Start
     */
    // Initialize Default Value
    live_check_time = default_live_check_time;
    lcm_duration_time = default_lcm_duration_time;
    lock_open_time = default_lock_open_time;
    tick_counter = 0;
    boot_time = 0;

    // Booting Operation Start.
    LOCK_CNTL_LOW();
    SELECTOR_WDT();
    POWER_OFF();
    __delay_ms(50);
    SELECTOR_LCM();
    LCM_MODE = NORMAL;
    __delay_ms(100);
    POWER_ON();
    // Booting Operation End.
    
    while (1) 
    {
        if(OPERATION_MODE == WDT) 
        {
            switch (WDT_MODE)
            {
                int i = 0;
                case CHECK_ALIVE:
#if 0 
                    for(i = 0; i < 3; i++)
                    {
                        request_wdt();                        //request XID state
                        __delay_ms(100);                     //Wait response real value : 66, test value : 1000
      
                        if(WDT_MODE == WDT_ENABLE || WDT_MODE == WDT_DISABLE)
                        {
                            LCM_MODE = NORMAL;
                            break;
                        }
                        else
                        {
                            if(i == 2)
                            {
                                SELECTOR_LCM();
                            }                            
                        } 
                      
                    }
#endif                      
#if 0
                    if(WDT_MODE == WDT_ENABLE)
					{
                        if (tick_counter >= live_check_time) 
                    	{
                        	RESET_POWER();
                    	}                    
                    }    
#endif
                    	break;

                case WDT_ENABLE:
                    if (tick_counter >= live_check_time) 
                    {
                        RESET_POWER();
                    }
                    break;
                case WDT_DISABLE:
                    break;
            }
        }
        else if (OPERATION_MODE == LCM) 
        {
            switch (LCM_MODE) 
            {
                case NORMAL:
                    break;
                case OPEN_ONCE:
                    if(!lock_cntl_state())
                    {
                        LOCK_CNTL_HIGH();
                        lock_open(lock_open_time);
                        if(LCM_MODE == OPEN_ONCE)
                        {
                            LCM_MODE = CLOSE;
                            LOCK_CNTL_LOW();
                        }                         
                    }
                    else
                    {
                        LCM_MODE = OPEN_CONTINUE;
                    }
                    break;                    
                case OPEN_CONTINUE:
                    if(!lock_cntl_state())
                    {
                        LOCK_CNTL_HIGH();                        
                    }
                    break;
                case CLOSE:
                    if(lock_cntl_state())
                    {
                        LOCK_CNTL_LOW();                        
                    }
                    break;
            }
            if(boot_time >= lcm_duration_time)
            {
                SELECTOR_WDT();
 //               WDT_MODE = CHECK_ALIVE;
                boot_time = 0;
            }
        }
#if 0
         else if(OPERATION_MODE == 0x14)
        {
               live_check_time = command[2];
                    break; 
        }       
#endif 
        else if(OPERATION_MODE == LCM_REBOOT)
        {
            RESET_POWER();
        }
        else
        {
            /* blank */
        }
        __delay_ms(100);     
    }
}
/**
 End of File
 */