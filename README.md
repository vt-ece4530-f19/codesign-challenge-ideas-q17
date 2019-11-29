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
