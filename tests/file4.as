; file file4.as
.entry LOOOP
.ext Y
MAIN: addd r50, LIST
LOOP: prn #48
 leap W, r6,
 tinc r6
 mov r3, #5
 mov r3
 bne r3,END
 sub r1,,, r4
 branch END
 ;
 
 cmp K, #-6
 inc %END
 dec X
 
.entry MAIN
 jump %LOOP
 add %LOOP, r3,
 add ,L3, L3
ENDOFPROG: stop END
STR: .strng "abcd"
LIST: .data 6 -9
 .data -100,,9
 .data abc
 .string abc
K: .data 31
.extern L4

