CROSS_COMPILE ?= aarch64-linux-gnu-
CC := ${CROSS_COMPILE}gcc
LD := ${CROSS_COMPILE}ld
object = transelect.o test4head.o
headtest : ${object}
	${CC} ${object} -o headtest
test4head.o : test4head.c
transelect.o : transelect.c
clean :
	rm headtest ${object}
