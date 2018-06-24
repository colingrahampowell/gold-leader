@echo off

set name="gold-leader"
set ppulib="ppu"

set path=%path%;..\bin\

set CC65_HOME=..\

cc65 -Oi %name%.c --add-source
cc65 -Oi %ppulib%.c -verbose --add-source
ca65 reset.s -g
ca65 ppu_background.s -g
ca65 %ppulib%.s -g 
ca65 %name%.s -g

ld65 -C gold-leader.cfg -o %name%.nes reset.o ppu_background.o %name%.o %ppulib%.o nes.lib -Ln gl.labels.txt -m glmap.txt

del *.o

python fceux_symbols.py

pause

%name%.nes
