#include "system.h"
#include <stdio.h>
#include <string.h>
#include "sys/alt_dma.h"


void main() {
  volatile float *FPLMAC = (float *) FLPMAC_0_BASE;

  printf("write A\n");
  FPLMAC[0] = 1.0;

  printf("write B\n");
  FPLMAC[1] = 2.0;

  printf("write C\n");
  FPLMAC[2] = 0.0;

  printf("read A %f\n", FPLMAC[0]);
  printf("read B %f\n", FPLMAC[1]);
  printf("read C %f\n", FPLMAC[2]);

  printf("write B 2\n");
  FPLMAC[1] = 2.0;

  printf("read A %f\n", FPLMAC[0]);
  printf("read B %f\n", FPLMAC[1]);
  printf("read C %f\n", FPLMAC[2]);

  printf("write B 2.5\n");
  FPLMAC[1] = 2.5;

  printf("read A %f\n", FPLMAC[0]);
  printf("read B %f\n", FPLMAC[1]);
  printf("read C %f\n", FPLMAC[2]);

  printf("write A 5\n");
  FPLMAC[0] = 5.0;

  printf("write B 2\n");
  FPLMAC[1] = 2.0;

  printf("read A %f\n", FPLMAC[0]);
  printf("read B %f\n", FPLMAC[1]);
  printf("read C %f\n", FPLMAC[2]);

  printf("write C\n");
  FPLMAC[2] = 0.0;

  printf("read A %f\n", FPLMAC[0]);
  printf("read B %f\n", FPLMAC[1]);
  printf("read C %f\n", FPLMAC[2]);

  while (1);

}
