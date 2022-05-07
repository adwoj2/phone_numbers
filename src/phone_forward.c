/** @file
 * Implementacja interfejsu klasy przechowującej przekierowania numerów 
 * telefonicznych
 *
 * @author Adam Wojciechowski <a.wojciecho2@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_NUMBER 57       ///< Kod ASCII cyfry 9.
#define MIN_NUMBER 48       ///< Kod ASCII cyfry 0.
#define HOW_MANY_NUMBERS 10 ///< Ilość cyfr.
#define CONVERSION 48       ///< Wartość konwertująca kody ASCII cyfr na cyfry.

/**
 * To jest struktura przechowująca przekierowania numerów telefonów.
 */
struct PhoneForward {
    struct PhoneFwd* tree;          ///< Wskaźnik na korzeń drzewa przekierowań.
    struct PhoneBwd* backward_tree; ///< Wskaźnik na korzeń drzewa odwróconych przekierowań.
};

/**
 * Typedef struktury PhoneForward ułatwiający implementacje.
 */
typedef struct PhoneForward PhoneForward;

/**
 * To jest struktura przechowująca węzeł drzewa przekierowań.
 */
struct PhoneFwd {
    struct PhoneFwd** children;     ///< Wskaźnik na tablicę dzieci danego węzła.
    struct PhoneFwd* parent;        ///< Wskaźnik na rodzica danego węzła.
    char* forwarded_prefix;         ///< Nowy prefiks.
};

/**
 * Typedef struktury PhoneFwd ułatwiający implementacje.
 */
typedef struct PhoneFwd PhoneFwd;

/**
 * To jest struktura przechowująca węzeł drzewa odwróconych przekierowań.
 */
struct PhoneBwd {
    struct PhoneBwd** children;     ///< Tablica dzieci danego węzła. 
    struct PhoneNumbers * forwarded_prefix; 
    ///< Ciąg napisów które przekierowują na dany prefiks.
};

/**
 * Typedef struktury PhoneBwd ułatwiający implementacje.
 */
typedef struct PhoneBwd PhoneBwd;

/**
 * To jest struktura przechowująca ciąg numerów telefonów.
 */
struct PhoneNumbers {
    char** number;                  ///< Ciąg napisów reprezentujących numer.
    size_t size;                    ///< Rozmiar tablicy napisów.
};

/**
 * Typedef struktury PhoneNumbers ułatwiający implementacje.
 */
typedef struct PhoneNumbers PhoneNumbers;

/**
 * @brief Tworzy nowy węzeł drzewa przekierowań.
 * Tworzy nowy węzeł drzewa przekierowań. Przyjmuje wskaźnik na rodzica i ustawia 
 * go w strukturze nowopowstałego węzła. Alokuje pamięć na potencjalnych synów.
 * @param[in] parent - wskaźnik na rodzica.
 * @return *Wskaźnik na nowo utworzony węzeł
 */
static PhoneFwd * phf_create_node(PhoneFwd* parent) {
    PhoneFwd * phf_ptr = malloc(sizeof(PhoneFwd));
    if (phf_ptr == NULL)
        return NULL;

    phf_ptr->forwarded_prefix = NULL;
    phf_ptr->parent = parent;
    phf_ptr->children = malloc(sizeof(PhoneForward*) * HOW_MANY_NUMBERS);
    if (phf_ptr->children == NULL)
        return NULL;

    for (int i = 0; i < HOW_MANY_NUMBERS; i++)
        phf_ptr->children[i] = NULL;
    return phf_ptr;
}

/**
 * @brief TODO w drugiej części.
 * TODO w drugiej części.
 */
static PhoneBwd * phf_create_backward_node() {
    return NULL;
}

/**
 * @brief Tworzy nową strukturę PhoneNumbers przechowującą listę numerów telefonów.
 * Tworzy nową strukturę PhoneNumbers
 * 
 * @param[in] number - lista numerów, która ma zostać umieszczona w strukturze.
 * @param[in] size - ilość elementów w liście numerów.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 *         alokować pamięci.
 */
static PhoneNumbers * phn_create(char** number, size_t size) {
    PhoneNumbers * phn = malloc(sizeof(PhoneNumbers));
    if (phn == NULL)
        return NULL;

    phn->size = size;
    phn->number = malloc(sizeof(char*) * size);
    if (phn->number == NULL)
        return NULL;

    for (size_t i = 0; i < size; i++) {
        phn->number[i] = malloc(sizeof(char) * (strlen(number[i]) + 1));
        if (phn->number[i] == NULL)
            return NULL;
        strcpy(phn->number[i], number[i]);
    }
    return phn;
}

/** @brief Tworzy nową strukturę.
 * Tworzy nową strukturę niezawierającą żadnych przekierowań.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 *         alokować pamięci.
 */
PhoneForward * phfwdNew(void) {
    PhoneForward* new_struct = malloc(sizeof(PhoneForward));
    if (new_struct == NULL)
        return NULL;

    new_struct->tree = phf_create_node(NULL);
    new_struct->backward_tree = phf_create_backward_node();
    return new_struct;
}

/**
 * @brief Zwalnia pamięć zajmowaną przez pojedynczy węzeł w drzewie przekierowań.
 * Zwalnia pamięć zajmowaną przez pojedynczy węzeł w drzewie przekierowań.
 * @param[in] pfd_node - wskaźnik na węzeł, który ma zostać usunięty.
 */
void free_node(PhoneFwd * pfd_node) {
    free(pfd_node->forwarded_prefix);
    free(pfd_node->children);
}

/**
 * @brief Zwalnia pamięć zajmowaną przez węzeł i wszystkich jego potomków i usuwaich z drzewa.
 * Zwalnia pamięć zajmowaną przez węzeł i wszystkich
 * jego potomków i usuwa ich z drzewa przekierowań podanego w argumencie. 
 * Jeśli dany węzeł miałrodzica to wskaźnikna usuwany węzeł w tablicy dzieci
 * jest zamieniany na wartość NULL.
 * @param[in] pfd_node Węzeł, który należy usunąć. 
 */
void delete_tree(PhoneFwd * pfd_node) {
    if (pfd_node == NULL)
        return;

    PhoneFwd * delete_border = pfd_node->parent;
    bool found_son = false;
    // Iteracja po drzewie.
    while (pfd_node != delete_border) {
        found_son = false;
        for (int i = 0; i < HOW_MANY_NUMBERS; i++) {
            if (pfd_node->children[i] != NULL) {
                found_son = true;
                pfd_node = pfd_node->children[i];
                break;
            }
        }
        // Gdy funkcja jest liściem to przejdź do rodzica i zwolnij pamięć.
        if (!found_son) {
            int son_number = 0;
            PhoneFwd* son = pfd_node;
            pfd_node = pfd_node->parent;
            free_node(son);
            free(son);

            if (pfd_node != NULL) {
                while (pfd_node->children[son_number] != son) 
                    son_number++;
                pfd_node->children[son_number] = NULL;
            }
        }
    }
}

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p pf. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL.
 * @param[in] pf – wskaźnik na usuwaną strukturę.
 */
void phfwdDelete(PhoneForward *pf) {
    if (pf == NULL)
        return;
    delete_tree(pf->tree);
    free(pf);
}

/**
 * @brief Sprawdza czy znak reprezentuje liczbę.
 * Sprawdza czy znak reprezentuje liczbe i zwraca odpowiednią wartość logiczną
 * 
 * @param[in] c znak
 * @return true - jeśli znak @p c reprezentuje liczbe
 * @return false - jeśli znak @p c nie reprezentuje liczby
 */
static bool is_number(char c) {
    return (c <= MAX_NUMBER && c >= MIN_NUMBER);
} 

/**
 * @brief Konwertuje znak do reprezentowanej przez niego liczby.
 * Konwertuje znak do reprezentowanej przez niego liczby. Funkcja ta nie sprawdza 
 * czy znak reprezentuje numer i działa przy założeniu, że tak jest. W przeciwnym
 * wypadku pewną liczbę całkowitą. 
 * 
 * @param[in] c - znak który ma zostać przekonwertowany
 * @return Liczba reprezentowana przez ten znak.
 */
static int convert_to_number(char c){
    return ((int)c - CONVERSION);
}

/**
 * @brief Sprawdza czy parametry podane do funkcji @p phfwdAdd są poprawne.
 * Funkcja pomocnicza dla funkcji @p phfwdAdd Sprawdza czy parametry podane do funkcji phfwdAdd są poprawne. Zwraca 
 * odpowiednią wartość logiczną. Dodatkowo modyfikuje wartość pod wkaźnikiem
 * @param[in] pf - wskaźnik na korzeń drzewa przekierowań.
 * @param[in] num1 - wskaźnik na napis przekierowywany.
 * @param[in] num2 - wkaźnik na napis, na który należy dodać przekierowanie.
 * @param[in, out] iterator_ptr - wskaźnik na wartość iterator.
 * @return true - parametry funkcji są poprawne.
 * @return false - parametry funkcji są niepoprawne.
 */
bool check_parameters(PhoneForward *pf, char const *num1,
                      char const *num2, size_t* iterator_ptr) {
    size_t iterator = 0;
    bool exact = true;
    // Sprawdzenie czy podany napis reprezentuje numer.
    if (!is_number(num1[0]) || !is_number(num2[0]) || pf == NULL) 
        return false;
    
    // Sprawdzenie czy napisy są identyczne i wyliczenie długośći przekierowania.
    while (is_number(num2[iterator])) {
        if (exact)
            if (num1[iterator] != num2[iterator] || !is_number(num1[iterator]))
                exact = false; 
        iterator++;
    }
    // Sprawdzenie poprawności napisu.
    if (num2[iterator] != '\0') 
        return false;
    // Sprawdzenie czy num2 jest równe num1 czy tylko jego prefiksem.
    if (exact)
        if (is_number(num1[iterator])) 
            exact = false; 
    if (exact) 
        return false;
    *iterator_ptr = iterator;
    return true;
}

/** @brief Dodaje przekierowanie.
 * Dodaje przekierowanie wszystkich numerów mających prefiks @p num1, na numery,
 * w których ten prefiks zamieniono odpowiednio na prefiks @p num2. Każdy numer
 * jest swoim własnym prefiksem. Jeśli wcześniej zostało dodane przekierowanie
 * z takim samym parametrem @p num1, to jest ono zastępowane.
 * Relacja przekierowania numerów nie jest przechodnia.
 * @param[in,out] pf – wskaźnik na strukturę przechowującą przekierowania
 *                     numerów;
 * @param[in] num1   – wskaźnik na napis reprezentujący prefiks numerów
 *                     przekierowywanych;
 * @param[in] num2   – wskaźnik na napis reprezentujący prefiks numerów,
 *                     na które jest wykonywane przekierowanie.
 * @return Wartość @p true, jeśli przekierowanie zostało dodane.
 *         Wartość @p false, jeśli wystąpił błąd, np. podany napis nie
 *         reprezentuje numeru, oba podane numery są identyczne lub nie udało
 *         się alokować pamięci.
 */
bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    if (pf == NULL || num1 == NULL || num2 == NULL)
        return false;

    PhoneFwd * pfd_node = pf->tree;
    size_t iterator = 0;
    char* forwarded = NULL;
    if (!check_parameters(pf, num1, num2, &iterator))
        return false;
    // Dodatkowe miejsce na '\0'.
    forwarded = malloc (sizeof(char) * (iterator + 1)); 
    if (forwarded == NULL)
        return false;
    // Dodanie znaku konca napisu. 
    forwarded[iterator] = '\0';

    iterator = 0;
    while (is_number(num2[iterator])) {
        forwarded[iterator] = num2[iterator];
        iterator++;
    }

    // Dodawanie numeru do drzewa prefiksowego.
    iterator = 0;
    while (is_number(num1[iterator + 1])) {;
        int value = convert_to_number(num1[iterator]);
        if (pfd_node->children[value] == NULL) 
            pfd_node->children[value] = phf_create_node(pfd_node);
        pfd_node = pfd_node->children[value];
        iterator++;
    }
    if (num1[iterator + 1] != '\0') {
        free(forwarded);
        return false;
    }
    int value = convert_to_number(num1[iterator]);
    if (pfd_node->children[value] == NULL)
        pfd_node->children[value] = phf_create_node(pfd_node);
    pfd_node = pfd_node->children[value];

    free(pfd_node->forwarded_prefix);
    pfd_node->forwarded_prefix = forwarded;
    return true;
}

/**
 * @brief Funkcja przechodząca do węzła drzewa po podanym napisie.
 * Funkcja przyjmuje napis i przechodzi po drzewie przekierowań po znakach 
 * tego napisu. Po przeczytaniu wszystkich znaków lub napotkaniu NULL-a w 
 * drzewie kończy działanie. W przypadku sukcesu zwraca węzeł na, na którym
 * zakończyło się działanie.
 * @param[in] pfd_node - wskaźnik na korzeń drzewa przekierowań.
 * @param[in] num - napis określający węzeł do którego funkcja ma się dostać.
 * @return Wskaźnik na węzeł opisany przez napis @p num lub NULL w przypadku
 * gdy ten węzeł nie występuje w drzewie.
 */
static PhoneFwd * go_to_prefix(PhoneFwd *pfd_node, char const *num) {
    if (num == NULL)
        return NULL;
    if (!is_number(num[0]))
        return NULL;

    size_t iterator = 0;

    while (is_number(num[iterator])) {
        int value = convert_to_number(num[iterator]);
        if (pfd_node->children[value] == NULL) 
            return NULL;
        
        pfd_node = pfd_node->children[value];
        iterator++;
    }
    return pfd_node;
}

/** @brief Usuwa przekierowania.
 * Usuwa wszystkie przekierowania, w których parametr @p num jest prefiksem
 * parametru @p num1 użytego przy dodawaniu. Jeśli nie ma takich przekierowań
 * lub napis nie reprezentuje numeru, nic nie robi.
 * @param[in,out] pf – wskaźnik na strukturę przechowującą przekierowania
 *                     numerów;
 * @param[in] num    – wskaźnik na napis reprezentujący prefiks numerów.
 */
void phfwdRemove(PhoneForward *pf, char const *num) {
    if (pf == NULL)
        return;
    PhoneFwd* pfd_node = go_to_prefix(pf->tree, num);
    delete_tree(pfd_node);
}

/**
 * @brief Zwraca strukturę PhoneNumbers zawierającą odpowiednie przekierowanie.
 * Funkcja pomocnicza dla funkcji phfwdGet. Zwraca strukturę PhoneNumbers 
 * zawierającą odpowiednie przekierowanie na podstawie odstatnio napotkanej 
 * wartości przekierowania w drzewie przekierowań.
 * W przypadku błędnych danych wejściowych zwraca NULL.
 * 
 * @param[in] num - napis zawierający numer który należy przekierować
 * @param[in] last_depth - głębokość drzewa, na której napotkano ostatnie przekierowanie.
 * @param[in] last - ostatnio napotkane przekierowanie w drzewie przekierowań.
 * @return Wskaźnik na strukturę przechowującą odpowiednie przekierowanie na podstawie
 * ostatnio napotkanych wartości w drzewie przekierowań. 
 */
PhoneNumbers * get_last_number (const char* num, size_t last_depth, char* last) {
    size_t num_len = last_depth, forwarded_len = 0;
    while (is_number(num[num_len]))
        num_len++;

    if (last == NULL) {
        num_len++; // miejsce na ostatni znak
        char* forwarded = malloc(sizeof(char) * num_len);
        if (forwarded == NULL)
            return NULL;
        strncpy(forwarded, num, num_len); // przekierowana część
        PhoneNumbers* res = phn_create(&forwarded, 1);
        free(forwarded);
        return res;
    }
    while (is_number(last[forwarded_len]))
        forwarded_len++;
    
    size_t size = num_len - last_depth + forwarded_len + 1;
    char* forwarded = malloc(sizeof(char) * size);
    if (forwarded == NULL)
        return NULL;

    strncpy(forwarded, last, forwarded_len); // przekierowana część
    // pozostała końcówka wraz z '\0'
    strncpy(forwarded + forwarded_len, num + last_depth, num_len - last_depth + 1);

    PhoneNumbers * result = phn_create(&forwarded, 1);
    free(forwarded);
    return result;
}

/** @brief Wyznacza przekierowanie numeru.
 * Wyznacza przekierowanie podanego numeru. Szuka najdłuższego pasującego
 * prefiksu. Wynikiem jest ciąg zawierający co najwyżej jeden numer. Jeśli dany
 * numer nie został przekierowany, to wynikiem jest ciąg zawierający ten numer.
 * Jeśli podany napis nie reprezentuje numeru, wynikiem jest pusty ciąg.
 * Alokuje strukturę @p PhoneNumbers, która musi być zwolniona za pomocą
 * funkcji @ref phnumDelete.
 * @param[in] pf  – wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
PhoneNumbers * phfwdGet(PhoneForward const *pf, char const *num) {
    if (pf == NULL)
        return NULL;
    if (num == NULL)
        return phn_create(NULL, 0); 

    size_t iterator = 0, last_depth = 0;
    PhoneFwd* probe = pf->tree;
    char* last = NULL;
    // Sprawdzenie czy num reprezentuje liczbe
    while (is_number(num[iterator]))
        iterator++;
    if (num[iterator] != '\0' || iterator == 0)
        return phn_create(NULL, 0);

    iterator = 0;
    while (is_number(num[iterator])) {
        int value = convert_to_number(num[iterator]);
        if (probe->children[value] == NULL) 
            break;
        
        probe = probe->children[value];
        if (probe->forwarded_prefix != NULL) {
            last = probe->forwarded_prefix;
            last_depth = iterator + 1;
        }
        iterator++;
    }
    
    return get_last_number(num, last_depth, last);
}

/** @brief Wyznacza przekierowania na dany numer.
 * Wyznacza następujący ciąg numerów: jeśli istnieje numer @p x, taki że wynik
 * wywołania @p phfwdGet z numerem @p x zawiera numer @p num, to numer @p x
 * należy do wyniku wywołania @ref phfwdReverse z numerem @p num. Dodatkowo ciąg
 * wynikowy zawsze zawiera też numer @p num. Wynikowe numery są posortowane
 * leksykograficznie i nie mogą się powtarzać. Jeśli podany napis nie
 * reprezentuje numeru, wynikiem jest pusty ciąg. Alokuje strukturę
 * @p PhoneNumbers, która musi być zwolniona za pomocą funkcji @ref phnumDelete.
 * @param[in] pf  – wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
PhoneNumbers * phfwdReverse(PhoneForward const *pf, char const *num) {
    return NULL;
}

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p pnum. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL.
 * @param[in] pnum – wskaźnik na usuwaną strukturę.
 */
void phnumDelete(PhoneNumbers *pnum) {
    if (pnum == NULL)
        return;

    for (size_t i = 0; i < pnum->size; i++)
        free(pnum->number[i]);

    free(pnum->number);
    free(pnum);
}

/** @brief Udostępnia numer.
 * Udostępnia wskaźnik na napis reprezentujący numer. Napisy są indeksowane
 * kolejno od zera.
 * @param[in] pnum – wskaźnik na strukturę przechowującą ciąg numerów telefonów;
 * @param[in] idx  – indeks numeru telefonu.
 * @return Wskaźnik na napis reprezentujący numer telefonu. Wartość NULL, jeśli
 *         wskaźnik @p pnum ma wartość NULL lub indeks ma za dużą wartość.
 */
char const * phnumGet(PhoneNumbers const *pnum, size_t idx) {
    if (pnum == NULL)
        return NULL;
        
    if (idx >= pnum->size)
        return NULL;

    return pnum->number[idx];
}
