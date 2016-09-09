#ifndef HUFFMAN_CO_H
#define HUFFMAN_CO_H

#include "BitUtils.h"
#include "Debug.h"
#include "FileInfo.h"
#include "PriorityList.h"
#include "Trie.h"

#include <bitset>
#include <cstring>
#include <fstream>
#include <iostream>
#include <gtk/gtk.h>
using namespace std;

/**
* @file
* Libreria contenente le funzioni e le strutture dati per la compressione dei file.
*/

/**
* @def KEY_BIT_SIZE
* Numero massimo di bit per keywords
*/
#define KEY_BIT_SIZE  32

/**
* @struct CharKey HuffmanCo.h "HuffmanCo.h"
* @brief Struttura contenente la keyword e la dimensione in bit della stessa.
* I bit da considerare sono quelli a partire dal meno significativo.
*
* @var CharKey::key
* keyword di codifica di un carattere
* @var CharKey::size
* Dimensione in bit della keyword
*/
struct CharKey;

/**
* @brief Comprime una lista di file in un unico archivio
*
* @param[in] path Path dove salvare l'archivio
* @param[in] fileList Lista di ::FileInfo contenente le informazioni dei file da comprimere
* @return TRUE se la compressione avviene con successo, altrimenti FALSE
*/
gboolean compressToFile(const gchar* path, GSList* fileList);

/**
* @brief Salva dentro il nuovo archivio l'header di un file
*
* L'header del file contiene:
* - nome del file compresso
* - dimensione originale
* - dimensione compressa
* - ::Trie delle keywords usate per comprimere
*
* @param[in] file Riferimento all'output stream dell'archivio in cui scrivere
* @param[in] fileInfo ::FileInfo di cui salvare i dati
* @return TRUE se la scrittura avviene con successo, altrimenti FALSE
*/
gboolean saveFileHeader(ofstream& file, FileInfo* fileInfo);

/**
* @brief Salva in un array le frequenze di ciascun carattere
*
* L'array passato deve avere dimensione ::CHAR_SET_SIZE
*
* @param[in] path Path del file
* @param[in] charSetFreq Array in cui salvare le frequenze
* @return TRUE se la lettura avviene con successo, altrimenti FALSE
*/
gboolean getFileFrequencies(const gchar* path, int* charSetFreq);

/**
* @brief Prepara la lista con priorità dei caratteri per la generazione del Trie delle keywords
* a partire dalle frequenze calcolate
*
* @param[in] charSetFreq Array delle frequenze
* @return :Trie delle keywords
*/
Trie* generateFileKeywords(int* charSetFreq);

/**
* @brief Calcola il ::Trie delle keywords data la lista con priorità dei caratteri
*
* @param[in] pList ::PriorityList dei caratteri
* @return :Trie delle keywords
*/
Trie* getKeywordsTrie(PriorityList* pList);

/**
* @brief Funzione ricorsiva per convertire in un array il ::Trie delle keywords
*
* La conversione da ::Trie ad array permette l'accesso diretto alla keyword avendo il carattere
* L'array passato deve avere dimensione ::CHAR_SET_SIZE.
*
* @param[in] keywordsTrie ::Trie delle keywords
* @param[in] keywordsArr Array di ::CharKey nel quale salvare le keywords
* @param[in] path Variabile di supporto per la ricorione. Contiene il percorso effettuato per arrivare al nodo corrente
* @param[in] deep Variabile di supporto per la ricorione. Contiene la profondità nell'albero del nodo corrente
*/
void keywordsTrieToKeywordsArray(Trie* keywordsTrie, CharKey* keywordsArr, hword path = 0, hword deep = 0);

/**
* @brief Calcola la dimensione compressa di un file a partire dal ::Trie delle keywords e dalle frequenze
*
* @param[in] keywordsTrie ::Trie delle keywords
* @param[in] charSetFreq Array delle frequenze
* @return Dimensione compressa
*/
word getFileCompressedSize(Trie* keywordsTrie, int* charSetFreq);

/**
* @brief Salva un ::Trie all'interno di un file
*
* @param[in] outputFile Stream di output del file
* @param[in] keywords ::Trie delle keywords
*/
void saveTrieKeywordsOnFile(ofstream& outputFile, Trie* keywords);

/**
* @brief Funzione ricorsiva che comprime un ::Trie per essere salvato su un file
*
* @param[in] root ::Trie delle keywords
* @param[in] trieChild Array di byte. L'n-simo bit indica se l'n-simo nodo del ::Trie ha dei figli
* @param[in] trieElem Array in cui verranno salvati tutti i caratteri dei nodi in pre-order
* @param[in] nodeCount Puntatore ad una variabile di supporto per la ricorsione. La variabile puntata deve essere settata a 0 prima della chiamata alla funzione
* @param[in] leafCount Puntatore ad una variabile di supporto per la ricorsione. La variabile puntata deve essere settata a 0 prima della chiamata alla funzione
*/
void trieToFile(Trie* root, byte* trieChild, byte* trieElem, int* nodeCount, int* leafCount);

/**
* @brief Compressione e salvataggio di un file nell'archivio
*
* @param[in] outputFile Stream di output dell'archivio
* @param[in] fileInfo ::FileInfo del file da comprimere
* @return TRUE se la scrittura è riuscita, altrimenti FALSE
*/
gboolean compressFile(ofstream& outputFile, FileInfo* fileInfo);


#endif
