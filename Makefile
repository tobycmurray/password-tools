find_password: find_password.c
	$(CC) $< -Werror -Wall -W -Wpedantic -fsanitize=address -fno-omit-frame-pointer $(CFLAGS) -o $@

clean:
	rm -f find_password.o find_password
