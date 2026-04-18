#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))


/**
 * Calcule la distance de Jaro-Winkler entre deux chaînes de caractères.
 * * L'algorithme de Jaro-Winkler mesure la similarité entre deux chaînes. 
 * Cette implémentation renvoie la distance (définie ici comme 1.0 - similarité).
 * Plus la valeur est proche de 0.0, plus les mots sont similaires.
 * La méthode calcule les correspondances dans une fenêtre glissante, compte les 
 * transpositions, applique la formule de Jaro, puis le bonus de préfixe de Winkler.
 * * Complexité en temps : O(N *min(M, R)) où N et M sont les longueurs des mots, 
 * et R est la taille de la fenêtre de recherche.
 * Complexité en espace : O(N + M) pour allouer les tableaux de correspondances.
 * * @param s1 Pointeur vers la première chaîne de caractères.
 * @param s2 Pointeur vers la deuxième chaîne de caractères.
 * @return   Un flottant représentant la distance (0.0f pour identique, 1.0f pour différent).
 */
float Distance(const char *s1, const char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);

    // Securite, si un des mots est vide alors on renvoie 1; 
    // Si les mots sont strictement identique alors renvoie 0
    if (len1 == 0 || len2 == 0) return 1.0f;
    if (strcmp(s1, s2) == 0) return 0.0f;

    // Distance de Jaro.
    // Deux lettres sont considérées comme un match si 
    // elles ne sont pas trop éloignées l'une de l'autre.
    // La formule est : (max_len / 2) - 1 (plus le mot est grand, plus la distance entre un meme caractere est autorise)
    int search_range = MAX(len1, len2) / 2 - 1;
    if (search_range < 0) //Securtie
    {
        search_range = 0;
    }

    // 2 tableaux (on s'assure quaucune des 2 ne se "rematch") pour se souvenir des lettres qu'on a déjà "matchées".
    // Taille de 256 car cest largement suffisant pour nimporte quel mot. 
    // (0 = pas matché, 1 = matché)
    int matched_s1[256] = {0};
    int matched_s2[256] = {0};

    int matches = 0;

    //Trouver le nombre de correspondances (m)
    for (int i = 0; i < len1; i++) {
        // On ne cherche pas dans tout le mot 2, juste la zone(search_range) qui est autour du caractere 'i'.
        int start = MAX(0, i - search_range);
        int end = MIN(i + search_range + 1, len2);

        for (int j = start; j < end; j++) {
            // Si la lettre de s2 est libre et que c'est la même lettre que s1
            if (matched_s2[j] == 0 && s1[i] == s2[j]) {
                matched_s1[i] = 1; // On marque la lettre de s1 comme prise
                matched_s2[j] = 1; // On marque la lettre de s2 comme prise
                matches++;
                break; // On passe à la lettre suivante de s1
            }
        }
    }

    // [CORRECTION BUG] S'il n'y a aucune lettre en commun, c'est totalement différent, donc la distance est de 1.0 !
    if (matches == 0) return 1.0f;

    /*
    Que fait le code pour le moment ?
    Ex.
    On compare s1 = Hello et s2 = Halo. (Le mot le plus grand fait 5)
    On trouve que search_range est égale à (5/2)-1 = 1
    matched_s1 = [0, 0, 0, 0, 0]
    matched_s2 = [0, 0, 0, 0]
    Pour i = 0 on a comme start = 0 et end = 2
    j=0 (lettre 'H') : C'est libre ET C'est la meme lettre.
    matched_s1[0]=1 ; matched_s2[0]=1 ; matches=1
    On passe à la lettre suivante

    A la fin du code on matche = 3.
    */

    // On va compter les matchs de lettre qui sont pas dans le meme ordre ! (Ex. le o de Hello et Halo)
    int transpositions = 0;
    int k = 0; // Compteur qui va avancer sur les lettres matchées de s2

    for (int i = 0; i < len1; i++) {
        if (matched_s1[i] == 1) { // On regarde si cette lettre a un match
            // On fait avancer k pour trouver le prochain match de s2
            while (matched_s2[k] == 0) {
                k++;
            }
            // Une fois quon a la position du match de chaquun, on regarde si cest la meme lettre, si les lettres sont differentes alors on augment transpositions
            if (s1[i] != s2[k]) {
                transpositions++;
            }
            k++;
        }
    }

    // Un croisement implique une paire de lettre donc pour notre exemple Hello et Halo transpositions vaut 2, il faut donc le diviser par 2
    float t = transpositions / 2.0f; 
    float m = (float)matches;

    // Formule de Jaro (score sur 3 parties)
    //(m / len1) : Quel pourcentage du 1er mot a réussi à trouver un partenaire ?
    //(m / len2) : Quel pourcentage du 2eme mot a réussi à trouver un partenaire ?
    //((m - t) / m) : Parmi les lettres qui ont matché, quel pourcentage était placé dans le bon ordre ?
    float jaro_score = ((m / len1) + (m / len2) + ((m - t) / m)) / 3.0f;
    //Score de Jaro(Hello et Halo) = 2.35 / 3.0 = 0.783


    //L'amélioration de Winkler : il considere qu'un Homme se trompe rarement sur les premiere lettre donc, si le mot commence avec les memes lettre alors je lui donne plus de points
    int prefix_len = 0;
    int max_prefix = MIN(MIN(len1, len2), 4); // Winkler limite le bonus à 4 lettres max

    for (int i = 0; i < max_prefix; i++) {
        if (s1[i] == s2[i]) {
            prefix_len++;
        } else {
            break; // Des qu'une lettre diffère, le préfixe commun est fini
        }
    }

    // Le poids standard inventé par Winkler est de 0.1
    float jaro_winkler_score = jaro_score + (prefix_len * 0.1f * (1.0f - jaro_score));

    return 1.0f - jaro_winkler_score; // On retourne la 1 - similarite pour que ça corresponde aux autres codes 
}