# codesign-challenge-ideas-q17

This repository demonstrates the use of NIOS-HPS communication through shared memory. The design is made for quartus 17.1 (while ECE4530 used quartus 18.1). The following is a walk-through of running the example including hardware synthesis, NIOS software compilation, HPS software compilation, and running the example.

## Compiling the hardware

Clone the repository on your computer and open the design in Quartus 17.1. Study the system architecture in platform designer. At high level, the design includes the following components.

* Nios II/e 
* 128K on-chip memory
* DDR interface to 64MB SDRAM
* Three PIO ports
* Two timers
* JTAG UART
* HPS bridge

The objective of the example is to demonstrate communication from NIOS to HPS and back, through the on-chip memory.
Compile the hardware in Quartus. This results in a bitstream exampleniossdram.sof. Next, download the bitstream to a DE1-SoC FPGA board.

~~~
nios2-configure-sof -d 2 exampleniossdram.sof
~~~

## Compiling the Nios-II software

Run the Nios BSP editor and create a BSP package with the following properties.

* Connect timer_0 to the system clock and timer_1 to the timestamp timer
* Disable the alt_load facility
* Create an additional data segment called `.onchipdata` mapped onto the onchip_memory2

Save the resulting BSP configuration and generate the library

(in `nios2 command shell`)
~~~
cd software
nios2-bsp-generate-files --settings hal_bsp/settings.bsp --bsp-dir hal_bsp
cd hal_bsp
make
~~~

Next, compile the NIOS part of the NIOS-HPS software.

(in `nios2 command shell`)
~~~
cd nios2hsp/nios
nios2-app-generate-makefile --bsp-dir ../../hal_bsp --elf-name main.elf --src-files main.c
make
~~~

Next, open a nios2 terminal and run the program

(in `nios2 command shell`)
~~~
nios2-terminal
~~~

(in `nios2 command shell`)
~~~
nios2-download main.elf --go
~~~

To understand the nios application, it will help to understand how a handshake protocol works (https://rijndael.ece.vt.edu/ece4530f19/lectures/lecture09-notes.html).

~~~
#include "system.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "altera_avalon_pio_regs.h"

unsigned int __attribute__((section (".onchipdata"))) demodata[100];

#define NIOSREQ 0x1
#define HPSREQ  0x2

void reset_handshake(int pattern) {
  unsigned p = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE);
  p = p & ~pattern;
  IOWR_ALTERA_AVALON_PIO_DATA(PIO_1_BASE, p);
}

void set_handshake(int pattern) {
  unsigned p = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE);
  p = p | pattern;
  IOWR_ALTERA_AVALON_PIO_DATA(PIO_1_BASE, p);
}

int isreset_handshake(int pattern) {
  unsigned p = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE);
  return ((p & pattern) == 0);
}

int isset_handshake(int pattern) {
  unsigned p = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE);
  return ((p & pattern) != 0);
}

int main() {
  unsigned i = 0;

  printf("Clear target\n");
  memset(demodata, 0, sizeof(unsigned int) * 100);

  // this is how HPS knows what address to read demodata from:
  // the base address is stored in a PIO port accessible from HPS
  //
  // Note that we pass demodata address as an int (although it is a 32-bit pointer)
  IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, (int) demodata);

  // reset handshake
  reset_handshake(NIOSREQ);
  reset_handshake(HPSREQ);

  usleep(500000); // 500ms

  int acc, runs = 0;

  while (1) {
    // initialize
    for (i=0; i<100; i++)
      demodata[99-i] = i;

    // set request
    set_handshake(NIOSREQ);

    // wait for HPS
    while (isreset_handshake(HPSREQ))
      usleep(1000); // 1ms

    // read data and accumulate
    acc = 0;
    for (i=0; i<100; i++)
      acc += demodata[i];

    printf("NIOS runs %d checksum %d\n", runs++, acc);

    // reset request
    reset_handshake(NIOSREQ);

    // wait for HPS
    while (isset_handshake(HPSREQ))
      usleep(1000); // 1ms
  }

  return 0;
}
~~~

