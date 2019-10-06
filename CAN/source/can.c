/*
 * can.c
 *
 *  Created on: Sep 27, 2019
 *      Author: Manuel Mollon
 *
 *  Bibliography: 	https://mcuxpresso.nxp.com/apidoc/group__flexcan__driver.html#ga8d7ebf086fb294130fa30a0f75a5f340
 *  				Kinetis K64 Reference Manual
 */

#include "can.h"
#include "MK64F12.h"
#include "hardware.h"
#include "pinsHandler.h"


/****************************************************
 * DEFINES
 ****************************************************/

/* Source Clock frequency in Hertz. */
#define SRC_CLK	50000000UL
/* FIFO Buffer defines. */
#define EMPTY	(-1) /* Top value when buffer is empty. */
#define BUFFER_SIZE	30	/* Size of FIFO buffer to store messages for user. */
/* Max bytes transmitted in one frame. */
#define BYTE_CANT	8
/* Number of time quantas in one bit time. */
#define TIME_QUANTA_CANT 10
/* Max vaue of PRESDIV in CTRL1 Register. */
#define MAX_PRESDIV	255
/* No message in FIFO buffer define */
#define NONE	((uint8_t)0)
/* Message Buffer Masks to set CS code. */
#define TX_MESSAGE_BUFFER_INDEX	0
#define INACTIVE_TX_MB_CODE	0b1000
#define ACTIVE_RX_MB_CODE	0b0100
#define DATA_TX_MB_CODE		0b1100
/* CAN ID mask converter to set the register */
#define CAN_ID(id)	(((uint32_t)(((uint32_t)(id)) << CAN_ID_STD_SHIFT)) & CAN_ID_STD_MASK)
/* ID mask to revert de CAN_ID mask */
#define REAL_ID(id)	((uint32_t)(((uint32_t)(id)) >> CAN_ID_STD_SHIFT))
/* GPIO Defines. */
#define CAN_MUX	((uint8_t)2)
#define NUM_PIN_TX	18
#define NUM_PIN_RX	19
#define DISABLE_MODE 0 /* Disable port interrupts */

/******************************************************
 * LOCAL VARIABLE DEFINITIONS
 ******************************************************/

typedef struct{
	uint8_t length;
	uint32_t id;
	uint32_t dataWord0;
	uint32_t dataWord1;
}can_frame_t;

typedef struct{
	int top;
	can_frame_t rxFrameArray[BUFFER_SIZE];
}FIFObufferRx_t;

/******************************************************
 * LOCAL VARIABLE DECLARATIONS
 ******************************************************/

static can_frame_t txFrame;
static can_frame_t rxFrame;
static FIFObufferRx_t rxBuffer;
static bool sendingTx = false;
static uint8_t MbNum = 0;
static uint32_t srcClockHz;
static char msgAux[BYTE_CANT] = {'0','0','0','0','0','0','0','0'};

/******************************************************
 * LOCAL FUNCTION DECLARATIONS
 ******************************************************/

/**
 * @brief Initializes the FlexCAN module with user-defined settings.
 * @param base FlexCAN peripheral base address.
 * @param config Pointer to user-defined configuration structure.
*/
static void FLEXCANinit(CAN_Type * base, const can_config_t * config);

/**
 * @brief Enables or disables the FlexCAN module.
 * @param base FlexCAN peripheral base address.
 * @param enable True to enable, false to disable.
*/
static void FLEXCANenable(CAN_Type * base, bool enable);

/**
 * @brief Resets the FlexCAN module.
 * @param base FlexCAN peripheral base address.
*/
static void FLEXCANreset(CAN_Type * base);

/**
 * @brief Enables the FlexCAN clock for the module.
 * @param base FlexCAN peripheral base address.
*/
static void FLEXCANenableClock(CAN_Type * base);

/**
 * @brief Sets Baud Rate for the CAN module. Enter this function only in Freeze Mode.
 * @param base FlexCAN peripheral base address.
 * @param srcClockHz Source clock frequency in Hertz.
 * @param baudRate desired baud rate for communication in bps.
*/
static void FLEXCANsetBaudRate(CAN_Type * base, uint32_t srcClockHz, uint32_t baudRate);

/**
 * @brief Aborts the previous transmission, cleans the Message Buffer, and configures it as a Transmit Message Buffer.
 * @param base FlexCAN peripheral base address.
 * @param mbIdx	The Message Buffer index.
 * @param config Pointer to FlexCAN Message Buffer configuration structure.
 * @param enable Enable/Disable Rx Message Buffer. True to enable, false to disable.
*/
static void FLEXCANsetRxMbConfig(CAN_Type * base, uint8_t mbIdx, uint32_t id, bool enable);

/**
 * @brief Cleans the Message Buffer, and configures it as a Transmit Message Buffer.
 * @param base FlexCAN peripheral base address.
 * @param mbIdx	The Message Buffer index.
*/
static void FLEXCANresetRxMbConfig(CAN_Type * base, uint8_t mbIdx);

/**
 * @brief Aborts the previous transmission, cleans the Message Buffer, and configures it as a Transmit Message Buffer.
 * @param base FlexCAN peripheral base address.
 * @param mbIdx	The Message Buffer index.
 * @param enable Enable/Disable Tx Message Buffer. True to enable, false to disable.
*/
static void FLEXCANsetTxMbConfig(CAN_Type * base, uint8_t mbIdx, bool enable);

/**
 * @brief Cleans the Message Buffer, and configures it as a Transmit Message Buffer, keeping the ID.
 * @param base FlexCAN peripheral base address.
 * @param mbIdx	The Message Buffer index.
*/
static void FLEXCANresetTxMbConfig(CAN_Type * base, uint8_t mbIdx);

/**
 * @brief writes a CAN Message to the specified Transmit Message Buffer and changes the Message Buffer state to start CAN Message transmit. After that the function returns immediately.
 * @param base FlexCAN peripheral base address.
 * @param mbIdx	The Message Buffer index.
 * @param txFrame Pointer to CAN message frame to be sent.
*/
static status_t FLEXCANwriteTxMb(CAN_Type * base, uint8_t mbIdx, const can_frame_t * txFrame);

/**
 * @brief reads a CAN Message to the specified Received Message Buffer and changes the Message Buffer state to start CAN Message transmit. After that the function returns immediately.
 * @param base FlexCAN peripheral base address.
 * @param mbIdx	The Message Buffer index.
 * @param rxFrame Pointer to CAN message frame to be sent.
*/
static status_t FLEXCANreadRxMb(CAN_Type * base, uint8_t mbIdx, can_frame_t * rxFrame);

/**
 * @brief Stored rxFrame in FIFO buffer with recursion to shift the array.
 * @param index where to start shifitng the array. Preferably with the buffer top.
*/
static void recursiveStore(int index);

/**
 * @brief Interrupt handler for received and sent messages in MB.
*/
void CAN0_ORed_Message_buffer_IRQHandler(void);

/******************************************************
 * LOCAL FUNCTION DEFINITIONS
 ******************************************************/

static void FLEXCANinit(CAN_Type * base, const can_config_t * config){
	uint32_t auxMCR;
	srcClockHz = SRC_CLK;

	/* Enable clock for module (clock gating) */
	FLEXCANenableClock(base);

    /* Clock source selection in Control Register */
    base->CTRL1 = base->CTRL1 & (~CAN_CTRL1_CLKSRC_MASK); /* sets CLKSRC to 0. */

    /* Enables CAN module */
    FLEXCANenable(base, true);

    while(base->MCR & CAN_MCR_LPMACK_MASK ){
        /* Waits until Low Power Mode is exit. */
    }

    /* Enter Freeze Mode for modification. */
    base->MCR |= CAN_MCR_HALT_MASK;
    while (!(base->MCR & CAN_MCR_FRZACK_MASK)){
    	/* Waits until freeze mode is enabled to continue. */
    }

    ///* Reset MCR to default values */
    //FLEXCANreset(base);
    auxMCR = base->MCR;
    /* Sets number of Mb */
    auxMCR = (auxMCR & ~CAN_MCR_MAXMB_MASK) | CAN_MCR_MAXMB(config->maxMbNum - 1); // first clears MAXMB field, then stores new value
    /* Save MCR Configuration. */
    base->MCR = auxMCR;
    /* Baud Rate Configuration.*/
    FLEXCANsetBaudRate(base, srcClockHz, config->baudRate);
    /* Enable Interrupts for the number of MB in config. */
    base->IMASK1 &= (CAN_IMASK1_BUFLM_MASK << config->maxMbNum);
    base->IMASK1 |= ~(CAN_IMASK1_BUFLM_MASK << config->maxMbNum);

    /* Exit Freeze Mode. */
    base->MCR &= (~CAN_MCR_FRZ_MASK) & (~CAN_MCR_HALT_MASK);
    while (base->MCR & CAN_MCR_FRZACK_MASK){
    	/* Waits until freeze mode is disabled to continue. */
    }
}

static void FLEXCANenableClock(CAN_Type * base){
	if(base == CAN0){
		/* Set FlexCAN0 Clock Gate Control value in 1, enabling clock. */
		SIM->SCGC6 |= SIM_SCGC6_FLEXCAN0_MASK;
	}
}

static void FLEXCANreset(CAN_Type * base){
    /* Inserts Soft Reset Signal to MCR. */
    base->MCR |= CAN_MCR_SOFTRST_MASK;
    /* Waits until reset finishes. */
    while (base->MCR & CAN_MCR_SOFTRST_MASK){
    }
}

static void FLEXCANsetBaudRate(CAN_Type * base, uint32_t srcClockHz, uint32_t baudRate){
	/* Parsing variables. Where baudRate needs to be positive and not to big. According to equations source clock should be greater than (Baud Rate)*(Number of time quantas) */
    if(!((baudRate > 0)&&(baudRate <= 1000000U)&&((baudRate * TIME_QUANTA_CANT) <= srcClockHz))){
    	return;
    }

    /* Starting from reference manual formula: Sclock frequency = PE clock frequency / (PRESDIV + 1). */
    uint32_t presdiv = (srcClockHz /(baudRate*TIME_QUANTA_CANT))- 1;

    if (presdiv > MAX_PRESDIV){
    	presdiv = MAX_PRESDIV;
    }

    /**
     *  Setting duration in number of time quantas for each segment, according to equation:
     * TIME_QUANTA_CANT = PSEG1 + PSEG2 + PROPSEG + 4
     */

    uint8_t rjw = 1;
    uint8_t propSeg = 2;
    uint8_t pSeg1 = 2;
    uint8_t pSeg2 = 2;

    /* Cleans Timing Setting to default */
    base->CTRL1 &= ~(CAN_CTRL1_PRESDIV_MASK | CAN_CTRL1_RJW_MASK | CAN_CTRL1_PSEG1_MASK | CAN_CTRL1_PSEG2_MASK | CAN_CTRL1_PROPSEG_MASK);

    /* Updates Control Register Setting to match new baud rate. */
    base->CTRL1 |= 	(CAN_CTRL1_PRESDIV(presdiv) | CAN_CTRL1_RJW(rjw) |
    				CAN_CTRL1_PSEG1(pSeg1) | CAN_CTRL1_PSEG2(pSeg2) | CAN_CTRL1_PROPSEG(propSeg));

}

static void FLEXCANenable(CAN_Type * base, bool enable){
	if(enable){
		base->MCR &= ~CAN_MCR_MDIS_MASK;
	}
	else{
		base->MCR |= CAN_MCR_MDIS_MASK;
	}
}

/**
 *  To be able to receive CAN frames into a Mailbox, the CPU must prepare it for reception
 * by executing the following steps:
 * 1. If the Mailbox is active (either Tx or Rx) inactivate the Mailbox (see Mailbox
 * inactivation), preferably with a safe inactivation (see Transmission abort
 * mechanism).
 * 2. Write the ID word
 * 3. Write the EMPTY code (0b0100) to the CODE field of the Control and Status word
 * to activate the Mailbox.
 */
static void FLEXCANsetRxMbConfig(CAN_Type * base, uint8_t mbIdx, uint32_t id, bool enable){
    if(mbIdx > (base->MCR & CAN_MCR_MAXMB_MASK)){
    	return;
    }

    /* Clean Message Buffer and inactivate. */
    base->MB[mbIdx].CS = 0;
    base->MB[mbIdx].ID = 0x0;
    base->MB[mbIdx].WORD0 = 0x0;
    base->MB[mbIdx].WORD1 = 0x0;

    if (enable){
        base->MB[mbIdx].ID = CAN_ID(id);
        /* Activates Message Buffer as Rx. */
        base->MB[mbIdx].CS = CAN_CS_CODE(ACTIVE_RX_MB_CODE);
    }
}

static void FLEXCANresetRxMbConfig(CAN_Type * base, uint8_t mbIdx){
    /* Clean Message Buffer and inactivate. */
    base->MB[mbIdx].CS = 0;
    base->MB[mbIdx].WORD0 = 0x0;
    base->MB[mbIdx].WORD1 = 0x0;

    /* Activates Message Buffer as Rx. */
    base->MB[mbIdx].CS = CAN_CS_CODE(ACTIVE_RX_MB_CODE);
}

static void FLEXCANsetTxMbConfig(CAN_Type * base, uint8_t mbIdx, bool enable){
    if(mbIdx > (base->MCR & CAN_MCR_MAXMB_MASK)){
    	return;
    }

    /* Clean Message Buffer and inactivate. */
    base->MB[mbIdx].CS = 0;
    base->MB[mbIdx].ID = 0;
    base->MB[mbIdx].WORD0 = 0;
    base->MB[mbIdx].WORD1 = 0;

    if (enable){
    	/* Activates Message Buffer as Tx. */
        base->MB[mbIdx].CS = CAN_CS_CODE(INACTIVE_TX_MB_CODE);
    }
}

static void FLEXCANresetTxMbConfig(CAN_Type * base, uint8_t mbIdx){
    /* Clean Message Buffer and inactivate. */
    base->MB[mbIdx].CS = 0;
    base->MB[mbIdx].WORD0 = 0;
    base->MB[mbIdx].WORD1 = 0;
    /* Activates Message Buffer as Tx. */
    base->MB[mbIdx].CS = CAN_CS_CODE(INACTIVE_TX_MB_CODE);
}

/**
 *  To transmit a CAN frame, the CPU must prepare a Message Buffer for transmission by
 * executing the following procedure:
 * 1. Check whether the respective interrupt bit is set and clear it.
 * 2. If the MB is active (transmission pending), write the ABORT code (0b1001) to the
 * CODE field of the Control and Status word to request an abortion of the
 * transmission. Wait for the corresponding IFLAG to be asserted by polling the IFLAG
 * register or by the interrupt request if enabled by the respective IMASK. Then read
 * back the CODE field to check if the transmission was aborted or transmitted (see
 * Transmission abort mechanism). If backwards compatibility is desired (MCR[AEN]
 * bit is negated), just write the INACTIVE code (0b1000) to the CODE field to
 * inactivate the MB but then the pending frame may be transmitted without notification
 * (see Mailbox inactivation).
 * 3. Write the ID word.
 * 4. Write the data bytes.
 * 5. Write the DLC, Control, and CODE fields of the Control and Status word to activate
 * the MB.
 * IRQ: status flag is set
 * in the Interrupt Flag Register and an interrupt is generated if allowed by the
 * corresponding Interrupt Mask Register bit.
 */
static status_t FLEXCANwriteTxMb(CAN_Type * base, uint8_t mbIdx, const can_frame_t * txFrame){
	/* If message length is greater than the maximum or message buffer index is out of range, return. */
    if((txFrame->length > BYTE_CANT)||(mbIdx > (base->MCR & CAN_MCR_MAXMB_MASK))){
    	return FLEXCAN_TxFail;
    }
    /* Check whether the respective interrupt bit is set and clear it. */
    if( CAN0->IFLAG1 & (CAN_IFLAG1_BUF0I_MASK << mbIdx)){
    	/* Clear Interrupt bit in IFLAG1 */
    	CAN0->IFLAG1 = CAN_IFLAG1_BUF0I_MASK << mbIdx;
    }
    /* Checks if Message Buffer is available. If Mb is active, then it's not available */
    if (CAN_CS_CODE(DATA_TX_MB_CODE) != (base->MB[mbIdx].CS & CAN_CS_CODE_MASK))
    {
        /* Inactive Tx Message Buffer by first clearing CS code and then writing CS code with inactive code */
    	base->MB[mbIdx].CS &= (~CAN_CS_CODE_MASK);
        base->MB[mbIdx].CS |= CAN_CS_CODE(INACTIVE_TX_MB_CODE);
        /* Fill Message ID field. */
        base->MB[mbIdx].ID = CAN_ID(txFrame->id);
        /* Load Message Buffer with message to send. */
        base->MB[mbIdx].WORD0 = txFrame->dataWord0;
        base->MB[mbIdx].WORD1 = txFrame->dataWord1;
        /**
         *  Activate Tx Message Buffer.
         * DLC: Length of Data in Bytes
         */
    	base->MB[mbIdx].CS &= (~CAN_CS_CODE_MASK) & (~CAN_CS_IDE_MASK) & (~CAN_CS_DLC_MASK) & (~CAN_CS_RTR_MASK) & (~CAN_CS_SRR_MASK);
        base->MB[mbIdx].CS |= CAN_CS_CODE(DATA_TX_MB_CODE) | CAN_CS_DLC(txFrame->length);
        /* Sets sending bool to true. This should be turned to false when message has finished sending (in IRQ) */
        sendingTx = true;
        return FLEXCAN_TxSuccess;
    }
    else
    {
        /* Tx Message Buffer is already activated (sending another message). */
        return FLEXCAN_TxFail;
    }
}

/**
 *  Now we can read the message following the sequence:
 * 1. Read the Control and Status word of that Mailbox.
 * 2. Check if the BUSY bit is deasserted, indicating that the Mailbox is locked. Repeat
 * step 1) while it is asserted.
 * 3. Read the contents of the Mailbox. Once Mailbox is locked now, its contents won’t be
 * modified by FlexCAN Move-in processes.
 * 4. Acknowledge the proper flag at IFLAG registers.
 * 5. Read the Free Running Timer. It is optional but recommended to unlock Mailbox as
 * soon as possible and make it available for reception.
 * IRQ: A status flag is set in the Interrupt Flag Register and an interrupt is generated if
 * allowed by the corresponding Interrupt Mask Register bit.
 */
static status_t FLEXCANreadRxMb(CAN_Type * base, uint8_t mbIdx, can_frame_t * rxFrame){
	/* If message buffer index is out of range, return. */
    if(mbIdx > (base->MCR & CAN_MCR_MAXMB_MASK)){
    	return FLEXCAN_TxFail;
    }

    uint32_t csAux;

    /* Reads the CS word of the Rx Message Buffer to lock it. */
    csAux = base->MB[mbIdx].CS;

    /* Read content in Mailbox*/
    /* Store Message ID in received struct. */
    rxFrame->id = base->MB[mbIdx].ID & (CAN_ID_EXT_MASK | CAN_ID_STD_MASK);

    /* Get the message length. */
    rxFrame->length = (csAux & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT;

    /* Store Message in buffer to the received struct. */
    rxFrame->dataWord0 = base->MB[mbIdx].WORD0;
    rxFrame->dataWord1 = base->MB[mbIdx].WORD1;

    /* Read free-running timer, corresponds to step 5 (to unlock mailbox) */
    base->TIMER;
    return FLEXCAN_TxSuccess;
}

/**
 * At the end of the successful transmission, the
 * value of the Free Running Timer is written into the Time Stamp field, the CODE field in
 * the Control and Status word is updated, the CRC Register is updated, a status flag is set
 * in the Interrupt Flag Register and an interrupt is generated if allowed by the
 * corresponding Interrupt Mask Register bit. The new CODE field after transmission
 * depends on the code that was used to activate the MB (see Table 49-4 and Table 49-5 in
 * Message buffer structure).
 */
static void FLEXCANTxHandler(uint8_t mbIdx){
	/* Prepare this MB for a new transmission or reception. */
	FLEXCANresetTxMbConfig(CAN0, mbIdx);
	/* Clear Interrupt bit in IFLAG1 */
	CAN0->IFLAG1 = CAN_IFLAG1_BUF0I_MASK << mbIdx;
	/* Sending message over. */
	sendingTx = false;
}

static void FLEXCANRxHandler(uint8_t mbIdx){
	/* Reads the received message from the receive message buffer. */
	FLEXCANreadRxMb(CAN0, mbIdx, &rxFrame);
	/* Cleans the receive message buffer status. */
	FLEXCANresetRxMbConfig(CAN0, mbIdx);
	/* Store message in FIFO buffer if there is space. */
	if(rxBuffer.top < (BUFFER_SIZE-1)){
		recursiveStore(rxBuffer.top);
			rxBuffer.top++;
	}
	/* Clear Interrupt bit in IFLAG1 */
	CAN0->IFLAG1 = CAN_IFLAG1_BUF0I_MASK << mbIdx;
}

static void recursiveStore(int index){
	if(index == -1){
		rxBuffer.rxFrameArray[index+1] = rxFrame;
	}
	else{
		rxBuffer.rxFrameArray[index+1] = rxBuffer.rxFrameArray[index];
		index--;
		recursiveStore(index);
	}

}

void CAN0_ORed_Message_buffer_IRQHandler(void){
	/* We read CANx_IFLAG1 to see witch MB caused an interruption and call to corresponding handler. */
	for(int i = 0;i<MbNum;i++){
		/* Determine if it interrupt bit is set. */
		if( CAN0->IFLAG1 & (CAN_IFLAG1_BUF0I_MASK << i)){
			/* Determine if it is Rx or Tx MB. */
			if(i == 0){
				/* If it is Tx, calls Tx handler. */
				FLEXCANTxHandler(TX_MESSAGE_BUFFER_INDEX);
			}
			else{
				/* If it is Rx, calls Rx handler. */
				FLEXCANRxHandler(TX_MESSAGE_BUFFER_INDEX+i);
			}
		}
	}
}

/******************************************************
 * GLOBAL FUNCTION DEFINITIONS
 ******************************************************/

void canInit(can_config_t  * config){
	/* Local initialization.*/
	rxBuffer.top = -1;
	MbNum = config->maxMbNum;

	/* GPIO initialization. */
	PORT_Type * portPointers[] = PORT_BASE_PTRS;
	setPCRmux(portPointers[PB], NUM_PIN_RX, CAN_MUX);
	setPCRmux(portPointers[PB], NUM_PIN_TX, CAN_MUX);
	/* Disables port interrupts preventing input or output interrupts. */
	setPCRirqc(portPointers[PB], NUM_PIN_RX, DISABLE_MODE);
	setPCRirqc(portPointers[PB], NUM_PIN_TX, DISABLE_MODE);

	/* FLEXCAN initialization. */
	FLEXCANinit(CAN0, config);
	/* Configures transmit buffers. */
	FLEXCANsetTxMbConfig(CAN0, TX_MESSAGE_BUFFER_INDEX, true);
	for(uint8_t i = 0; i < config->rxMbNum; i++){ /* For each Rx mailbox. */
		/* Sets up the receive message buffer if Rx. */
			FLEXCANsetRxMbConfig(CAN0, (TX_MESSAGE_BUFFER_INDEX+1+i), config->ids[i], true);
	}
	/* Enable Interrupts. */
	NVIC_EnableIRQ(CAN0_ORed_Message_buffer_IRQn);
}

void canWriteMsg(char * messageArray, uint32_t id, uint8_t cant){
	for(int i = 0;i<cant;i++){
		msgAux[i] = messageArray[i];
	}
	/* Prepares the transmit frame for sending. */
	txFrame.length = cant;
	txFrame.dataWord0 = CAN_WORD0_DATA_BYTE_0(msgAux[0]) |
						CAN_WORD0_DATA_BYTE_1(msgAux[1]) |
						CAN_WORD0_DATA_BYTE_2(msgAux[2]) |
						CAN_WORD0_DATA_BYTE_3(msgAux[3]);
	txFrame.dataWord1 = CAN_WORD1_DATA_BYTE_4(msgAux[4]) |
						CAN_WORD1_DATA_BYTE_5(msgAux[5]) |
						CAN_WORD1_DATA_BYTE_6(msgAux[6]) |
						CAN_WORD1_DATA_BYTE_7(msgAux[7]);
	txFrame.id = id;
	/* Writes a transmit message buffer to send a CAN Message. */
	FLEXCANwriteTxMb(CAN0, TX_MESSAGE_BUFFER_INDEX, &txFrame);
}

bool canIsTxMsgComplete(void){
	return !sendingTx;
}

bool canIsRxMsg(void){
	bool isRxMsg = false;
	/* Sets to true if (top + 1) is greater than 0, which means at least one message to read. */
	if((rxBuffer.top+1)>0){
		isRxMsg = true;
	}
	return isRxMsg;
}

uint8_t canGetRxMsgLength(void){
	if((rxBuffer.top+1)>0){
		return rxBuffer.rxFrameArray[rxBuffer.top].length;
	}
	else{
		return NONE;
	}
}

uint8_t canReadMsg(char * msg, uint32_t * id, uint8_t cant){
	/* If real size of the message to return is greater than cant, then doesn't return a message. */
	uint8_t realCant = rxBuffer.rxFrameArray[rxBuffer.top].length;
	if(cant >= realCant){
		cant = realCant;

		/* Fills msgAux with chars extracted from the uint32_t words. */
		uint8_t * uintAux = (uint8_t *)&rxBuffer.rxFrameArray[rxBuffer.top].dataWord0;
		msgAux[0] = uintAux[3];
		msgAux[1] = uintAux[2];
		msgAux[2] = uintAux[1];
		msgAux[3] = uintAux[0];
		uintAux = (uint8_t *)&rxBuffer.rxFrameArray[rxBuffer.top].dataWord1;
		msgAux[4] = uintAux[3];
		msgAux[5] = uintAux[2];
		msgAux[6] = uintAux[1];
		msgAux[7] = uintAux[0];

		/* Pastes message in string and corresponding id. */
		*id = REAL_ID(rxBuffer.rxFrameArray[rxBuffer.top].id);
		for(int i = 0; i < realCant;i++){
			msg[i] = msgAux[i];
		}

		/* Erase message from FIFO buffer. */
		rxBuffer.top--;
	}
	else{
		cant = 0;
	}
	return cant;
}
