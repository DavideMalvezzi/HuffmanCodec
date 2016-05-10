#ifndef FILE_INFO_H
#define FILE_INFO_H

#include "BitUtils.h"
#include "Trie.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <gtk/gtk.h>
using namespace std;

/**
* @file
* Libreria per la gestione delle informazioni relative ai file compressi e da comprimere.
*/

/**
* @brief Numero massimo di caratteri ASCII
*/
#define CHAR_SET_SIZE   256

/**
* @struct FileInfo FileInfo.h "FileInfo.h"
* @brief Struttura contenente le informazioni di un singolo file
*
* @var FileInfo::name
* Stringa contenente il nome del file
* @var FileInfo::path
* Stringa contenente il path completo del file
* @var FileInfo::size
* Intero contenente la lunghezza in byte del file originale
* @var FileInfo::comprSize
* Intero contenente la lunghezza in byte del file compresso
* @var FileInfo::charSetFreq
* Array di interi, di dimensione ::CHAR_SET_SIZE, contenente le frequenze di ogni carattere del file
* @var FileInfo::keywords
* Puntatore al ::Trie contenente la codifica di ogni carattere compresso
*/
struct FileInfo;

/**
* @struct TrieItem FileInfo.h "FileInfo.h"
* @brief Struttura usata per salvare dentro un ::Trie i dati relativi ai caratteri e alle rispettive frequenze
*
* @var TrieItem::c
* Carattere
* @var TrieItem::frequency
* Frequenza del carattere
*/
struct TrieItem{
  byte c;
  int frequency;
};

/**
* @brief Crea una nuova istanza di ::FileInfo a partire dal path di un file
*
* ::FileInfo::name viene inizializzata a con il nome del file estratto dal path in input\n
* ::FileInfo::size viene inizializzata con la dimensione corrente del file\n
* ::FileInfo::comprSize viene inizializzata a 0\n
* ::FileInfo::keywords viene inizializzata a NULL\n
* @param[in] path Path del file da aggiungere
* @return Puntatore alla nuova istanza o NULL se l'apertura del file fallisce
*/
FileInfo* createNewFileInfo(const gchar* path);

/**
* @brief Crea una nuova istanza di ::FileInfo a partire dalle informazioni passate
*
* ::FileInfo::path viene inizializzata a NULL\n
* @param[in] name Nome del file da aggiungere
* @param[in] size Dimensione originale del file
* @param[in] compSize Dimensione compressa del file
* @param[in] keywords Puntatore all'istanza del ::Trie contenente la codifica di ogni carattere compresso
* @return Puntatore alla nuova istanza
*/
FileInfo* createNewFileInfo(const gchar* name, word size, word compSize, Trie* keywords = NULL);

/**
* @brief Cancellazione di un'istanza di ::FileInfo
*
* @param[in] ptr Puntatore all'istanza da eliminare
*/
void deleteFileInfo(gpointer ptr);

/**
* @brief Cancellazione di un'istanza di ::TrieItem
*
* @param[in] ptr Puntatore all'istanza da eliminare
*/
void deleteTrieItem(gpointer ptr);

/**
* @brief Ritorna il nome di un'istanza di ::FileInfo
*
* @param[in] info Puntatore all'istanza di ::FileInfo
* @return Puntatore al nome del file
*/
const gchar* getFileName(FileInfo* info);

/**
* @brief Ritorna il path di un'istanza di ::FileInfo
*
* @param[in] info Puntatore all'istanza di ::FileInfo
* @return Puntatore al path del file
*/
const gchar* getFilePath(FileInfo* info);

/**
* @brief Ritorna la tipologia di file di un'istanza di ::FileInfo
*
* @param[in] info Puntatore all'istanza di ::FileInfo
* @return Stringa contenente la tipologia del file
*/
const gchar* getFileFormat(FileInfo* info);

/**
* @brief Ritorna la dimensione del file di un'istanza di ::FileInfo
*
* @param[in] info Puntatore all'istanza di ::FileInfo
* @return Dimensione del file
*/
word getFileSize(FileInfo* info);

/**
* @brief Ritorna la dimensione del file compresso di un'istanza di ::FileInfo
*
* @param[in] info Puntatore all'istanza di ::FileInfo
* @return Dimensione del file compresso
*/
word getFileCompressedSize(FileInfo* info);

/**
* @brief Setta la dimensione del file compresso di un'istanza di ::FileInfo
*
* @param[in] info Puntatore all'istanza di ::FileInfo
* @param[in] size Dimensione del file compresso
*/
void setFileCompressedSize(FileInfo* info, word size);

/**
* @brief Ritorna un puntatore al ::Trie contenente la codifica di ogni carattere compresso di un'istanza di ::FileInfo
*
* @param[in] info Puntatore all'istanza di ::FileInfo
* @return Puntatore a ::Trie
*/
Trie* getFileKeywords(FileInfo* info);

/**
* @brief Setta il puntatore al ::Trie contenente la codifica di ogni carattere compresso di un'istanza di ::FileInfo
*
* @param[in] info Puntatore all'istanza di ::FileInfo
* @param[in] keywords Puntatore a ::Trie contenente la codifica di ogni carattere compresso
*/
void setFileKeywords(FileInfo* info, Trie* keywords);

/**
* @brief Ritorna un array contenente la frequenza di ogni carattere nel file di un'istanza di ::FileInfo
*
* @param[in] info Puntatore all'istanza di ::FileInfo
* @return Array di interi
*/
gint* getFileCharsFrequencies(FileInfo* info);

/**
* @brief Setta l'array contenente la frequenza di ogni carattere nel file di un'istanza di ::FileInfo
*
* @param[in] info Puntatore all'istanza di ::FileInfo
* @param[in] charSetFreq Array di interi
*/
void setFileCharsFrequencies(FileInfo* info, int* charSetFreq);

/**
* @brief Confronta il nome di un'istanza di ::FileInfo con una stringa data.
*
* @param[in] a Puntatore all'istanza di ::FileInfo
* @param[in] b Stringa da confrontare
* @return 0 se il le due string sono uguali, -1 se la prima è minore altrimenti 1
*/
gint findFileByName(gconstpointer a, gconstpointer b);


#endif
