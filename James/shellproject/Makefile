FLAGS = -Wall

all: shell

clean:
	rm -f shell my_cat my_ls

shell: shell.c my_cat.c my_ls.c
	gcc -o shell shell.c $(FLAGS)

