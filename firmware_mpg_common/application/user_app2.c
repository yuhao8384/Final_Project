/**********************************************************************************************************************
File: user_app2.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app2 as a template:
 1. Copy both user_app2.c and user_app2.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app2" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp2" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP2" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app2.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp2Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp2RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp2Flags;                       /* Global state flags */


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
Variable names shall start with "UserApp2_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp2_StateMachine;            /* The state machine function pointer */
static u32 UserApp2_u32Timeout;                      /* Timeout counter used across states */

static u8 u8Morse[50]       = "xxxxxxxxxxxxxxxxxxxxx";           //morse input
static u8 u8Character[50]   = "xxxxxxxxxxxxxxxxxxxxx";    //morse to the characters

static u8 u8MorseCode1[]    =   ".";                         
static u8 u8MorseCode2[]    =   "-";
static u8 u8TipMessage1[]   =   "Master!";                //tip message
static u8 u8ErrorMessage[]  =   "Error!";                 //error message
static u8 UserApp_CursorPosition;

static u8 *u8CharacterPointer = u8Character;          
static u8 u8CharacterCounter = 0;                         //number of characts
static u8 u8MorseCounter = 0;

static u8 u8MorseCode[][6] = {                      //charact code 
  {'.','.','.','.','*','H'},//H
  {'.','.','.','-','*','V'},//V
  {'.','.','.','*','*','S'},//S
  {'.','.','-','.','*','F'},//F
  {'.','.','-','*','*','U'},//U
  {'.','.','*','*','*','I'},//I
  {'.','-','.','.','*','L'},//L
  {'.','-','-','.','*','P'},//P
  {'.','-','-','-','*','J'},//J
  {'.','-','.','*','*','R'},//R
  {'.','-','-','*','*','W'},//W
  {'.','-','*','*','*','A'},//A
  {'.','*','*','*','*','E'},//E
  {'-','*','*','*','*','T'},//T
  {'-','-','-','*','*','O'},//O
  {'-','-','-','.','*',' '},//space
  {'-','-','-','-','*','#'},//#end
  {'-','-','.','-','*','Q'},//Q
  {'-','-','.','.','*','Z'},//Z
  {'-','-','.','*','*','G'},//G
  {'-','-','*','*','*','M'},//M
  {'-','.','-','-','*','Y'},//Y
  {'-','.','-','.','*','C'},//C
  {'-','.','.','-','*','X'},//X
  {'-','.','.','.','*','B'},//B
  {'-','.','-','*','*','K'},//K
  {'-','.','.','*','*','D'},//D
  {'-','.','*','*','*','N'},//N
};

static bool bChannelState = FALSE;                        //channel state oopen or close
/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
/*Transform morse code into character*/
static u8 MorseToChar(u8 *u8String, u8 u8Counter)
{
  u8 u8Temp = 0;
  for(u8 i = 0; i < u8Counter; i++)    
  {    
    for(u8 j = u8Temp; j < 26; j++)
    {
      if(u8String[i] == u8MorseCode[j][i]&&(u8MorseCode[j][u8Counter] == u8MorseCode[j][4]))
      {
        u8Temp = j;
        break;
      }
    }
  }
  return u8MorseCode[u8Temp][5];
}
/*--------------------------------------------------------------------------------------------------------------------*/
/*The encryption algorithm*///************///*From the first character,An odd number of character ascii increase by one and An even number of character ascii decrease by one*/
static void u8Encrypt(u8 *u8String1, u8 *u8String2)
{
  /*Defined a empty string*/
  u8 u8StringTemp[] = "**********";
  for(u8 i = 0; i<10;i++)
  {
    u8StringTemp[i] = '\0';
  }
  /*Encrypt*/
  for(u8 j = 0; j < strlen(u8String1); j++)
  {
    u8StringTemp[j] = u8String1[j];
    if(j%2 == 0)
    {
      u8StringTemp[j]++;
    }
    else
    {
      u8StringTemp[j]--;
    }
    u8String2[j] = u8StringTemp[j];
  }  
} 
/*-------------------------------------------------------------------------------------------------------------------*/
/*Message sent fuction*/
static void u8SentMessage(u8 *u8String, u8 u8Counter)
{
  u8 u8StringTemp[] = "**********";
  for(u8 i = 0; i<10;i++)
  {
    u8StringTemp[i] = '\0';
  }
  u8Encrypt(u8String, u8StringTemp);
  AntQueueBroadcastMessage(u8StringTemp);
  for(u8 j = 0; j < u8CharacterCounter; j++)
  {
    u8Character[j] = '\0';
  }
}
/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp2Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp2Initialize(void)
{
  /* Configure ANT for this application */
  G_stAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP2;
  G_stAntSetupData.AntSerialLo         = ANT_SERIAL_LO_USERAPP2;
  G_stAntSetupData.AntSerialHi         = ANT_SERIAL_HI_USERAPP2;
  G_stAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP2;
  G_stAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP2;
  G_stAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP2;
  G_stAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP2;
  G_stAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP2;
  G_stAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP2;

  for(u8 i = 0; i < 50; i++)
  {
    u8Morse[i] = 0;
    u8Character[i] = 0;
  }
  PWMAudioSetFrequency(BUZZER1, 1000);                        
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, u8TipMessage1); 
  LedOn(LCD_RED);
  LedOn(LCD_GREEN);
  LedOff(LCD_BLUE);
  
  LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON | LCD_DISPLAY_CURSOR | LCD_DISPLAY_BLINK);
  LCDCommand(LCD_ADDRESS_CMD | LINE2_START_ADDR);
  /* If good initialization, set state to Idle */
  if( AntChannelConfig(ANT_MASTER) )
  {
    UserApp2_StateMachine = UserApp2SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp2_StateMachine = UserApp2SM_FailedInit;
  }
} /* end UserApp2Initialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserApp2RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp2RunActiveState(void)
{
  UserApp2_StateMachine();
} /* end UserApp2RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserApp2SM_Idle(void)
{
  static u16 u16BlinkCounter = 0;                            //system timer
  static u16 u16Counter = 0;                                 //LCD display refresh rate
/*-------------------------------------------------------------------------------------------------------------------*/  
  if( AntReadData() )
  {
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      LCDMessage(LINE1_START_ADDR,G_au8AntApiCurrentData);   //According to the feedback information
    }
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {       
      /* A channel period has gone by: typically this is when new data should be queued to be sent */
    }
  }
/*---------------------------------------------------------------------------------------------------------------*/ 
  /**/
  if(u16BlinkCounter == 10000)                
  {
    u16BlinkCounter = 0;
  }
/*---------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------The function of button0----------------------------------------00000*/   
  /*short press said enter '.'*/
  if(WasButtonPressed(BUTTON0))
  { 
    ButtonAcknowledge(BUTTON0);
    LCDClearChars(LINE1_START_ADDR, 20);
    strcat(u8Morse, u8MorseCode1);
    u8MorseCounter++;
    LCDMessage(LINE1_START_ADDR, u8Morse);
  }
////////////////////////////////////////////////////////////////////////////////  
  //when the BUTTON0 is pressed ,trun on the blue LED and BUZZER1
  /*if(IsButtonPressed(BUTTON0))
  {    
    PWMAudioOn(BUZZER1);
    LedOn(BLUE);    
  }
  else
  {
    PWMAudioOff(BUZZER1);
    LedOff(BLUE);
  }*/
////////////////////////////////////////////////////////////////////////////////  
  /*long press 500ms said enter the '_'*/
  if(IsButtonHeld(BUTTON0, 500) )
  {
    LedOn(PURPLE);
    /*To prevent press input many times*/
    if(u16BlinkCounter > 600)
    {     
      u8Morse[--u8MorseCounter] = '\0';             //clear charract when the button was pressed
      strcat(u8Morse, u8MorseCode2);
      u8MorseCounter++;
      u16BlinkCounter = 0;
    }
    /*Make the LCD display normally*/
    if(u16Counter == 100)
    {
      LCDClearChars(LINE1_START_ADDR, 20);
      LCDMessage(LINE1_START_ADDR, u8Morse);
      u16Counter = 0;
    }
    u16Counter++;
  }
  else
  {
    LedOff(PURPLE);
  }
/*-----------------------------------------------------------------------------------------------------------------*/  
  if(u8MorseCounter > 5)
  {
    LCDMessage(LINE1_START_ADDR, u8ErrorMessage);
    for(u8 k = 0; k < u8MorseCounter; k++)
    {
      u8Morse[k] = '\0';                                           //delete morse in u8morse
    }
    u8MorseCounter = 0;
  }
/*-------------------------------------------The function of button1------------------------------------------11111*/ 
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    if(u8MorseCounter)   
    {     
      *u8CharacterPointer = MorseToChar(u8Morse, u8MorseCounter);
      u8MorseCounter = 0;
      //translate morse into charact
      u8CharacterCounter++;
      for(u8 i = 0; i < 8; i++)
      {
        u8Morse[i] = '\0';                                           //delete morse in u8morse
      }
      u8CharacterPointer++;     
      LCDMessage(LINE1_START_ADDR, u8Morse);       
      LCDMessage(LINE2_START_ADDR, u8Character);                     
    }   
  }
////////////////////////////////////////////////////////////////////////////////
  if(IsButtonHeld(BUTTON1, 1000))
  {
    LedOn(GREEN);
    if(u16BlinkCounter > 1200)
    { 
      u16BlinkCounter = 0;
      if(u8CharacterCounter < 7 && u8CharacterCounter)
      {        
        u8SentMessage(u8Character, u8CharacterCounter); 
        u8CharacterCounter = 0;
        u8CharacterPointer = u8Character;
        LCDCommand(LCD_CLEAR_CMD|LCD_HOME_CMD);        
      }
    }
  }
  else
  {
    LedOff(GREEN);
  }
/*-------------------------------------------The function of button2-----------------------------------------22222*/   
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    /*if LCDline1 has morecode ,delete them*/
    if(u8MorseCounter)
    {                                                                               
      LCDClearChars(LINE1_START_ADDR + (u8MorseCounter-1), 1);
      u8Morse[--u8MorseCounter] = '\0';
    }
    /*if LCDline1 hasn't morecode but line2 has characters,delete them*/
    if(u8CharacterCounter && u8MorseCounter == 0)
    {
      u8Character[--u8CharacterCounter] = '\0';
      u8CharacterPointer--;
      //LCDClearChars(LINE1_START_ADDR , 20);
      LCDClearChars(LINE2_START_ADDR + u8CharacterCounter, 1);
      //LCDMessage(LINE2_START_ADDR,u8Character);
    }     
  }
/*-----------------------------------------The function of button3--------------------------------------------33333*/  
  /*open and close channel*/
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    bChannelState = ~bChannelState;                        //Each time you press the button and turn state again  
    if(bChannelState)
    {
      AntOpenChannel();
      LedOn(RED);                                          //open channel and trun on red led
    }
    else
    {
      AntCloseChannel();                                   //close channel and trun off led
      LedOff(RED);
    }   
  }
/*-------------------------------------------------------------------------------------------------------------------*/  
  if(u8CharacterCounter > 6)
  {
    AntQueueBroadcastMessage(u8Character);
    u8CharacterPointer = u8Character;
    LCDCommand(LCD_CLEAR_CMD|LCD_HOME_CMD);
    for(u8 j = 0; j < u8CharacterCounter; j++)
    {
      u8Character[j] = '\0';
    }   
    u8CharacterCounter = 0;
  }
/*-------------------------------------------------------------------------------------------------------------------*/  
  u16BlinkCounter++;   
/*-------------------------------------------------------------------------------------------------------------------*/     
} /* end UserApp2SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp2SM_Error(void)          
{
  
} /* end UserApp2SM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp2SM_FailedInit(void)          
{
    
} /* end UserApp2SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
