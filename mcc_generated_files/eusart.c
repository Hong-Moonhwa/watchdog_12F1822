/**
  EUSART Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    eusart.c

  @Summary
    This is the generated driver implementation file for the EUSART driver using MPLAB(c) Code Configurator

  @Description
    This header file provides implementations for driver APIs for EUSART.
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

/**
  Section: Included Files
*/
#include "eusart.h"
#include "main.h"
/**
  Section: Macro Declarations
*/
#define EUSART_TX_BUFFER_SIZE 8
#define EUSART_RX_BUFFER_SIZE 8

/**
  Section: Global Variables
*/
/**
  Section: EUSART APIs
*/

void EUSART_Initialize(void)
{
    // disable interrupts before changing states
    PIE1bits.RCIE = 0;
    PIE1bits.TXIE = 0;

    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE disabled; ABDEN disabled; 
    BAUDCON = 0x08;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled; 
    RCSTA = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave; 
    TXSTA = 0x24;

    // Baud Rate = 9600; SP1BRGL 12; 
    SPBRGL = 0x0C;

    // Baud Rate = 9600; SP1BRGH 0; 
    SPBRGH = 0x00;
    
    // enable receive interrupt
    PIE1bits.RCIE = 1;
}

void EUSART_Write(uint8_t txData)
{
    TXREG = txData;
    while (!TRMT);						//check tx shift register is clear (i.e: complete transmission)
}

void Response_ACK(uint8_t * ack)
{
    int i = 0;
    for(i = 0 ; i < 3 ; i++) 
    {
        EUSART_Write(ack[i]);
    }
}

void EUSART_Receive_ISR(void)
{
    if(1 == RCSTAbits.OERR)
    {
        // EUSART error - restart
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }
    
    static bool header_received = false;
    static bool data_received = false;
    static uint8_t index = 0;
    uint8_t buffer = 0;
    
    buffer = RCREG;

    if(!header_received)
    {
        if(buffer == 0x02)          // received header byte(STX)
        {
            header_received = true;
            command[index] = 0x02;      
            index++;
        }
        else
        {
            header_received = false;
            data_received = false;
            index = 0;
        }
    }
    else
    {
        command[index] = buffer;
        index++;
        if(index == sizeof(command))
        {
#if 0
            if ( (command[1] == 0x11) || (command[1] == 0x12) || (command[1] == 0x13) || (command[1] == 0x14)
              || (command[1] == 0x15) || (command[1] == 0x21) || (command[1] == 0x22) || (command[1] == 0x23) 
              || (command[1] == 0xA1) || (command[1] == 0xA2) || (command[1] == 0xA3) || (command[1] == 0xA4) )

#else
			  if ( (command[1] == 0x11) || (command[1] == 0x12) || (command[1] == 0x13) 
              || (command[1] == 0x15) || (command[1] == 0x21) || (command[1] == 0x22) || (command[1] == 0x23) 
              || (command[1] == 0xA1) || (command[1] == 0xA2) || (command[1] == 0xA3) || (command[1] == 0xA4) )
#endif
            {
                data_received = true;
                index = 0;
#if 0                
                if(command[1] == 0x13)
                {
                    WDT_MODE = WDT_ENABLE;
                     WDT_MODE = WDT_ENABLE;
                }
                
                if(command[1] == 0x14)
                {  
                    live_check_time = command[2];
                   
                }
#endif 
            } 
            else
            {
                header_received = false;
                data_received = false;
                index = 0;                
            }
        }       
    }
    
    if(data_received)
    {
        tick_counter = 0;
        header_received = false;
        data_received = false;
        
        if(OPERATION_MODE == WDT)
        {
            switch(command[1])
            {
                case 0x11:
                    //tick_counter = 0;
                    break;
                case 0x12:
                    WDT_MODE = WDT_DISABLE;
                    break;
                case 0x13:
                    WDT_MODE = WDT_ENABLE;
                    break;
#if 0
                case 0x14:
                    live_check_time = command[2];
                    break;
#endif
                case 0x15:
                    WDT_MODE = WDT_DISABLE;
                    live_check_time = default_live_check_time;
                    lcm_duration_time = default_lcm_duration_time;
                    lock_open_time = default_lock_open_time;
                    break; 
                case 0x21:
                    if(command[2] == 0x00)
                    {
                        LCM_MODE = NORMAL;                    
                    }
                    else if(command[2] == 0x01)
                    {
                        LCM_MODE = OPEN_CONTINUE;                    
                    }
                    else if(command[2] == 0x02)
                    {
                        LCM_MODE = CLOSE;
                    }               
                    break;
                case 0x22:
                    lock_open_time = command[2];
                    break;
                case 0x23:
                    lcm_duration_time = command[2];
                    break;
                case 0xA4:
                    if(command[2] == 0) 
                    {
                        OPERATION_MODE = REBOOT;
                    }                    
                    break;                    
            }            
        }
        else if(OPERATION_MODE == LCM)
        {
            if(command[2] == 0)
            {
                switch(command[1])
                {
                case 0xA1:
                    LCM_MODE = OPEN_ONCE;
                    break;
                case 0xA2:
                    LCM_MODE = OPEN_CONTINUE;
                    break;
                case 0xA3:
                    LCM_MODE = CLOSE;
                    break;
                case 0xA4:
                    OPERATION_MODE = REBOOT;
                    break;
   
                }                
            }             
        }
        Response_ACK(&command);
    }
}
/**
  End of File
*/
