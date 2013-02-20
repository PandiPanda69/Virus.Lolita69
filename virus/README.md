Virus.Lolita69
==============
Equipe :
- BAILLY Vincent
- FRADET Rémi
- MERIOT Sébastien
- NOTIN Clément


Virus source code.

Distribution du virus : un fichier lolita_vN.final.pyc (avec N = numéro de version)
Exécution : $ python lolita_vN.final.pyc

A l'exécution, le virus va infecter /usr/lib/python2.6/*.pyc puis va se quitter prétextant une erreur. Le code malicieux est donc présent dans tous les .pyc de la distrib python, mais il n'est pas exécuté à ce moment là. En revanche dès que l'interpréteur python sera lancé le premier .pyc chargé va exécuter l'action malicieuse.

Action malicieuse : ajouter "127.0.0.1	google.fr" dans /etc/hosts. C'est suffisamment louche, sans endommager profondément la VM. Si la ligne est déjà présente rien ne se passe (c'est indispensable car chaque .pyc chargé par l'interpréteur est susceptible d'infecter donc le résultat serait de très nombreuses lignes pas très discrètes...)


Protection : le payload utile est compilé (simple parsing et génération de bytecode .pyc), puis compressé (zlib) et encodé en base64 pour être inclus facilement dans un .py. L'objectif est de chiffrer le payload, et d'inclure cette version chiffrée dans un fichier source capable de la déchiffrer, puis de compiler cette source et de boucler de cette manière. Ainsi on peut définir des algos de chiffrement et un template pour déchiffrer et les exécuter en boucle dans un builder.
A l'exécution, la 1ère couche déchiffre son payload et l'exécute, c'est à nouveau un code de déchiffrage pour la couche 2 et ainsi de suite jusqu'à arriver au payload utile. Le déchiffrement s'enchaîne de couche en couche automatiquement.

2 algos de "chiffrement" sont définis :
- XOR
- brainfuck

Le XOR génère une clé (1 octet) aléatoire, et renvoie [clé][chiffré_clé(input)]. Le déchiffrement suit la même opération grâce à la clé stockée avant les données.
Le brainfuck convertit l'entrée en code brainfuck (cf wikipedia) valide, qui sera exécuté par un interpréteur brainfuck qui est la fonction de déchiffrement. Nous avons de plus obfusqué manuellement le générateur de brainfuck et l'interpréteur.

Le builder permet d'empiler facilement N couches de chiffrement, de les mélanger etc. Ainsi on obtient un virus difficilement reversable à la main (même si rien n'est secret (ie la clé du XOR stockée devant les données) c'est suffisamment fastidieux pour être décourageant) et qui a la particularité d'être polymorphique (différent à chaque génération, grâce à la clé de xor aléatoire).
La seule partie qui reste constante est le petit bout de code qui sert à déchiffrer la première couche et à amorcer le reste. Comme c'est un code très court, anodin et légitime à première vue (utiliser base64, zlib et xor n'est pas significatif d'un virus...) l'antivirus ne peut pas se baser dessus.


Ce mécanisme est très efficace pour contrer l'analyse statique, mais rien n'est implémenté encore contre une analyse dynamique.

A noter : le virus regarde l'hostname de la machine pour décider d'où chercher les pyc à infecter, et de l'action malicieuse à entreprendre. C'est une sécurité pour éviter d'endommager nos postes de dev...