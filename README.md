# SAP-1 Emulator
This project is an emulation of the 8-bit simple-as-possible-1 computer detailed in the book *Digital Computer Electronics* by Albert P. Malvino. I was inspired by the video series from Ben Eater (https://youtube.com/playlist?list=PLowKtXNTBypGqImE405J2565dvjafglHU) in which he physically implements the SAP-1.

The emulation is focused on managing control flow between the components of the CPU under a rudimentary set of assembly instructions and with limited memory. The file example.txt is an example program for SAP-1 that computes the value of 16 + 20 + 24 - 32. Once compiled with `make`, the program can be run with the call `./SAP1 example.txt`.
The output of SAP-1 is purposefully verbose to improve my understanding of the change of state in each register, the bus, and the control bus over the life cycle of a program. 
