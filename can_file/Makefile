CROSS_COMPILE ?= aarch64-linux-gnu-
CC := ${CROSS_COMPILE}gcc
LD := ${CROSS_COMPILE}ld
object = transmit.o transelect.o transmit2_one_fd.o transmit2_two_fd.o test4head.o
headtest : ${object}
	${CC} ${object} -o headtest
test4head.o : test4head.c
transmit.o : transmit.c 
transmit2_one_fd.o : transmit2_one_fd.c 
transmit2_two_fd.o : transmit2_two_fd.c 
transelect.o : transelect.c
clean :
	rm headtest ${object}