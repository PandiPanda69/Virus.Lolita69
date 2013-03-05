#!/bin/sh

echo "Suprimer hardlink et clean fichier /tmp/hosts"
rm /tmp/hardlink
echo "127.0.0.1 localhost" > /tmp/hosts
cat /tmp/hosts

echo "\nExecuting hardlink_write.py"
strace -e "write" python hardlink_write.py
#python hardlink_write.py
echo "Executed"

echo "\nRésultat :"
cat /tmp/hosts


echo "\nBye."
