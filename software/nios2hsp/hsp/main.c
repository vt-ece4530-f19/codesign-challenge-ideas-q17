#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "socal/socal.h"
#include "socal/hps.h"

// 128MB region in FPGA slave area
#define FPGABRIDGEBASE  0xc0000000 
#define FPGABRIDGESPAN   0x8000000

#include "system.h" // from NIOS BSP

#define NIOSREQ 0x1
#define HPSREQ  0x2

static volatile unsigned *pio_0;
static volatile unsigned *pio_1;

void reset_handshake(int pattern) {
  unsigned p = alt_read_word(pio_1);
  p = p & ~pattern;
  alt_write_word(pio_1, p);
}

void set_handshake(int pattern) {
  unsigned p = alt_read_word(pio_1);
  p = p | pattern;
  alt_write_word(pio_1, p);
}

int isreset_handshake(int pattern) {
  unsigned p = alt_read_word(pio_1);
  return ((p & pattern) == 0);
}
	  
int isset_handshake(int pattern) {
  unsigned p = alt_read_word(pio_1);
  return ((p & pattern) != 0);
}

int main() {
  int fd;
  void *virtual_base;

  printf("Opening shared-memory channel to NIOS\n");
  
  if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
    printf( "ERROR: could not open \"/dev/mem\"...\n" );
    return( 1 );
  }
  virtual_base = mmap( NULL,
           FPGABRIDGESPAN,
           ( PROT_READ | PROT_WRITE ),
           MAP_SHARED,
           fd,
           FPGABRIDGEBASE );  
  if( virtual_base == MAP_FAILED ) {
    printf( "ERROR: mmap() failed...\n" );
    close( fd );
    return(1);
  }

  pio_0 = virtual_base + (PIO_0_BASE);
  pio_1 = virtual_base + (PIO_1_BASE);

  while (isset_handshake(HPSREQ))
    usleep(1000);  // wait for clear from NIOS, if set

  volatile int *data;
  int runs = 0;
  int i;
  
  while (1) {

    // wait for request from NIOS
    while (isreset_handshake(NIOSREQ))
      usleep(1000);

    // now we can access data. shared data address is accessible to pio_0
    // we need to adjust the address to out virtual memory map
    data = (int *) (virtual_base + alt_read_word(pio_0));

    printf("HPS runs %d\n", runs++);
    
    // read 100 integers, and add 1000 to them
    for (i=0; i<100; i++)
      data[i] = data[i] + 1000;

    // signal NIOS that we are done
    set_handshake(HPSREQ);

    // wait for NIOS to clear handshake
    while (isset_handshake(NIOSREQ))
      usleep(1000);

    // clear HPS handshake
    reset_handshake(HPSREQ);
  }
  
  return 0;
}
