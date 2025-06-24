# Makefile per compilare il progetto con Bison, Flex, e symbol table

# Compilatore e flag
CC = gcc
CFLAGS = -Wall -g -std=c99 -D_GNU_SOURCE -Wno-unused-function
LDFLAGS = -lm

# File di input
FLEX = scanner.flex
BISON = parser.y

# File generati
FLEX_OUT = scanner.c
BISON_OUT = parser.tab.c
BISON_HEADER = parser.tab.h

# Sorgenti
SRCS = main.c $(BISON_OUT) $(FLEX_OUT) sym_tables.c
OBJS = $(SRCS:.c=.o)

# Nome eseguibile
EXEC = tour_analyzer

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Regola per Bison
$(BISON_OUT) $(BISON_HEADER): $(BISON)
	bison -d $(BISON)

# Regola per Flex
$(FLEX_OUT): $(FLEX) $(BISON_HEADER)
	flex -o $(FLEX_OUT) $(FLEX)

# Regola generica per compilare i .c in .o
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Test con file di esempio
test: $(EXEC)
	./$(EXEC) < input_corretto.txt

test_error_syntactic: $(EXEC)
	./$(EXEC) < input_errore_sintattico.txt

test_error_lexical: $(EXEC)
	./$(EXEC) < input_errore_lessicale.txt

# Pulizia dei file generati
clean:
	rm -f $(EXEC) *.o $(FLEX_OUT) $(BISON_OUT) $(BISON_HEADER)

.PHONY: all clean test test_error_syntactic test_error_lexical