#include "HuffmanCo.h"
#include "HuffmanDec.h"

#include <cstdlib>
#include <iostream>
#include <gtk/gtk.h>
using namespace std;

/**
* @file
* Libreria contenente le funzioni per la gestione dell'interfaccia grafica
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
* @brief Handler della chiusara del programma
*/
GTK_HANDLER(gboolean onWindowDelete);
