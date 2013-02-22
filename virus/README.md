Virus.Lolita69
==============
Equipe :
- BAILLY Vincent
- FRADET Rémi
- MERIOT Sébastien
- NOTIN Clément


Virus source code.

Distribution et exécution
-------------------------
Un fichier lolita_vN.final.pyc (avec N = numéro de version)
Exécution : $ python lolita_vN.final.pyc

A l'exécution, le virus va infecter /usr/lib/python2.6/{site,codecs}.pyc puis va quitter. Le code malicieux est donc présent dans ces .pyc de la distrib python, mais il n'est pas exécuté à ce moment là. En revanche dès que l'interpréteur python sera lancé le premier .pyc chargé va exécuter pour de vrai l'action malicieuse.

Comment générer et tester lolita69 ?
--------------------------
Dans le dossier du virus, lancer le script meta_builder.py et alors après plusieurs étapes qui durent quelques secondes le fichier lolita.final.pyc sera disponible. Le build doit se faire sur la même version de Python que sur la machine cible (un .pyc n'est pas portable d'une version à l'autre).
Pour tester, des faux-scripts cibles sont prêts dans le dossier targets/, le test se lance par le script test.sh. Le test nettoie l'environnement d'exécution, montre que les fichiers cibles sont sains puis exécute une infection en deux temps : lolita infecte targets/hello.pyc et targets/hello2.pyc. On peut vérifier que le payload (la grosse string base64) est bien obfusqué et différent pourtant provenant du même infecteur. Ensuite en lançant targets/hello.pyc, cela provoque l'infection de targets/targets/hello_bis.pyc et hello_bis2.pyc. Idem on peut vérifier que les strings base64 sont différents.

A noter : le virus regarde l'hostname de la machine pour décider d'où chercher les pyc à infecter, et de l'action malicieuse à entreprendre. C'est une sécurité pour éviter d'endommager nos postes de dev... Le virus peut donc être tranquillement testé sur sa propre machine, tant que l'hostname est différent de OT-Wargame. Il cible alors ./targets/*.pyc et affiche un message explicite ("..INFECTED...") sans toucher au fichier /etc/hosts.


Action malicieuse
-----------------
ajouter "127.0.0.1\tgoogle.fr" dans /etc/hosts. C'est suffisamment louche, sans endommager profondément la VM. Si la ligne est déjà présente rien ne se passe (c'est indispensable car chaque .pyc chargé par l'interpréteur est susceptible d'infecter donc le résultat serait de très nombreuses lignes pas très discrètes...)

Obfuscation et polymorphisme
----------------------------
Le payload utile est compilé (simple parsing et génération de bytecode .pyc), puis compressé (zlib) et encodé en base64 pour être inclus facilement dans un .py. L'objectif est de chiffrer le payload, et d'inclure cette version chiffrée dans un fichier source capable de la déchiffrer, puis de compiler cette source et de boucler de cette manière. Ainsi on peut définir des algos de chiffrement et un template pour déchiffrer et les exécuter en boucle dans un builder.
A l'exécution, la 1ère couche déchiffre son payload et l'exécute, c'est à nouveau un code de déchiffrage pour la couche 2 et ainsi de suite jusqu'à arriver au payload utile. Le déchiffrement s'enchaîne de couche en couche automatiquement.

Le code injecté dans les .pyc est lui aussi obfusqué. Lors de l'infection une couche de XOR est rajoutée au code déjà obfusqué (lolita.final.pyc se lit lui-même et ajoute la couche avec une clé aléatoire). Ainsi le code injecté est différent pour chaque fichier infecté !

Algos de "chiffrement" définis :
- XOR
- brainfuck
- invAll
- inv2

Le XOR génère une clé (1 octet) aléatoire, et renvoie [clé][chiffré_clé(input)]. Le déchiffrement suit la même opération grâce à la clé stockée avant les données.
Le brainfuck convertit l'entrée en code brainfuck (cf wikipedia) valide, qui sera exécuté par un interpréteur brainfuck qui est la fonction de déchiffrement. Nous avons de plus obfusqué manuellement le générateur de brainfuck et l'interpréteur.
Le builder permet d'empiler facilement N couches de chiffrement, de les mélanger etc. Ainsi on obtient un virus difficilement reversable à la main (même si rien n'est secret (ie la clé du XOR stockée devant les données) c'est suffisamment fastidieux pour être décourageant) et qui a la particularité d'être polymorphique (différent à chaque génération, grâce à la clé de xor aléatoire).
La seule partie qui reste constante est le petit bout de code qui sert à déchiffrer la première couche et à amorcer le reste. Comme c'est un code très court, anodin et légitime à première vue (utiliser base64, zlib et xor n'est pas significatif d'un virus...) l'antivirus ne peut pas se baser dessus.
invAll inverse son input ("ABCD" devient "DCBA")
inv2 inverse 2 à 2 les octets de l'input ("ABCDEFGH" devient "BADCFEHG")

Auto-réplication
----------------
Que serait un virus sans auto-réplication ? Le code injecté dans les .pyc est capable de ré-infecter les autres .pyc s'ils ont été recompilés ou nettoyés. Quand il ré-infecte il construit le payload à partir de lui-même et en rajoutant une couche de XOR avec clé aléatoire, ainsi un pyc infectant et l'infecté partagent le même code utile mais sous une forme différente ! (cf § polymorphisme)

Protection contre l'analyse dynamique ?
---------------------------------------
Ce mécanisme est très efficace pour contrer l'analyse statique, mais rien n'est implémenté encore contre une analyse dynamique.

