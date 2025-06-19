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

```
Costa Mediterranea: Palermo -> Napoli 600.00€
Italia: Palermo -> Roma 450.00€
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
- **Errori Lessicali**: Caratteri o sequenze non riconosciute
- **Errori Sintattici**: Violazioni della grammatica
- **Errori Semantici**: Tour duplicati, città inesistenti, riferimenti non validi

## Compilazione ed Esecuzione

```bash
# Compilazione
make

# Esecuzione con input corretto
make test

# Test errori
make test_error_semantic
make test_error_syntactic  
make test_error_lexical

# Pulizia
make clean
```

## Miglioramenti Implementati

1. **Regex migliorate** per float e interi (eliminando problemi di riconoscimento)
2. **Gestione memoria** ottimizzata con free() appropriati
3. **Funzione di hash perfetta** per i tour (zero collisioni)
4. **Validazione robusta** degli input semantici
5. **Gestione errori completa** con messaggi informativi

## Argomenti Teorici Correlati

- Analisi lessicale e regex
- Grammatiche context-free e derivazioni
- Parser LR/LALR e shift-reduce
- Symbol table e funzioni di hash
- Gestione errori nei compilatori
- Allocazione dinamica della memoria

## File di Test

- `input_corretto.txt`: Input valido per test funzionale
- `input_errore_semantico.txt`: Tour duplicato (errore semantico)
- `input_errore_sintattico.txt`: Parentesi mancante (errore sintattico)
- `input_errore_lessicale.txt`: Carattere '@' non valido (errore lessicale)