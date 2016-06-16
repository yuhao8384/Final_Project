/**********************************************************************************************************************
File: user_app2.h                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app2 as a template:
1. Follow the instructions at the top of user_app2.c
2. Use ctrl-h to find and replace all instances of "user_app2" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp2" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP2" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

Description:
Header file for yournewtaskname.c

**********************************************************************************************************************/

#ifndef __USER_APP2_H
#define __USER_APP2_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/
#define USER_INPUT_BUFFER_SIZE  (u16)(DEBUG_SCANF_BUFFER_SIZE + 1)    /* Size of buffer for scanf messages */
/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
/* Required constants for ANT channel configuration */
#define ANT_CHANNEL_USERAPP2             (u8)0                 // Channel 0 - 7
#define ANT_SERIAL_LO_USERAPP2           (u8)0x7C              // Low byte of two-byte Device #
#define ANT_SERIAL_HI_USERAPP2           (u8)0x08              // High byte of two-byte Device #
#define ANT_DEVICE_TYPE_USERAPP2         (u8)1                 // 1 - 255
#define ANT_TRANSMISSION_TYPE_USERAPP2   (u8)1                 // 1-127 (MSB is pairing bit)
#define ANT_CHANNEL_PERIOD_LO_USERAPP2   (u8)0x00              // Low byte of two-byte channel period 0x0001 - 0x7fff
#define ANT_CHANNEL_PERIOD_HI_USERAPP2   (u8)0x20              // High byte of two-byte channel period 
#define ANT_FREQUENCY_USERAPP2           (u8)50                // 2400MHz + this number 0 - 99 MHz
#define ANT_TX_POWER_USERAPP2            RADIO_TX_POWER_0DBM   // RADIO_TX_POWER_0DBM, RADIO_TX_POWER_MINUS5DBM, RADIO_TX_POWER_MINUS10DBM, RADIO_TX_POWER_MINUS20DBM

/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp2Initialize(void);
void UserApp2RunActiveState(void);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp2SM_Idle(void);    

static void UserApp2SM_Error(void);         
static void UserApp2SM_FailedInit(void);        


#endif /* __USER_APP2_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
/**********************************************************************************************************************
File: user_app2.h                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app2 as a template:
1. Follow the instructions at the top of user_app2.c
2. Use ctrl-h to find and replace all instances of "user_app2" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp2" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP2" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

Description:
Header file for yournewtaskname.c

**********************************************************************************************************************/

#ifndef __USER_APP2_H
#define __USER_APP2_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/
//#define USER_INPUT_BUFFER_SIZE  (u16)(DEBUG_SCANF_BUFFER_SIZE + 1)    /* Size of buffer for scanf messages */

/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
/* Required constants for ANT channel configuration */
#define ANT_CHANNEL_USERAPP2             (u8)0                 // Channel 0 - 7
#define ANT_SERIAL_LO_USERAPP2           (u8)0x7C              // Low byte of two-byte Device #
#define ANT_SERIAL_HI_USERAPP2           (u8)0x08              // High byte of two-byte Device #
#define ANT_DEVICE_TYPE_USERAPP2         (u8)1                 // 1 - 255
#define ANT_TRANSMISSION_TYPE_USERAPP2   (u8)1                 // 1-127 (MSB is pairing bit)
#define ANT_CHANNEL_PERIOD_LO_USERAPP2   (u8)0x00              // Low byte of two-byte channel period 0x0001 - 0x7fff
#define ANT_CHANNEL_PERIOD_HI_USERAPP2   (u8)0x20              // High byte of two-byte channel period 
#define ANT_FREQUENCY_USERAPP2           (u8)50                // 2400MHz + this number 0 - 99 MHz
#define ANT_TX_POWER_USERAPP2            RADIO_TX_POWER_0DBM   // RADIO_TX_POWER_0DBM, RADIO_TX_POWER_MINUS5DBM, RADIO_TX_POWER_MINUS10DBM, RADIO_TX_POWER_MINUS20DBM

/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp2Initialize(void);
void UserApp2RunActiveState(void);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp2SM_Idle(void);    

static void UserApp2SM_Error(void);         
static void UserApp2SM_FailedInit(void);        


#endif /* __USER_APP2_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
