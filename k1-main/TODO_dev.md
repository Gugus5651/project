Voici un readme sur le projet d'info en c.



---------------------------------------------------------------------------------------
bktree : 
essaie d'implémenter bk tree.
INCOMPLET
----------------------------------------------------------------------------------------
DamarauLevensteinDistance :
Va implémenter la distance de damareau levenstein.
Elle compte le nombre de modification qui permet de retrouver un mot à partir d'un mot. changer une lettre, l'enlever, en rajouter et en transposer 2 coute 1.
----------------------------------------------------------------------------------------
MultiDicFind : 
document qui essaye de résoudre le problème de choix de dico. Pour chaque mot, il va regarder dans chaque dictionnaire les mots 
les plus proches(leur nombre dépends de MAX_WORDS).
Ensuiste il va choisir les meilleurs dico qui mach le plus (leur nombre dépends de MAX_DICTS)(tjs pour chaque mots) et va les retenirs.

void freeDictMatch(DictMatch* dico)
free la mémoire utiliser par un DictMatch.

DictMatch* find_best_correction_multi(char* s, Dictionary_t* dict)
Pour un dictionnaire en particulier, va trouver les MAX_WORDS les plus proche de s. Attention, tout les mots qui revienne ont la même distance.
En gros on ne prends que les mots égaux à la première place.


LanguageMatch* find_best_language_one_word(char*s, Dictionary_t* dict[])
pour un mot s, va chosir les 3 meilleurs dictionnaires contenant des mots les plus proches
INCOMPLET
-------------------------------------------------------------------------------------------
