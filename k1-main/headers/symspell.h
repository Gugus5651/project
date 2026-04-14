#ifndef SYMSPELL_H
#define SYMSPELL_H

#include <stddef.h>
#include <stdbool.h>
#include "common.h" // Pour Dictionary_t

#define MAX_WORD_LEN 256

// --- STRUCTURES DE DONNÉES ---

// Un noeud de la liste chaînée (pour gérer les collisions dans la table)
typedef struct SymNode {
    char cle[MAX_WORD_LEN];  // Le mot avec une lettre en moins (ex: "cat")
    char *mot_original;      // Pointeur vers le vrai mot du dico (ex: "chat")
    struct SymNode *suivant; // Pointeur vers le prochain noeud (collision)
} SymNode_t;

// La Table de Hachage principale
typedef struct {
    SymNode_t **tableau;     // Un tableau de pointeurs vers des SymNode_t
    size_t taille;           // La taille totale allouée pour le tableau
} HashTable_t;


// --- PROTOTYPES DES FONCTIONS ---

// 1. Initialisation et Nettoyage
HashTable_t* initialiser_symspell(Dictionary_t *dict);
void liberer_table_hachage(HashTable_t *ht);

// 2. Fonctions internes (la machinerie)
unsigned long hacher_chaine(const char *str); 
int generer_suppressions(const char *mot, char resultats[][MAX_WORD_LEN]);
void inserer_dans_table(HashTable_t *ht, const char *cle, char *mot_original);

// 3. L'algorithme de correction final (à appeler dans le main)
char* proposer_correction_symspell(HashTable_t *ht, const char *mot_faux);

#endif // SYMSPELL_H