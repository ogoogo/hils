#include <stdio.h>
#include <cstring>
#include <pigpio.h>
#include <iostream>
#include <unistd.h>

using namespace std;

int getControlBits(int, bool);

const int slaveAddress = 0x10; // <-- Your address of choice
bsc_xfer_t xfer; // Struct to control data flow

int main(){
	int nTransferred, nRxFIFO, nTxFIFO;
	gpioInitialise();
	cout << "Initialized GPIOs\n";

	// Close old device (if any)
	xfer.control = getControlBits(slaveAddress, false); // To avoid conflicts when restarting
	bscXfer(&xfer);
	cout << "I2C Disabled\n\n";

       // Transmit Hello World to the I2C
	xfer.control = getControlBits(slaveAddress, true);
	memcpy(xfer.txBuf, "Hello World", 11);
	xfer.txCnt = 11;
	int status = bscXfer(&xfer);

	if (status > 0) {
		nTransferred = (status >> 16) & 0x1F ;		// # of bytes transferred to FIFO
		nRxFIFO = (status >> 11) & 0x1F;				// #bytes in Rx FIFO
		nTxFIFO = (status >> 6) & 0x1F;					// #bytes in Tx FIFO
		printf("nTX %d, nRxFIFO %d, nTxFIFO %d\n", nTransferred, nRxFIFO, nTxFIFO);
		if (status & 0x20) printf("Receive Busy\n");
		if (status & 0x10) printf("Transmit FIFO Empty\n");
		if (status & 0x08) printf("Receive FIFO Full\n");
		if (status & 0x04) printf("Transmit FIFO Full\n");
		if (status & 0x02) printf("Receive FIFO Empty\n");
		if (status & 0x01) printf("Transmit Busy\n");
	}

	sleep(1);

	printf("\nDisabling I2C\n");
	xfer.control = getControlBits(slaveAddress, false); 
	xfer.txCnt = 0;
	status = bscXfer(&xfer);
	if (status > 0) {
		nTransferred = (status >> 16) & 0x1F ;
		nRxFIFO = (status >> 11) & 0x1F;
		nTxFIFO = (status >> 6) & 0x1F;
		printf("nTX %d, nRxFIFO %d, nTxFIFO %d\n", nTransferred, nRxFIFO, nTxFIFO);
		// printf("FIFO Size = %d\n", BSC_FIFO_SIZE);
		if (status & 0x20) printf("Receive Busy\n");
		if (status & 0x10) printf("Transmit FIFO Empty\n");
		if (status & 0x08) printf("Receive FIFO Full\n");
		if (status & 0x04) printf("Transmit FIFO Full\n");
		if (status & 0x02) printf("Receive FIFO Empty\n");
		if (status & 0x01) printf("Transmit Busy\n");
		// printf("Status = %d, %x\n", status, status);
	}
	return 0;
}

int getControlBits(int address /* max 127 */, bool open) {
		/*
		Excerpt from http://abyz.me.uk/rpi/pigpio/cif.html#bscXfer regarding the control bits:

		22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
		a	a	a	a	a	a	a	-	-	IT HC TF IR RE TE BK EC ES PL PH I2 SP EN

		Bits 0-13 are copied unchanged to the BSC CR register. See pages 163-165 of the Broadcom 
		peripherals document for full details. 

		aaaaaaa defines the I2C slave address (only relevant in I2C mode)
		IT	invert transmit status flags
		HC	enable host control
		TF	enable test FIFO
		IR	invert receive status flags
		RE	enable receive
		TE	enable transmit
		BK	abort operation and clear FIFOs
		EC	send control register as first I2C byte
		ES	send status register as first I2C byte
		PL	set SPI polarity high
		PH	set SPI phase high
		I2	enable I2C mode
		SP	enable SPI mode
		EN	enable BSC peripheral
		*/

		// Flags like this: 0b/*IT:*/0/*HC:*/0/*TF:*/0/*IR:*/0/*RE:*/0/*TE:*/0/*BK:*/0/*EC:*/0/*ES:*/0/*PL:*/0/*PH:*/0/*I2:*/0/*SP:*/0/*EN:*/0;

		int flags;
		if(open)
				flags = /*RE: */ (1 << 9) | /*TE:*/ (1 << 8) | /*I2:*/ (1 << 2) | /*EN:*/ (1 << 0);
		else // Close/Abort
				flags = /*BK:*/ (1 << 7) | /*I2:*/ (0 << 2) | /*EN:*/ (0 << 0);

		return (address << 16 /*= to the start of significant bits*/) | flags;
}