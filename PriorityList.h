#ifndef PRIORITY_LIST_H
#define PRIORITY_LIST_H

#include <cmath>
#include <iostream>
#include <gtk/gtk.h>
using namespace std;

/**
* @file
* Libreria contenente le funzioni e le strutture dati per la gestione di liste con priorità
*/

/**
* @struct PriorityListItem PriorityList.h "PriorityList.h"
* @brief Struttura utilizzata per memorizzare un singolo nodo dell'heap binario
*
* @var PriorityListItem::priority
* Priorità del nodo
* @var PriorityListItem::data
* Puntatore all'informazione contenuta nel nodo
*/
struct PriorityListItem;

/**
* @struct PriorityList PriorityList.h "PriorityList.h"
* @brief Struttura utilizzata per implementare lo heap binario
*
* @var PriorityList::items
* Array di ::PriorityListItem usato per implementare lo heap binario
* @var PriorityList::last
* Indice del primo posto libero dove inserire un nuovo elemento
* @var PriorityList::size
* Dimensione massima di ::PriorityList::items
*/
struct PriorityList;

/**
* @brief Crea una nuova istanza di ::PriorityList

* @param[in] size Dimensione massima della ::PriorityList
* @return Puntatore all'istanza della lista creata
*/
PriorityList* createNewPriorityList(const int size);

/**
* @brief Elimina una istanza di ::PriorityList e libera la memoria in uso

* @param[in] list Lista da eliminare
* @param[in] deleteFun Funzione per liberare la memoria usata per ::PriorityListItem::data
*/
void deletePriorityList(PriorityList* list, GDestroyNotify deleteFun);

/**
* @brief Inserisce un nuovo elemento nella ::PriorityList

* @param[in] list Lista in cui inserire un nuovo elemento
* @param[in] data Puntatore dei dati da salvare nel nuovo elemento
* @param[in] priority Priorità del nuovo elemento
*/
void insertListItem(PriorityList* list, void* data, const int priority);

/**
* @brief Rimuove l'elemento di priorità minima e ritorna il puntatore all'informazione
*
* @param[in] list Lista in cui inserire un nuovo elemento
* @return Puntatore all'informazione
*/
void* getListMinData(PriorityList* list);

/**
* @brief Ritorna il numero di elementi presenti nella lista
*
* @param[in] list Lista di cui contare gli elementi
* @return Numero di elementi
*/
int getListSize(PriorityList* list);

/**
* @brief Controlla e modifica la lista per mantenere le proprietà di un min-heap
*
* @param[in] list Lista da controllare
*/
void heapify(const PriorityList* list);

/**
* @brief Ritorna l'indice del nodo padre
*
* @param[in] list Lista da utilizzare
* @param[in] index Indice del nodo
* @return Indice del nodo padre
*/
int getParent(const PriorityList* list, int index);

/**
* @brief Ritorna l'indice del nodo figlio sinistro
*
* @param[in] list Lista da utilizzare
* @param[in] index Indice del nodo padre
* @return Indice del nodo figlio sinistro o -1 se il nodo non ha un figlio sinistro
*/
int getLeftChild(const PriorityList* list, int index);

/**
* @brief Ritorna l'indice del nodo figlio destro
*
* @param[in] list Lista da utilizzare
* @param[in] index Indice del nodo padre
* @return Indice del nodo figlio destro o -1 se il nodo non ha un figlio destro
*/
int getRightChild(const PriorityList* list, int index);

/**
* @brief Ritorna l'indice del nodo figlio con priorità più bassa
*
* @param[in] list Lista da utilizzare
* @param[in] index Indice del nodo padre
* @return Indice del nodo figlio con priorità più bassa o -1 se il nodo non ha figli
*/
int getMinChild(const PriorityList* list, int index);

#endif
