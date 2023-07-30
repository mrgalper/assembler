.extern W
MAIN: mov @r3 ,MAIN
LOOP: jmp L1
prn -5
bne W
sub @r1, @r4
bne L3
L1: inc K
stam:  .entry LOOP
jmp W
label32:  .extern   W 
END: stop
.string "abcdef"
.data 6,-9,15
K: .data 22
.extern L3