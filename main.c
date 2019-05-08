/**
 * @mainpage Ball Trap Documentation
 * <hr/>
 * This project interfaces a MMA8452Q accelerometer and nRF24 wireless module with the thief brainframe
 * using the MSP430F5529 LaunchPad. The goal is to make a ball trap which can detect if it hits any human
 * or object. The ball is armed by the thief brainframe then rolled. If, after a specified grace period,
 * the ball hits a human or other desired object, the trap will send a trip message to the brainframe.
 * If the trap does not hit any human within a specified time frame, the trap will disarm itself
 * and send an access message to the brainframe. This trap is inspired by classic rolling boulder
 * traps as seen in Indiana Jones films.
 *
 *
 * @section getting_started Getting Started
 * @subsection project_settings Project Settings
 * - The project uses a processor clock speed of (24MHz)
 * - I2C channel 0 should be used with I2C_MAX_TX_SIZE at least 2 and I2C_MAX_RX_SIZE at least 6
 * - SPI channel 1 should be used with SPI_MAX_SIZE at least 33
 * - I2C and SPI should be on different channels to prevent overlapping pin assignments
 *
 * An example of project settings defines are shown below:
 * @code
 * #define FCPU 24000000
 * #define USE_MODULE_TASK
 * #define USE_MODULE_TIMING
 * #define USE_MODULE_LIST
 * #define USE_MODULE_BUFFER
 * #define USE_MODULE_BUFFER_PRINTF
 * #define USE_MODULE_UART
 * #define USE_MODULE_SUBSYSTEM
 * #define USE_UART1
 * #define SUBSYSTEM_IO SUBSYSTEM_IO_UART
 * #define SUBSYSTEM_UART 1
 * #define SUBSYS_UART 1
 * #define UART1_TX_BUFFER_LENGTH 512
 * #define TASK_MAX_LENGTH 50
 * #define USE_I2C0
 * #define I2C_MAX_TX_SIZE 2
 * #define I2C_MAX_RX_SIZE 6
 * #define USE_SPI_B1
 * #define SPI_MAX_SIZE 33
 * #define THIEF_SPI SPI_B0
 * #define THIEF_BRAINFRAME_NETWORK
 * #define THIS_NODE BALL_TRAP
 * @endcode
 *
 * @subsection testing_environment Testing Environment
 * This project was developed using a MSP430F5529 LaunchPad and Code Composer 8.0 IDE.
 * The embedded-software library was used for hardware abstraction. PuTTY software was
 * used as a UART interface. Please see embedded-software library documentation
 * for Code Composer Studio configuration. Other hardware: nRF24, Sparkfun MMA8452Q Breakout
 * @see https://github.com/muhlbaier/embedded-software
 *
 * @subsection operation Operation
 * The master brainframe should send an \a arm command to the ball trap node to start operation. In case of manual
 * operation without a master brainframe, the system can be armed using the following command in a UART terminal.
 * @code
 * $thief arm
 * @endcode
 *
 * Once armed, the ball should be pushed to be put in motion. The trap has an initial grace period of starting to detect
 * human interface. The default grace period is 2s. (e.g the trap will not start detecting humans until this time has been
 * passed, started after being armed). If the ball trap hits an object it will send a trip message to the brainframe. If the trap does not hit an object after
 * a specified time (default 6s), it will disarm and send an access message to the brainframe.
 *
 * Note: The trap will detect collision with any solid object. It is not limited to human detection and the threshold for collision
 * detections is dynamic.
 *
 * @section collision_detection Collision Detection Calibration
 * The accelerometer was calibrated for object detection using the following methodology:
 * - Read current X, Y, Z data from accelerometer every 200ms and output to UART
 * - Log UART output to PuTTY and export as a CSV
 * - Using Microsoft Excel, a threshold was calculated which would serve as the point at which a collision should be ticked
 *
 * To test, the accelerometer was placed on a rolling chair and pushed off
 * towards a wall. When the rolling chair hits the wall, it should detect a collision. A line graph plotting the
 * X, Y, Z data over the period of 17s in this scenario is shown in the figure below.
 * \image html accel_trial1.jpg
 *
 * As seen from the figure, the first smaller spike is from the chair being pushed off towards the wall. The following
 * multiple spikes are from multiple collisions of the accelerometer with the wall. To find a reasonable threshold value,
 * the absolute values of the X, Y, Z columns are summed together (Manhattan Distance) and (1) is subtracted from the
 * total to normalize the constant acceleration of gravity.
 * @code
 * float normAccel = (fabs(x) + fabs(y) + fabs(z)) - 1;
 * @endcode
 * By analyzing the Manhattan distance of the accelerometer at certain points of collision impact, we chose an arbitrary
 * value of 0.35f to be the threshold of a collision. E.g if any normalized acceleration distance is found above that value,
 * it is determined to be a collision. In our tests, it is found that this value offers good collision detection suitable
 * for our application. It is also assumed that in production, the ball will be pushed during the initial arming grace period,
 * else the system may detect the initial push as a false positive.
 *
 * The collision threshold determined from this calibration is defined by COLLISION_THRESHOLD definition in the main.c file.
 * The default threshold value is 0.35f. Reducing the threshold value increases the sensitivity for detecting a collision.
 * Inversely, increasing the threshold value increases the sensitivity for collision detection. The default threshold
 * value is rated for a "medium speed" collision with a solid object.
 *
 *
 * @section accelerometer_notes Accelerometer
 * - The I2C module and I2C HAL for MSP430F5529 in the embedded-software library was modified to add the ability to specify if a
 * stop-bit should be added at the end of a I2C transaction. This was added to interface with the MMA8452Q module. Anyone using
 * this project should use the same I2C code or be updated to also support this functionality.
 * - The accelerometer should use I2C channel I2C_B0 because I2C_B1 is not fully implemented in MSP430F5529 HAL
 * - If the accelerometer uses I2C_B0, the nRF24 must use SPI_B1 because SPI_B0 and I2C_B0 use the same pins on MSP430F5529
 * - The default scale for the accelerometer is configured for +/-2G. This can be changed in the accelerometer source code
 * macro definition. However, in testing it was found this did not improve results.
 * - The optimal sampling rate for the accelerometer (Accelerometer_Init) was found to be approximately 200ms
 *
 * @section Results
 * The results of the trap align with the initial project goals. The trap is able to successfully use an accelerometer
 * to detect impact of object and then report the collision to the thief brainframe. One revision which was required
 * from the initial project goals was the transition from a rolling ball enclosure to a rolling chair setup. If the device
 * was in a rolling ball enclosure, it would be difficult to debug using a USB cable for UART logging (the cable would
 * prevent smooth rolling of a ball). In addition, we determined the mechanical engineering aspect of the ball
 * enclosure would distract us from the main embedded software focus.
 *
 * The resulting project aligns with the principles of good embedded software design. Our code is clean, modular, and reusable.
 * The accelerometer code was written simply so anyone can simply include the module and read data from the sensor. The ball trap
 * portion of the code is well documented and has optional debug logging to UART by (un)commenting a user-define. Additionally,
 * the methodology of our collision detection calibration was clearly outlined and defined so any additional user of our trap
 * can understand how we came to our results and modify it for their use-case if need be.
 *
 * @section technology_usage Technology Usage
 * @subsection Slack
 * Slack was useful for easily communicating with our team-members. A Slack private channel was created for our team
 * and we communicated mostly in that channel for: discussion for due dates, project goals, logistics, etc. Overall,
 * it was helpful.
 *
 * @subsection Trello
 * Trello was useful for staying organizing and to understand what the goals and deadlines were for each sprint. Additionally,
 * everyone was able to see each group members' individual contributions to a task. Overall, Trello was useful for
 * the project and solving the logisitics of a group project.
 *
 * @section authors Authors
 * - Stephen Glass
 * - Lonnie Souder
 *
 * @section Appendix
 * \image html prototype.jpg
 *
 * @file main.c
 * @author Stephen Glass
 * @author Lonnie Souder
 * @date May 5 2019
 * @brief Interface with nRF24 module and accelerometer. Detect an impact using the accelerometer
 * and report back trip or access message using thief module depending on results.

 * @see https://github.com/muhlbaier/embedded-software
 *
 * Dependencies: embedded-software library
 * Board: MSP430F5529
 */

#include <msp430.h> 
#include <math.h>
#include "library.h"
#include "project_settings.h"
#include "nrf24.h"
#include "nrf24network.h"
#include "timing.h"
#include "subsystem.h"
#include "task.h"
#include "uart.h"
#include "i2c.h"
#include "spi.h"
#include "accelerometer.h"
#include "hal_general.h"
#include "thief.h"
#include "thief_brainframe.h"

/// Define if you want logging of accelerometer events
#define LOG_ACCEL_EVENTS

#define GPIO_LED1           BIT0  // P1.0
#define GPIO_LED1_OUT       P1OUT // PORT1
#define GPIO_LED1_DIR       P1DIR // P1DIR

#define GPIO_LED2           BIT7  // P4.7
#define GPIO_LED2_OUT       P4OUT // PORT4
#define GPIO_LED2_DIR       P4DIR // P4DIR

#define TASK_VERSION        (version_t)0x01010014u
#define I2C_CLOCK_SPEED     100000

#define ACCEL_PERIOD        200   /// Default period is 200ms for sampling accelerometer
#define TRAP_GRACE          2000  /// Default grace period after arming trap is 2s
#define TRAP_ACCESS         6000  /// Default time for trap to collide with human is 6s

/**
 * Threshold for collision detection. A lower threshold results
 * in greater sensitivity for detecting a collision with a solid object.
 * The default threshold is calibrated for "medium speed" impact with a solid table.
 */
#define COLLISION_THRESHOLD 0.35

// Configuration functions
void InitGPIO(void);
void BlinkLED(uint8_t type);
void SetBlinkFrequency(void);
void CallbackAccelerometer(float x, float y, float z); // Accelerometer callback function

// nRF24 functions
void RF1_CE(uint8_t out);
void RF1_CSN(uint8_t out);
void RF1_PollIRQ(void);
void RF1_Init(void);
void RF1_RxPayloadHandler(uint8_t * data, uint8_t length);
void RF1_AckPayloadHandler(uint8_t * data, uint8_t length);
void RF1_AckReceivedHandler(void);
void RF1_MaxRetriesHandler(void);

// Trap functions
void ThisNode_TestMsgHandler(uint8_t * data, uint8_t length, uint8_t from);
void Trap_Arm(void);
void Trap_ArmGracePeriod(void);
void Trap_Disarm(void);
void Trap_Access(void);

// System functions
void initClockTo16MHz();
void SetClk24MHz(void);
uint16_t setVCoreUp(uint8_t level);
uint8_t increaseVCoreToLevel2();

uint8_t gTrapArmed = 0; /// Global flag for status of trap being armed
static uint8_t sys_id;

void InitGPIO(void) {
    // LED1
    GPIO_LED1_DIR |= (GPIO_LED1); // Configure LED as output
    GPIO_LED1_OUT &= ~(GPIO_LED1); // Default disable LED
    // LED2
    GPIO_LED2_DIR |= (GPIO_LED2); // Configure LED as output
    GPIO_LED2_OUT &= ~(GPIO_LED2); // Default disable LED
}

void BlinkLED(uint8_t type) {
    switch(type) {
    case GPIO_LED1:
        GPIO_LED1_OUT ^= (GPIO_LED1);
        LogMsg(sys_id, "[BlinkLED] LED1 blinked");
        break;
    case GPIO_LED2:
        GPIO_LED2_OUT ^= (GPIO_LED2);
        LogMsg(sys_id, "[BlinkLED] LED2 blinked");
        break;
    default:
        break;
    }
}

/**
 * @brief This function is scheduled by the task scheduler at a specific period
 * When this function is called, the task scheduler queues the LED to blink
 * using the BlinkLED abstracted function.
 */
void SetBlinkFrequency(void) {
    Task_Queue((task_t)BlinkLED, (uint8_t *)GPIO_LED2);
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
	
	SetClk24MHz();
	//initClockTo16MHz();

	/* Initialize the F5529 GPIO */
	InitGPIO();
	gTrapArmed = 0;

	DisableInterrupts();
	Timing_Init();
	Task_Init();
	UART_Init(SUBSYSTEM_UART);

    // Setup the SPI channel to be used by the NRF nodes
    spi_settings_t spi_settings;
    spi_settings.channel = THIEF_SPI;
    spi_settings.bit_rate = 100000;
    spi_settings.hal_settings.char7bit = 0;
    spi_settings.hal_settings.msb_first = 1;
    spi_settings.mode = 0;
    SPI_Init(&spi_settings);

    //Setup IRQ, CE, CSN for node 1
    P2DIR |= BIT3 | BIT4; // CE, CSN as output
    P2DIR &= ~BIT5; // IRQ as input
    RF1_CSN(1);

	EnableInterrupts();

	Log_EchoOn();
	// Initialize LED blinking subsystem for logging
	sys_id = Subsystem_Init("task", TASK_VERSION, 0);
	UART_printf(SUBSYSTEM_UART, "System Initialized\r\n");

	// Setup the default network, this uses the THIS_NODE macro found in project_settings.h
	Thief_Init(RF1_CE, RF1_CSN, Trap_Arm, Trap_Disarm, 0);

	// Initialize accelerometer with default period and callback function
    int8_t retVal = Accelerometer_Init(ACCEL_PERIOD, CallbackAccelerometer);
    if(retVal < 0) { // error
        // The only errors which exist are: a.) TX/RX buffers not long enough b.) Device not online
        // Refer to accelerometer.h for error codes
        UART_printf(SUBSYSTEM_UART, "Error encountered configuring accelerometer!");
        while(1);
    }

	while(1) {
	    SystemTick();
	    // Poll the interrupt pins coming from radios so we know when something happened
	    RF1_PollIRQ();
	}
}

/**
 * @brief Arm the trap if the message is received by thief module
 */
void Trap_Arm(void) {
    UART_printf(SUBSYSTEM_UART, "Trap Armed!\r\n");
    Task_Schedule((task_t)Trap_ArmGracePeriod, 0, TRAP_GRACE, 0); // Start grace period
    Task_Schedule((task_t)Trap_Access, 0, TRAP_ACCESS, 0); // Start timer for end condition if no collision
}

/**
 * @brief Set the flag to arm the trap after the grace period
 */
void Trap_ArmGracePeriod(void) {
    gTrapArmed = 1; // Arm the trap when the grace period has concluded
}

/**
 * @brief Disarm the trap
 */
void Trap_Disarm(void) {
    UART_printf(SUBSYSTEM_UART, "Trap Disarmed manually!\r\n");
    gTrapArmed = 0;
    //Accelerometer_Deinit();
}

/**
 * @brief Trap access function
 */
void Trap_Access(void) {
    if(gTrapArmed) { // If hasn't been armed in TRAP_ACCESS, it was never hit, therefore can access
        Thief_Access(1); // Send message to thief module for access
        gTrapArmed = 0; // Disarm
    }
    //Accelerometer_Deinit(); // Deinit accelerometer if allowed to access or not
}

/**
 * @brief Accelerometer callback function
 */
void CallbackAccelerometer(float x, float y, float z) {
    if(!gTrapArmed) return; // If not armed (in grace period), do not check
#ifdef LOG_ACCEL_EVENTS
    UART_printf(SUBSYSTEM_UART, "%f, %f, %f\r\n", x, y, z);
#endif
    float normAccel = (fabs(x) + fabs(y) + fabs(z)) - 1;  // Manhattan Distance
    // Subtracting 1 to normalize the effect of gravity on acceleration
    if (normAccel > COLLISION_THRESHOLD) { // Arbitrary threshold value (refer to documentation)
#ifdef LOG_ACCEL_EVENTS
        UART_printf(SUBSYSTEM_UART, "Tripped!\r\n", x, y, z);
#endif
        Thief_Trip(1); // Collision detected, so trip to thief module
        gTrapArmed = 0;
        //Accelerometer_Deinit();
    }
}

/**
 * @brief This function is provided to the network layer in the init function and is used to control the Chip Enable pin on the radio
 * @param out output pin
 */
// This function is provided to the network layer in the init function and is used to control the
// Chip Enable pin on the radio
void RF1_CE(uint8_t out){
    P2OUT = (P2OUT & ~BIT4) | (out << 4);
}

/**
 * @brief This function is provided to the network layer in the init function and is used to control the Chip Select pin on the radio
 * @param out output pin
 */
void RF1_CSN(uint8_t out){
    P2OUT = (P2OUT & ~BIT3) | (out << 3);
    // Poll the interrupt here so that we poll all the time especially as SPI transactions are happening
    RF1_PollIRQ();
}

/**
 * @brief Method to poll the interrupt pin and see if an interrupt has occured
 */
void RF1_PollIRQ(void){
    static uint8_t pin_state = 1;
    uint8_t new_state = (P2IN & BIT5) >> 5;

    if( (new_state != pin_state) && !new_state) {
        nrf24_NetworkISRHandler();
    }
    pin_state = new_state;
}

void initClockTo16MHz()
{
    increaseVCoreToLevel2();
    UCSCTL3 |= SELREF_2;                      // Set DCO FLL reference = REFO
    UCSCTL4 |= SELA_2;                        // Set ACLK = REFO
    __bis_SR_register(SCG0);                  // Disable the FLL control loop
    UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_5;                      // Select DCO range 16MHz operation
    UCSCTL2 = FLLD_0 + 487;                   // Set DCO Multiplier for 16MHz
                                              // (N + 1) * FLLRef = Fdco
                                              // (487 + 1) * 32768 = 16MHz
                                              // Set FLL Div = fDCOCLK
    __bic_SR_register(SCG0);                  // Enable the FLL control loop

    // Worst-case settling time for the DCO when the DCO range bits have been
    // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
    // UG for optimization.
    // 32 x 32 x 16 MHz / 32,768 Hz = 500000 = MCLK cycles for DCO to settle
    __delay_cycles(500000);//
    // Loop until XT1,XT2 & DCO fault flag is cleared
    do
    {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG); // Clear XT2,XT1,DCO fault flags
        SFRIFG1 &= ~OFIFG;                          // Clear fault flags
    }while (SFRIFG1&OFIFG);                         // Test oscillator fault flag
}

uint16_t setVCoreUp(uint8_t level){
    uint32_t PMMRIE_backup, SVSMHCTL_backup, SVSMLCTL_backup;

    //The code flow for increasing the Vcore has been altered to work around
    //the erratum FLASH37.
    //Please refer to the Errata sheet to know if a specific device is affected
    //DO NOT ALTER THIS FUNCTION

    //Open PMM registers for write access
    PMMCTL0_H = 0xA5;

    //Disable dedicated Interrupts
    //Backup all registers
    PMMRIE_backup = PMMRIE;
    PMMRIE &= ~(SVMHVLRPE | SVSHPE | SVMLVLRPE |
                SVSLPE | SVMHVLRIE | SVMHIE |
                SVSMHDLYIE | SVMLVLRIE | SVMLIE |
                SVSMLDLYIE
                );
    SVSMHCTL_backup = SVSMHCTL;
    SVSMLCTL_backup = SVSMLCTL;

    //Clear flags
    PMMIFG = 0;

    //Set SVM highside to new level and check if a VCore increase is possible
    SVSMHCTL = SVMHE | SVSHE | (SVSMHRRL0 * level);

    //Wait until SVM highside is settled
    while((PMMIFG & SVSMHDLYIFG) == 0)
    {
        ;
    }

    //Clear flag
    PMMIFG &= ~SVSMHDLYIFG;

    //Check if a VCore increase is possible
    if((PMMIFG & SVMHIFG) == SVMHIFG)
    {
        //-> Vcc is too low for a Vcore increase
        //recover the previous settings
        PMMIFG &= ~SVSMHDLYIFG;
        SVSMHCTL = SVSMHCTL_backup;

        //Wait until SVM highside is settled
        while((PMMIFG & SVSMHDLYIFG) == 0)
        {
            ;
        }

        //Clear all Flags
        PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG |
                     SVMLVLRIFG | SVMLIFG |
                     SVSMLDLYIFG
                     );

        //Restore PMM interrupt enable register
        PMMRIE = PMMRIE_backup;
        //Lock PMM registers for write access
        PMMCTL0_H = 0x00;
        //return: voltage not set
        return false;
    }

    //Set also SVS highside to new level
    //Vcc is high enough for a Vcore increase
    SVSMHCTL |= (SVSHRVL0 * level);

    //Wait until SVM highside is settled
    while((PMMIFG & SVSMHDLYIFG) == 0)
    {
        ;
    }

    //Clear flag
    PMMIFG &= ~SVSMHDLYIFG;

    //Set VCore to new level
    PMMCTL0_L = PMMCOREV0 * level;

    //Set SVM, SVS low side to new level
    SVSMLCTL = SVMLE | (SVSMLRRL0 * level) |
               SVSLE | (SVSLRVL0 * level);

    //Wait until SVM, SVS low side is settled
    while((PMMIFG & SVSMLDLYIFG) == 0)
    {
        ;
    }

    //Clear flag
    PMMIFG &= ~SVSMLDLYIFG;
    //SVS, SVM core and high side are now set to protect for the new core level

    //Restore Low side settings
    //Clear all other bits _except_ level settings
    SVSMLCTL &= (SVSLRVL0 + SVSLRVL1 + SVSMLRRL0 +
                 SVSMLRRL1 + SVSMLRRL2
                 );

    //Clear level settings in the backup register,keep all other bits
    SVSMLCTL_backup &=
        ~(SVSLRVL0 + SVSLRVL1 + SVSMLRRL0 + SVSMLRRL1 + SVSMLRRL2);

    //Restore low-side SVS monitor settings
    SVSMLCTL |= SVSMLCTL_backup;

    //Restore High side settings
    //Clear all other bits except level settings
    SVSMHCTL &= (SVSHRVL0 + SVSHRVL1 +
                 SVSMHRRL0 + SVSMHRRL1 +
                 SVSMHRRL2
                 );

    //Clear level settings in the backup register,keep all other bits
    SVSMHCTL_backup &=
        ~(SVSHRVL0 + SVSHRVL1 + SVSMHRRL0 + SVSMHRRL1 + SVSMHRRL2);

    //Restore backup
    SVSMHCTL |= SVSMHCTL_backup;

    //Wait until high side, low side settled
    while(((PMMIFG & SVSMLDLYIFG) == 0) &&
          ((PMMIFG & SVSMHDLYIFG) == 0))
    {
        ;
    }

    //Clear all Flags
    PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG |
                SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG
                );

    //Restore PMM interrupt enable register
    PMMRIE = PMMRIE_backup;

    //Lock PMM registers for write access
    PMMCTL0_H = 0x00;

    return true;
}

uint8_t increaseVCoreToLevel2()
{
    uint8_t level = 2;
    uint8_t actlevel;
    uint8_t status = true;

    //Set Mask for Max. level
    level &= PMMCOREV_3;

    //Get actual VCore
    actlevel = PMMCTL0 & PMMCOREV_3;

    //step by step increase or decrease
    while((level != actlevel) && (status == true))
    {
        if(level > actlevel)
        {
            status = setVCoreUp(++actlevel);
        }
    }

    return (status);
}

void SetClk24MHz(){
    // Increase Vcore setting to level3 to support fsystem=25MHz
    // NOTE: Change core voltage one level at a time..
    setVCoreUp (0x01);
    setVCoreUp (0x02);
    setVCoreUp (0x03);

    P5SEL |= BIT2+BIT3;
    UCSCTL6 &= ~XT2OFF; // Enable XT2
    UCSCTL6 &= ~XT2BYPASS;
    UCSCTL3 = SELREF__XT2CLK; // FLLref = XT2
    UCSCTL4 |= SELA_2 + SELS__DCOCLKDIV + SELM__DCOCLKDIV;

    UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
    // Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
    do
    {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                        // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

    // Disable the FLL control loop
    __bis_SR_register(SCG0);

    // Select DCO range 24MHz operation
    UCSCTL1 = DCORSEL_7;
    /* Set DCO Multiplier for 24MHz
    (N + 1) * FLLRef = Fdco
    (5 + 1) * 4MHz = 24MHz  */
    UCSCTL2 = FLLD0 + FLLN0 + FLLN2;
    // Enable the FLL control loop
    __bic_SR_register(SCG0);

  /* Worst-case settling time for the DCO when the DCO range bits have been
     changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
     UG for optimization.
     32 x 32 x 24MHz / 4MHz = 6144 = MCLK cycles for DCO to settle */
  __delay_cycles(70000);

    // Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
    do {
        // Clear XT2,XT1,DCO fault flags
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
        // Clear fault flags
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG); // Test oscillator fault flag
}
