#!/bin/bash

# CI uses clang-format-13 version
if [ -n "$(command -v clang-format-13)" ]; then
    format_command="clang-format-13"
else
    format_command="clang-format"
fi
sources="$(find . -type f \( -name '*.h' -or -name '*.c'  -or -name '*.cpp' \) )"
num_errors=0

for source in $sources
do
    echo "Checking format of $source"
    diff "$source" <($format_command "$source")
    if [ "$?" != "0" ]; then
        ((num_errors++))
    fi
done

echo "======================="

if [ "$num_errors" -eq 0 ]; then
    echo "All files are formatted correctly"
elif [ "$num_errors" -eq 1 ]; then
    2>&1 echo "$num_errors file is formatted incorrectly"
else
    2>&1 echo "$num_errors files are formatted incorrectly"
fi

exit $num_errors
