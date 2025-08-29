# 🧪 Minishell Test Cases & Bug Report

**Test Date**: August 29, 2025  
**Project Status**: 90% Complete - Core functionality working  
**Test Focus**: Redirection, Pipes, Edge Cases

---

## 🎯 **TEST SUMMARY**

### ✅ **WORKING FEATURES** (No issues found)

#### **✅ Core Built-in Commands**
- ✅ `echo "Hello World"` → `Hello World`
- ✅ `pwd` → `/home/hsyn/desktop/minishell`
- ✅ `env` → Shows all environment variables
- ✅ `export TEST=hello` → Sets environment variable
- ✅ `cd ..` → Changes directory properly
- ✅ `exit` → Terminates shell properly
- ✅ `unset HOME` → Removes environment variable

#### **✅ External Commands & PATH Resolution**
- ✅ `ls` → Shows directory listing
- ✅ External program execution working
- ✅ PATH resolution functional

#### **✅ Variable Expansion**
- ✅ `echo $HOME` → `/home/hsyn`
- ✅ `echo $USER` → `hsyn`
- ✅ `echo $?` → `0`
- ✅ `echo $$` → Shows process ID
- ✅ `echo $TEST` after `export TEST=hello` → `hello`

#### **✅ Advanced Pipeline Operations**
- ✅ **Simple Pipes**: `ls | grep minishell` → `minishell`
- ✅ **Multiple Pipes**: `env | grep HOME | head -1` → `HOME=/home/hsyn`
- ✅ **Built-in to External**: Pipeline combinations work

#### **✅ Input Redirection**
- ✅ `cat < test_file.txt` → Reads file content properly

#### **✅ Here Documents**
- ✅ `cat << EOF` → Multiline input works perfectly
- ✅ Delimiter detection working
- ✅ Input processing correct

#### **✅ Error Handling**
- ✅ `nonexistent_command` → `minishell: nonexistent_command: command not found`
- ✅ Proper error messages displayed

---

## 🚨 **CRITICAL BUGS FOUND**

### ❌ **BUG #1: Output Redirection Not Working**

**Test Case:**
```bash
minishell$ echo "Hello World" > output.txt
# Expected: Creates output.txt with "Hello World"
# Actual: No file created, text prints to terminal
```

**Evidence:**
```bash
$ echo "Hello World" > output.txt
Hello World  # ❌ Should not print to terminal
$ ls -la output.txt
ls: cannot access 'output.txt': No such file or directory  # ❌ File not created
```

**Symptoms:**
- ✅ Command executes without errors
- ❌ File is not created
- ❌ Content still prints to terminal
- ✅ Debug shows "[DEBUG] Commands executed successfully"

**Affected Files:**
- `executer/redirections.c` - Lines 55-65 (T_REDIRECT_OUT handling)
- `executer/executor.c` - Redirection setup call timing
- `loop_dir/parser.c` - Redirection parsing validation

**Severity**: 🔴 **CRITICAL** - Core shell feature not working

---

### ❌ **BUG #2: Append Redirection Not Working**

**Test Case:**
```bash
minishell$ echo "test" >> append_test.txt
# Expected: Creates append_test.txt with "test"
# Actual: No file created, text prints to terminal
```

**Evidence:**
```bash
$ echo "test" >> append_test.txt
test  # ❌ Should not print to terminal
$ ls -la append_test.txt
ls: cannot access 'append_test.txt': No such file or directory  # ❌ File not created
```

**Symptoms:**
- ✅ Command executes without errors
- ❌ File is not created
- ❌ Content still prints to terminal
- ✅ Parse recognizes `>>` token correctly

**Affected Files:**
- `executer/redirections.c` - Lines 67-77 (T_APPEND handling)
- `source/tokenizer.c` - T_APPEND token recognition
- `loop_dir/parser.c` - Append redirection parsing

**Severity**: 🔴 **CRITICAL** - Core shell feature not working

---

## 🔍 **ROOT CAUSE ANALYSIS**

### **Hypothesis 1: Redirection Setup Timing Issue**
**Theory**: Redirections are parsed correctly but `setup_redirections()` is not called at the right time during command execution.

**Evidence Supporting:**
- ✅ Input redirection (`<`) works → Basic redirection logic is sound
- ❌ Output redirections (`>`, `>>`) don't work → Timing/execution issue
- ✅ Here documents (`<<`) work → Complex input redirection works

**Investigation Needed:**
- Check when `setup_redirections(cmd)` is called in executor
- Verify if redirection is applied before or after command execution
- Check if redirection setup happens in child process correctly

### **Hypothesis 2: File Descriptor Management Issue**
**Theory**: Output redirection file descriptors are not properly managed or closed.

**Evidence Supporting:**
- ✅ `dup2(fd, STDOUT_FILENO)` code exists in `redirections.c`
- ❌ Files are not created → `open()` might not be called
- ❌ Output still goes to terminal → `dup2()` not effective

**Investigation Needed:**
- Add debug prints to `handle_single_redirection()`
- Verify `open()` call success for output files
- Check if `dup2()` is actually called

### **Hypothesis 3: Parser-Executor Communication Issue**
**Theory**: Redirection information is lost between parser and executor.

**Evidence Supporting:**
- ✅ Complex pipes work → Basic command execution communication works
- ❌ Only output redirections fail → Specific redirection data issue
- ✅ Input redirections work → Parsing is partially correct

**Investigation Needed:**
- Debug print redirection list in executor before `setup_redirections()`
- Verify `t_redirect` structure population
- Check if redirection list is properly linked to command

---

## 🧪 **ADDITIONAL TEST CASES TO RUN**

### **📋 Priority 1: Debug Output Redirection**
```bash
# Test these systematically:
echo "test1" > file1.txt
echo "test2" > file2.txt  
ls -la file*.txt

# Advanced output redirection:
pwd > current_dir.txt
env > env_dump.txt
cat env_dump.txt
```

### **📋 Priority 2: Debug Append Redirection**
```bash
# Test append functionality:
echo "line1" >> multi_line.txt
echo "line2" >> multi_line.txt
echo "line3" >> multi_line.txt
cat multi_line.txt  # Should show all 3 lines
```

### **📋 Priority 3: Combined Redirection Tests**
```bash
# Test complex combinations:
echo "hello" > temp.txt && cat < temp.txt
ls | grep minishell > filtered.txt
cat filtered.txt
```

### **📋 Priority 4: Signal Handling Tests**
```bash
# Interactive tests (manual):
./minishell
# Then test:
# Ctrl+C during command execution
# Ctrl+D to exit
# Ctrl+\ behavior
```

---

## 🎯 **RECOMMENDED FIX PRIORITY**

### **🔴 IMMEDIATE (This Session)**
1. **Fix Output Redirection (`>`)** - Most critical basic feature
2. **Fix Append Redirection (`>>`)** - Essential functionality
3. **Debug redirection setup timing** - Root cause investigation

### **🟡 SHORT TERM (Next Session)**
4. **Signal handling improvements** - User experience
5. **Enhanced error messages** - Better debugging
6. **Memory leak detection** - Stability

### **🟢 LONG TERM (Polish Phase)**
7. **Edge case handling** - Robustness
8. **Performance optimization** - Efficiency
9. **Comprehensive test suite** - Quality assurance

---

## 📊 **PROJECT STATUS UPDATE**

```
Overall Completion: [████████░░] 85%

✅ WORKING (No fixes needed):
- Core built-ins (7/7)       [██████████] 100%
- External commands          [██████████] 100%
- Variable expansion         [██████████] 100%
- Pipeline operations        [██████████] 100%
- Input redirection (<)      [██████████] 100%
- Here documents (<<)        [██████████] 100%
- Error handling             [██████████] 100%

❌ BROKEN (Needs immediate fix):
- Output redirection (>)     [░░░░░░░░░░] 0%
- Append redirection (>>)    [░░░░░░░░░░] 0%

🔄 UNTESTED (Needs verification):
- Signal handling            [░░░░░░░░░░] Unknown
- Advanced edge cases        [░░░░░░░░░░] Unknown
```

---

## 🚀 **NEXT SESSION PLAN**

### **Step 1: Immediate Debug (30 minutes)**
1. Add debug prints to `handle_single_redirection()`
2. Verify when `setup_redirections()` is called
3. Check redirection list population in commands

### **Step 2: Fix Implementation (60 minutes)**
1. Fix output redirection timing issue
2. Ensure file creation works properly
3. Test and verify fixes

### **Step 3: Validation (30 minutes)**
1. Re-run all failing test cases
2. Verify fixes don't break existing functionality
3. Document solution

---

**🎯 GOAL**: Convert the shell from 85% to 95%+ completion by fixing the critical output redirection bugs!

**📋 FILES TO EXAMINE**:
- `executer/redirections.c` (redirection logic)
- `executer/executor.c` (redirection setup timing)
- `loop_dir/parser.c` (redirection parsing verification)
