Changelog
=========

Version 2
---------
* ajout des layers invAll et inv2
* obfuscation du code injecté dans les pyc avec un XOR et clé aléatoire
* ajout de la fonctionnalité d'auto réplication (un pyc infecté tente d'infecter les autres s'ils ne le sont pas déjà, en ajoutant une nouvelle couche de xor au passage...). Tous les fichiers infectés ont un payload différent (car clé de XOR aléatoire)
* plus de discrétion (suppression de la bannière et de nombreux prints)
* obfuscation des noms des fonctions/variables dans les layers
* infection plus précise, au lieu de /usr/lib/python2.6/*.pyc on se limite à site.pyc et codecs.pyc. Ils sont chargés automatiquement par l'interpréteur Python, c'est donc suffisant et c'est plus discret.
