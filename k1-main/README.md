## Contexte

Le projet implémente un correcteur orthographique capable de corriger un texte automatiquement à partir du dictionnaire correspondant à la langue détectée par le programme lui-même. 

# Structure du projet

Les fonctions nécessaires à la détection des mots mal orthographiés et de la langue dans laquelle est écrit le texte donné par l'utilisateur se trouvent dans le fichier "detection.c". Le fichier "correction.c" contient les méthodes utilisées pour proposer une correction du texte analysé. Les headers associés sont respectivement "detection.h" et "correction.h".
Dans le dossier "src", on trouve aussi différents programmes "Distance-{nom}.c" qui sont des implémentations de différentes distances valables pour comparer des Strings, utiles lors de la détection de la langue du texte fourni. Le but est de les comparer pour voir laquelle renvoie les résultats les plus cohérents. Ces fichiers sont tous accompagnés d'un header sous le format "Distance-{nom}.h".
Le fichier "vector.c" avec son header "vector.h" contient une implémentation d'un vecteur dynamique (équivalent d'une ArrayList Java en C) d'éléments de type "int". Lors de la détection des fautes, cette structure de données permet de rendre le stockage de la position des mots mal orthographiés efficace, notamment grâce à la fonction "vector_add()". 

Le dossier "optimizations" contient des fichiers encore non fonctionnels représentant des pistes d'améliorations que nous pourrions apporter à notre projet pour rendre le correcteur encore plus efficace. 

Les tests liés aux métriques sur les Strings utilisées dans le projet sont dans le dossier "tests" sous le format "test-{nom}.c".

# Execution

Pour éxécuter le programme, on peut utiliser le Makefile en entrant la commande "make" et en éxécutant via 
"./spellchecker --dicts <dict_path> --input <input_path> | iconv -f ISO-8859-1". Par défaut, si aucun fichier n'est donné en argument, un texte standard repris dans le fichier "input.h" est corrigé sur base du dictionnaire "dict.h". 
Les tests peuvent être éxécutés en entrant la commande "make test" dans le terminal.

## Format de sortie

Le résultat de la correction est renvoyé sous la forme :
"[{numéro de ligne}], {ligne originale avec les mots corrigés affichés en vert}"
