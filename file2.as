; file file2.as
.entry LIST
.extern W
MAIN: add r3, LIST
mcro GEN_MC
 lea STR, r6
 inc r6
 mov r3, K
mcroend
LOOP: prn #48
GEN_MC
 sub r1, r4
 bne END
 mcro GEN_MC2
 lea STR, r3
 inc r3
 mov r4, K
mcroend
 GEN_MC
 cmp val1, #-6
 bne %END
 dec K
 jmp %LOOP
 GEN_MC
 GEN_MC2
 GEN_MC2
END: stop
STR: .string "abcd"
LIST: .data 6, -9
 .data -100
.entry K
K: .data 31
.extern val
