/**
 MIT License
 
 Copyright (c) 2018 Matias Barrientos.
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#ifndef SortedMap_h
#define SortedMap_h

typedef struct SortedMap SortedMap;

/**
 @typedef
 La función compara dos objetos del tipo que definio el usuario.
 
 @param key1 Primero parametro.
 @param key2 Segundo parametro.
 @return retorna 3 valores con respecto a la comparacion realizada.
 1: p1 es mayor que p2.
 0: p1 es igual a p2.
 -1: p1 es menor que p2.
 */
typedef int (* SortedMapCompareCallBack)(const void * key1, const void * key2);

/**
 Crea un nuevo puntero de tipo SortedMap.

 @param compare Funcion comparar.
 @return Puntero al nuevo tipo de SortedMap.
 */
SortedMap * createSortedMap(SortedMapCompareCallBack compare);

/**
 Inserta un dato con clave en el SortedMap.
 
 Complejidad: O(log N)

 @param map Puntero al SortedMap.
 @param key Puntero a la llave del dato.
 @param value Puntero al dato.
 */
void insertSortedMap(SortedMap * map, const void * key, const void * value);

/**
 Elimina un nodo del SortedMap con la llave, no libera memoria del dato guardado.

 Complejidad: O(log N)
 
 @param map Puntero al SortedMap.
 @param key Puntero a la llave del dato.
 @return Puntero al dato eliminado del SortedMap.
 */
void * eraseKeySortedMap(SortedMap * map, const void * key);

/**
 La cantidad de datos ingresados al SortedMap.
 
 Complejidad: O(1)

 @param map Puntero al SortedMap.
 @return Cantidad de datos.
 */
long sortedMapCount(SortedMap * map);

/**
 Prueba si el SortedMap está vacio.
 
 Complejidad: O(1)
 
 @param map Puntero al SortedMap.
 @return 1 (true) si y solo si el SortedMap no contiene elementos; 0 (false) lo contrario.
 */
int emptySortedMap(SortedMap * map);

/**
 Busca un dato en el SortedMap con la llave.
 
 Complejidad: O(log N)

 @param map Puntero al SortedMap.
 @param key Puntero a la llave del dato.
 @return Puntero al dato.
 */
void * searchSortedMap(SortedMap * map, const void * key);

/**
 Busca un dato con clave que sea mayor o igual a el.
 
 Complejidad: O(log N)

 @param map Puntero al SortedMap.
 @param key Puntero a la llave del dato.
 @return Puntero al dato.
 */
void * upperBoundSortedMap(SortedMap * map, const void * key);

/**
 Busca el menor elemento dentro del SortedMap.
 
 Complejidad: O(log N)

 @param map Puntero al SortedMap.
 @return Puntero al dato.
 */
void * firstSortedMap(SortedMap * map);

/**
 Busca el sucesor al menor elemento dentro del SortedMap.
 
 Complejidad: O(1)
 
 @param map Puntero al SortedMap.
 @return Puntero al dato.
 */
void * nextSortedMap(SortedMap * map);

/**
 Elimina todos los nodos del SortedMap, no libera memoria de los datos guardados en el SortedMap.
 
 Complejidad: O(N)

 @param map Puntero a la struct SortedMap.
 */
void removeAllSortedMap(SortedMap * map);

#endif /* SortedMap_h */
