#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symspell.h" // <-- Correction des guillemets ici

// --- 1. FONCTIONS UTILITAIRES ---

/*
 * Fonction de hachage DJB2
 * Transforme une chaîne de caractères en un grand nombre entier non signé.
 */
unsigned long hacher_chaine(const char *str) {
    unsigned long hash = 5381; // Valeur magique de départ de DJB2
    int c;

    // On parcourt chaque caractère de la chaîne jusqu'au '\0'
    while ((c = *str++)) {
        // hash = hash * 33 + c
        hash = ((hash << 5) + hash) + c; 
    }

    return hash;
}

// Génère les mots amputés d'une lettre
int generer_suppressions(const char *mot, char resultats[][MAX_WORD_LEN]) {
    if (mot == NULL) return 0;
    int len = strlen(mot);
    if (len <= 1) return 0; 

    for (int i = 0; i < len; i++) {
        if (i > 0) strncpy(resultats[i], mot, i);
        strcpy(resultats[i] + i, mot + i + 1);
    }
    return len;
}

// Insère une clé (ex: "cat") et son mot d'origine (ex: "chat") dans la table
void inserer_dans_table(HashTable_t *ht, const char *cle, char *mot_original) {
    if (ht == NULL || cle == NULL || mot_original == NULL) return;

    // 1. Calculer l'index grâce à notre fonction de hachage DJB2
    unsigned long index = hacher_chaine(cle) % ht->taille;

    // 2. Créer un nouveau noeud en mémoire
    SymNode_t *nouveau_noeud = (SymNode_t*) malloc(sizeof(SymNode_t));
    if (nouveau_noeud == NULL) return; // Sécurité en cas de RAM pleine

    // 3. Remplir le noeud
    strncpy(nouveau_noeud->cle, cle, MAX_WORD_LEN - 1);
    nouveau_noeud->cle[MAX_WORD_LEN - 1] = '\0'; // Toujours s'assurer du caractère nul
    
    // L'ASTUCE MÉMOIRE : On ne copie pas "chat", on pointe juste vers le dico du prof !
    nouveau_noeud->mot_original = mot_original; 

    // 4. Insertion en tête de la liste chaînée (pour gérer les collisions)
    nouveau_noeud->suivant = ht->tableau[index];
    ht->tableau[index] = nouveau_noeud;
}

// Petite fonction utilitaire privée pour chercher une clé exacte
static char* chercher_dans_table(HashTable_t *ht, const char *cle) {
    unsigned long index = hacher_chaine(cle) % ht->taille;
    SymNode_t *courant = ht->tableau[index];
    
    while (courant != NULL) {
        if (strcmp(courant->cle, cle) == 0) {
            return courant->mot_original; // On a trouvé un match !
        }
        courant = courant->suivant;
    }
    return NULL;
}


// --- 3. L'API PUBLIQUE (Ce qu'on appelle depuis le main) ---

// Construit l'index SymSpell à partir du dictionnaire fourni par le prof
HashTable_t* initialiser_symspell_(Dictionary_t *dict) {
    if (dict == NULL || dict->word_count == 0) return NULL;

    HashTable_t *ht = (HashTable_t*) malloc(sizeof(HashTable_t));
    if (ht == NULL) return NULL;

    // Bonne pratique : une taille de tableau = 2 à 3 fois le nombre de mots
    ht->taille = dict->word_count * 3; 
    
    // calloc initialise toutes les cases à NULL
    ht->tableau = (SymNode_t**) calloc(ht->taille, sizeof(SymNode_t*));
    if (ht->tableau == NULL) {
        free(ht);
        return NULL;
    }

    // On fait chauffer l'usine : on parcourt tout le dico !
    for (size_t i = 0; i < dict->word_count; i++) {
        char *mot_original = dict->words[i];

        // A. On insère le mot normal
        inserer_dans_table(ht, mot_original, mot_original);

        // B. On génère toutes ses suppressions ("chat" -> "hat", "cat", etc.)
        char suppressions[MAX_WORD_LEN][MAX_WORD_LEN];
        int nb_suppressions = generer_suppressions(mot_original, suppressions);

        // C. On insère toutes les suppressions dans la table
        for (int j = 0; j < nb_suppressions; j++) {
            inserer_dans_table(ht, suppressions[j], mot_original);
        }
    }

    return ht;
}

// Libère proprement toute la structure
void liberer_table_hachage(HashTable_t *ht) {
    if (ht == NULL) return;

    // On passe dans chaque case du tableau
    for (size_t i = 0; i < ht->taille; i++) {
        SymNode_t *courant = ht->tableau[i];
        
        // On détruit la liste chaînée noeud par noeud
        while (courant != NULL) {
            SymNode_t *suivant = courant->suivant;
            free(courant); 
            courant = suivant;
        }
    }

    free(ht->tableau); // On libère le grand tableau
    free(ht);          // On libère la structure elle-même
}

// L'arme secrète : propose une correction pour le mot mal orthographié
char* proposer_correction_symspell(HashTable_t *ht, const char *mot_faux) {
    if (ht == NULL || mot_faux == NULL) return NULL;

    // CAS 1 : L'utilisateur a OUBLIÉ une lettre
    char *resultat = chercher_dans_table(ht, mot_faux);
    if (resultat != NULL) {
        return resultat;
    }

    // CAS 2 : L'utilisateur a AJOUTÉ, REMPLACÉ, ou INVERSÉ une lettre
    char suppressions[MAX_WORD_LEN][MAX_WORD_LEN];
    int nb_suppressions = generer_suppressions(mot_faux, suppressions);

    // On cherche si une de ces suppressions existe dans notre table
    for (int i = 0; i < nb_suppressions; i++) {
        resultat = chercher_dans_table(ht, suppressions[i]);
        if (resultat != NULL) {
            return resultat; 
        }
    }

    // Si on arrive ici, le mot est trop détruit
    return NULL; 
}
