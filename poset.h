#ifndef POSET_POSET_H
#define POSET_POSET_H

typedef unsigned long size_t;

#ifdef __cplusplus
#include <iostream>
#include <cstddef>
namespace jnp1 {
    extern "C" {

#else
#include <stddef.h>
#include <stdbool.h>

#endif

/**@brief Tworzy nowy poset.
 * @return identyfikator stworzonego posetu
 */
    unsigned long poset_new();

/**@brief Usuwa poset.
 * Jeżeli istnieje poset o identyfikatorze id, usuwa go, a w przeciwnym
 * przypadku nic nie robi.
 * @param id - identyfikator posetu
 */
    void poset_delete(unsigned long id);

/**@brief Zwraca liczbę elementów posetu.
 * Jeżeli istnieje poset o identyfikatorze id, to wynikiem jest liczba jego
 * elementów, a w przeciwnym przypadku 0.
 * @param id - identyfikator posetu
 * @return liczba elementów jeśli poset istnieje, 0 wpp.
 */
    size_t poset_size(unsigned long id);

/**@brief Dodaje element do posetu.
 * Jeżeli istnieje poset o identyfikatorze id i element value nie należy do
 * tego zbioru, to dodaje element do zbioru, a w przeciwnym przypadku nic nie
 * robi. Nowy element nie jest w relacji z żadnym elementem.
 * @param id - identyfikator posetu
 * @param value - element do dodania do posetu
 * @return @p true, gdy element został dodany, @p false wpp.
 */
    bool poset_insert(unsigned long id, char const *value);

/**@brief Usuwa element z posetu.
 * Jeżeli istnieje poset o identyfikatorze id i element value należy do tego
 * zbioru, to usuwa element ze zbioru oraz usuwa wszystkie relacje tego
 * elementu, a w przeciwnym przypadku nic nie robi.
 * @param id - identyfikator posetu
 * @param value - element do usunięcia z posetu
 * @return @p true, gdy element został usunięty, @p false wpp.
 */
    bool poset_remove(unsigned long id, char const *value);

/**@brief Dodaje relację do posetu.
 * Jeżeli istnieje poset o identyfikatorze id oraz elementy value1 i value2
 * należą do tego zbioru i nie są w relacji, to rozszerza relację w taki
 * sposób, aby element value1 poprzedzał element value2 (domyka relację
 * przechodnio), a w przeciwnym przypadku nic nie robi.
 * @param id - identyfikator posetu
 * @param value1 - element poprzedzający w relacji
 * @param value2 - element następujący w relacji
 * @return @p true, gdy relacja została rozszerzona, @p false wpp.
 */
    bool poset_add(unsigned long id, char const *value1, char const *value2);

/**@brief Usuwa relację z posetu.
 * Jeżeli istnieje poset o identyfikatorze id, elementy value1 i value2
 * należą do tego zbioru, element value1 poprzedza element value2 oraz
 * usunięcie relacji między elementami value1 i value2 nie zaburzy warunków
 * bycia częściowym porządkiem, to usuwa relację między tymi elementami,
 * a w przeciwnym przypadku nic nie robi.
 * @param id - identyfikator posetu
 * @param value1 - jeden z elementów w relacji do usunięcia
 * @param value2 - drugi z elementów w relacji do usunięcia
 * @return @p true, gdy relacja została zmieniona, @p false wpp.
 */
    bool poset_del(unsigned long id, char const *value1, char const *value2);

/**@brief Sprawdza czy jeden element poprzedza drugi w relacji.
 * Sprawdza czy istnieje poset o identyfikatorze id, czy elementy value1 i value2
 * należą do tego zbioru oraz czy element value1 poprzedza element value2.
 * @param id - identyfikator posetu
 * @param value1 - pierwszy element w relacji do sprawdzenia
 * @param value2 - drugi element w relacji do sprawdzenia
 * @return @p true, jeśli value1 poprzedza value2 w relacji, @p false wpp.
 */
    bool poset_test(unsigned long id, char const *value1, char const *value2);

/**@brief Usuwa wszystkie elementy z posetu.
 * Jeżeli istnieje poset o identyfikatorze id, usuwa wszystkie jego elementy
 * oraz relacje między nimi, a w przeciwnym przypadku nic nie robi.
 * @param id - identyfikator posetu
 */
    void poset_clear(unsigned long id);

#ifdef __cplusplus
    }
}
#endif


#endif //POSET_POSET_H
