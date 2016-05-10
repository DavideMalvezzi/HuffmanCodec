#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <iomanip>
using namespace std;

/**
* @file
* Libreria per la gestione dei messaggi di debug a console.
* Le funzionalità di debug possono essere disabilitate definendo NDEBUG.
*/

#ifndef NDEBUG

/**
* @def DPRINT(msg)
* Stampa di un messaggio a console
*/
#define DPRINT(msg) cout << msg

/**
* @def DPRINTLN(msg)
* Stampa di un messaggio a console e passa alla riga successiva
*/
#define DPRINTLN(msg) cout << msg << endl

/**
* @def DFILL(n, c)
* Riempe @a n spazi con il carattere @a c
*/
#define DFILL(n, c) cout << setfill(c) << setw(n)

/**
* @def DLINE(fill)
* Riempe una linea con il carattere @a fill
*/
#define DLINE(fill) cout << setfill(fill) << setw(80) << "" << endl

#else

#define DPRINT(msg)
#define DPRINTLN(msg)
#define DFILL(n, c)
#define DLINE(fill)

#endif

#endif
