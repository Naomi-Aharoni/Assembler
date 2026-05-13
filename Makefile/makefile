assembler: assembler.o preAssembler.o firstPass.o secondPass.o table.o utils.o outputFiles.o
	gcc -g -ansi -pedantic -Wall assembler.o preAssembler.o firstPass.o secondPass.o table.o utils.o outputFiles.o -o assembler

assembler.o: assembler.c globals.h preAssembler.h firstPass.h secondPass.h outputFiles.h table.h
	gcc -c -ansi -pedantic -Wall assembler.c -o assembler.o

preAssembler.o: preAssembler.c preAssembler.h utils.h globals.h
	gcc -c -ansi -pedantic -Wall preAssembler.c -o preAssembler.o

firstPass.o: firstPass.c firstPass.h utils.h table.h globals.h
	gcc -c -ansi -pedantic -Wall firstPass.c -o firstPass.o

secondPass.o: secondPass.c secondPass.h utils.h table.h globals.h
	gcc -c -ansi -pedantic -Wall secondPass.c -o secondPass.o

table.o: table.c table.h globals.h
	gcc -c -ansi -pedantic -Wall table.c -o table.o

utils.o: utils.c utils.h globals.h
	gcc -c -ansi -pedantic -Wall utils.c -o utils.o

outputFiles.o: outputFiles.c outputFiles.h globals.h
	gcc -c -ansi -pedantic -Wall outputFiles.c -o outputFiles.o

clean:
	rm -f *.o assembler
