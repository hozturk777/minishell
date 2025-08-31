#!/bin/bash

# Minishell Variable Expansion Test Script

echo "=== Variable Expansion Test ==="
echo "Testing variable expansion functionality..."

# Test 1: HOME variable expansion
echo "echo $HOME" | timeout 3 ./minishell

# Test 2: USER variable expansion  
echo "echo $USER" | timeout 3 ./minishell

# Test 3: Exit status expansion
echo "echo \$?" | timeout 3 ./minishell

# Test 4: Process ID expansion
echo "echo \$\$" | timeout 3 ./minishell

# Test 5: Mixed expansion
echo "echo My home is $HOME and I am $USER" | timeout 3 ./minishell

# Test 6: Quoted expansion (double quotes)
echo "echo \"My home: $HOME\"" | timeout 3 ./minishell

# Test 7: Single quotes (should not expand)
echo "echo 'My home: $HOME'" | timeout 3 ./minishell

echo "=== Test completed ==="
