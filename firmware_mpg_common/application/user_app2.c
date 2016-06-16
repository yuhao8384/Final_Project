/******************************************************************************************************************
File: user_app2.c                                                                

-------------------------------------------------------------------------------------------------------------------
Sender mode struction:
1.If BUTTON0 is pressed ,the orange led turn on and input code '.',else it will be turn off. 
2.If button0 is held pressed for 500ms,then turn on the purple led and input code '-', else it will be turn off.
3.If BUTTON1 is pressed ,translate morse code into character
4.The button1 is held pressed for 500ms, turn on the cyan led and sent message
5.The BUTTON2 is used to delete the wrong character or morse code,and when the buffer is empty, switch automatic 
  and Manual 
6.The button3 is pressed,then the  channel state will be turn.
  If the channel is open ,the red led will blink at the rate of 2HZ else the red led will be turn off.

7.On Manual mode,if the character count is more than seven,then led white blink at 2Hz.
  On Autommatic mode, if the character count is more than seven,then led Green blink at 2Hz.
  when the message was sent,led white and green are all turn off,at the same time clean screen.
-------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app2.c file template 

--------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp2Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp2RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


*******************************************************************************************************************/

#include "configuration.h"

/*******************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
*******************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp2Flags;                       /* Global state flags */


/*---------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern AntSetupDataType G_stAntSetupData;                         /* From ant.c */

extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentData[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
/*******************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp2_" and be declared as static.
*******************************************************************************************************************/
static fnCode_type UserApp2_StateMachine;            /* The state machine function pointer */
static u32 UserApp2_u32Timeout;                      /* Timeout counter used across states */

static u8 u8Morse[USER_INPUT_BUFFER_SIZE]       = "xxxxxxxxxxxxxxxxxxxxx";           //morse input
static u8 u8Character[USER_INPUT_BUFFER_SIZE]   = "xxxxxxxxxxxxxxxxxxxxx";           //morse to the characters
static u8 u8MessageSent[USER_INPUT_BUFFER_SIZE]   = "xxxxxxxx";                      //The character sequence whst is ready to sent 

static u8 u8MorseCode1[]    =   ".";                         
static u8 u8MorseCode2[]    =   "-";
static u8 u8MorseCode3[]    =   " ";
static u8 u8TipMessage1[]   =   "Sender!";                //tip message
static u8 u8ErrorMessage[]  =   "Error!";                 //error message
static u8 u8AckMessage[]    =   "RD!";                    //ACK message

static u8 *u8CharacterPointer = u8Character; 
static u8 *u8CharacterLCDPointer = u8Character;
static u8 u8CharacterCounter = 0;                         //number of characts
static u8 u8MorseCounter = 0;

static u8 u8MorseCode[][8] = {                            //charact code 
  {'.','*','*','*','*','*','*','E'},//E
  {'.','.','.','.','.','*','*','5'},//5
  {'.','.','.','.','-','*','*','4'},//4
  {'.','.','.','.','*','*','*','H'},//H
  {'.','.','.','*','*','*','*','S'},//S
  {'.','.','.','-','.','.','-','$'},//$
  {'.','.','.','-','-','*','*','3'},//3
  {'.','.','.','-','*','*','*','V'},//V
  {'.','.','*','*','*','*','*','I'},//I
  {'.','.','-','*','*','*','*','U'},//U
  {'.','.','-','.','*','*','*','F'},//F
  {'.','.','-','-','.','.','*','?'},//?
  {'.','.','-','-','.','-','*','_'},//_
  {'.','.','-','-','-','*','*','2'},//2
  {'.','-','*','*','*','*','*','A'},//A
  {'.','-','.','*','*','*','*','R'},//R
  {'.','-','.','.','*','*','*','L'},//L
  {'.','-','.','.','.','*','*','&'},//&
  {'.','-','.','.','-','.','*','"'},//"
  {'.','-','.','-','.','-','*','.'},//.	
  {'.','-','-','*','*','*','*','W'},//W
  {'.','-','-','.','*','*','*','P'},//P
  {'.','-','-','.','-','.','*','@'},//@	
  {'.','-','-','-','*','*','*','J'},//J
  {'.','-','-','-','-','*','*','1'},//1
  {'.','-','-','-','-','.','*','\''},//' 
  {'-','*','*','*','*','*','*','T'},//T
  {'-','.','*','*','*','*','*','N'},//N
  {'-','.','.','*','*','*','*','D'},//D
  {'-','.','.','.','*','*','*','B'},//B
  {'-','.','.','.','.','*','*','6'},//6
  {'-','.','.','.','.','-','*','-'},//-
  {'-','.','.','.','-','*','*','='},//=
  {'-','.','.','-','*','*','*','X'},//X
  {'-','.','.','-','.','*','*','/'},///
  {'-','.','-','*','*','*','*','K'},//K
  {'-','.','-','.','*','*','*','C'},//C
  {'-','.','-','.','-','.','*',';'},//;
  {'-','.','-','.','-','-','*','!'},//!
  {'-','.','-','-','*','*','*','Y'},//Y
  {'-','.','-','-','.','*','*','('},//(
  {'-','.','-','-','.','-','*',')'},//)
  {'-','-','*','*','*','*','*','M'},//M
  {'-','-','.','*','*','*','*','G'},//G
  {'-','-','.','.','*','*','*','Z'},//Z
  {'-','-','.','.','-','-','*',','},//,
  {'-','-','.','-','*','*','*','Q'},//Q
  {'-','-','-','*','*','*','*','O'},//O
  {'-','-','-','.','*','*','*',' '},//space
  {'-','-','-','.','.','*','*','8'},//8 
  {'-','-','-','.','.','.','*',':'},//:
  {'-','-','-','-','.','*','*','9'},//9
  {'-','-','-','-','*','*','*','#'},//#end
  {'-','-','-','-','-','*','*','0'},//0
  {'*','*','*','*','*','*','*','*'},//error
};

static bool bChannelState =     FALSE;                        //channel state oopen or close
static bool bAutoMode     =     FALSE;                        //Automatic mode open or close
static bool bReady        =     FALSE;                        //Automatic mode open and ready to sent
static bool bFinish       =     FALSE;                        //Automatic mode open and finish to sent
static bool bStart        =     FALSE;                        //Automatic mode open and start to sent
static bool bTemp         =     FALSE;
/*******************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*----------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                               */
/*----------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                            */
/*----------------------------------------------------------------------------------------------------------------*/
/*Function: UserApp2Initialize
Description: translate morse code to character
Requires:Please input correct code,otherwise you will get a wrong character
  -
Promises:
  - 
*/
static u8 MorseToChar(u8 *u8String, u8 u8Counter)
{
  u8 u8Temp = 0;
  for(u8 i = 0; i < u8Counter; i++)    
  {    
    for(u8 j = u8Temp; j < 56; j++)
    {
      if((u8String[i] == u8MorseCode[j][i])&&(u8MorseCode[j][u8Counter] == u8MorseCode[j][6]))
      {
        u8Temp = j;
        break;
      }
    }    
  }
  return u8MorseCode[u8Temp][7];
}
/*----------------------------------------------------------------------------------------------------------------*/
/*Function: UserApp2Initialize
Description: From the first character,An odd number of character ascii increase by one and An even number of 
             character ascii decrease by one
Requires:
  -
Promises:
  - 
*/
static void u8Encrypt(u8 *u8String1, u8 *u8String2)
{
  /*Defined a empty string*/
  u8 u8StringTemp[] = "********";
  for(u8 i = 0; i<8;i++)
  {
    u8StringTemp[i] =   0;
  }
  /*Encrypting*/
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
/*---------------------------------------------------------------------------------------------------------------*/
/*Function: UserApp2Initialize
Description: Characters  will sent to receiver.
Requires:
  -
Promises:
  - 
*/
static void u8SentMessage(u8 *u8String, u8 u8Counter)
{
  u8 u8StringTemp[] = "********";
  u8Counter %= 7;
  for(u8 i = 0; i < 8;i++)
  {
    u8StringTemp[i] = 0;
  }
  if(u8Counter < 7 && u8Counter)
  {
    strcat(u8String ,u8MorseCode3);
  }
  u8Encrypt(u8String, u8StringTemp);                                            //encrypt and sent
  AntQueueBroadcastMessage(u8StringTemp);
  for(u8 j = 0; j < ++u8Counter; j++)
  {
    u8String[j] = 0;
  }
}
/*---------------------------------------------------------------------------------------------------------------*/
/*Function: UserApp2Initialize
Description: Characters  will auto sent to receiver.
Requires:
  -
Promises:
  - 
*/
static u8 *AutoSent(u8* u8Pointer , u8 *u8String)
{
  u8 u8StringTemp[] = "********";
  for(u8 i = 0; i < 8; i++)
  {
    u8String[i] = 0;
    u8StringTemp[i] = 0;
  }
  
  for(u8 j = 0; j < 7; j++)
  {
    u8String[j] = *u8Pointer;
    if(*u8Pointer)
    {
      *u8Pointer++;
    }
    else
    {
      break;
    }
  }
  u8Encrypt(u8String, u8StringTemp);                                            //encrypt and sent
  AntQueueBroadcastMessage(u8StringTemp);
  return u8Pointer;
}
/*---------------------------------------------------------------------------------------------------------------*/
/*Function: UserApp2Initialize
Description: Characters  rolling display at screen
Requires:
  -
Promises:
  - 
*/
static void LCDRollDisplay(u8 *string, u8 *u8Pointer)
{
  u8 u8StringTemp[] = "********************"; 
  for(u8 i = 0; i < 20; i++)
  {
    u8StringTemp[i] = 0;    
  }
  for(u8 j = 0; j < 20; j++)
  {
    u8StringTemp[j] = *u8Pointer;
    u8Pointer++;    
    if(!(*u8Pointer))
    {
      break;
    }
  }
  LCDClearChars(LINE2_START_ADDR, 20);
  LCDMessage(LINE2_START_ADDR, u8StringTemp);  
}
/*----------------------------------------------------------------------------------------------------------------
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

  for(u8 i = 0; i < USER_INPUT_BUFFER_SIZE; i++)
  {
    u8Morse[i] = 0;
    u8Character[i] = 0;
    u8MessageSent[i] = 0;
  }
  PWMAudioSetFrequency(BUZZER1, 1000);                        //Set the buzzer frequency
  LCDCommand(LCD_CLEAR_CMD);                                  //Remove the original information to display the new information
  LCDMessage(LINE1_START_ADDR, u8TipMessage1);                //Show tooltip
  LedOn(LCD_RED);                                             //Set the backlight
  LedOn(LCD_GREEN);
  LedOff(LCD_BLUE);
  
  
  //LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON | LCD_DISPLAY_CURSOR | LCD_DISPLAY_BLINK);
 // LCDCommand(LCD_ADDRESS_CMD | LINE2_START_ADDR);
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


/*------------------------------------------------------------------------------------------------------------------
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


/*----------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                              */
/*----------------------------------------------------------------------------------------------------------------*/


/*******************************************************************************************************************
State Machine Function Definitions
*******************************************************************************************************************/

/*----------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserApp2SM_Idle(void)
{
  static u16 u16BlinkCounter = 0;                            //system timer
  static u16 u16Counter = 0;                                 //LCD display refresh rate
/*----------------------------------------------------------------------------------------------------------------*/  
  if( AntReadData() )
  {
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      if(strcmp(G_au8AntApiCurrentData, u8AckMessage))
      {
        LCDMessage(LINE1_START_ADDR,G_au8AntApiCurrentData);   //According to the feedback information
      }
      else
      {
        LCDMessage(LINE1_START_ADDR,G_au8AntApiCurrentData);
        if(bAutoMode || bTemp)
        {
          bReady = TRUE;
        }
      }
    }
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {       
      /* A channel period has gone by: typically this is when new data should be queued to be sent */
    }
  }
/*---------------------------------------------------------------------------------------------------------------*/ 
  /*Cross the border to prevent the counter*/
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
  if(IsButtonPressed(BUTTON0))
  {    
    PWMAudioOn(BUZZER1);
    LedOn(ORANGE); 
  }
  else
  {
    PWMAudioOff(BUZZER1);
    LedOff(ORANGE);
  }
////////////////////////////////////////////////////////////////////////////////  
  /*long press 500ms said enter the '_'*/
  if(IsButtonHeld(BUTTON0, 500) )
  {
    LedOn(PURPLE);
    /*To prevent press input many times*/
    if(u16BlinkCounter > 600)
    {     
      u8Morse[--u8MorseCounter] = 0;             //clear charract when the button was pressed
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
  //tips for wrong morse input
  if(u8MorseCounter > 7)
  {
    LCDMessage(LINE1_START_ADDR, u8ErrorMessage);
    for(u8 k = 0; k < u8MorseCounter; k++)
    {
      u8Morse[k] = 0;                                           //delete morse in u8morse
    }
    u8MorseCounter = 0;
  }
/*-------------------------------------------The function of button1------------------------------------------11111*/ 
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    if(u8MorseCounter)   
    { 
      LCDClearChars(LINE1_START_ADDR, 20);        
      *u8CharacterPointer = MorseToChar(u8Morse, u8MorseCounter);
      u8MorseCounter = 0;
      //translate morse into charact
      u8CharacterCounter++;
      for(u8 i = 0; i < 8; i++)
      {
        u8Morse[i] = 0;                                           //delete morse in u8morse
      } 
      //If character counter is more over than twenty,then clean LCD screen
      if((u8CharacterCounter-1)%20 == 0)
      {
        LCDClearChars(LINE2_START_ADDR, 20);        
      }
      LCDMessage(LINE2_START_ADDR+(u8CharacterCounter-1)%20, u8CharacterPointer);
      u8CharacterPointer++;     
    }   
  }
////////////////////////////////////////////////////////////////////////////////
  if(IsButtonHeld(BUTTON1, 1000))
  {
    LedOn(CYAN);    
    if(u16BlinkCounter > 1200)
    {  
      u16BlinkCounter = 0;     
      //Automatic mode
      if(bAutoMode)
      {
        bReady  = TRUE;
        bStart  =   TRUE;
        bFinish =   FALSE;
        LedBlink(GREEN, LED_2HZ);
        u8CharacterPointer = u8Character;                             //Make the pointer back to the initial position
      }
      //Manual mode
      else
      { 
        if(u8CharacterCounter < 7)
        {        
          u8SentMessage(u8Character, u8CharacterCounter);            //Sent message
          u8CharacterCounter = 0;
          u8CharacterPointer = u8Character;                         //Make the pointer back to the initial position         
          LCDCommand(LCD_CLEAR_CMD);
        }
        else
        {
          bTemp   =   TRUE;
          bReady  =   TRUE;
          bStart  =   TRUE;
          bFinish =   FALSE;
          LedBlink(WHITE, LED_2HZ);
          u8CharacterPointer = u8Character;           
        }
      }
    }
  }
  else
  {
    LedOff(CYAN);
  }
/*-------------------------------------------------------------------------------------------------------------------*/  
 
/*-------------------------------------------The function of button2-----------------------------------------22222*/   
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    /*if LCDline1 has morecode ,delete them*/
    if(u8MorseCounter)
    {                                                                               
      LCDClearChars(LINE1_START_ADDR + (u8MorseCounter-1), 1);
      u8Morse[--u8MorseCounter] = 0;
    }
    /*if LCDline1 hasn't morecode but line2 has characters,delete them*/
    else if(u8CharacterCounter)
    {
      u8Character[--u8CharacterCounter] = 0;
      u8CharacterPointer--;
      //LCDClearChars(LINE1_START_ADDR , 20);
      LCDClearChars(LINE2_START_ADDR + u8CharacterCounter, 1);
      //LCDMessage(LINE2_START_ADDR,u8Character);
    }
    else
    {
      bAutoMode = ~bAutoMode;
      LCDCommand(LCD_CLEAR_CMD);
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
      LedBlink(RED, LED_2HZ);                                          //open channel and trun on red led
    }
    else
    {
      AntCloseChannel();                                   //close channel and trun off led
      LedOff(RED);
    }   
  }
/*-------------------------------------------------------------------------------------------------------------------*/  
  /*The automatic mode, the buffer with seven characters to send one more time*/
  if(bAutoMode)
  {
    LedOn(BLUE);
  }
  else
  {
    LedOff(BLUE);
  }
/*-------------------------------------------------------------------------------------------------------------------*/
  /*If I receive an ACK, sent next message*/
  if(bReady)
  {  
    bReady = FALSE;  
    if(u8CharacterCounter/7)
    {      
      u8CharacterPointer = AutoSent(u8CharacterPointer, u8MessageSent);
      u8CharacterCounter -= 7;
      if(u8CharacterCounter == 0)
      {
        bFinish = TRUE;
        bStart = FALSE;
        u8CharacterPointer = u8Character;
      }
    }
    else if(u8CharacterCounter)
    {
      u8CharacterPointer = AutoSent(u8CharacterPointer, u8MessageSent);
      u8CharacterCounter = 0;
      bFinish = TRUE;
      bStart = FALSE;
      u8CharacterPointer = u8Character;
    }
  }
/*-------------------------------------------------------------------------------------------------------------------*/ 
  //If the characters is too long ,rolling display on LCD  
  if(u16BlinkCounter%200 == 0)
  {
    if(bStart && (!bFinish))
    {
      if(strlen(u8Character) > 20 )
      LCDRollDisplay(u8Character, u8CharacterLCDPointer);
      if(*(u8CharacterLCDPointer+1))
      {       
        u8CharacterLCDPointer++;
      }
      else
      {
        u8CharacterLCDPointer = u8Character;
      }      
    }       
  }
/*-------------------------------------------------------------------------------------------------------------------*/ 
  //message was sent ,clean screen and turn off led and prepare for next transport
  if(bFinish)
  {
    bTemp   = FALSE;
    bFinish = FALSE;
    LedOff(GREEN);
    LedOff(WHITE);
    LCDCommand(LCD_CLEAR_CMD);
    for(u8 i = 0; i < USER_INPUT_BUFFER_SIZE; i++)
    {
      u8Character[i] = 0;
    }
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
