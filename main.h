#include "HuffmanCo.h"
#include "HuffmanDec.h"

#include <cstdlib>
#include <iostream>
#include <gtk/gtk.h>
using namespace std;

/**
* @mainpage Huffman CoDec
* @section Introduzione Introduzione
* Lo scopo del progetto è quello di realizzare un compressore/decompressore di file testuali attraverso l'utilizzo
* della codifica di Huffman.
* Con questo programma sarà possibile:
*		- @b comprimere uno o più file testuali all'interno di un unico file-archivio compresso
* 		in formato *.hca (Huffman Compressed Archive), e allo stesso modo decomprimere;
*		- @b decomprimere un file *.hca all'interno di una cartella per riotterenere i file compressi.
*
* Entrambe le funzionalità sono accessibili sia via interfaccia grafica, sia via linea di comando.
*
* @section Funzionalità Funzionalità
* @subsection Compressione Compressione
*	Una volta aperto il programma, per comprimere uno o più file in un archivio sarà sufficiente premere il pulsante
* @a Add nella toolbar e selezionare i file che si è scelto di comprimere. Dato l'ok, la lista dei file si aggiornerà e
* saranno mostrate le informazioni dei file selezionati. E' possibile aggiungere altri file, sempre utilizzando
* il pulsante @a Add, oppure rimuovere uno o più file, selezionando dalla lista la riga contenente il file da eliminare e
* premendo il pulsante @a Remove. A questo punto è possibile comprimere i file premendo il pulsante @a Compress e selezionado il nome
* dell'archivio da creare.<br>
* In caso si riscontrino dei problemi durante la compressione verrà mostrato una dialog di errore.
*
* Per comprimere via linea di comando:
* @code{.sh}
  	./HuffmanCodec -c File1.txt ... FileN.txt -o ArchiveFile.hca
*	@endcode
*
* @subsection Decompressione Decompressione
* Una volta aperto il programma, per decomprimere un file archivio sarà sufficiente premere il pulsante
* @a Open nella toolbar. Se l'archivio è valido, nella lista dei file verranno caricate le informazioni dei file
* contenuti al suo interno. A questo punto si può procedere alla decompressione cliccando sul pulsante @a Decompress
* e selezionare la cartella in cui salvare i file.<br>
* In caso si riscontrino dei problemi durante la decompressione verrà mostrato una dialog di errore.
*
* Per decomprimere via linea di comando:
* @code{.sh}
  	./HuffmanCodec -d ArchiveFile.hca -o DecompressFolderPath/
*	@endcode
*
* @section Moduli Moduli
* Il progetto è stato suddiviso nei seguenti moduli:
* 	- main.h: modulo contenente tutte le funzionalità di interfacciamento con l'utente, sia a livello di GUI, sia a linea di comando.
* 	- HuffmanCo.h: modulo contenente tutte le funzionalità di compressione.
* 	- HuffmanDec.h: modulo contenente tutte le funzionalità di decompressione.
* 	- PriorityList.h: modulo contenente l'implementazione di una lista con priorità attraverso un heap binario.
* 	- Trie.h: modulo contenente l'implementazione di un trie.
* 	- FileInfo.h: modulo contenente le strutture dati per la gestione delle informazioni dei file.
* 	- BitUtils.h: modulo contenente tutte le funzionalità di gestione dei bit.
* 	- Debug.h: modulo contenente tutte le funzionalità di debug.
*
* @section Struttura Struttura file archivio
* Ogni file archivio è diviso in due sezioni:
*		- @b Header: contiene le informazioni relative ad ogni file compresso all'interno dell'archivio;
*		- @b Files: contiene, accodati uno dopo l'altro, tutti i file compressi.
*
* I primi 4 byte dell'header rappresentano il numero di file presenti all'interno dell'archivio.
*
* L'header è poi composto da tanti sotto-header, uno per ogni file, contenenti le seguenti informazioni:
*		- Lunghezza del nome del file (1 byte);
* 	- Nome del file (1-256 byte);
* 	- Dimensione file originale in byte (8 byte);
*		- Dimensione file compresso in byte (8 byte);
*		- Numero nodi del Trie delle keywords (4 byte);
*		- Array di bit, vedi ::trieFromFile (Dimensione variabile);
*		- Contenuto nodi del Trie delle keywords (Dimensione variabile).
*
* @author Davide Malvezzi
*/

/**
* @file
* Libreria contenente le funzioni per la gestione dell'interfaccia grafica e a linea di comando.
*/

/**
* @brief Macro per la creazione degli handler dei segnali generati dai widget GTK
*
* @param[in] constructor Tipo di ritorno e nome della funzione che dovrà fare da handler
*/
#define GTK_HANDLER(constructor) extern "C" constructor(GtkWidget* widget, GdkEvent* event, gpointer user_data)

/**
* @enum FileListStoreCols
* Enumerato contenente i nomi delle colonne del modello del GtkTree che contiene i file selezionati per la
* compressione/decomenpressione
*/
enum FileListStoreCols{
	COL_NAME,
	COL_TYPE,
	COL_SIZE,
	COL_CMP_SIZE,
	COL_CMP_RATEO
};

/**
* @enum Mode
* @brief Enumerato contenente gli stati in cui può lavorare il programma
*/
enum Mode{
	COMPRESS, DECOMPRESS
};

/**
* @var Mode Mode::COMPRESS
* In questa modalità sarà possibile aggiungere e rimuovere file alla lista di compressione, per poi procedere con la
* compressione stessa. In caso di apertura di un file archivio, il programma passerà in modalità ::DECOMPRESS e
* si perderà la lista di file selezionati precedentemente
* @var Mode Mode::DECOMPRESS
* In questa modalità sarà possibile decomprimere uno o più file presenti all'interno di un archivio. In caso sia aggiunto
* un file con, l'archivio verrà chiuso e il programma passerà in modalità ::COMPRESS
*/

/**
* @var fileToDecompress
* Path dell'archivio da decomprimere
*/
static gchar* fileToDecompress;

/**
* @var currentMode
* Modalità di lavoro del programma. Di default è ::COMPRESS.
*/
static Mode	currentMode;

/**
* @var fileList
* Lista contenente un puntatore a ::FileInfo per ogni file da comprimere/decomprimere
*/
static GSList* fileList;

/**
* @var builder
* Puntatore a GtkBuilder per accedere ai widget dell'interfaccia grafica
*/
static GtkBuilder* builder;


/**
* @var fileListModel
* Puntatore alla finestra principale del programma
*/
static GtkWidget* mainWindow;

/**
* @var fileListModel
* Puntatore a GtkListStore contenente il modello di fileListView
*/
static GtkListStore* fileListModel;

/**
* @brief Inizializza l'interfaccia grafica
*
* @param[in] argc Numero argomenti
* @param[in] argv Vettore degli argomenti
*/
void initGUI(int argc, char* argv[]);

/**
* @brief Esegue le operazioni richieste da linea di comando
*
* @param[in] argc Numero argomenti
* @param[in] argv Vettore degli argomenti
*/
void parseCommandLine(int argc, char* argv[]);

/**
* @brief Aggiunge a ::fileList i file passati da linea di comando
*
* @param[in] argc Numero argomenti
* @param[in] argv Vettore degli argomenti
* @return TRUE se tutti i file esistono, altrimenti FALSE
*/
gboolean parseArgFileList(int argc, char* argv[]);

/**
* @brief Aggiunge un'istanza di ::FileInfo a ::fileList
*
* @param[in] path Path del file da aggiungere
*/
void addToFileList(const gchar* path);

/**
* @brief Aggiunge un'istanza di ::FileInfo a ::fileListModel
*
* @param[in] info Puntatore all'istanza di ::FileInfo da aggiungere
*/
void addToFileView(FileInfo* info);

/**
* @brief Rimuove tutte le righe da ::fileListModel
*/
void clearFileView();

/**
* @brief Libera la memoria occupata da ::fileList
*/
void clearFileList();

/**
* @brief Rimuove tutte le righe da ::fileListModel e reinserisce tutti i dati presenti in ::fileList
*/
void reloadFileView();

/**
*	@brief Ritorna l'indice della riga correntemente selezionata nella fileListView
*
* @return Indice della riga selezionata o -1 se nessuna riga è selezionata
*/
int getFileViewSelectedRow();

/**
* @brief Crea un'istanza di GtkFileChooser con i parametri passati
*
* @param parent Finestra padre della dialog
* @param action Tipologia del GtkFileChooser
* @param title Titolo del GtkFileChooser
* @return Puntatore all'istanza
*/
GtkWidget* createFileChooser(GtkWindow* parent, GtkFileChooserAction action, const gchar* title);

/**
* @brief Crea e mostra una GtkDialog con i parametri passati
*
* @param parent Finestra padre della dialog
* @param type Tipologia
* @param msg Messaggio
*/
void showMessageDialog(GtkWindow* parent, GtkMessageType type, const gchar* msg);

/**
* @brief Handler pressione del tasto open
*/
GTK_HANDLER(void onOpenClick);

/**
* @brief Handler pressione del tasto decompress
*/
GTK_HANDLER(void onDecompressClick);

/**
* @brief Handler pressione del tasto add
*/
GTK_HANDLER(void onAddClick);

/**
* @brief Handler pressione del tasto remove
*/
GTK_HANDLER(void onRemoveClick);

/**
* @brief Handler pressione del tasto compress
*/
GTK_HANDLER(void onCompressClick);

/**
* @brief Handler pressione tasto info
*/
GTK_HANDLER(void onInfoClicked);

/**
* @brief Handler della chiusura del programma
*/
GTK_HANDLER(gboolean onWindowDelete);

/**
* @brief Handler chiusura aboutDialog, per prevenire distruzione della finestra
*/
GTK_HANDLER(gboolean onInfoClosed);
