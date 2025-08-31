#!/bin/bash

echo "=== Lexer Test Script ==="
echo "Testing various minishell scenarios"

# Test komutları
test_commands=(
    "echo hello"
    "ls | grep test"  
    'echo "hello world"'
    "cat > output.txt"
    "echo hello >> file.txt"
    "cat < input.txt"
    "cat << EOF"
    'echo '\''single quotes'\'''
    "echo hello | grep h | cat"
    "   echo   hello   world   "
)

for cmd in "${test_commands[@]}"; do
    echo ""
    echo "Testing: $cmd"
    echo "$cmd" | timeout 2 ./minishell
done

echo ""
echo "=== Test Complete ==="
