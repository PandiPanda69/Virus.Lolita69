#!/bin/sh

export VIRUS_HOME="`pwd`"

echo "Exec before infect:"
echo -n "" > ./tmp/hosts
(cd targets; rm *.pyc; python -c "import hello,hello2"; (cd targets; rm *.pyc; python -c "import hello_bis, hello_bis2"))
echo "Executed."

echo "\nBuilding..."
python meta_builder.py
echo "Build complete."

echo "\nExecuting lolita..."
python lolita.final.pyc
echo "Executed."

echo "\nExec hello_bis, should still be clean:"
(cd targets/targets; python -c "import hello_bis"; echo "Python interpreter returned: "$?" (0 is a cool value!)")
echo "Executed."

echo "\nFake hosts file, should be empty:"
cat ./tmp/hosts
echo "EOF"

echo "\nExec hello, should infect hello_bis:"
(
    cd targets; python -c "import hello"; echo "Python interpreter returned: "$?" (0 is a cool value!)"
)
echo "Executed."

echo "\nFake hosts file, should be modified:"
cat ./tmp/hosts
echo "EOF"

echo "\nExec hello_bis, should be infected now:"
(
    cd targets/targets; python -c "import hello_bis"; echo "Python interpreter returned: "$?" (0 is a cool value!)"
)
echo "Executed."
