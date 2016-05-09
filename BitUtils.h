#ifndef BIT_UTILS_H
#define BIT_UTILS_H

/**
* @file
* Libreria per la lettura e scrittura dei bit della variabili di tipo byte
*/

/**
* @def MEM_BUFFER(variable)
* Macro per convertire una variabile in un array di byte
*/
#define MEM_BUFFER(variable) (reinterpret_cast<char*>(&variable))

/**
* @brief Definizione del tipo byte
*/
typedef unsigned char byte;

/**
* @brief Definizione del tipo hword
*/
typedef unsigned int hword;

/**
* @brief Definizione del tipo word
*/
typedef unsigned long long word;

/**
* @brief Setta a 1 l'n-simo bit di una variabile di tipo word

* @param[in] b Riferimento alla word da modificare
* @param[in] bit Numero del bit da settare
* @return Nuova word
*/
word setBit(word b, int bit);

/**
* @brief Setta a 0 l'n-simo bit di una variabile di tipo word

* @param[in] b Riferimento alla word da modificare
* @param[in] bit Numero del bit da settare
* @return Nuova word
*/
word clearBit(word b, int bit);

/**
* @brief Ritorna il valore dell'n-simo bit di una variabile di tipo word

* @param[in] b Word da cui leggere
* @param[in] bit Numero del bit da leggere
* @return 1 se il bit è settato altrimenti 0
*/
int readBit(word b, int bit);

#endif
