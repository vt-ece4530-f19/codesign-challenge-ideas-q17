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


// This code demonstrates reading and writing from PIO port
//  IOWR_ALTERA_AVALON_PIO_DATA(PIO_1_BASE, k);
//  while (1) {
//    i = IORD_ALTERA_AVALON_PIO_DATA(PIO_0_BASE);
//    i = i + 1;
//    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, i);
//    if ((i & 0xFFFFF) == 0x0) {
//      k = IORD_ALTERA_AVALON_PIO_DATA(PIO_1_BASE);
//      k = (k == 0x80) ? 1 : k << 1;
//      IOWR_ALTERA_AVALON_PIO_DATA(PIO_1_BASE, k);
//    }
//  }
