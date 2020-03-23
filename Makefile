%: %.c
	gcc -o $@ $@.c -Wall -fsanitize=undefined -fsanitize=address
