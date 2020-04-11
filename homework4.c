#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"


/*
 * Helen Huavil
 * PID: hhuavil
 */
int main(void)
{
    char rChar;
    char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";

    // TODO: Declare the variables that main uses to interact with your state machine.
    bool success = false;
    // Stops the Watchdog timer.
    initBoard();
    // TODO: Declare a UART config struct as defined in uart.h.
    //       To begin, configure the UART for 9600 baud, 8-bit payload (LSB first), no parity, 1 stop bit.

    /*
     * !! I noticed in the examples that this struct was declared as eUSCI_UART_Config
     *  instead of eUSCI_UART_ConfigV1 - I updated my SDK and followed all the setup instructions
     *  but my uart.h file uses eUSCI_UART_ConfigV1, change to eUSCI_UART_Config if doesn't compile locally. ty!
     */
    const eUSCI_UART_ConfigV1 uartConfig = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        19,
        8,
        0x55,
        EUSCI_A_UART_NO_PARITY,
        EUSCI_A_UART_LSB_FIRST,
        EUSCI_A_UART_ONE_STOP_BIT,
        EUSCI_A_UART_MODE,
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,
        EUSCI_A_UART_8_BIT_LEN};

    // TODO: Make sure Tx AND Rx pins of EUSCI_A0 work for UART and not as regular GPIO pins.
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION); //config Rx for P1.2
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION); //config Tx for P1.3
    // TODO: Initialize EUSCI_A0
    UART_initModule(EUSCI_A0_BASE, &uartConfig); //initialize module

    // TODO: Enable EUSCI_A0
    UART_enableModule(EUSCI_A0_BASE);

    while(1)
    {
        // TODO: Check the receive interrupt flag to see if a received character is available.
        //       Return 0xFF if no character is available.
        if (UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
        {
            rChar = UART_receiveData(EUSCI_A0_BASE);
        }
        else
        {
            rChar = 0xFF;
        }
        // TODO: If an actual character was received, echo the character to the terminal AND use it to update the FSM.
        //       Check the transmit interrupt flag prior to transmitting the character.
        if (UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG
                && rChar != 0xFF) //check flag and that character was received
        {
            UART_transmitData(EUSCI_A0_BASE, rChar);
            success = charFSM(rChar); //send char to FSM
        }

        // TODO: If the FSM indicates a successful string entry, transmit the response string.
        //       Check the transmit interrupt flag prior to transmitting each character and moving on to the next one.
        //       Make sure to reset the success variable after transmission.
        if (success)
        {
            int i = 0;
            //while not string terminator (null)
            while(response[i]!='\0')
            {
               //check transmit flag before each char is printed
               if(UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
               {
                   //if transmit flag is set, print response at i, increment i
                  UART_transmitData(EUSCI_A0_BASE, response[i]);
                  ++i;
               }
            }
            success = false;
        }


    }
}

void initBoard()
{
    WDT_A_hold(WDT_A_BASE);
}

// TODO: FSM for detecting character sequence.
bool charFSM(char rChar)
{
    bool finished = false;
    static State current  = one;
        switch (current)
        {
            //nothing has been entered
            case one:
               if (rChar == '2')
               {
                   // 2 has been entered, move to state two
                   current = two;
               }
            break;
            //2 has been entered
            case two:
                if (rChar == '2')
                {
                    // 2 has been entered, remain in same state
                    current = two;
                }
                else if (rChar == '5')
                {
                    // 5 has been entered move to case 3
                    current = three;
                }
                else
                {
                    //something else has been entered return to start
                    current = one;
                }
            break;
            //25 have been entered
            case three:
                if (rChar == '2')
                {
                    // 2 has been entered go back to two
                    current = two;
                }
                else if (rChar == '3')
                {
                    // 3 has been entered move to case 4
                    current = four;
                }
                else
                {
                    //something else has been entered return to start
                    current = one;
                }
            break;
            //253 have been entered
            case four:
                if (rChar == '2')
                {
                    // 2 has been entered go back to two
                    current = two;
                }
                else if (rChar == '4')
                {
                    // "2534" has been entered change finish
                    finished = true;
                    current = one;
                }
                else
                {
                    //something else has been entered return to start
                    current = one;
                }
            break;
        }

    return finished;
}
