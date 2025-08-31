# 42 Ecole Minishell Test Cases 🐚

Bu dosya 42 School minishell projesinin test case'lerini **basitten zora** doğru sıralayarak kapsamlı bir test rehberi sunar.

## İçindekiler
1. [Lexer Test Cases (Mevcut)](#lexer-test-cases-mevcut)
2. [Basit Shell Komutları](#basit-shell-komutları)
3. [Redirection Tests](#redirection-tests)
4. [Pipe Tests](#pipe-tests)
5. [Quote Tests](#quote-tests)
6. [Environment Variables](#environment-variables)
7. [Built-in Commands](#built-in-commands)
8. [Advanced Features](#advanced-features)
9. [Error Handling](#error-handling)
10. [Edge Cases](#edge-cases)

---

## Lexer Test Cases (Mevcut)

**Şu anda sadece lexer çalışıyor. Bu komutları test edebilirsiniz:**

### 🟢 Level 1 - Basit Token'lar
```bash
# Test 1: Tek kelime
echo
ls
pwd

# Test 2: İki kelime
echo hello
ls -la
cat file.txt

# Test 3: Basit pipe
ls | grep
echo hello | cat
```

### 🟢 Level 2 - Operators
```bash
# Pipe
echo hello | grep hello

# Output redirect
echo hello > file.txt
ls > output.txt

# Append
echo world >> file.txt

# Input redirect
cat < file.txt

# Heredoc
cat << EOF
```

### 🟢 Level 3 - Quotes
```bash
# Double quotes
echo "hello world"
echo "test with spaces"

# Single quotes
echo 'single quotes'
echo 'no expansion $USER'

# Mixed quotes
echo "double" 'single' normal
```

### 🟢 Level 4 - Complex Lexer Tests
```bash
# Multiple spaces
echo    hello    world
ls     |     grep     test

# Complex commands
echo "hello world" | grep hello > output.txt
cat < input.txt | grep test >> result.txt
```

---

## Basit Shell Komutları

**Parser ve executor ekledikten sonra test edilecek:**

### 🟡 Level 1 - Temel Komutlar
```bash
# Sistem komutları
ls
pwd
whoami
date
uname

# Echo variations
echo
echo hello
echo hello world
echo -n hello
echo -n "no newline"
```

### 🟡 Level 2 - Dosya İşlemleri
```bash
# Dosya listeleme
ls
ls -l
ls -la
ls -a

# Dosya okuma
cat file.txt
less file.txt
head file.txt
tail file.txt

# Dosya oluşturma
touch test.txt
mkdir testdir
```

---

## Redirection Tests

### 🟡 Level 1 - Output Redirection
```bash
# Basit output
echo hello > output.txt
ls > files.txt
pwd > current_dir.txt

# Append
echo hello >> log.txt
date >> log.txt
```

### 🟡 Level 2 - Input Redirection
```bash
# Input redirect
cat < input.txt
grep hello < data.txt
sort < unsorted.txt
```

### 🟡 Level 3 - Heredoc
```bash
# Basit heredoc
cat << EOF
Hello World
This is a test
EOF

# Heredoc with commands
grep hello << END
hello world
test hello
END
```

### 🔴 Level 4 - Complex Redirections
```bash
# Multiple redirections
cat < input.txt > output.txt
echo hello > file1.txt > file2.txt  # Last one wins
ls > /dev/null 2>&1                # Error redirection
```

---

## Pipe Tests

### 🟡 Level 1 - Basit Pipes
```bash
# İki komut
ls | grep test
echo hello | cat
date | cat

# Üç komut
ls | grep .txt | cat
echo hello | grep h | cat
```

### 🟡 Level 2 - Pipe + Redirection
```bash
# Pipe ile output
ls | grep test > result.txt
echo hello | cat > output.txt

# Pipe ile input
cat < input.txt | grep hello
```

### 🔴 Level 3 - Complex Pipes
```bash
# Uzun pipe chains
ls -la | grep .txt | sort | head -5
ps aux | grep bash | awk '{print $2}' | head -1

# Pipe + multiple redirections
cat < input.txt | grep hello | sort > sorted.txt
```

---

## Quote Tests

### 🟡 Level 1 - Basit Quotes
```bash
# Double quotes
echo "hello world"
echo "test 123"
ls "file with spaces.txt"

# Single quotes
echo 'single quotes'
echo 'no $expansion here'
```

### 🟡 Level 2 - Quote Combinations
```bash
# Mixed quotes
echo "double" 'single' normal
echo 'single' "double" word

# Empty quotes
echo ""
echo ''
echo "" ""
```

### 🔴 Level 3 - Advanced Quote Handling
```bash
# Quotes with special chars
echo "pipe | inside quotes"
echo "redirect > inside quotes"
echo 'single quotes with "double" inside'

# Escaped quotes
echo "hello \"world\""
echo 'can'\''t escape in single quotes'
```

---

## Environment Variables

### 🟡 Level 1 - Basit Variables
```bash
# Built-in variables
echo $USER
echo $HOME
echo $PWD
echo $PATH

# Variable expansion
echo "Hello $USER"
echo "Home: $HOME"
```

### 🟡 Level 2 - Variable Operations
```bash
# Setting variables
export TEST_VAR=hello
echo $TEST_VAR

# Multiple variables
echo $USER $HOME $PWD
```

### 🔴 Level 3 - Advanced Variables
```bash
# Variable in quotes
echo "User: $USER, Home: $HOME"
echo 'No expansion: $USER'

# Variable with braces
echo ${USER}
echo ${HOME}/documents

# Exit status
echo $?
ls non_existent_file
echo $?
```

---

## Built-in Commands

### 🟡 Level 1 - Basit Built-ins
```bash
# pwd
pwd

# echo
echo hello
echo -n hello

# cd
cd
cd ~
cd ..
cd /tmp
pwd
```

### 🟡 Level 2 - Environment Built-ins
```bash
# env
env
env | grep USER

# export
export TEST=value
echo $TEST

# unset
unset TEST
echo $TEST
```

### 🟡 Level 3 - Advanced Built-ins
```bash
# exit
exit
exit 0
exit 42

# cd with error handling
cd non_existent_dir
echo $?
cd
pwd
```

---

## Advanced Features

### 🔴 Level 1 - Signal Handling
```bash
# Ctrl+C during command
sleep 10
# Press Ctrl+C

# Ctrl+D (EOF)
cat
# Press Ctrl+D

# Ctrl+\ (quit signal)
sleep 10
# Press Ctrl+\
```

### 🔴 Level 2 - Complex Command Combinations
```bash
# Subshells (if implemented)
(cd /tmp && pwd)
pwd

# Background processes (if implemented)
sleep 5 &
ps
```

### 🔴 Level 3 - Advanced Parsing
```bash
# Complex quote scenarios
echo "test with $HOME and 'single quotes'"
echo 'test with "double quotes" inside'

# Multiple operators
echo hello > file1.txt && echo world > file2.txt
echo test | grep t && echo found
```

---

## Error Handling

### 🟡 Level 1 - Command Errors
```bash
# Non-existent commands
non_existent_command
command_not_found

# Permission errors
cat /etc/shadow
ls /root
```

### 🟡 Level 2 - File Errors
```bash
# File not found
cat non_existent_file.txt
ls non_existent_dir

# Permission denied
echo hello > /etc/test.txt
mkdir /etc/newdir
```

### 🔴 Level 3 - Parse Errors
```bash
# Syntax errors
echo hello |
echo hello >
cat <<

# Invalid quotes
echo "unclosed quote
echo 'another unclosed

# Invalid redirections
< > file.txt
>> < file.txt
```

---

## Edge Cases

### 🔴 Level 1 - Whitespace Handling
```bash
# Multiple spaces
echo     hello     world
ls   |   grep   test

# Tabs
echo	hello	world
ls	|	cat

# Leading/trailing spaces
   echo hello   
   ls | grep test   
```

### 🔴 Level 2 - Empty Inputs
```bash
# Empty command
""
''

# Only spaces
   
	

# Only operators
|
>
<
>>
<<
```

### 🔴 Level 3 - Boundary Cases
```bash
# Very long commands
echo this is a very very very long command with many many words that should still work properly

# Many pipes
echo hello | cat | cat | cat | cat | cat

# Deep redirections
echo hello > file1.txt
cat < file1.txt > file2.txt
cat < file2.txt > file3.txt
```

---

## Test Workflow

### Lexer Phase (Current)
1. ✅ Test token recognition
2. ✅ Test operators
3. ✅ Test quotes
4. ✅ Test whitespace handling

### Parser Phase (Next)
1. 🟡 Build AST from tokens
2. 🟡 Handle syntax errors
3. 🟡 Validate command structure

### Executor Phase (Final)
1. 🔴 Execute simple commands
2. 🔴 Handle pipes
3. 🔴 Handle redirections
4. 🔴 Implement built-ins

---

## Quick Test Commands

**Şu anda test edebileceğiniz komutlar:**

```bash
# Minishell'i başlat
./minishell

# Basit test'ler
echo hello
ls | grep test
echo "quoted string"
cat > output.txt
echo hello >> file.txt

# Çıkış
exit
```

**Her test sonrasında token listesini görüp lexer'ın doğru çalışıp çalışmadığını kontrol edin!**

---

## Debug İpuçları

1. **Token Debug**: Her komut sonrası token listesini kontrol edin
2. **Memory Leaks**: `valgrind ./minishell` ile test edin
3. **Norm Check**: `norminette` ile kod stilini kontrol edin
4. **Real Bash**: Komutları önce gerçek bash'te test edin

**İyi çalışmalar! 🚀**
