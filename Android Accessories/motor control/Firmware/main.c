/*******************************************************************************

    USB Android Accessory basic demo with accessory in host mode

*******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************

FileName:        main.c
Dependencies:    None
Processor:       PIC24/dsPIC30/dsPIC33/PIC32MX
Compiler:        C30/C32
Company:         Microchip Technology, Inc.

Software License Agreement

The software supplied herewith by Microchip Technology Incorporated
(the "Company") for its PICmicro(R) Microcontroller is intended and
supplied to you, the Companyï¿½s customer, for use solely and
exclusively on Microchip PICmicro Microcontroller products. The
software is owned by the Company and/or its supplier, and is
protected under applicable copyright laws. All rights are reserved.
Any use in violation of the foregoing restrictions may subject the
user to criminal sanctions under applicable laws, as well as to
civil liability for the breach of the terms and conditions of this
license.

THIS SOFTWARE IS PROVIDED IN AN ï¿½AS ISï¿½ CONDITION. NO WARRANTIES,
WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

Change History
  Rev      Description
  -----    ----------------------------------
  2.9      Initial revision
*******************************************************************************/

// Include files
#include "USB/usb.h"
#include "USB/usb_host_android.h"
#include "Compiler.h"
#include "HardwareProfile.h"




// If a maximum current rating hasn't been defined, then define 500mA by default
#ifndef MAX_ALLOWED_CURRENT
    #define MAX_ALLOWED_CURRENT             (500)         // Maximum power we can supply in mA
#endif

// Configuration bits for the device.  Please refer to the device datasheet for each device
//   to determine the correct configuration bit settings
#if defined __C30__ || defined __XC16__
    #if defined(__PIC24FJ256GB110__)
        _CONFIG3( WPEND_WPENDMEM & /* Write Protect from WPFP to the last page of memory */
               WPCFG_WPCFGDIS & /* Write Protect Configuration Page Disabled */
               WPDIS_WPDIS    & /* Write Protection Disable */
               WPFP_WPFP511 )   /* Write Protection Flash Page 511 */

        _CONFIG2( IESO_OFF       & /* Two Speed Start-up Disabled */
               //FNOSC_FRC      & /* Oscillator Selection Fast RC oscillator */
               PLL_96MHZ_ON   & /* 96MHz PLL Enabled */
               PLLDIV_DIV2    & /* 96MHz PLL Prescaler Divide by 2 (8MHz input) */
               FCKSM_CSDCMD   & /* Clock switching and clock monitor Both disabled */
               OSCIOFNC_ON    & /* OSCO/RC15 function RC15 */
               IOL1WAY_OFF    & /* RP Register Protection Unlimited Writes To RP Registers */
               //DISUVREG_OFF   & /* Disable Internal USB Voltage Regulator Disabled */
               FNOSC_PRIPLL   &
               POSCMOD_HS     &
               POSCMOD_NONE );  /* Oscillator Selection Primary disabled */

      _CONFIG1( JTAGEN_OFF     & /* JTAG Disabled */
                GCP_OFF        & /* Code Protect Disabled */
                GWRP_OFF       & /* Write Protect Disabled */
                ICS_PGx2       & /* ICD pins select EMUC/EMUD share PGC1/PGD1 */
                FWDTEN_OFF     & /* Watchdog Timer Disabled */
                WINDIS_OFF     & /* Windowed WDT Disabled */
                FWPSA_PR128    & /* Watchdog prescaler 1:128 */
                WDTPS_PS32768 ); /* Watchdog postscale 1:32,768 */
        //_CONFIG2(FNOSC_PRIPLL & POSCMOD_HS & PLL_96MHZ_ON & PLLDIV_DIV2) // Primary HS OSC with PLL, USBPLL /2
        //_CONFIG1(JTAGEN_OFF & FWDTEN_OFF & ICS_PGx2)   // JTAG off, watchdog timer off

    #elif defined(__PIC24FJ64GB004__)
        _CONFIG1(WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
        _CONFIG2(POSCMOD_HS & I2C1SEL_PRI & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_ON)
        _CONFIG3(WPFP_WPFP0 & SOSCSEL_SOSC & WUTSEL_LEG & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)
        _CONFIG4(DSWDTPS_DSWDTPS3 & DSWDTOSC_LPRC & RTCOSC_SOSC & DSBOREN_OFF & DSWDTEN_OFF)
    #elif defined(__PIC24FJ64GB502__)
        _CONFIG1(WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
        _CONFIG2(I2C1SEL_PRI & IOL1WAY_OFF & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
        _CONFIG3(WPFP_WPFP0 & SOSCSEL_SOSC & WUTSEL_LEG & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)
        _CONFIG4(DSWDTPS_DSWDTPS3 & DSWDTOSC_LPRC & RTCOSC_SOSC & DSBOREN_OFF & DSWDTEN_OFF)
    #elif defined(__PIC24FJ256GB106__)
        _CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)
        _CONFIG2( 0xF7FF & IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRIPLL & PLLDIV_DIV3 & IOL1WAY_ON)
    #elif defined(__PIC24FJ256DA210__) || defined(__PIC24FJ256GB210__)
        _CONFIG1(FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
        _CONFIG2(POSCMOD_HS & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
    #elif defined(IOIO)
        _CONFIG1(FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
        _CONFIG2(POSCMOD_NONE & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_NODIV & IESO_OFF)
    #elif defined(__dsPIC33EP512MU810__) || defined(PIC24EP512GU810_PIM)
        _FOSCSEL(FNOSC_FRC);
        _FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);
        _FWDT(FWDTEN_OFF);
    #endif
#elif defined( __PIC32MX__ )
    #pragma config UPLLEN   = ON            // USB PLL Enabled
    #pragma config FPLLMUL  = MUL_15        // PLL Multiplier
    #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
    #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
    #pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
    #pragma config FWDTEN   = OFF           // Watchdog Timer
    #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
    //#pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
    #pragma config OSCIOFNC = OFF           // CLKO Enable
    #pragma config POSCMOD  = HS            // Primary Oscillator
    #pragma config IESO     = OFF           // Internal/External Switch-over
    #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
    #pragma config FNOSC    = PRIPLL        // Oscillator Selection
    #pragma config CP       = OFF           // Code Protect
    #pragma config BWP      = OFF           // Boot Flash Write Protect
    #pragma config PWP      = OFF           // Program Flash Write Protect
    #pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
    //#pragma config DEBUG    = ON            // Background Debugger Enable

#else
    #error Cannot define configuration bits.
#endif

// C30 and C32 Exception Handlers
// If your code gets here, you either tried to read or write
// a NULL pointer, or your application overflowed the stack
// by having too many local variables or parameters declared.
#if defined(__C30__) || defined __XC16__
	void _ISR __attribute__((__no_auto_psv__)) _AddressError(void)
	{
        while(1){}
	}
	void _ISR __attribute__((__no_auto_psv__)) _StackError(void)
	{
        while(1){}
	}

#elif defined(__C32__)
	void _general_exception_handler(unsigned cause, unsigned status)
	{
        #if defined(DEBUG_MODE)
            unsigned long address = _CP0_GET_EPC();
        #endif

        while(1){}
	}
#endif

//Definitions of the various application commnands that can be sent
typedef enum _ACCESSORY_DEMO_COMMANDS
{
    COMMAND_SET_LEDS            = 0x01,
    COMMAND_UPDATE_PUSHBUTTONS  = 0x02,
    COMMAND_UPDATE_POT          = 0x03,

    /* Job Management*/
    COMMAND_ADD_JOB             = 0x04,
    COMMAND_REMOVE_JOB          = 0x05,

    /* PWM Motor*/
    COMMAND_SET_PWM             = 0x06,

   /* Text Output*/
    COMMAND_receiveText       = 0x07,
    COMMAND_getText           = 0x08,

    COMMAND_APP_CONNECT         = 0xFE,
    COMMAND_APP_DISCONNECT      = 0xFF
} ACCESSORY_DEMO_COMMANDS;

//Creation of the data packet that is going to be sent.  In this example
//  there is just a command code and a one byte data.
typedef struct __attribute__((packed))
{
    BYTE command;
    BYTE data;
} ACCESSORY_APP_PACKET;

//Local prototypes
#if defined(__C32__)
static void InitPIC32(void);
#endif

static void SetMOTOR(void);
static BYTE getGear(void);
static void SetLEDs(BYTE setting);
static BYTE GetPushbuttons(void);
static BYTE ReadPOT(void);
static BYTE toSetting(int percentage);


//local variables
static BYTE read_buffer[64];
static ACCESSORY_APP_PACKET outgoing_packet;
static void* device_handle = NULL;
static BOOL device_attached = FALSE;

static char manufacturer[] = "Microchip Technology Inc.";
static char model[] = "Basic Accessory Demo";
static char description[] = DEMO_BOARD_NAME_STRING;
static char version[] = "2.0";
static char uri[] = "http://www.microchip.com/android";
static char serial[] = "N/A";

ANDROID_ACCESSORY_INFORMATION myDeviceInfo =
{
    manufacturer,
    sizeof(manufacturer),
    model,
    sizeof(model),
    description,
    sizeof(description),
    version,
    sizeof(version),
    uri,
    sizeof(uri),
    serial,
    sizeof(serial)
};

#define  PWM_PERIOD 200



 void InitializeTimer2For_PWM(void)
{
    T2CONbits.TON = 0;      /*  Timer2 is used for generating PWM frequency */
    
    //OC9CON1bits.OCTSEL	= 0x0;		// Timer 2 is the clock source for output Compare

    T2CON				= 0;		// Timer 2 is clock for OC9.
    T2CONbits.TCKPS		= 2;		// Timer Input Clock Prescale is 1:64

    OC9CON1				= 0; 		// It is a good practice to clear off the control bits initially
    OC9CON2				= 0;

    //T2CONbits.TCS = 0;
    //T2CONbits.TGATE = 0;
    //T2CONbits.TCKPS = 0b00;
    TMR2 = 0x00;
    PR2 = 0x7FFF;           // FOR 488 Hz
    T2CONbits.TON = 1;      // TIMER 2 ON

}

 

  void PwmInit( void )
    {
      /* Unmap RP31(RF13) */
      //RPOR15bits.RP31R = 0; //Change for output

      /* Let's unmap RD10 instead*/
      RPOR1bits.RP3R = 0;


      LATDbits.LATD10 = 0;
      TRISDbits.TRISD10 = 0;


      //PORTDbits.RD10 = 0;

      /* Drive RF13 low and make it an output */
      //LATFbits.LATF13 = 0;  //Change for output
      //TRISFbits.TRISF13 = 0;    //Change for output

      //LATDbits.LATD10 = 0;
      //TRISDbits.TRISD10 = 0;

      Nop();




      /* Reset PWM */
      OC1CON1 = 0x0000;
      OC1CON2 = 0x0000;

      /* set PWM duty cycle to 50% */


      //OC1R    = curPWM_PERIOD;

      /*
      BYTE gear;

      gear = getGear();
      DWORD temp = PWM_PERIOD > gear;

      DWORD curPWM = PWM_PERIOD - temp;
      */

      DWORD curRate = PWM_PERIOD * ReadPOT();
      curRate = curRate / 100;

      pot2LEDs();
      //rpm2LEDs(curRate);

      OC1R    = curRate; /* set the duty cycle to 50% */
      OC1RS   = PWM_PERIOD - 1;  /* set the period */

      /* configure PWM */
      OC1CON2 = 0x001F;   /* Sync with This OC module                               */
      OC1CON1 = 0x1C08;   /* Clock sourc Fcyc, trigger mode 1, Mode 0 (disable OC1) */

      /* enable the PWM */
      OC1CON1 = OC1CON1 | 0x0006;   /* Mode 6, Edge-aligned PWM Mode */

      /* Make pin RP31(RF13) OC1 (PWM output) */
      //RPOR15bits.RP31R = 18;    //Change for output

      RPOR1bits.RP3R = 18;
      }

      //PORTDbits.RD10 = 18;
      //RP3R = 18;


  

  void SetDutyCycle_PWM(unsigned int DutyCycle) {
    
    OC1R = DutyCycle;
    OC1RS = DutyCycle - 1;  /*  with 488Hz frquency we have 15bits resolution   */

    OC1CON2 = 0x001F;   /* Sync with This OC module                               */
    OC1CON1 = 0x1C08;   /* Clock sourc Fcyc, trigger mode 1, Mode 0 (disable OC1) */

    /* enable the PWM */
    OC1CON1 = OC1CON1 | 0x0006;   /* Mode 6, Edge-aligned PWM Mode */
}

  void pot2LEDs() {
      BYTE setting;
      BYTE potPercentage = ReadPOT();

      setting = toSetting(potPercentage);

      SetLEDs(setting);

  }

  BYTE toSetting(int percentage) {
      BYTE setting;
      if (percentage >= 100)   { setting = 0b11111000;}
      if (percentage >= 75)    { setting = 0b11110000;}
      if (percentage >= 50)    { setting = 0b11100000;}
      if (percentage >= 25)    { setting = 0b11000000;}
      if (percentage >= 10)    { setting = 0b10000000;}
      if (percentage == 0)     { setting = 0b00000000;}

      return setting;
  }


  void rpm2LEDs(DWORD curRate) {
      //if (curRate < 1000)  { SetLEDs(0b00000000); MotorState = FALSE; curRate = 10;}
      //else { MotorState = TRUE; }
      int per = (curRate / PWM_PERIOD) * 100;
      
      
      int setting = toSetting(per);

      SetLEDs(setting);
      /*
      SetLEDs(setting);
      if (curRate <  25 ) { SetLEDs(0b00000000); }
      if (curRate >= 50 ) { SetLEDs(0b10000000); }
      if (curRate >= 100) { SetLEDs(0b11000000); }
      if (curRate >= 150) { SetLEDs(0b11100000); }
      if (curRate >= 175) { SetLEDs(0b11110000); }
      if (curRate >= 200) { SetLEDs(0b11111000); }

      */
  }


  void setPWM() {
    BYTE potPercentage = ReadPOT();
    DWORD temp = PWM_PERIOD * potPercentage;
    temp = temp / 100;
    //pot2LEDs();

    //if (temp < 25) { temp = 0;}

    pot2LEDs();

    SetDutyCycle_PWM(temp);
  }

  void setRPM(unsigned int targetRate) {
      if (targetRate <= PWM_PERIOD && targetRate >= 0) {
          rpm2LEDs(targetRate);
          SetDutyCycle_PWM(targetRate);
      }
  }

/****************************************************************************
  Function:
    int main(void)

  Summary:
    main function

  Description:
    main function

  Precondition:
    None

  Parameters:
    None

  Return Values:
    int - exit code for main function

  Remarks:
    None
  ***************************************************************************/
int main(void)
{
    DWORD size = 0;
    BOOL responseNeeded;

    BYTE mode = 0;
    
    BYTE wasMode = 0;
    BYTE pushButtonValues = 0xFF;
    BYTE potPercentage = 0xFF;
    BOOL buttonsNeedUpdate = FALSE;
    BOOL potNeedsUpdate = FALSE;
    BOOL motorON = FALSE;
    BOOL readyToRead = TRUE;
    BOOL writeInProgress = FALSE;
    BYTE tempValue = 0xFF;
    BYTE errorCode;
    ACCESSORY_APP_PACKET* command_packet = NULL;

    CLKDIV =  0; /* set for default clock operations Fcyc = 4MHz */
    AD1PCFGL = 0xffff;
    AD1PCFGH = 0x0003;

    BOOL connected_to_app = FALSE;
    BOOL need_to_disconnect_from_app = FALSE;

    #if defined(__PIC32MX__)
        InitPIC32();
    #endif

  #if defined(__dsPIC33EP512MU810__) || defined (__PIC24EP512GU810__)

    // Configure the device PLL to obtain 60 MIPS operation. The crystal
    // frequency is 8MHz. Divide 8MHz by 2, multiply by 60 and divide by
    // 2. This results in Fosc of 120MHz. The CPU clock frequency is
    // Fcy = Fosc/2 = 60MHz. Wait for the Primary PLL to lock and then
    // configure the auxilliary PLL to provide 48MHz needed for USB
    // Operation.

	PLLFBD = 38;				/* M  = 60	*/
	CLKDIVbits.PLLPOST = 0;		/* N1 = 2	*/
	CLKDIVbits.PLLPRE = 0;		/* N2 = 2	*/
	OSCTUN = 0;

    /*	Initiate Clock Switch to Primary
     *	Oscillator with PLL (NOSC= 0x3)*/

    __builtin_write_OSCCONH(0x03);
	__builtin_write_OSCCONL(0x01);
	while (OSCCONbits.COSC != 0x3);

    // Configuring the auxiliary PLL, since the primary
    // oscillator provides the source clock to the auxiliary
    // PLL, the auxiliary oscillator is disabled. Note that
    // the AUX PLL is enabled. The input 8MHz clock is divided
    // by 2, multiplied by 24 and then divided by 2. Wait till
    // the AUX PLL locks.

    ACLKCON3 = 0x24C1;
    ACLKDIV3 = 0x7;
    ACLKCON3bits.ENAPLL = 1;
    while(ACLKCON3bits.APLLCK != 1);

    TRISBbits.TRISB5 = 0;
    LATBbits.LATB5 = 1;

    #endif

    USBInitialize(0);
    AndroidAppStart(&myDeviceInfo);

    responseNeeded = FALSE;
    mInitPOT();
    InitializeTimer2For_PWM();
    PwmInit();

    //InitMOTOR();

    DEBUG_Init(0);

    InitAllLEDs();

    while(1)
    {
        //Keep the USB stack running
        USBTasks();

        

        //If the device isn't attached yet,
        if(device_attached == FALSE || mode == 1)
        {
            buttonsNeedUpdate = TRUE;
            potNeedsUpdate = TRUE;
            need_to_disconnect_from_app = FALSE;
            connected_to_app = FALSE;
            size = 0;

            /**/
            BYTE curPush = GetPushbuttons();

            if ((curPush == 0x8) || (mode == 1)) {
                LED0_On();
                
                mode = 1;
                if (wasMode == 0) {
                
                    pot2LEDs();
                    PwmInit();
                    
                    
                }

                tempValue = ReadPOT();


                wasMode = 1;

                //If it is different than the last time we read the pot, then we need
                //  to send it to the Android device

                if(tempValue != potPercentage) {
                    potNeedsUpdate = TRUE;
                    //setRPM(tempValue);

                    setPWM();
                }
            }
            if ((curPush == 0x4)  || (mode == 0)) {
                    mode = 0;
                    //LED0_Off();
                    
                    if (wasMode == 1) {
                        SetLEDs(0b00000000);
                        wasMode = 0;
                        setRPM(0);
                    }
                    //Reset the accessory state variables
                    InitAllLEDs();

                    //Continue to the top of the while loop to start the check over again.
                    continue;
                }
               /* //Reset the accessory state variables
                InitAllLEDs();
                
                //Continue to the top of the while loop to start the check over again.
                continue;
            }*/
            //}


            


            
        }

        //If the accessory is ready, then this is where we run all of the demo code

        if(readyToRead == TRUE && mode == 0)
        {
            errorCode = AndroidAppRead(device_handle, (BYTE*)&read_buffer, (DWORD)sizeof(read_buffer));
            //If the device is attached, then lets wait for a command from the application
            if( errorCode != USB_SUCCESS)
            {
                //Error
                DEBUG_PrintString("Error trying to start read");
            }
            else
            {
                readyToRead = FALSE;
            }
        }

        size = 0;

        if(AndroidAppIsReadComplete(device_handle, &errorCode, &size) == TRUE)
        {
            //We've received a command over the USB from the Android device.
            if(errorCode == USB_SUCCESS)
            {
                //Maybe process the data here.  Maybe process it somewhere else.
                command_packet = (ACCESSORY_APP_PACKET*)&read_buffer[0];
            }
            else
            {
                //Error
                DEBUG_PrintString("Error trying to complete read request");
            }

        }

        while(size > 0)
        {
            if(connected_to_app == FALSE)
            {
                if(command_packet->command == COMMAND_APP_CONNECT)
                {
                    connected_to_app = TRUE;
                    need_to_disconnect_from_app = FALSE;
                }
            }
            else
            {
                switch(command_packet->command)
                {
                    case COMMAND_SET_LEDS:
                        SetLEDs(command_packet->data);
                        break;

                    case COMMAND_APP_DISCONNECT:
                        need_to_disconnect_from_app = TRUE;
                        break;

                    case COMMAND_SET_PWM:
                        setRPM(command_packet->data);
                        break;

                    default:
                        //Error, unknown command
                        DEBUG_PrintString("Error: unknown command received");
                        break;
                }
            }
            //All commands in this example are two bytes, so remove that from the queue
            size -= 2;
            //And move the pointer to the next packet (this works because
            //  all command packets are 2 bytes.  If variable packet size
            //  then need to handle moving the pointer by the size of the
            //  command type that arrived.
            command_packet++;

            if(need_to_disconnect_from_app == TRUE)
            {
                break;
            }
        }

        if(size == 0)
        {
            readyToRead = TRUE;
        }

        //Get the current pushbutton settings
        tempValue = GetPushbuttons();

        //If the current button settings are different than the last time
        //  we read the button values, then we need to send an update to the
        //  attached Android device
        if(tempValue != pushButtonValues)
        {
            buttonsNeedUpdate = TRUE;
            pushButtonValues = tempValue;
        }

        //Get the current potentiometer setting
        tempValue = ReadPOT();

        //If it is different than the last time we read the pot, then we need
        //  to send it to the Android device
        if(tempValue != potPercentage)
        {
            potNeedsUpdate = TRUE;
            potPercentage = tempValue;
        }

        //If there is a write already in progress, we need to check its status
        if( writeInProgress == TRUE )
        {
            if(AndroidAppIsWriteComplete(device_handle, &errorCode, &size) == TRUE)
            {
                writeInProgress = FALSE;
                if(need_to_disconnect_from_app == TRUE)
                {
                    connected_to_app = FALSE;
                    need_to_disconnect_from_app = FALSE;
                }

                if(errorCode != USB_SUCCESS)
                {
                    //Error
                    DEBUG_PrintString("Error trying to complete write");
                }
            }
        }

        if((need_to_disconnect_from_app == TRUE) && (writeInProgress == FALSE))
        {
            outgoing_packet.command = COMMAND_APP_DISCONNECT;
            outgoing_packet.data = 0;
            writeInProgress = TRUE;

            errorCode = AndroidAppWrite(device_handle,(BYTE*)&outgoing_packet, 2);
            if( errorCode != USB_SUCCESS )
            {
                DEBUG_PrintString("Error trying to send button update");
            }
        }

        if(connected_to_app == FALSE)
        {
            //If the app hasn't told us to start sending data, let's not do anything else.
            continue;
        }

        //If we need up update the button status on the Android device and we aren't
        //  already busy in a write, then we can send the new button data.
        if((buttonsNeedUpdate == TRUE) && (writeInProgress == FALSE))
        {
            outgoing_packet.command = COMMAND_UPDATE_PUSHBUTTONS;
            outgoing_packet.data = pushButtonValues;



            errorCode = AndroidAppWrite(device_handle,(BYTE*)&outgoing_packet, 2);
            if( errorCode != USB_SUCCESS )
            {
                DEBUG_PrintString("Error trying to send button update");
            }

            buttonsNeedUpdate = FALSE;
            writeInProgress = TRUE;
        }

        //If we need up update the pot status on the Android device and we aren't
        //  already busy in a write, then we can send the new pot data.
        if((potNeedsUpdate == TRUE) && (writeInProgress == FALSE))
        {
            outgoing_packet.command = COMMAND_UPDATE_POT;
            outgoing_packet.data = potPercentage;

            errorCode = AndroidAppWrite(device_handle,(BYTE*)&outgoing_packet, 2);
            if( errorCode != USB_SUCCESS )
            {
                DEBUG_PrintString("Error trying to send pot update");
            }

            potNeedsUpdate = FALSE;
            writeInProgress = TRUE;
        }
    } //while(1) main loop
}



/****************************************************************************
  Function:
    BOOL USB_ApplicationDataEventHandler( BYTE address, USB_EVENT event, void *data, DWORD size )

  Summary:
    Handles USB data application events

  Description:
    Handles USB data application events

  Precondition:
    None

  Parameters:
    BYTE address - address of the device causing the event
    USB_EVENT event - the event that has occurred
    void* data - data associated with the event
    DWORD size - the size of the data in the data field

  Return Values:
    BOOL - Return TRUE of the event was processed.  Return FALSE if the event
           wasn't handled.

  Remarks:
    None
  ***************************************************************************/
BOOL USB_ApplicationDataEventHandler( BYTE address, USB_EVENT event, void *data, DWORD size )
{
    return FALSE;
}


/****************************************************************************
  Function:
    BOOL USB_ApplicationEventHandler( BYTE address, USB_EVENT event, void *data, DWORD size )

  Summary:
    Handles USB application events

  Description:
    Handles USB application events

  Precondition:
    None

  Parameters:
    BYTE address - address of the device causing the event
    USB_EVENT event - the event that has occurred
    void* data - data associated with the event
    DWORD size - the size of the data in the data field

  Return Values:
    BOOL - Return TRUE of the event was processed.  Return FALSE if the event
           wasn't handled.

  Remarks:
    None
  ***************************************************************************/
BOOL USB_ApplicationEventHandler( BYTE address, USB_EVENT event, void *data, DWORD size )
{
    switch( (INT)event )
    {
        case EVENT_VBUS_REQUEST_POWER:
            // The data pointer points to a byte that represents the amount of power
            // requested in mA, divided by two.  If the device wants too much power,
            // we reject it.
            if (((USB_VBUS_POWER_EVENT_DATA*)data)->current <= (MAX_ALLOWED_CURRENT / 2))
            {
                return TRUE;
            }
            else
            {
                DEBUG_PrintString( "\r\n***** USB Error - device requires too much current *****\r\n" );
            }
            break;

        case EVENT_VBUS_RELEASE_POWER:
        case EVENT_HUB_ATTACH:
        case EVENT_UNSUPPORTED_DEVICE:
        case EVENT_CANNOT_ENUMERATE:
        case EVENT_CLIENT_INIT_ERROR:
        case EVENT_OUT_OF_MEMORY:
        case EVENT_UNSPECIFIED_ERROR:   // This should never be generated.
        case EVENT_DETACH:                   // USB cable has been detached (data: BYTE, address of device)
        case EVENT_ANDROID_DETACH:
            device_attached = FALSE;
            return TRUE;
            break;

        // Android Specific events
        case EVENT_ANDROID_ATTACH:
            device_attached = TRUE;
            device_handle = data;
            return TRUE;

        default :
            break;
    }
    return FALSE;
}


/****************************************************************************
  Function:
    void InitPIC32(void)

  Summary:
    Initialize the PIC32 core to the correct modes and clock speeds

  Description:
    Initialize the PIC32 core to the correct modes and clock speeds

  Precondition:
    Only runs on PIC32

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
#if defined(__PIC32MX__)
static void InitPIC32(void)
{
    int  value;

    #if defined(RUN_AT_60MHZ)
        // Use OSCCON default
    #else
        OSCCONCLR = 0x38000000; //PLLODIV
        #if defined(RUN_AT_48MHZ)
            OSCCONSET = 0x08000000; //PLLODIV /2
        #elif defined(RUN_AT_24MHZ)
            OSCCONSET = 0x10000000; //PLLODIV /4
        #else
            #error Cannot set OSCCON
        #endif
    #endif

    value = SYSTEMConfigWaitStatesAndPB( GetSystemClock() );

    // Enable the cache for the best performance
    CheKseg0CacheOn();

    INTEnableSystemMultiVectoredInt();

    DDPCONbits.JTAGEN = 0;

    value = OSCCON;
    while (!(value & 0x00000020))
    {
        value = OSCCON;    // Wait for PLL lock to stabilize
    }

    INTEnableInterrupts();
}
#endif


/****************************************************************************
  Function:
    void SetLEDs(BYTE setting)

  Summary:
    change the LED settings of the boards

  Description:
    change the LED settings of the boards

  Precondition:
    None

  Parameters:
    BYTE setting - bitmap for desired LED setting (1 = On, 0 = Off)
        bit 0 = LED 0
        bit 1 = LED 1
        bit 2 = LED 2
        ...
        bit 7 = LED 7

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
static void SetLEDs(BYTE setting)
{
    if((setting & 0x01) == 0x01) { LED0_On(); } else { LED0_Off(); }
    if((setting & 0x02) == 0x02) { LED1_On(); } else { LED1_Off(); }
    if((setting & 0x04) == 0x04) { LED2_On(); } else { LED2_Off(); }
    if((setting & 0x08) == 0x08) { LED3_On(); } else { LED3_Off(); }
    if((setting & 0x10) == 0x10) { LED4_On(); } else { LED4_Off(); }
    if((setting & 0x20) == 0x20) { LED5_On(); } else { LED5_Off(); }
    if((setting & 0x40) == 0x40) { LED6_On(); } else { LED6_Off(); }
    if((setting & 0x80) == 0x80) { LED7_On(); } else { LED7_Off(); }
}

/****************************************************************************
  Function:
    BYTE GetPushbuttons(void)

  Summary:
    Reads the current push button status.

  Description:
    Reads the current push button status.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    BYTE - bitmap for button representations (1 = pressed, 0 = not pressed)
        bit 0 = button 1
        bit 1 = button 2
        bit 2 = button 3
        bit 3 = button 4

  Remarks:
    None
  ***************************************************************************/

void SetMOTOR(void) {
    BYTE percentPOT = ReadPOT();

    BYTE gear = percentPOT > 5;

    BYTE temp;

    temp = PWM_PERIOD > gear;

    SetDutyCycle_PWM(temp);
}

static BYTE getGear(void) {
    BYTE percentPOT = ReadPOT();

    BYTE gear = percentPOT > 4;

    return gear;
}

static BYTE GetPushbuttons(void)
{
    BYTE toReturn;

    InitAllSwitches();

    toReturn = 0;

    if(Switch1Pressed()){ toReturn |= 0x1; }
    if(Switch2Pressed()){ toReturn |= 0x2; }
    if(Switch3Pressed()){ toReturn |= 0x4; }
    if(Switch4Pressed()){ toReturn |= 0x8; }

    return toReturn;
}


/****************************************************************************
  Function:
    BYTE ReadPOT(void)

  Summary:
    Reads the pot value and returns the percentage of full scale (0-100)

  Description:
    Reads the pot value and returns the percentage of full scale (0-100)

  Precondition:
    A/D is initialized properly

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/
static BYTE ReadPOT(void)
{
    WORD_VAL w;
    DWORD temp;

    #if defined(__18CXX)
        mInitPOT();

        ADCON0bits.GO = 1;              // Start AD conversion
        while(ADCON0bits.NOT_DONE);     // Wait for conversion

        w.v[0] = ADRESL;
        w.v[1] = ADRESH;

    #elif defined(__C30__) || defined(__C32__) || defined __XC16__
        #if defined(PIC24FJ256GB110_PIM) || \
            defined(PIC24FJ256DA210_DEV_BOARD) || \
            defined(PIC24FJ256GB210_PIM)

            AD1CHS = 0x5;           //MUXA uses AN5

            // Get an ADC sample
            AD1CON1bits.SAMP = 1;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++){Nop();} //Sample delay, conversion start automatically
            AD1CON1bits.SAMP = 0;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++){Nop();} //Sample delay, conversion start automatically
            while(!AD1CON1bits.DONE);       //Wait for conversion to complete

            temp = (DWORD)ADC1BUF0;
            temp = temp * 100;
            temp = temp/1023;

        #elif defined(PIC24F_ADK_FOR_ANDROID)
            AD1CHS = 0x9;           //MUXA uses AN9

            // Get an ADC sample
            AD1CON1bits.SAMP = 1;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++){Nop();} //Sample delay, conversion start automatically
            AD1CON1bits.SAMP = 0;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++){Nop();} //Sample delay, conversion start automatically
            while(!AD1CON1bits.DONE);       //Wait for conversion to complete

            temp = (DWORD)ADC1BUF0;
            temp = temp * 100;
            temp = temp/1023;

        #elif defined(PIC24FJ64GB004_PIM) || defined(PIC24FJ64GB502_MICROSTICK)
            AD1CHS = 0x7;           //MUXA uses AN7

            // Get an ADC sample
            AD1CON1bits.SAMP = 1;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++){Nop();} //Sample delay, conversion start automatically
            AD1CON1bits.SAMP = 0;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++){Nop();} //Sample delay, conversion start automatically
            while(!AD1CON1bits.DONE);       //Wait for conversion to complete

            temp = (DWORD)ADC1BUF0;
            temp = temp * 100;
            temp = temp/1023;

        #elif defined(PIC24F_STARTER_KIT)
            AD1CHS = 0x0;           //MUXA uses AN0

            // Get an ADC sample
            AD1CON1bits.SAMP = 1;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++){Nop();} //Sample delay, conversion start automatically
            AD1CON1bits.SAMP = 0;           //Start sampling
            for(w.Val=0;w.Val<1000;w.Val++){Nop();} //Sample delay, conversion start automatically
            while(!AD1CON1bits.DONE);       //Wait for conversion to complete

            temp = (DWORD)ADC1BUF0;
            temp = temp * 100;
            temp = temp/1023;

        #elif defined(PIC32MX460F512L_PIM) || defined(PIC32MX795F512L_PIM)
            AD1PCFG = 0xFFFB; // PORTB = Digital; RB2 = analog
            AD1CON1 = 0x0000; // SAMP bit = 0 ends sampling ...
            // and starts converting
            AD1CHS = 0x00020000; // Connect RB2/AN2 as CH0 input ..
            // in this example RB2/AN2 is the input
            AD1CSSL = 0;
            AD1CON3 = 0x0002; // Manual Sample, Tad = internal 6 TPB
            AD1CON2 = 0;
            AD1CON1SET = 0x8000; // turn ADC ON

            AD1CON1SET = 0x0002; // start sampling ...
            for(w.Val=0;w.Val<1000;w.Val++){Nop();} //Sample delay, conversion start automatically
            AD1CON1CLR = 0x0002; // start Converting
            while (!(AD1CON1 & 0x0001));// conversion done?

            temp = (DWORD)ADC1BUF0;
            temp = temp * 100;
            temp = temp/1023;

        #elif   defined(PIC32_USB_STARTER_KIT) || \
                defined (DSPIC33E_USB_STARTER_KIT) || \
                defined(PIC32_ETHERNET_STARTER_KIT) || \
                defined(DSPIC33EP512MU810_PIM) || \
                defined (PIC24EP512GU810_PIM) || \
                defined (DSPIC33E_USB_STARTER_KIT) || \
                defined (CEREBOT_32MX7)
            //Doesn't have a Pot
            w.Val = 50;
            temp = 50;
        #else
            #error
        #endif

    #endif

    return (BYTE)temp;
}//end ReadPOT
