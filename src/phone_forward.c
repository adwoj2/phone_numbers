/** @file
 * Implementacja interfejsu klasy przechowującej przekierowania numerów 
 * telefonicznych.
 *
 * @author Adam Wojciechowski <a.wojciecho2@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define HOW_MANY_NUMBERS 12 ///< Ilość cyfr wraz z dodatkowymi znakami.
#define STAR_VALUE 10       ///< Wartość znaku *.
#define HASH_VALUE 11       ///< Wartość znaku #.


/*! \def TARGET
    \brief Makro skracające zapis funkcji.
*/

/**
 * To jest struktura przechowująca przekierowania numerów telefonów.
 */
struct PhoneForward {
    struct PhoneFwd* tree;          ///< Wskaźnik na korzeń drzewa przekierowań.
    struct PhoneBwd* backward_tree; ///< Wskaźnik na korzeń drzewa odwróconych przekierowań.
};

/**
 * Pozbycie się konieczności używania słowa kluczowego "struct".
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
 * Pozbycie się konieczności używania słowa kluczowego "struct".
 */
typedef struct PhoneFwd PhoneFwd;

/**
 * To jest struktura przechowująca węzeł drzewa odwróconych przekierowań.
 */
struct PhoneBwd {
    struct PhoneBwd** children;     ///< Tablica dzieci danego węzła. 
    struct PhoneBwd* parent;        ///< Wskaźnik na rodzica danego węzła.
    struct PhoneNumbers * forwarded_prefix; 
    ///< Ciąg napisów które przekierowują na dany prefiks.
};

/**
 * Pozbycie się konieczności używania słowa kluczowego "struct".
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
 * Pozbycie się konieczności używania słowa kluczowego "struct".
 */
typedef struct PhoneNumbers PhoneNumbers;

// Zapewnienie widoczności funkcji phnumDelete innym funkcjom.
void phnumDelete(PhoneNumbers *pnum);

/**
 * @brief Sprawdza, czy znak reprezentuje liczbę.
 * Sprawdza, czy znak reprezentuje liczbę wraz z dodatkowymi liczbami w treści 
 * i zwraca odpowiednią wartość logiczną.
 * @param[in] c znak.
 * @return true - jeśli znak @p c reprezentuje liczbe.
 * @return false - jeśli znak @p c nie reprezentuje liczby.
 */
static bool is_number(char c) {
    return ((c <= '9' && c >= '0') || c == '*' || c == '#');
} 

/**
 * @brief Konwertuje znak do reprezentowanej przez niego liczby.
 * Konwertuje znak do reprezentowanej przez niego liczby. Funkcja ta nie sprawdza, 
 * czy znak reprezentuje numer i działa przy założeniu, że tak jest. W przeciwnym
 * wypadku zwraca pewną liczbę całkowitą. Znaki '*' i '#' są konwertowane na 
 * odpowiednio 10 i 11.
 * @param[in] c - znak który ma zostać przekonwertowany.
 * @return Liczba reprezentowana przez ten znak.
 */
static int convert_to_number(char c) {
    if (c == '*')
        return STAR_VALUE;
    if (c == '#')
        return HASH_VALUE;
    return ((int)c - '0');
}

/**
 * @brief Konwertuje liczbę na reprezentujący ją znak.
 * Konwertuje liczbę na reprezentujący ją znak. Liczby 10 i 11 są konwertowane 
 * na odpowiednio '*' u '#'.
 * @param[in] i - liczba który ma zostać przekonwertowany
 * @return Liczba reprezentowana przez ten znak.
 */
static char convert_to_char(int i) {
    if (i == STAR_VALUE)
        return '*';
    if (i == HASH_VALUE)
        return '#';
    return ((char)((int)'0' + i));
}

/**
 * @brief Tworzy nową strukturę PhoneNumbers przechowującą listę numerów telefonów.
 * Tworzy nową strukturę PhoneNumbers.
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
    if (phn->number == NULL){
        free(phn);
        return NULL;
    }

    for (size_t i = 0; i < size; i++) {
        phn->number[i] = malloc(sizeof(char) * (strlen(number[i]) + 1));
        if (phn->number[i] == NULL) {
            for (size_t j = 0; j < i; j++)
                free(phn->number[j]);
            free(phn->number);
            free(phn);
            return NULL;
        }
        strcpy(phn->number[i], number[i]);
    }
    return phn;
}

/**
 * @brief Tworzy nowy węzeł drzewa przekierowań.
 * Tworzy nowy węzeł drzewa przekierowań. Przyjmuje wskaźnik na rodzica i ustawia 
 * go w strukturze nowopowstałego węzła. Alokuje pamięć na potencjalnych synów.
 * @param[in] parent - wskaźnik na rodzica
 * @return *Wskaźnik na nowo utworzony węzeł.
 */
static PhoneFwd * phf_create_node(PhoneFwd* parent) {
    PhoneFwd * phf_ptr = malloc(sizeof(PhoneFwd));
    if (phf_ptr == NULL)
        return NULL;

    phf_ptr->forwarded_prefix = NULL;
    phf_ptr->parent = parent;
    phf_ptr->children = malloc(sizeof(PhoneForward*) * HOW_MANY_NUMBERS);
    if (phf_ptr->children == NULL) {
        free(phf_ptr);
        return NULL;
    }

    for (int i = 0; i < HOW_MANY_NUMBERS; i++)
        phf_ptr->children[i] = NULL;
    return phf_ptr;
}

/**
 * @brief Tworzy nowy węzeł drzewa odwróconych przekierowań.
 * Tworzy nowy węzeł drzewa przekierowań. Przyjmuje wskaźnik na rodzica i ustawia
 * go w strukturze nowopowstałego węzła. Alokuje pamięć na potencjalnych synów.
 * @param[in] parent - wskaźnik na rodzica
 * @return *Wskaźnik na nowo utworzony węzeł.
 */
static PhoneBwd * phf_create_backward_node(PhoneBwd* parent) {
    PhoneBwd * bwd_ptr = malloc(sizeof(PhoneBwd));
    if (bwd_ptr == NULL)
        return NULL;

    bwd_ptr->forwarded_prefix = phn_create(NULL, 0);
    if (bwd_ptr->forwarded_prefix == NULL) {
        free(bwd_ptr);
        return NULL;
    }
    bwd_ptr->parent = parent;
    bwd_ptr->children = malloc(sizeof(PhoneForward*) * HOW_MANY_NUMBERS);
    if (bwd_ptr->children == NULL) {
        free(bwd_ptr->forwarded_prefix);
        free(bwd_ptr);
        return NULL;
    }

    for (int i = 0; i < HOW_MANY_NUMBERS; i++)
        bwd_ptr->children[i] = NULL;
    return bwd_ptr;
}

/**
 * @brief Zwalnia pamięć zajmowaną przez pojedynczy węzeł w drzewie przekierowań.
 * Zwalnia pamięć zajmowaną przez pojedynczy węzeł w drzewie przekierowań.
 * @param[in] pfd_node - wskaźnik na węzeł, który ma zostać usunięty.
 */
static void free_node(PhoneFwd * pfd_node) {
    if (pfd_node == NULL)
        return;
    free(pfd_node->forwarded_prefix);
    free(pfd_node->children);
    free(pfd_node);
}

/**
 * @brief Zwalnia pamięć zajmowaną przez pojedynczy węzeł w odwrócoych drzewie przekierowań.
 * Zwalnia pamięć zajmowaną przez pojedynczy węzeł w drzewie przekierowań.
 * @param[in] pbd_node - wskaźnik na węzeł, który ma zostać usunięty.
 */
static void free_backward_node(PhoneBwd * pbd_node) {
    if (pbd_node == NULL)
        return;
    phnumDelete(pbd_node->forwarded_prefix);
    free(pbd_node->children);
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
    new_struct->backward_tree = phf_create_backward_node(NULL);
    if (new_struct->tree == NULL || new_struct->backward_tree == NULL) {
        free_node(new_struct->tree);
        free_backward_node(new_struct->backward_tree);
        free(new_struct);
        return NULL;
    }
    return new_struct;
}

/** @brief Usuwa pojedyncze odwrócone przekierowanie z drzewa odwróconych przekierowań.
 * Usuwa pojedyncze odwrócone przekierowanie z drzewa odwróconych przekierowań.
 * @param[in] pfd_backward_node – wskaźnik na korzeń drzewa odwróconych przekierowań;
 * @param[in] forward - wkaźnik na słowo, na które przekierowanie chcemy usunąć;
 * @param[in] to_remove - wkaźnik na napis reprezentując przekierowanie, które usuwamy.
 */
static void delete_forward_from_bwd(PhoneBwd * pfd_backward_node, 
                                    const char *forward, const char* to_remove) {
    size_t iterator = 0;

    while (is_number(forward[iterator])) {
        int value = convert_to_number(forward[iterator]);
        if (pfd_backward_node->children[value] == NULL) 
            return; 
        pfd_backward_node = pfd_backward_node->children[value];
        iterator++;
    }
#define TARGET pfd_backward_node->forwarded_prefix  
    for (size_t i = 0; i < TARGET->size; i++)
        if (strcmp(TARGET->number[i], to_remove) == 0) {
            TARGET->size--;
            if (TARGET->size == 0)
                free(TARGET->number[0]);
            else {
                free(TARGET->number[i]);
                if (TARGET->size != i) {
                    TARGET->number[i] = TARGET->number[TARGET->size];
                }
            }
            break;
        }
#undef TARGET
}

/**
 * @brief Usuwa drzewo odwróconych przekierowań zwalniając pamięć.
 * Usuwa drzewo odwróconych przekierowań zwalniając pamięć.
 * @param[in] pfd_backward_node Korzeń na drzewo odwróconych przekierowań.
 */
static void remove_backward_tree(PhoneBwd * pfd_backward_node) {
    if (pfd_backward_node == NULL)
        return;

    bool found_son = false;
    // Iteracja po drzewie.
    while (pfd_backward_node != NULL) {
        found_son = false;
        for (int i = 0; i < HOW_MANY_NUMBERS; i++) {
            if (pfd_backward_node->children[i] != NULL) {
                found_son = true;
                pfd_backward_node = pfd_backward_node->children[i];
                break;
            }
        }
        // Gdy funkcja jest liściem, to przejdź do rodzica i zwolnij pamięć.
        if (!found_son) {
            int son_number = 0;
            PhoneBwd* son = pfd_backward_node;
            pfd_backward_node = pfd_backward_node->parent;
            free_backward_node(son);
            free(son);

            if (pfd_backward_node != NULL) {
                while (pfd_backward_node->children[son_number] != son) {
                    son_number++;
                }
                pfd_backward_node->children[son_number] = NULL;
            }
        }
    }
}

/**
 * @brief Zwalnia pamięć zajmowaną przez węzeł i wszystkich jego potomków i usuwa ich z drzewa.
 * Zwalnia pamięć zajmowaną przez węzeł i wszystkich jego potomków i usuwa ich
 * z drzewa przekierowań. Jeśli wartość @p pfd_backward_tree jest różna od 
 * wartości null to usuwa również odpowiadający mu węzeł w drzewie odwrotnych
 * przekierowań, którego korzeniem jest podany węzeł. 
 * Jeśli dany węzeł miał rodzica to wskaźnik na usuwany węzeł w tablicy dzieci
 * jest zamieniany na wartość NULL. 
 * @param[in] pfd_node Węzeł, który należy usunąć.;
 * @param[in] pfd_backward_tree Wskaźnik na korzeń drzewa odwrotnych przekierowań;
 * @param[in] path Napis opisujący zwalniany węzeł.
 */
static void delete_tree(PhoneFwd * pfd_node, PhoneBwd * pfd_backward_tree, 
                        const char* path) {
    if (pfd_node == NULL)
        return;
    /* Path musi być terminowane nullem bo gdyby nie było to pfd_node byłoby
    równe NULL ze względu na działanie funkcji pomocniczej go_to_prefix. */
    size_t current_path_length = strlen(path) + 1, max_path_length = current_path_length * 2; 
    char* current_path = NULL;
    if (pfd_backward_tree != NULL) {
        current_path = malloc(sizeof(char) * max_path_length);
        if (current_path == NULL) 
            return;
        strcpy(current_path, (char*)path);
    }

    PhoneFwd * delete_border = pfd_node->parent;
    bool found_son = false;
    // Iteracja po drzewie.
    while (pfd_node != delete_border) {
        found_son = false;
        for (int i = 0; i < HOW_MANY_NUMBERS; i++) {
            if (pfd_node->children[i] != NULL) {
                found_son = true;
                pfd_node = pfd_node->children[i];
                if (pfd_backward_tree != NULL) {
                    current_path_length++;
                    if (current_path_length > max_path_length) {
                        max_path_length *= 2;
                        current_path = realloc(current_path, sizeof(char) * max_path_length);
                        if (current_path == NULL) 
                            return;
                    }
                // Dodatkowe miejsce na znak '\0'.
                current_path[current_path_length - 2] = convert_to_char(i);
                }
                break;
            }
        }
        // Gdy funkcja jest liściem, to przejdź do rodzica i zwolnij pamięć.
        if (!found_son) {
            int son_number = 0;
            PhoneFwd* son = pfd_node;
            pfd_node = pfd_node->parent;

            if (pfd_backward_tree != NULL)  {
                if (son->forwarded_prefix != NULL) {
                    current_path[current_path_length - 1] = '\0';
                    delete_forward_from_bwd(pfd_backward_tree, son->forwarded_prefix, current_path);
                }
            current_path_length--;
            }

            free_node(son);
            if (pfd_node != NULL) {
                while (pfd_node->children[son_number] != son) 
                    son_number++;
                pfd_node->children[son_number] = NULL;
            }
        }
    }
    free(current_path);
}

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p pf. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL.
 * @param[in] pf – wskaźnik na usuwaną strukturę.
 */
void phfwdDelete(PhoneForward *pf) {
    if (pf == NULL)
        return;
    delete_tree(pf->tree, NULL, "");
    remove_backward_tree(pf->backward_tree);
    free(pf);
}

/**
 * @brief Sprawdza, czy parametry podane do funkcji @p phfwdAdd są poprawne.
 * Funkcja pomocnicza dla funkcji @p phfwdAdd Sprawdza, czy parametry podane do funkcji phfwdAdd są poprawne. Zwraca 
 * odpowiednią wartość logiczną. Dodatkowo modyfikuje wartość pod wkaźnikiem.
 * @param[in] pf - wskaźnik na korzeń drzewa przekierowań;
 * @param[in] num1 - wskaźnik na napis przekierowywany;
 * @param[in] num2 - wkaźnik na napis, na który należy dodać przekierowanie;
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

/** @brief Dodaje nowy napis do struktury PhoneNumbers.
 * Dodaje nowy napis do struktury PhoneNumbers. Zwraca prawdę jeśli alokowanie
 * pamięci się powiodło i fałsz w przeciwnym przypadku.
 * @param[in, out] phnum – wskaźnik na strukturę do której dodajemy napis;
 * @param[in] new - napis, który ma zostać dodany do struktury.
 */
static bool add_to_phnum(PhoneNumbers* phnum, const char* new) {
    // Poprawność argumentów została sprawdzona wcześniej.
    phnum->size++;
    phnum->number = realloc(phnum->number, sizeof(char*) * phnum->size);
    if (phnum->number == NULL){
        phnum->size--;
        return false;
    }
    phnum->number[phnum->size - 1] = malloc(sizeof(char) * (strlen(new) + 1));
    if (phnum->number[phnum->size - 1] == NULL) {
        free(phnum->number);
        phnum->size--;
        return false;
    }
    strcpy(phnum->number[phnum->size - 1], new);
    return true;
}

/**
 * @brief Tworzy nowy odwrócone przekierowanie i dodaje je do odpowiedniego drzewa.
 * Tworzy nowy węzeł drzewa odwróconych przekierowań. Przyjmuje napis 
 * reprezentujący numer przekierowany i ten, na który ma zostać przekierowany.
 * Zwraca prawde, gdy alokowanie pamięci się powiodło i fałsz w przeciwnym przypadku.
 * @param[in] pbd_node - wskaźnik na korzeń drzewa przekierowań;
 * @param[in] num1 - napis przekierowywany; 
 * @param[in] num2 - napis, na który ma zostać dodane przekierowanie;
 * @return *Wskaźnik na nowo utworzony węzeł.
 */
static bool add_forward_to_backward_tree(PhoneBwd * pbd_node, char const *num1, char const *num2) {
    // Poprawność danych została sprawdzona w funkcji phfwdAdd.
    int iterator = 0;
    while (is_number(num2[iterator])) {
        int value = convert_to_number(num2[iterator]);
        if (pbd_node->children[value] == NULL) 
            pbd_node->children[value] = phf_create_backward_node(pbd_node);
        if (pbd_node->children[value] == NULL)
            return false;
        pbd_node = pbd_node->children[value];
        iterator++;
    }

    // Dodanie kolejnej odwrotności przekierowania.
    return add_to_phnum(pbd_node->forwarded_prefix, num1);
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
    forwarded = malloc(sizeof(char) * (iterator + 1)); 
    if (forwarded == NULL)
        return false;
    iterator = 0;
    while (is_number(num2[iterator])) {
        forwarded[iterator] = num2[iterator];
        iterator++;
    }
    // Dodanie znaku końca napisu.
    forwarded[iterator] = '\0';
    // Dodawanie numeru do drzewa prefiksowego.
    iterator = 0;
    if (!is_number(num1[iterator])) {
        free(forwarded);
        return false;
    }
    while (is_number(num1[iterator + 1])) {
        int value = convert_to_number(num1[iterator]);
        if (pfd_node->children[value] == NULL) 
            pfd_node->children[value] = phf_create_node(pfd_node);
        if (pfd_node->children[value] == NULL) {
            free(forwarded);
            return false;
        }   
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
    if (pfd_node->children[value] == NULL) {
        free(forwarded);
        return false;
    }   
    pfd_node = pfd_node->children[value];

    if (pfd_node->forwarded_prefix != NULL) {
        delete_forward_from_bwd(pf->backward_tree, pfd_node->forwarded_prefix, num1);
        free(pfd_node->forwarded_prefix);
    }
    pfd_node->forwarded_prefix = forwarded;
    return add_forward_to_backward_tree(pf->backward_tree, num1, num2);
}

/**
 * @brief Funkcja przechodząca do węzła drzewa po podanym napisie.
 * Funkcja przyjmuje napis i przechodzi po drzewie przekierowań po znakach 
 * tego napisu. Po przeczytaniu wszystkich znaków lub napotkaniu NULL-a w 
 * drzewie kończy działanie. W przypadku sukcesu zwraca węzeł na, na którym
 * zakończyło się działanie.
 * @param[in] pfd_node - wskaźnik na korzeń drzewa przekierowań;
 * @param[in] num - napis określający węzeł do którego funkcja ma się dostać.
 * @return Wskaźnik na węzeł opisany przez napis @p num lub NULL w przypadku,
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
    if (is_number(num[iterator]) != '\0')
        return NULL;
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
    delete_tree(pfd_node, pf->backward_tree, num);
}

/**
 * @brief Zwraca strukturę PhoneNumbers zawierającą odpowiednie przekierowanie.
 * Funkcja pomocnicza dla funkcji phfwdGet. Zwraca strukturę PhoneNumbers 
 * zawierającą odpowiednie przekierowanie na podstawie odstatnio napotkanej 
 * wartości przekierowania w drzewie przekierowań.
 * W przypadku błędnych danych wejściowych zwraca NULL.
 * 
 * @param[in] num - napis zawierający numer który należy przekierować;
 * @param[in] last_depth - głębokość drzewa, na której napotkano ostatnie przekierowanie;
 * @param[in] last - ostatnio napotkane przekierowanie w drzewie przekierowań.
 * @return Wskaźnik na strukturę przechowującą odpowiednie przekierowanie na podstawie
 * ostatnio napotkanych wartości w drzewie przekierowań. 
 */
static PhoneNumbers * get_last_number(const char* num, size_t last_depth, char* last) {
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
 * funkcji @ref phnumDelete. Zwraca wartość NULL dla @p pf o wartości NULL.
 * @param[in] pf  – wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się alokować pamięci bądź dane wejściowe były niepoprawne.
 */
PhoneNumbers * phfwdGet(PhoneForward const *pf, char const *num) {
    if (pf == NULL)
        return NULL;
    if (num == NULL)
        return phn_create(NULL, 0); 

    size_t iterator = 0, last_depth = 0;
    PhoneFwd* probe = pf->tree;
    char* last = NULL;
    // Sprawdzenie czy num reprezentuje liczbe.
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

/** @brief Komparator dla funkcji bibliotecznej qsort.
 * Komparator dla funkcji bibliotecznej qsort.
 * @param[in] first – wskaźnik na pierwszy porównywany napis;
 * @param[in] second – wskaźnik na drugi porównywany napis.
 * @return 1 jeżeli pierwsze słowo jest leksykograficznie późniejsze od drugiego;
 * -1 w przeciwnym przypadku; 0 w przypadku gdy są one równe.
 */
static int string_comparator(const void* first, const void* second) {
    const char* a = *(const char**)first;
    const char* b = *(const char**)second;

    for (size_t i = 0; a[i]; i++) {
        if (convert_to_number(a[i]) > convert_to_number(b[i]))
            return 1;
        else if (convert_to_number(a[i]) < convert_to_number(b[i]))
            return -1;
    }
    size_t end_index = strlen(a);
    // Porównanie znaków końca napisu.
    if (convert_to_number(b[end_index]) > convert_to_number(b[end_index]))
        return 1;
    if (convert_to_number(a[end_index]) < convert_to_number(b[end_index]))
        return -1;
    return 0;
}

/** @brief Umieszcza nowe odwrotne przekierowanie w strukturze PhoneNumbers.
 * Funkcja pomocnicza która mieszcza nowe odwrotne przekierowanie w strukturze 
 * PhoneNumbers. Przyjmuje przekierowywany prefiks, nieprzekierowany fragment 
 * słowa oraz jego długość i łączy je w pojedynczy napis. Zwraca prawdę
 * jeśli alokacja pamięci się uda i fałsz w przeciwnym przypadku.
 * @param[in, out] result  – wskaźnik na strukturę przechowującą odwrotne przekierowania numerów;
 * @param[in] to_add – wskaźnik na strukturę przechowującą dodawany numer;
 * @param[in] rest – nieprzekierowywana część słowa.
 */
static bool insert_to_phnum(PhoneNumbers *result, PhoneNumbers *to_add,
                            char* rest) {
    int target_bonus_len = strlen(rest) + 1; // +1 na znak końca napisu
    size_t result_current_size = result->size;
    result->size += to_add->size;
    result->number = realloc(result->number, sizeof(char*) * result->size);
    if (result->number == NULL)
        return false;
    bool repeated = false;

    for (size_t i = 0; i < to_add->size; i++) {
        repeated = false;
        // Nie dodajemy powtórzeń.
        for (size_t j = 0; j < result_current_size; j++) {
            if (strcmp(result->number[j], to_add->number[i]) == 0) {
                result->size--;
                result->number = realloc(result->number, sizeof(char*) * result->size);
                if (result->number == NULL)
                    return false;
                repeated = true;
                break;
            }
        }
        if (repeated) 
            continue;

        result_current_size++;
        size_t new_num_len = strlen(to_add->number[i]);
        result->number[result_current_size - 1] = 
            malloc(sizeof(char) * (target_bonus_len + new_num_len));
        if (result->number[result_current_size - 1] == NULL) {
            
            return false; 
        }

        // Kopiowanie przekierowanego prefiksu.
        strcpy(result->number[result_current_size - 1], to_add->number[i]);
        // Kopiowanie sufiksu pozostałego słowa.
        strcpy(result->number[result_current_size - 1] + new_num_len, rest);
    }
    return true;
}

/** @brief Wyznacza przekierowania na dany numer.
 * Wyznacza następujący ciąg numerów: jeśli istnieje numer @p x, taki że jeśli
 * w drzewie przekierowań istnieje takie przekierowanie, które przekierowuje
 * numer @p x na @p num, to numer @p x należy do wyniku wywołania 
 * @ref phfwdReverse z numerem @p num. Dodatkowo ciągwynikowy zawsze zawiera
 * też numer @p num. Wynikowe numery są posortowane
 * leksykograficznie i nie mogą się powtarzać. Jeśli podany napis nie
 * reprezentuje numeru, wynikiem jest pusty ciąg. Alokuje strukturę
 * @p PhoneNumbers, która musi być zwolniona za pomocą funkcji @ref phnumDelete.
 * @param[in] pf  – wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
PhoneNumbers * phfwdReverse(PhoneForward const *pf, char const *num){
    if (pf == NULL)
        return NULL;
    if (num == NULL)
        return phn_create(NULL, 0); 

    size_t iterator = 0;
    PhoneBwd* probe = pf->backward_tree;
    // Sprawdzenie czy num reprezentuje liczbe.
    while (is_number(num[iterator]))
        iterator++;
    if (num[iterator] != '\0' || iterator == 0)
        return phn_create(NULL, 0);

    iterator = 0;
    PhoneNumbers *result = phn_create(NULL, 0);
    if (result == NULL)
        return NULL;

    while (is_number(num[iterator])) {
        int value = convert_to_number(num[iterator]);
        if (probe->children[value] == NULL) 
            break;
        
        probe = probe->children[value];
        if (probe->forwarded_prefix->size != 0) 
            if (!insert_to_phnum(result, probe->forwarded_prefix, (char*)(num + iterator + 1))) {
                phnumDelete(result);
                return NULL;
            }
        iterator++;
    }
    PhoneNumbers* itself = phn_create((char**)&num, 1);
    if (itself == NULL) {
        phnumDelete(result);
        return NULL;
    }
    if(!insert_to_phnum(result, itself, "")) {
        phnumDelete(itself);
        phnumDelete(result);
        return NULL;
    }

    phnumDelete(itself);

    qsort(result->number, result->size, sizeof(char*), string_comparator);
    return result;
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

/** @brief Wyznacza przekierowania na dany numer.
 * Wyznacza następujący ciąg numerów: jeśli istnieje numer @p x, taki że jeśli
 * wywołamy funkcję @ref phfwdGet na numerze @p x i w wyniku otrzymamy numer
 * @p num, to numer @p x należy do wyniku wywołania @ref phfwdGetReverse 
 * z numerem @p num. Dodatkowo ciągwynikowy zawsze zawiera
 * też numer @p num. Wynikowe numery są posortowane
 * leksykograficznie i nie mogą się powtarzać. Jeśli podany napis nie
 * reprezentuje numeru, wynikiem jest pusty ciąg. Alokuje strukturę
 * @p PhoneNumbers, która musi być zwolniona za pomocą funkcji @ref phnumDelete.
 * @param[in] pf  – wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
PhoneNumbers * phfwdGetReverse(PhoneForward const *pf, char const *num) {
    if (pf == NULL)
        return NULL;
    if (num == NULL)
        return phn_create(NULL, 0);

    int iterator = 0;
    // Sprawdzenie czy numer reprezentuje liczbe
    while (is_number(num[iterator]))
        iterator++;
    if (num[iterator] != '\0' || iterator == 0)
        return phn_create(NULL, 0);

    PhoneNumbers* reversed = phfwdReverse(pf, num);
    PhoneNumbers* res = phn_create(NULL, 0); 
    if (res == NULL)
        return NULL;
    // Sprawdzenie czy wyniki reverse należą do przeciwobrazu funkcji phfwdGet.
    for (size_t i = 0; i < reversed->size; i++) {
        const char* current_number = phnumGet(reversed, i);
        PhoneNumbers* forward_current_number_phn = phfwdGet(pf, current_number);
        const char* forward_current_number = phnumGet(forward_current_number_phn, 0);
        if (strcmp(forward_current_number, num) == 0)
            add_to_phnum(res, current_number);
        phnumDelete(forward_current_number_phn);
    }
    phnumDelete(reversed);
    return res;
}