# Gold Leader 

Let's make a NES game.

Gold Leader (working title) will be a space shooter in the tradition of "Gradius", and will be developed for the Nintendo Entertainment System. 

To ease myself into the world of low-level video game programming, I'm writing game code in C and compiling down to 6502 assembly language with the CC65 C compiler, which you can find here: [https://github.com/cc65/cc65](https://github.com/cc65/cc65). CC65 also includes a handy macro assembler (CA65) and linker (LD65). I'm using v. 2.17.

I have begun by following Tim Cheeseman's [excellent CC65-based tutorials](https://timcheeseman.com/nesdev/). Some boilerplate code (specifically the reset.s initialization code) has been adapted from Doug Fraker's CC65 tutorial at [nesdoug.com](nesdoug.com).

To compile: use the included compile.bat (Windows) or makefile (MacOS / Linux).
To run: I'd suggest [FCEUX](fceux.com).

More to come soon.