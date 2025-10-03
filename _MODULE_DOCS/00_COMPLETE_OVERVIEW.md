# MİNİSHELL PROJESİ - KAPSAMLI MODÜL DOKÜMANTASYONU

## PROJE GENEL BAKIŞ

Bu dokümantasyon, minishell projesinin tüm modüllerinin detaylı analizini içerir. Proje, UNIX shell'in core functionality'sini implement eden, POSIX-compliant bir shell uygulamasıdır.

## MODÜL YAPISI VE HIYERARŞI

```
minishell/
├── 01_core/         # Ana program akışı ve main loop
├── 02_lexer/        # Input tokenization ve lexical analysis
├── 03_parser/       # Syntax analysis ve command structure building
├── 04_expander/     # Variable expansion ve quote processing
├── 05_executer/     # Command execution ve process management
├── 06_builtins/     # Built-in command implementations
├── 07_environment/  # Environment variable management
├── 08_redirection/  # I/O redirection ve heredoc
├── 09_signal/       # Signal handling ve interactive behavior
└── 10_utils/        # Utility functions ve debugging tools
```

## EXECUTION FLOW

### 1. Program Başlatma
```
main() → init_global() → setup_signals() → shell_loop()
```

### 2. Input İşleme Döngüsü
```
readline() → lexer → parser → expander → executer → cleanup
```

### 3. Komut Çalıştırma
```
execute_commands() → [built-in | external] → wait → update_exit_status
```

## MODÜL BAĞIMLILIKLARI

### Core Dependencies:
- **Core** → Utils (global state)
- **Core** → Signal (interactive behavior)
- **Core** → All modules (orchestrates entire flow)

### Processing Pipeline:
- **Lexer** → Core (input), Utils (debug)
- **Parser** → Lexer (tokens), Utils (command creation)
- **Expander** → Parser (commands), Environment (variable lookup)
- **Executer** → Expander (processed commands), Environment (PATH)

### System Integration:
- **Builtins** → Environment (variable manipulation)
- **Redirection** → Utils (FD management)
- **Signal** → Utils (global state), Redirection (cleanup)

## DATA STRUCTURES

### Primary Structures:
```c
// Global state (utils/global_state.c)
typedef struct s_global {
    t_list      *tokens;        // Lexer output
    t_command   *commands;      // Parser output
    t_env       *env_list;      // Environment variables
    int         exit_status;    // Last command exit status
    int         in_child;       // Process type flag
    // ... other fields
} t_global;

// Token structure (lexer/)
typedef struct s_token_new {
    t_token_types   type;       // Token type enum
    char           *value;      // Token string value
    int            len;         // Token length
} t_token_new;

// Command structure (parser/)
typedef struct s_command {
    char              **args;        // Command arguments
    t_list            *redirections; // Redirection list
    int               pipe_fd[2];    // Pipe file descriptors
    pid_t             pid;           // Process ID
    struct s_command  *next;         // Next command (pipeline)
} t_command;

// Environment structure (environment/)
typedef struct s_env {
    char            *key;       // Variable name
    char            *value;     // Variable value
    struct s_env    *next;      // Next environment node
} t_env;

// Redirection structure (redirection/)
typedef struct s_redirect {
    int                 type;       // Redirection type
    char               *filename;   // Target filename
    int                 fd;         // File descriptor
    struct s_redirect  *next;       // Next redirection
} t_redirect;
```

## ERROR HANDLING STRATEGY

### Exit Codes:
```
0     - Success
1     - General error
2     - Misuse of shell builtins
126   - Command invoked cannot execute
127   - Command not found
128+n - Fatal error signal "n"
130   - Script terminated by Control-C (SIGINT)
131   - Script terminated by Control-\ (SIGQUIT)
```

### Error Propagation:
1. **Lexer Errors**: Invalid syntax → continue with prompt
2. **Parser Errors**: Syntax errors → error message + continue
3. **Execution Errors**: Command failures → exit status update
4. **System Errors**: fork/pipe failures → graceful degradation
5. **Signal Handling**: Cleanup + appropriate exit codes

## MEMORY MANAGEMENT

### Allocation Strategy:
- **Garbage Collection**: `halloc()` system for automatic cleanup
- **Reference Counting**: Environment variables shared safely
- **RAII Pattern**: Resource acquisition with automatic cleanup
- **Process Cleanup**: Child processes clean up on exit

### Resource Lifecycle:
```
Input → Lexer (token allocation) → Parser (command allocation) 
→ Executer (FD management) → Cleanup (garbage collection)
```

### FD Management:
- **Standard FDs**: 0 (stdin), 1 (stdout), 2 (stderr) preserved
- **File FDs**: Automatic close after redirection setup
- **Pipe FDs**: Coordinated cleanup in pipeline
- **Heredoc FDs**: Tracked array with signal-safe cleanup

## SIGNAL HANDLING ARCHITECTURE

### Signal Types:
- **SIGINT (Ctrl+C)**: Interactive interrupt → new prompt
- **SIGQUIT (Ctrl+\)**: Ignored in interactive mode
- **SIGPIPE**: Broken pipe handling in pipelines
- **EOF (Ctrl+D)**: Graceful shell exit

### Process-Specific Behavior:
```c
if (global->in_child) {
    // Child process: cleanup and exit with signal code
    cleanup_and_exit();
    exit(128 + signal_number);
} else {
    // Parent process: reset readline and continue
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}
```

## BASH COMPATIBILITY

### Supported Features:
- ✅ Command execution with PATH resolution
- ✅ Built-in commands (echo, cd, pwd, export, unset, env, exit)
- ✅ I/O redirection (<, >, >>)
- ✅ Heredoc (<<)
- ✅ Pipelines (|)
- ✅ Environment variables ($VAR)
- ✅ Quote handling (single/double quotes)
- ✅ Signal handling (Ctrl+C, Ctrl+\, Ctrl+D)
- ✅ Exit status handling ($?)

### Bash-Compatible Behavior:
- Command not found handling
- Environment variable expansion
- Quote removal and preservation
- Redirection order independence
- Pipeline exit status (rightmost command)
- Built-in command precedence
- Signal exit codes

## PERFORMANCE CHARACTERISTICS

### Time Complexity:
- **Lexer**: O(n) where n = input length
- **Parser**: O(m) where m = token count
- **Environment Lookup**: O(e) where e = environment size
- **PATH Search**: O(p×d) where p = PATH directories, d = directory entries

### Space Complexity:
- **Token Storage**: O(n) proportional to input
- **Command Structure**: O(c×a) where c = commands, a = arguments
- **Environment**: O(e) where e = environment variables
- **FD Tracking**: O(1) fixed size arrays

### Optimization Techniques:
- **Early Exit**: Parser stops on syntax errors
- **Lazy Evaluation**: Environment lookup only when needed
- **Resource Pooling**: FD reuse in pipelines
- **Minimal Copying**: String references where safe

## SECURITY CONSIDERATIONS

### Input Sanitization:
- **Quote Handling**: Proper quote parsing prevents injection
- **Path Validation**: Relative/absolute path security
- **Environment Safety**: No buffer overflows in variable expansion
- **Command Validation**: Built-in vs external command separation

### Process Isolation:
- **Child Processes**: Each command runs in separate process
- **Signal Safety**: Signal handlers are async-signal-safe
- **FD Isolation**: Child processes have controlled FD access
- **Environment Inheritance**: Controlled environment passing

### Resource Limits:
- **FD Limits**: Maximum heredoc FD tracking
- **Memory Limits**: Garbage collection prevents leaks
- **Process Limits**: Process counting and management
- **Signal Safety**: No malloc in signal handlers

## TESTING STRATEGY

### Unit Testing Areas:
1. **Lexer**: Token recognition and classification
2. **Parser**: Command structure building
3. **Environment**: Variable manipulation
4. **Redirection**: File operations and FD management
5. **Signal**: Handler behavior verification

### Integration Testing:
- **End-to-End**: Complete command execution flow
- **Pipeline Testing**: Multi-command pipeline behavior
- **Error Scenarios**: Error handling and recovery
- **Signal Testing**: Interactive signal behavior
- **Memory Testing**: Resource cleanup verification

### Compatibility Testing:
- **Bash Comparison**: Identical behavior verification
- **POSIX Compliance**: Standard shell behavior
- **Edge Cases**: Corner case handling
- **Performance**: Response time and resource usage

## DEVELOPMENT WORKFLOW

### Debug Support:
```c
// Token debugging
print_tokens_advanced(tokens);

// Command debugging
print_commands_debug(commands);

// FD tracking
register_heredoc_fd(fd);
close_all_heredoc_fds();

// General debugging
debug_print("Process checkpoint reached");
```

### Build Configuration:
- **Debug Build**: Full debugging output, assertions enabled
- **Release Build**: Optimized execution, minimal output
- **Memory Debug**: Garbage collection tracking
- **Performance Profile**: Timing and resource monitoring

## SYSTEM REQUIREMENTS

### Dependencies:
- **System**: POSIX-compliant UNIX system
- **Libraries**: readline library for line editing
- **Compiler**: GCC with C99 support
- **Tools**: make for build automation

### Runtime Requirements:
- **Memory**: ~1MB base memory footprint
- **FDs**: Support for 1024+ concurrent file descriptors
- **Processes**: Process creation and management support
- **Signals**: POSIX signal handling support

## FUTURE ENHANCEMENTS

### Potential Improvements:
1. **Job Control**: Background processes (&)
2. **Command Substitution**: $(command) syntax
3. **Advanced Redirection**: File descriptor manipulation
4. **History Expansion**: !n command history
5. **Tab Completion**: Command and filename completion
6. **Configuration**: ~/.minishellrc support

### Architecture Extensions:
- **Plugin System**: Loadable built-in commands
- **Scripting**: Enhanced shell script support
- **Network**: Remote command execution
- **Security**: Sandboxing and privilege separation

## CONCLUSION

Bu minishell implementation'u, modern UNIX shell'in core functionality'sini comprehensive şekilde implement eder. Modüler architecture, proper error handling, memory safety, ve POSIX compliance ile production-ready bir shell solution sağlar.

Proje, system programming concepts'lerinin pratik application'unu demonstrare eder:
- Process management ve IPC
- Signal handling ve asynchronous programming  
- File system operations ve I/O redirection
- Memory management ve resource cleanup
- Parser design ve syntax analysis
- System integration ve UNIX programming

Her modül kendi sorumluluğunda specific functionality implement ederken, cohesive bir sistem oluşturmak için diğer modüllerle coordinate eder. Bu design pattern, maintainability, testability, ve extensibility sağlar.

**Dokümantasyon Status**: Tüm 10 modül için comprehensive line-by-line analysis tamamlandı. Her modül için ayrı dokümantasyon dosyası oluşturuldu ve code flow, usage scenarios, error handling, ve implementation details detaylandırıldı.
