#ifndef HUFFMAN_DEC_H
#define HUFFMAN_DEC_H

#include "BitUtils.h"
#include "Debug.h"
#include "FileInfo.h"
#include "HuffmanCo.h"
#include "PriorityList.h"
#include "Trie.h"

#include <fstream>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <bitset>
#include <gtk/gtk.h>
using namespace std;

/**
* @brief Salva in una lista un puntatore a ::FileInfo per ogni file trovato dentro un archivio
*
* @param[in] fileToDecompress Path del file da decomprimere
* @param[out] fileList Lista in cui salvare i puntatori a ::FileInfo
* return TRUE se la lettura va a buon fine, altrimenti FALSE
*/
gboolean getCompressedFileInfo(const gchar* fileToDecompress, GSList*& fileList);

/**
* @brief Crea un'istanza di ::FileInfo a partire dall'header di un archivio
*
* @param[in] inputFile Stream di lettura del file archivio
* return Puntatore all'istanza di ::FileInfo creata
*/
FileInfo* parseFileHeader(ifstream& inputFile);

/**
* @brief Legge una stringa da file
*
* La stringa deve essere salvata nel formato <size><stringa> con \n
* <size> byte contenente la dimensione della stringa \n
* <stringa> stringa effettiva
*
* @param[in] inputFile Stream di lettura del file archivio
* return Puntatore alla stringa letta
*/
gchar* parseString(ifstream& inputFile);

/**
* @brief Legge un intero (4 byte) da file
*
* @param[in] inputFile Stream di lettura del file archivio
* return Intero letto
*/
int parseInt(ifstream& inputFile);

/**
* @brief Legge una word (8 byte) da file
*
* @param[in] inputFile Stream di lettura del file archivio
* return word letta
*/
word parseWord(ifstream& inputFile);

/**
* @brief Legge un ::Trie contenente le keywords da file
*
* @param[in] inputFile Stream di lettura del file archivio
* return Puntatore al ::Trie letto
*/
Trie* parseKeywordsTrie(ifstream& inputFile);

/**
* @brief Funzione ricorsiva che crea un ::Trie di keywords a partire dai dati letti da file
*
* @param[in] trieChild Array di bit. L'n-simo bit indica se l'n-simo ::Trie ha figli oppure se è una foglia
* @param[in] trieElem Array contenente i valori dei singoli nodi del ::Trie
* @param[in] count Puntatore a variabile di supporto per la ricorsione. Deve essere inizializzato a 0
* return Puntatore al ::Trie letto
*/
Trie* trieFromFile(byte* trieChild, byte* trieElem, int* count);

/**
* @brief Decomprime un archivio all'interno una cartella
*
* @param[in] fileToDecompress Path dell'archivio da decomprimere
* @param[in] decompressPath Path della cartella in cui decomprimere l'archivio
* @param[in] fileList Lista dei ::FileInfo da decomprimere
* return TRUE se la decompressione di tutti i file va a buon fine, altrimenti FALSE
*/
gboolean decompressFileToDir(const gchar* fileToDecompress, const gchar* decompressPath, GSList* fileList);

/**
* @brief Decomprime un file all'interno dell'archivio
*
* @param[in] inputFile Stream di lettura dell'archivio
* @param[in] destPath Path in cui salvare il file da decomprimere
* @param[in] info ::FileInfo del file da decomprimere
* return TRUE se la decompressione va a buon fine, altrimenti FALSE
*/
gboolean decompressFile(ifstream& inputFile, const gchar* destPath, FileInfo* info);

#endif
