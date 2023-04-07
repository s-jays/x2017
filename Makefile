CC=gcc
CFLAGS=-fsanitize=address -Wvla -Wall -Werror -s -std=gnu11 -lasan

vm_x2017: vm_x2017.c parser.c
	$(CC) $(CFLAGS) $^ -o $@

vm_x2017.c: objects.h parser.h vm.h

objdump_x2017: objdump_x2017.c parser.c
	$(CC) $(CFLAGS) $^ -o $@

objdump_x2017.c parser.c: parser.h objdump.h objects.h

tests:
	echo "tests"

run_tests:
	bash test.sh

clean:
	rm objdump_x2017 && rm vm_x2017

