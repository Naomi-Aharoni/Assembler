; file file3.as
.entry LIST
.extern W
MAIN: add r3, LIST
hi mcro GEN_MC
 lea STR, r6
 inc r6
 mov r3, K
mcroend hello
LOOP: prn #48
GEN_MC
 sub r1, r4
 bne END
 mcro sub
 lea STR, r3
 inc r3
 mov r4, K
mcroend
 GEN_MC
 cmp val1, #-6
 mcro string
 lea STR, r3
 inc r3
 mov r4, K
mcroend
 bne %END
 dec K
 jmp %LOOP
 GEN_MC
 sub
 sub
END: stop
STR: .string "abcd"
LIST: .data 6, -9
 .data -100
.entry K
K: .data 31
.extern val
