#!/bin/bash

# Export Test Script for Minishell
# Tests various export command scenarios

echo "=== MINISHELL EXPORT TEST SUITE ==="
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

MINISHELL_PATH="./minishell"
TEMP_DIR="/tmp/minishell_test"
mkdir -p $TEMP_DIR

# Test counter
TEST_COUNT=0
PASS_COUNT=0
FAIL_COUNT=0

# Function to run test
run_test() {
    local test_name="$1"
    local minishell_cmd="$2"
    local expected_behavior="$3"
    
    TEST_COUNT=$((TEST_COUNT + 1))
    echo -e "${YELLOW}Test $TEST_COUNT: $test_name${NC}"
    echo "Command: $minishell_cmd"
    
    # Create test input
    echo "$minishell_cmd" > $TEMP_DIR/input.txt
    echo "exit" >> $TEMP_DIR/input.txt
    
    # Run minishell
    $MINISHELL_PATH < $TEMP_DIR/input.txt > $TEMP_DIR/minishell_output.txt 2>&1
    
    # Check if minishell ran without crashing
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ PASS${NC} - No crash"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo -e "${RED}✗ FAIL${NC} - Crashed or error"
        FAIL_COUNT=$((FAIL_COUNT + 1))
        echo "Output:"
        cat $TEMP_DIR/minishell_output.txt
    fi
    echo "Expected: $expected_behavior"
    echo "---"
}

# Function to run comparison test with bash
run_comparison_test() {
    local test_name="$1"
    local cmd="$2"
    local check_cmd="$3"
    
    TEST_COUNT=$((TEST_COUNT + 1))
    echo -e "${YELLOW}Test $TEST_COUNT: $test_name${NC}"
    echo "Setup: $cmd"
    echo "Check: $check_cmd"
    
    # Test with bash
    bash -c "$cmd; $check_cmd" > $TEMP_DIR/bash_output.txt 2>&1
    
    # Test with minishell
    echo "$cmd" > $TEMP_DIR/input.txt
    echo "$check_cmd" >> $TEMP_DIR/input.txt
    echo "exit" >> $TEMP_DIR/input.txt
    
    $MINISHELL_PATH < $TEMP_DIR/input.txt 2>/dev/null > $TEMP_DIR/minishell_full_output.txt
    
    # Extract only the echo result line
    MINISHELL_RESULT=$(grep "^test=" $TEMP_DIR/minishell_full_output.txt)
    
    # Compare outputs
    BASH_RESULT=$(cat $TEMP_DIR/bash_output.txt)
    
    if [ "$BASH_RESULT" = "$MINISHELL_RESULT" ]; then
        echo -e "${GREEN}✓ PASS${NC} - Output matches bash"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo -e "${RED}✗ FAIL${NC} - Output differs from bash"
        FAIL_COUNT=$((FAIL_COUNT + 1))
        echo "Bash output: '$BASH_RESULT'"
        echo "Minishell output: '$MINISHELL_RESULT'"
    fi
    echo "---"
}

echo "Starting tests..."
echo

# Test 1: Basic export with quotes
run_test "Basic export with double quotes" \
         'export test="hello world"' \
         "Should set test=hello world"

# Test 2: Export with single quotes  
run_test "Export with single quotes" \
         "export test='hello world'" \
         "Should set test=hello world"

# Test 3: Export without quotes
run_test "Export without quotes" \
         'export test=hello' \
         "Should set test=hello"

# Test 4: Export with empty value
run_test "Export with empty value" \
         'export test=""' \
         "Should set test to empty string"

# Test 5: Export with spaces and special chars
run_test "Export with spaces and special chars" \
         'export test="hello world! @#$%"' \
         "Should set test=hello world! @#$%"

# Test 6: Multiple exports
run_test "Multiple exports" \
         'export test1="value1"; export test2="value2"' \
         "Should set both variables"

# Test 7: Export with equals in value
run_test "Export with equals in value" \
         'export test="key=value"' \
         "Should set test=key=value"

# Test 8: Export with nested quotes
run_test "Export with nested quotes" \
         'export test="He said '\''hello'\''"' \
         "Should handle nested quotes"

# Test 9: Export without value (declare only)
run_test "Export without value" \
         'export test' \
         "Should declare variable without value"

# Test 10: Export with variable substitution
run_test "Export with variable substitution" \
         'export USER="john"; export test="Hello $USER"' \
         "Should substitute USER variable"

# Comparison tests with bash
echo
echo "=== COMPARISON TESTS WITH BASH ==="
echo

run_comparison_test "Basic quoted export verification" \
                   'export test="ho selam"' \
                   'echo "test=$test"'

run_comparison_test "Single quote export verification" \
                   "export test='ho selam'" \
                   'echo "test=$test"'

run_comparison_test "No quotes export verification" \
                   'export test=hoselam' \
                   'echo "test=$test"'

run_comparison_test "Empty value export verification" \
                   'export test=""' \
                   'echo "test=$test"'

run_comparison_test "Special chars export verification" \
                   'export test="hello@world#2024"' \
                   'echo "test=$test"'

# Summary
echo
echo "=== TEST SUMMARY ==="
echo "Total tests: $TEST_COUNT"
echo -e "${GREEN}Passed: $PASS_COUNT${NC}"
echo -e "${RED}Failed: $FAIL_COUNT${NC}"

if [ $FAIL_COUNT -eq 0 ]; then
    echo -e "${GREEN}All tests passed! 🎉${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed! ❌${NC}"
    exit 1
fi

# Cleanup
rm -rf $TEMP_DIR