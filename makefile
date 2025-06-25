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

# Test con file di esempio - output in test_output.txt
test: $(EXEC)
	@echo "Eseguendo test con input corretto..."
	./$(EXEC) -o test_output.txt < test/input_corretto.txt
	@echo "Output generato in: test_output.txt"
	@echo "Contenuto del file:"
	@cat test_output.txt

# Test errore sintattico
test_error_syntactic: $(EXEC)
	@echo "Eseguendo test con errore sintattico..."
	./$(EXEC) -o error_syntactic_output.txt < test/input_errore_sintattico.txt || true

# Test errore lessicale
test_error_lexical: $(EXEC)
	@echo "Eseguendo test con errore lessicale..."
	./$(EXEC) -o error_lexical_output.txt < test/input_errore_lessicale.txt || true

# Test personalizzato con parametri specifici
# Uso: make test_custom INPUT=file_input OUTPUT=file_output
test_custom: $(EXEC)
	@if [ -z "$(INPUT)" ] || [ -z "$(OUTPUT)" ]; then \
		echo "Uso: make test_custom INPUT=file_input OUTPUT=file_output"; \
		echo "Esempio: make test_custom INPUT=test/input_corretto.txt OUTPUT=risultato.txt"; \
		exit 1; \
	fi
	@echo "Eseguendo test personalizzato..."
	@echo "Input: $(INPUT)"
	@echo "Output: $(OUTPUT)"
	./$(EXEC) -o $(OUTPUT) < $(INPUT)
	@echo "Test completato. Controlla il file $(OUTPUT)"

# Test rapido che mostra anche l'output
test_verbose: $(EXEC)
	@echo "=== TEST CON INPUT CORRETTO ==="
	./$(EXEC) -o test_verbose_output.txt < test/input_corretto.txt
	@echo ""
	@echo "=== CONTENUTO OUTPUT ==="
	@cat test_verbose_output.txt
	@echo ""
	@echo "=== FINE TEST ==="

# Mostra l'help per l'uso del programma
help: $(EXEC)
	./$(EXEC) --help

# Pulizia dei file generati
clean:
	rm -f $(EXEC) *.o $(FLEX_OUT) $(BISON_OUT) $(BISON_HEADER)
	rm -f test_output.txt error_syntactic_output.txt error_lexical_output.txt test_verbose_output.txt
	rm -f output.txt  # file di output di default

# Pulizia completa (include anche eventuali file di output personalizzati)
cleanall: clean
	rm -f *.txt

.PHONY: all clean cleanall test test_error_syntactic test_error_lexical test_custom test_verbose help