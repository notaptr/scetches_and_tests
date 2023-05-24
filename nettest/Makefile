#

PROGRAM = nettest
SOURCE	= $(PROGRAM).c

all:
	gcc $(SOURCE) -o $(PROGRAM)
	strip -s $(PROGRAM)

debug:
	gcc $(SOURCE) -ggdb -o $(PROGRAM)
	objdump -S -d -M intel $(PROGRAM) > $(PROGRAM).s
