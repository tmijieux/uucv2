CC=gcc
YACC=bison
LEX=flex
MAKE_DEPENDENCIES=$(wildcard .*.mkdep)
CC1_SRC=main.c lex.yy.c \
	expression.c list.c declarator.c \
	specifier.c type.c symbol_table.c \
	hash_table.c error.c statement.c \
	function.c enumerator.c parser.c

CC1_OBJ=$(CC1_SRC:.c=.o)
CFLAGS=-Wall -Wextra -pedantic -fmax-errors=2
CC1_LDFLAGS=
CC1_LIBS=

ifeq ($(DEBUG),)
CFLAGS+=-DNDEBUG -O2
CC1_LDFLAGS+=-O2 -fstack-protector-strong
else
CFLAGS+=-O0 -ggdb
CC1_LDFLAGS+= -O0 -ggdb
endif



all: cc1 uuc

-include $(MAKE_DEPENDENCIES)

uuc: frontend.c
	$(CC) -o $@ $^ $(CC1_LDFLAGS)

main.c: scanner.h

scanner.h: scanner.l

scanner.h lex.yy.c: scanner.l
	$(LEX) --header-file=scanner.h $<

%.o: %.c
	@$(CC) -MM -c -o .$*.mkdep $(CFLAGS) $<
	$(CC) -c -o $@ $(CFLAGS) $<

cc1: $(CC1_OBJ)
	$(CC) -o $@ $(CC1_OBJ) $(CC1_LDFLAGS) $(CC1_LIBS)

clean:
	$(RM) $(CC1_OBJ) scanner.h lex.yy.c

fullclean: clean
	$(RM) *~  .*.mkdep cc1 uuc
