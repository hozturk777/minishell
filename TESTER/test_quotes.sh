#!/bin/bash

# ============================================================================
# MINISHELL QUOTE CONCATENATION TESTER
# ============================================================================
# Test: Adjacent quote/word token concatenation
# Author: hasivaci & huozturk
# Date: 2025-09-21
# Description: Comprehensive tester for quote concatenation behavior
# ============================================================================

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Test configuration
MINISHELL_PATH="../minishell"
TIMEOUT_DURATION=3

# ============================================================================
# HELPER FUNCTIONS
# ============================================================================

print_header() {
    echo -e "${CYAN}============================================================================${NC}"
    echo -e "${CYAN}                    MINISHELL QUOTE CONCATENATION TESTER                   ${NC}"
    echo -e "${CYAN}============================================================================${NC}"
    echo -e "${BLUE}Testing adjacent quote/word token concatenation behavior${NC}"
    echo -e "${BLUE}Comparing minishell output with bash behavior${NC}"
    echo -e "${CYAN}============================================================================${NC}"
    echo
}

run_test() {
    local test_name="$1"
    local command="$2"
    local description="$3"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo -e "${YELLOW}[TEST $TOTAL_TESTS]${NC} $test_name"
    echo -e "${BLUE}Command:${NC} $command"
    echo -e "${PURPLE}Description:${NC} $description"
    
    # Run bash
    bash_output=$(timeout $TIMEOUT_DURATION bash -c "$command" 2>/dev/null)
    bash_exit=$?
    
    # Run minishell
    # Get minishell output by line number (command output is typically line 23)
    minishell_raw=$(echo -e "$command\nexit" | timeout $TIMEOUT_DURATION $MINISHELL_PATH 2>/dev/null)
    
    # Find the line with our command output (line after "minishell$")
    minishell_output=$(echo "$minishell_raw" | grep -A 1 "minishell\$ $command" | tail -1 | sed 's/^[[:space:]]*//')
    
    # If that didn't work, try alternative approach
    if [ -z "$minishell_output" ] || echo "$minishell_output" | grep -q "minishell\$\|DEBUG\|╔\|║\|╚"; then
        minishell_output=$(echo "$minishell_raw" | sed -n '23p' | sed 's/^[[:space:]]*//')
    fi
    minishell_exit=$?
    
    echo -e "${CYAN}Bash output:${NC} '$bash_output'"
    echo -e "${CYAN}Minishell output:${NC} '$minishell_output'"
    
    # Compare outputs
    if [ "$bash_output" = "$minishell_output" ]; then
        echo -e "${GREEN}✅ PASS${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}❌ FAIL${NC}"
        echo -e "${RED}Expected:${NC} '$bash_output'"
        echo -e "${RED}Got:${NC} '$minishell_output'"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    echo "---"
}

print_summary() {
    echo
    echo -e "${CYAN}============================================================================${NC}"
    echo -e "${CYAN}                              TEST SUMMARY                                 ${NC}"
    echo -e "${CYAN}============================================================================${NC}"
    echo -e "${BLUE}Total Tests:${NC} $TOTAL_TESTS"
    echo -e "${GREEN}Passed:${NC} $PASSED_TESTS"
    echo -e "${RED}Failed:${NC} $FAILED_TESTS"
    
    if [ $FAILED_TESTS -eq 0 ]; then
        echo -e "${GREEN}🎉 ALL TESTS PASSED! Quote concatenation working perfectly! 🎉${NC}"
    else
        echo -e "${RED}⚠️  Some tests failed. Check concatenation logic.${NC}"
    fi
    echo -e "${CYAN}============================================================================${NC}"
}

# ============================================================================
# MAIN TEST SUITE
# ============================================================================

print_header

# Check if minishell exists
if [ ! -f "$MINISHELL_PATH" ]; then
    echo -e "${RED}Error: $MINISHELL_PATH not found. Please compile minishell first.${NC}"
    echo "Run: make"
    exit 1
fi

# ============================================================================
# BASIC ADJACENT QUOTE TESTS
# ============================================================================

echo -e "${YELLOW}=== BASIC ADJACENT QUOTE TESTS ===${NC}"

run_test "Single Quote + Word" \
    "echo 'hello'world" \
    "Single quoted string followed immediately by word"

run_test "Double Quote + Word" \
    "echo \"hello\"world" \
    "Double quoted string followed immediately by word"

run_test "Word + Single Quote" \
    "echo hello'world'" \
    "Word followed immediately by single quoted string"

run_test "Word + Double Quote" \
    "echo hello\"world\"" \
    "Word followed immediately by double quoted string"

# ============================================================================
# COMPLEX ADJACENT PATTERNS
# ============================================================================

echo -e "${YELLOW}=== COMPLEX ADJACENT PATTERNS ===${NC}"

run_test "Quote + Quote" \
    "echo 'hello'\"world\"" \
    "Single quote followed by double quote"

run_test "Multiple Adjacent" \
    "echo 'a'b'c'" \
    "Multiple adjacent quote-word-quote pattern"

run_test "Long Adjacent Chain" \
    "echo 'hello'world'test'end" \
    "Long chain of adjacent quotes and words"

run_test "Empty Quote + Word" \
    "echo ''world" \
    "Empty single quote followed by word"

run_test "Word + Empty Quote" \
    "echo hello''" \
    "Word followed by empty single quote"

# ============================================================================
# VARIABLE EXPANSION TESTS
# ============================================================================

echo -e "${YELLOW}=== VARIABLE EXPANSION WITH CONCATENATION ===${NC}"

run_test "Variable in Quote + Word" \
    "echo '\$HOME'test" \
    "Variable in single quote (no expansion) + word"

run_test "Variable in Double Quote + Word" \
    "echo \"\$USER\"test" \
    "Variable in double quote (expansion) + word"

run_test "Original Problem Case" \
    "echo '\$USER\"test\"'a''" \
    "Original problematic case that was fixed"

run_test "Complex Variable Pattern" \
    "echo '\$HOME':\$USER'@'host" \
    "Complex pattern with variables and quotes"

# ============================================================================
# EDGE CASES
# ============================================================================

echo -e "${YELLOW}=== EDGE CASES ===${NC}"

run_test "Special Characters" \
    "echo 'hello@#$%'world" \
    "Special characters in quotes + word"

run_test "Numbers and Quotes" \
    "echo '123'abc'456'" \
    "Numbers mixed with quotes and words"

run_test "Spaces in Quotes" \
    "echo 'hello world'test" \
    "Quotes containing spaces + word"

run_test "Escape Characters" \
    "echo 'hello\nworld'test" \
    "Escape characters in quotes + word"

# ============================================================================
# NON-ADJACENT TESTS (Should NOT concatenate)
# ============================================================================

echo -e "${YELLOW}=== NON-ADJACENT TESTS (Should NOT concatenate) ===${NC}"

run_test "Quote Space Word" \
    "echo 'hello' world" \
    "Quote and word separated by space (should NOT concatenate)"

run_test "Word Space Quote" \
    "echo hello 'world'" \
    "Word and quote separated by space (should NOT concatenate)"

run_test "Multiple Spaces" \
    "echo 'hello'   world" \
    "Quote and word separated by multiple spaces (should NOT concatenate)"

# ============================================================================
# COMMAND ARGUMENT TESTS
# ============================================================================

echo -e "${YELLOW}=== COMMAND ARGUMENT TESTS ===${NC}"

run_test "Command with Adjacent Args" \
    "echo 'arg1'ext 'arg2'ext" \
    "Multiple arguments with adjacent concatenation"

run_test "Mixed Argument Types" \
    "echo normal 'quoted'word another" \
    "Mix of normal args and concatenated args"

# ============================================================================
# STRESS TESTS
# ============================================================================

echo -e "${YELLOW}=== STRESS TESTS ===${NC}"

run_test "Very Long Concatenation" \
    "echo 'a'b'c'd'e'f'g'h'i'j'k'l'm'n'o'p" \
    "Very long chain of adjacent concatenations"

run_test "Deep Nesting Pattern" \
    "echo 'outer'middle\"inner\"'end'" \
    "Complex nesting of different quote types"

run_test "Unicode in Quotes" \
    "echo 'héllo'wörld" \
    "Unicode characters in adjacent concatenation"

# ============================================================================
# COMPATIBILITY TESTS
# ============================================================================

echo -e "${YELLOW}=== BASH COMPATIBILITY TESTS ===${NC}"

run_test "Bash Standard Pattern 1" \
    "echo foo'bar'baz" \
    "Standard bash concatenation pattern"

run_test "Bash Standard Pattern 2" \
    "echo 'prefix'_suffix" \
    "Prefix with underscore suffix"

run_test "Bash Standard Pattern 3" \
    "echo file'.'ext" \
    "Filename with quoted dot extension"

run_test "Path Concatenation" \
    "echo '/home/'\$USER'/test'" \
    "Path construction with variable"

# ============================================================================
# FINAL SUMMARY
# ============================================================================

print_summary

# Exit with appropriate code
if [ $FAILED_TESTS -eq 0 ]; then
    exit 0
else
    exit 1
fi