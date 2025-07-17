# Progetto Compilatori - Tour Agency Analyzer

Progetto realizzato in Flex+Bison per la gestione di tour organizzati da un'agenzia di viaggi.

## Descrizione del Problema

Il programma analizza un file di testo diviso in tre sezioni separate da `%%%`:

1. **Prima sezione**: Definizione delle città con codice 3 lettere maiuscole, nome tra virgolette e coordinate
2. **Seconda sezione**: Definizione dei tour con ID (lettera maiuscola + 2 cifre), nome tour e città di partenza
3. **Terza sezione**: Percorsi dei tour specificando ID tour e prossima tappa

## Formato Input

```
PMO "Palermo" (10.0,25.0)
NPL "Napoli" (10.0,60.0)
%%%
X25
Costa Mediterranea
PMO
&&&
F56
Italia
PMO
%%%
X25 NPL
F56 RMA
```

## Formato Output

Il programma genera l'output in un file specificato (default: `output.txt`):

```
Costa Mediterranea: Palermo -> Napoli 600.00€
Italia: Palermo -> Roma 450.00€
```

## Uso del Programma

### Sintassi Base
```bash
./tour_analyzer [-o file_output]
```

### Parametri
- `-o file_output`: Specifica il file di output (default: `output.txt`)
- `-h, --help`: Mostra il messaggio di aiuto

### Esempi d'Uso
```bash
# Output nel file di default (output.txt)
./tour_analyzer < input.txt

# Output in un file specifico
./tour_analyzer -o risultato.txt < input.txt

# Mostra help
./tour_analyzer --help
```

## Struttura del Progetto

### Symbol Table
- **Hash Table Città**: Gestisce collisioni con chaining, hash ottimizzato per ridurre collisioni
- **Hash Table Tour**: Funzione di hash perfetta per ID tour (lettera + 2 cifre)
  - Formula: `(lettera - 'A') * 100 + numero`
  - Esempio: X25 → (23 * 100) + 25 = 2325

### Calcolo Costi
- **Costo base**: 250€ per ogni tappa
- **Costo distanza**: 10€ × distanza euclidea tra città consecutive
- **Formula distanza**: `√((x₁-x₂)² + (y₁-y₂)²)`

### Gestione Errori
- **Errori Lessicali**: Caratteri o sequenze non riconosciute (output su stderr)
- **Errori Sintattici**: Violazioni della grammatica (output su stderr)
- **Errori Semantici**: Tour duplicati, città inesistenti, riferimenti non validi (output su stderr)

## Compilazione ed Esecuzione

### Compilazione
```bash
make
```

### Test Predefiniti

#### Test con input corretto
```bash
make test
# Genera output in test_output.txt e mostra il contenuto
```

#### Test con errori
```bash
# Test errore sintattico
make test_error_syntactic

# Test errore lessicale  
make test_error_lexical
```

#### Test personalizzato
```bash
make test_custom INPUT=mio_input.txt OUTPUT=mio_output.txt
```

#### Test verboso (mostra anche l'output)
```bash
make test_verbose
```

### Esempi Pratici

```bash
# Compilazione
make

# Test base con visualizzazione output
make test_verbose

# Test personalizzato
make test_custom INPUT=test/input_corretto.txt OUTPUT=risultati_tour.txt

# Esecuzione diretta
./tour_analyzer -o analisi_tour.txt < test/input_corretto.txt

# Verifica dell'output generato
cat analisi_tour.txt
```

### Pulizia

```bash
# Pulizia file compilazione
make clean

# Pulizia completa (include file di output)
make cleanall
```

## Output e Gestione File

- **Output principale**: Scritto nel file specificato con `-o` (default: `output.txt`)
- **Messaggi di errore**: Sempre su terminale (stderr)
- **Messaggi di stato**: Su terminale (stderr), es. "Parsing completato con successo"

## File di Test

- `test/input_corretto.txt`: Input valido per test funzionale
- `test/input_errore_sintattico.txt`: Parentesi mancante (errore sintattico)
- `test/input_errore_lessicale.txt`: Carattere '@' non valido (errore lessicale)

## Note Tecniche

Il programma legge sempre da stdin e scrive l'output nel file specificato. Gli errori e i messaggi di stato sono sempre visualizzati su terminale per permettere un debug efficace anche quando l'output è rediretto su file.
