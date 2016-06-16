
/**********************************************************************************************************************
File: user_app.c                                                                

Description:
This is a user_app.c file of Modern Telegram.
------------------------------------------------------------------------------------------------------------------------
API:
Public functions:
Protected System functions:
void UserAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.
void UserAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.
**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserAppFlags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern AntSetupDataType G_stAntSetupData;                         /* From ant.c */

extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentData[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */

static u8 u8CursorPosition = 0;           
/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------
Function: u8Decrypt
Description:
Decrypt the received message.
Requires:
  -
Promises:
  - 
*/
static void u8Decrypt(u8 *u8String1, u8 *u8String2)
{
  u8 u8StringTemp[8];
 for(u8 i = 0; i < 8; i++)
 {
   u8String2[i] = 0;
   u8StringTemp[i] = 0;
 }
  for(u8 j = 0; j < strlen(u8String1); j++)
  {
    u8StringTemp[j] = u8String1[j];
    if(j%2 == 0)
    {
      u8StringTemp[j]--;
    }
    else
    {
      u8StringTemp[j]++;
    }
    u8String2[j] = u8StringTemp[j];
  }  
}
/*--------------------------------------------------------------------------------------------------------------------
Function: UserAppInitialize
Description:
Initializes the State Machine and its variables.
Requires:
  -
Promises:
  - 
*/
void UserAppInitialize(void)
{
   /*Clear the screen*/
  LCDCommand(LCD_CLEAR_CMD);
   /* Home the cursor */
  LCDCommand(LCD_HOME_CMD);
  /*Display the cursor */
  LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON | LCD_DISPLAY_CURSOR | LCD_DISPLAY_BLINK);
  
   /* Configure ANT for this application */
  G_stAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  G_stAntSetupData.AntSerialLo         = ANT_SERIAL_LO_USERAPP;
  G_stAntSetupData.AntSerialHi         = ANT_SERIAL_HI_USERAPP;
  G_stAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  G_stAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  G_stAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  G_stAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  G_stAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  G_stAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;
  
  /* If good initialization, set state to Idle */
  if( AntChannelConfig(ANT_SLAVE) )
  {
    /* Channel is configured, so turn on RED */
    LedOn(RED);
    UserApp_StateMachine = UserAppSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp_StateMachine = UserAppSM_FailedInit;
  }

} /* end UserAppInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserAppRunActiveState()
Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.
Requires:
  - State machine function pointer points at current state
Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserAppRunActiveState(void)
{
  UserApp_StateMachine();

} /* end UserAppRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserAppSM_Idle(void)
{
  static u8 au8DataContent[u16BufferSize];        /*Buffer of Data*/
  static u8* au8DataContent_Pointer = au8DataContent;
  static bool bEND = FALSE;                    /*The END of a communication*/
  static u8 i ;                                /*A variable for the loop*/
  static u8 au8AfterDecrypt[8];                   
  static u16 u16TotalChars;                    /*The total chars of a communication */

   /* Look for BUTTON 3 to open channel */
  if(WasButtonPressed(BUTTON3))
  {
    /* Got the button, so complete one-time actions  */
    ButtonAcknowledge(BUTTON3);
    
    /* Queue open channel and change RED to BLUE to indicate channel is opening */
    AntOpenChannel();
    LedOff(RED);
    LedOn(BLUE);
  }
     
 /*Always check the message*/ 
  if( AntReadData() )
  {
    /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
     /*Decrypt the message*/
      u8Decrypt(G_au8AntApiCurrentData,au8AfterDecrypt);
                   
       /*whether it is a new message*/
        if( (strcmp(au8AfterDecrypt,au8DataContent_Pointer - i) != 0) )
        {  
          /*Tell the sender that the new message was received*/
          AntQueueBroadcastMessage("RD!");
          
          /*if it's the next communication, home the pointer and clear the buffer*/
          if( *(au8DataContent_Pointer - 1) = '#' )
          {
            for(u8 j = 0; j < u16TotalChars; j++)
            {
              au8DataContent[j] = 0;
            }
            au8DataContent_Pointer = au8DataContent;               
          }
          
            i=0;
            LedBlink( YELLOW, LED_4HZ );               /*The sender is typing*/
        
           /*It's new, so add it to au8DataContent*/
          while( au8AfterDecrypt[i] != '\0' )
          {
            if( u16TotalChars < u16BufferSize )
            {
              *au8DataContent_Pointer = au8AfterDecrypt[i];
              
              if( au8AfterDecrypt[i] == '#' )          /*Look for '#' to finish*/
              {
                bEND = TRUE;
                LedBlink( GREEN, LED_4HZ );            /*The sender stops typing*/
                LedOff(YELLOW);
              }
              i++;
              
              u16TotalChars++;
              
              /*Display the letter instantly*/
              LCDMessage(LINE1_START_ADDR + u8CursorPosition, au8DataContent_Pointer);
   
              au8DataContent_Pointer++;        
              
              /*Move the cursor in a circle*/
              if( u8CursorPosition == LINE1_END_ADDR ) 
              {
                u8CursorPosition = LINE2_START_ADDR;
              }
              else if(u8CursorPosition == LINE2_END_ADDR)
              {
                u8CursorPosition = LINE1_START_ADDR;
                
                /*Clear the screen*/
                LCDCommand(LCD_CLEAR_CMD);
              }
              else
              {
                u8CursorPosition++;
              }
            }
            else              /*The buffer of data is full,so clear it and initialize relevant variables*/
            {
              /*Tell the sender do not send any new message!*/
              AntQueueBroadcastMessage("FULL!");
              
              /*Show that the last message will display again*/
              LCDMessage(LINE1_START_ADDR + u8CursorPosition, "!");         
              
              /*The cursor moves forward*/
              if( u8CursorPosition == LINE1_END_ADDR ) 
              {
                u8CursorPosition = LINE2_START_ADDR;
              }
              else if(u8CursorPosition == LINE2_END_ADDR)
              {
                u8CursorPosition = LINE1_START_ADDR;
                
                /*Clear the screen*/
                LCDCommand(LCD_CLEAR_CMD);
              }
              else
              {
                u8CursorPosition++;
              }
              
              au8AfterDecrypt[i] = '\0' ;
              i = 0;
              
              /*Clear the buffer*/
             for(u8 j = 0; j < u16TotalChars; j++)
             {
               au8DataContent[j] = 0;
             }
             au8DataContent_Pointer = au8DataContent;
                         
              u16TotalChars = 0;                           /*Clear the number*/                  
            }
          }
        }               
    }
  }
  
 /*The END of a communication*/
 if( bEND == TRUE ) 
 {
    /*Look for BUTTON 0 to check and send the acknowledge message*/
      if(WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      LedOff(GREEN);                                  /*Clear the state of end*/
      
      /*Clear the screen*/
      LCDCommand(LCD_CLEAR_CMD);
      u8CursorPosition = 0;
      
      u16TotalChars = 0;                              /*Clear the number*/
      
      /*send acknowledge message*/
      AntQueueBroadcastMessage("CT!");                 /*Copy That!*/
    }
}    
  
  /*Look for BUTTON 1 to clear the screen*/
      if(WasButtonPressed(BUTTON1))
    { 
       ButtonAcknowledge(BUTTON1);
      /*Clear the screen*/
      LCDCommand(LCD_CLEAR_CMD);
      u8CursorPosition = 0;
    }
  
  /* Look for BUTTON 2 to close channel */
    if(WasButtonPressed(BUTTON2))
    {
      /* Got the button, so complete one-time actions  */
      ButtonAcknowledge(BUTTON2);
      /*Close the channel*/
      AntCloseChannel();
      LedOff(BLUE);
      LedOn(RED);
    } 
  
} /* end UserAppSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{
  
} /* end UserAppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/



