#!/bin/sh

echo "\nExec before infect:"
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

echo "\nExec hello, should infect hello_bis:"
(
    cd targets; python -c "import hello"; echo "Python interpreter returned: "$?" (0 is a cool value!)"
)
echo "Executed."

echo "\nExec hello_bis, should be infected now:"
(
    cd targets/targets; python -c "import hello_bis"; echo "Python interpreter returned: "$?" (0 is a cool value!)"
)
echo "Executed."
