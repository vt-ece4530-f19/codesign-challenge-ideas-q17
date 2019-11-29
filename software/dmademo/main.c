#include "system.h"
#include <stdio.h>
#include <string.h>
#include "sys/alt_dma.h"

unsigned testdata[100] = {
7, 2, 1, 0, 4, 1, 4, 9, 5, 9,
0, 6, 9, 0, 1, 5, 9, 7, 3, 4,
9, 6, 6, 5, 4, 0, 7, 4, 0, 1,
3, 1, 3, 4, 7, 2, 7, 1, 2, 1,
1, 7, 4, 2, 3, 5, 1, 2, 4, 4,
6, 3, 5, 5, 6, 0, 4, 1, 9, 5,
7, 8, 9, 3, 7, 4, 6, 4, 3, 0,
7, 0, 2, 9, 1, 7, 3, 2, 9, 7,
7, 6, 2, 7, 8, 4, 7, 3, 6, 1,
3, 6, 9, 3, 1, 4, 1, 7, 6, 9};

unsigned int __attribute__((section (".onchipdata"))) targetdata[100];

volatile int doneflag = 0;
static void dmadone() {
  doneflag = 1;
}

void main() {
  unsigned i;

  printf("Clear target\n");
  memset(targetdata, 0, sizeof(unsigned int) * 100);

  printf("Setup DMA\n");
  alt_dma_txchan txchan;
  alt_dma_rxchan rxchan;
  txchan = alt_dma_txchan_open ("/dev/dma_0");
  if (txchan == NULL)
    printf("Error opening tx channel\n");
  rxchan = alt_dma_rxchan_open ("/dev/dma_0");
  if (rxchan == NULL)
    printf("Error opening rx channel\n");

  for (i=0; i<10; i++) {

    alt_dma_txchan_send( txchan, 
                         testdata + i*10, 
                         10 * sizeof(unsigned int), 
                         NULL, 
                         NULL);

    alt_dma_rxchan_prepare( rxchan, 
                            targetdata + i*10, 
                            10 * sizeof(unsigned int), 
                            dmadone, 
                            NULL);

    while (!doneflag) 
      printf("Wait for DMA\n");

    doneflag = 0;
  }

  for (i=0; i<100; i++)
    printf("Data[%d] = %d\n", i, targetdata[i]);

  while (1);

}
