#!/bin/sh

echo "\nExec before infect:"
(cd targets; rm *.pyc; python -c "import hello"; (cd targets; rm *.pyc; python -c "import hello_bis"))
echo "Executed."

echo "Building..."
python meta_builder.py
python -c "import builder_generated"
python builder_generated.pyc
echo "Build complete."

echo "\nExec hello_bis, should be clean:"
(cd targets/targets; python -c "import hello_bis"; echo "Python interpreter returned: "$?" (0 is a cool value!)")
echo "Executed."

echo "\nExec hello, should infect hello_bis:"
(cd targets; python -c "import hello"; echo "Python interpreter returned: "$?" (0 is a cool value!)")
echo "Executed."

echo "\nExec hello_bis, should be infected now:"
(cd targets/targets; python -c "import hello_bis"; echo "Python interpreter returned: "$?" (0 is a cool value!)")
echo "Executed."
