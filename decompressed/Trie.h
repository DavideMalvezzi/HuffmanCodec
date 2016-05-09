#ifndef TRIE_H
#define TRIE_H

#include <cassert>
#include <fstream>
#include <iostream>
#include <gtk/gtk.h>
using namespace std;

/**
* @file
* Libreria contenente le funzioni e le strutture dati per la gestione di trie
*/

/**
* @struct Trie Trie.h "Trie.h"
* @brief Struttura utilizzata per memorizzare un singolo nodo di un trie
*
* @var Trie::left
* Puntatore al nodo figlio di sinistra
* @var Trie::right
* Puntatore al nodo figlio di destra
* @var Trie::data
* Puntatore ai dati contenuti nel nodo
*/
struct Trie;

/**
* @enum TrieDirection
* @brief Enumerato contenente le possibili direzioni in cui scorrere un ::Trie
*/
enum TrieDirection{
  LEFT,
  RIGHT
};

/**
* @brief Ritorna una nuova istanza di ::Trie
*
* @param[in] left Puntatore al figlio sinistro o NULL in caso sia una foglia
* @param[in] right Puntatore al figlio destro o NULL in caso sia una foglia
* @param[in] data Puntatore ai dati del nodo da creare
* @return Puntatore alla nuova istanza del ::Trie
*/
Trie* createNewTrieRoot(Trie* left, Trie* right, void* data);

/**
* @brief Funzione ricorsiva per l'eliminazione dell'albero radicato in un ::Trie
*
* @param[in] trie Puntatore al nodo radice
* @param[in] deleteFun Puntatore ad una funzione utilizzata per deallocare la memoria utilizzata per contenere i dati di ogni nodo
*/
void deleteTrie(Trie* trie, GDestroyNotify deleteFun);

/**
* @brief Ritorna il nodo figlio di un ::Trie data la ::TrieDirection in cui scorrere
*
* @param[in] root Nodo radice dell'albero
* @param[in] direction ::TrieDirection in cui scorrere l'albero
* @return Puntatore al nodo figlio
*/
Trie* navigateTrie(Trie* root, TrieDirection direction);

/**
* @brief Controlla se un ::Trie è una foglia
*
* Un nodo ::Trie è una foglia se entrambi i puntatori ai suoi figlio sono NULL
*
* @param[in] root ::Trie da controllare
* @return TRUE se il nodo è una foglia, altrimenti FALSE
*/
gboolean isTrieALeaf(Trie* root);

/**
* @brief Setta il puntatore del figlio sinistro ad un nodo ::Trie
*
* @param[in] root Nodo a cui settare il figlio sinistro
* @param[in] child Puntatore al figlio sinistro
*/
void setTrieLeftChild(Trie* root, Trie* child);

/**
* @brief Setta il puntatore del figlio destro ad un nodo ::Trie
*
* @param[in] root Nodo a cui settare il figlio destro
* @param[in] child Puntatore al figlio destro
*/
void setTrieRightChild(Trie* root, Trie* child);

/**
* @brief Funzione ricorsiva per il conteggio dei nodi radicati in un ::Trie
*
* Il conteggio tiene conto anche del nodo radice
*
* @param[in] root Nodo da cui partire per il conteggio
* @param[out] count Puntatore ad una variabile in cui salvare il conteggio
*/
void getTrieElementCount(Trie* root, gint* count);

/**
* @brief Ritorna il puntatore ai dati in un nodo ::Trie
*
* @param[in] trie Puntatore a ::Trie
* @return Puntatore ai dati del nodo
*/
void* getTrieData(Trie* trie);

#endif
