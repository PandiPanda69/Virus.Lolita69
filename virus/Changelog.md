Changelog
=========

Version 3
---------
* Détection de chroot en cherchant le fichier /var/lib/pycentral/pkgremove. Si ce fichier est absent (car il n'a pas été recopié dans le chroot) alors rien n'est fait.
* Furtivité++. On n'écrit plus directement dans /etc/hosts et dans les .pyc à infecter. L'écriture est découpée en plusieurs blocs de taille aléatoire, pour chaque bloc un hardlink dans /tmp avec un nom aléatoire (UUID) et pour cible le fichier voulu est créé, l'écriture se fait dans ce hardlink qui sera supprimé ensuite.
* Changement du nom et de la valeur de la constante qui sert à délimiter le code à copier pour l'infection.
* Ajout de NOP aléatoires dans lolita.final.pyc entre chaque opcode au build.
* La sortie d'erreur est redirigée vers /dev/null pour éviter de leaker des informations si le virus rencontre une erreur non gérée.
* Ajout d'une couche (xor_light) d'obfuscation sans aucune dépendance (à part getattr, __import__ et marshal) pour plus de discrétion. Cette couche est la première visible dans lolita.final.pyc. C'est un XOR avec clé aléatoire stockée après le chiffré (contrairement à la couche xor). C'est aussi la couche qui est rajoutée lors de l'infection pour créer le polymorphisme. Cette couche est obfusquée à la main (pour qu'on ne voit pas qu'on utilise marshal.loads)
* Plus aucune dépendance à base64 (les binaires sont écrits avec des \x[hexa] dans les .py avant compilation, le résultat est le même (stocker du binaire dans un fichier source) mais ça fait une lib de moins).
* Passage au niveau 9 (max) de compression zlib.
* Utilisation d'un minifier lors du build pour réduire un peu la taille du code et supprimer les commentaires.


Version 2
---------
* ajout des layers invAll et inv2
* obfuscation du code injecté dans les pyc avec un XOR et clé aléatoire
* ajout de la fonctionnalité d'auto réplication (un pyc infecté tente d'infecter les autres s'ils ne le sont pas déjà, en ajoutant une nouvelle couche de xor au passage...). Tous les fichiers infectés ont un payload différent (car clé de XOR aléatoire)
* plus de discrétion (suppression de la bannière et de nombreux prints)
* obfuscation des noms des fonctions/variables dans les layers
* infection plus précise, au lieu de /usr/lib/python2.6/*.pyc on se limite à site.pyc et codecs.pyc. Ils sont chargés automatiquement par l'interpréteur Python, c'est donc suffisant et c'est plus discret.
