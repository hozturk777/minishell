# UTILS MODÜL DOKÜMANTASYONU

## GENEL BAKIŞ

Utils modülü, minishell projesi için yardımcı fonksiyonlar, debugging araçları ve global state management sağlar. Projenin diğer modüllerince kullanılan ortak utility fonksiyonlarını barındırır.

## MODÜLÜNDEKİ DOSYALAR

1. **global_state.c** - Global state management ve FD tracking
2. **debug_utils.c** - Command debugging ve utility fonksiyonları
3. **debug.c** - Token debugging ve environment expansion utilities

## GLOBAL STATE YÖNETİMİ

### Global State Yapısı
```c
typedef struct s_global
{
    t_list      *tokens;           // Lexer'dan gelen token listesi
    t_command   *commands;         // Parser'dan gelen command listesi
    t_env       *env_list;         // Environment variables linked list
    char        *input_line;       // Kullanıcı input satırı
    int         exit_status;       // Son komutun exit status'u
    int         pipe_count;        // Pipeline'daki pipe sayısı
    int         heredoc_count;     // Heredoc sayısı (temp file naming için)
    int         interactive;       // Interactive mode flag'i
    int         in_child;          // Child process flag'i
    int         should_exit;       // Exit flag'i (EOF için)
    int         heredoc_fds[MAX_HEREDOC_FDS]; // Heredoc FD tracking array
    int         heredoc_fd_count;  // Açık heredoc FD sayısı
} t_global;
```

## İŞLEV AKIŞI

```
Program Startup
├── init_global()                # Global state başlatma
├── update_shlvl()              # SHLVL environment variable güncellemesi
└── FD Tracking Setup           # Heredoc FD tracking başlatma

Runtime Operations
├── register_heredoc_fd()       # Heredoc FD kaydı
├── close_all_heredoc_fds()     # Tüm heredoc FD'leri kapatma
└── cleanup_and_exit()          # Process exit cleanup

Debugging Support
├── print_commands_debug()      # Command structure debug
├── print_tokens_advanced()     # Token debugging
└── debug_print()               # General debug messages
```

## 1. GLOBAL_STATE.C - GLOBAL STATE YÖNETİMİ

### Kod Analizi - init_global()

```c
t_global	*init_global(char **envp, t_global *global)
{
	int	i;                                      // 1. Loop counter

	if (!global)                                // 2. Global struct allocation kontrolü
		return (NULL);                          // 3. Allocation başarısızsa null dön
		
	// 4. Tüm pointer'ları ve değişkenleri başlat
	global->tokens = NULL;                      // 5. Token listesi başlangıçta boş
	global->commands = NULL;                    // 6. Command listesi başlangıçta boş
	global->env_list = init_env_from_envp(envp); // 7. System environment'ı yükle
	update_shlvl(global->env_list);             // 8. SHLVL'yi artır (nested shell)
	global->exit_status = 0;                    // 9. Exit status başlangıç değeri
	global->pipe_count = 0;                     // 10. Pipe sayısı sıfır
	global->heredoc_count = 0;                  // 11. Heredoc sayısı sıfır
	global->input_line = NULL;                  // 12. Input line başlangıçta boş
	global->interactive = 1;                    // 13. Interactive mode aktif
	global->in_child = 0;                       // 14. Parent process'te başlıyoruz
	global->should_exit = 0;                    // 15. Exit flag'i false
	global->heredoc_fd_count = 0;               // 16. Heredoc FD sayısı sıfır
	
	// 17. Heredoc FD array'ini başlat (-1 = invalid FD)
	i = 0;
	while (i < MAX_HEREDOC_FDS)
		global->heredoc_fds[i++] = -1;          // 18. Tüm FD'leri invalid yap

	return (global);                            // 19. Başlatılan global'i dön
}
```

### Kod Analizi - update_shlvl()

```c
static void	update_shlvl(t_env *env)
{
	int	value;                                  // 1. SHLVL değeri

	value = ft_atoi(get_env_value(env, "SHLVL")); // 2. Mevcut SHLVL'yi al ve int'e çevir
	value++;                                    // 3. Değeri artır (nested shell)
	set_env_var(get_global(), "SHLVL", ft_itoa(value)); // 4. Yeni değeri environment'a set et
}
```

**SHLVL (Shell Level) Açıklaması:**
- UNIX shell'lerde nested shell depth'i gösterir
- Minishell başladığında parent shell'in SHLVL'sini +1 artırır
- Bash: SHLVL=1, minishell başlatınca SHLVL=2 olur
- Child process'ler parent'ın SHLVL'sini inherit eder

### Kod Analizi - register_heredoc_fd()

```c
void	register_heredoc_fd(int fd)
{
	t_global	*global;                        // 1. Global state pointer'ı

	global = get_global();                      // 2. Global state'i al
	if (!global || fd <= 2)                     // 3. Global yok veya invalid FD mi?
		return ;                                // 4. Kaydetme
		
	// 5. FD array'inde yer var mı?
	if (global->heredoc_fd_count < MAX_HEREDOC_FDS)
	{
		global->heredoc_fds[global->heredoc_fd_count] = fd; // 6. FD'yi array'e ekle
		global->heredoc_fd_count++;             // 7. Count'u artır
		debug_print("Registered heredoc FD");   // 8. Debug mesajı
	}
	else                                        // 9. Array dolu
	{
		debug_print("WARNING: Max heredoc FDs reached, cannot register more");
		close(fd);                              // 10. FD'yi kapat (memory leak prevent)
	}
}
```

### Kod Analizi - close_all_heredoc_fds()

```c
void	close_all_heredoc_fds(void)
{
	t_global	*global;                        // 1. Global state pointer'ı
	int			i;                              // 2. Loop counter

	global = get_global();                      // 3. Global state'i al
	if (!global)                                // 4. Global state yok mu?
		return ;                                // 5. İşlem yapma
	
	i = 0;                                      // 6. Loop counter'ı sıfırla
	while (i < global->heredoc_fd_count)        // 7. Her registered FD için
	{
		if (global->heredoc_fds[i] > 2)         // 8. Standard FD değilse (0,1,2)
		{
			close(global->heredoc_fds[i]);      // 9. FD'yi kapat
			global->heredoc_fds[i] = -1;        // 10. Array'de invalid yap
		}
		i++;                                    // 11. Sonraki FD'ye geç
	}
	global->heredoc_fd_count = 0;               // 12. Count'u sıfırla
}
```

### Kod Analizi - cleanup_and_exit()

```c
void	cleanup_and_exit(void)
{
	close_all_heredoc_fds();                   // 1. Tüm heredoc FD'leri kapat
	clear_garbage();                           // 2. Garbage collector'ı çalıştır
	rl_clear_history();                        // 3. Readline history'sini temizle
}
```

**Cleanup Akışı:**
1. **FD Cleanup**: Tüm açık heredoc FD'leri kapatır
2. **Memory Cleanup**: Garbage collector ile allocated memory'yi temizler
3. **Readline Cleanup**: Command history'sini temizler

**Çağrılma Durumları:**
- Process exit sırasında
- Signal handler'da (SIGINT, SIGQUIT)
- Fatal error durumlarında
- Child process termination'da

## 2. DEBUG_UTILS.C - DEBUGGING VE COMMAND UTILITIES

### Kod Analizi - print_commands_debug()

```c
void	print_commands_debug(t_command *commands)
{
	t_command	*current;                       // 1. Mevcut command pointer'ı
	int			cmd_count;                      // 2. Command counter

	current = commands;                         // 3. İlk command'dan başla
	cmd_count = 1;                              // 4. Counter'ı 1'den başlat
	while (current)                             // 5. Her command için
	{
		printf("Command %d:\n", cmd_count);     // 6. Command numarasını yazdır
		print_command_args(current);            // 7. Command argümanlarını yazdır
		printf("  Redirections: ");             // 8. Redirection header'ı
		print_command_redirections(current);    // 9. Redirection'ları yazdır
		printf("  Pipe: %s\n", current->next ? "YES" : "NO"); // 10. Pipe durumu
		printf("  Pid: %d\n\n", current->pid); // 11. Process ID
		current = current->next;                // 12. Sonraki command'a geç
		cmd_count++;                            // 13. Counter'ı artır
	}
}
```

### Kod Analizi - print_command_args()

```c
static void	print_command_args(t_command *cmd)
{
	int	i;                                      // 1. Argüman counter'ı

	printf("  Args: ");                         // 2. Args header'ı yazdır
	if (cmd->args)                              // 3. Args array'i var mı?
	{
		i = 0;                                  // 4. Counter'ı sıfırla
		while (cmd->args[i])                    // 5. Her argüman için
		{
			printf("$%s$", cmd->args[i]);       // 6. Argümanı delimiters ile yazdır
			if (cmd->args[i + 1])               // 7. Sonraki argüman var mı?
				printf(", ");                   // 8. Comma separator ekle
			i++;                                // 9. Sonraki argümana geç
		}
	}
	else                                        // 10. Args yok
		printf("(none)");                       // 11. Boş mesajı
	printf("\n");                               // 12. Newline ekle
}
```

### Kod Analizi - print_command_redirections()

```c
static void	print_command_redirections(t_command *cmd)
{
	t_list		*node;                          // 1. Redirection list node'u
	t_redirect	*redirect;                      // 2. Redirection struct'ı

	node = cmd->redirections;                   // 3. Redirection listesinin başı
	if (!node)                                  // 4. Redirection yok mu?
	{
		printf("(none)\n");                     // 5. Boş mesajı yazdır
		return ;                                // 6. Fonksiyondan çık
	}
	
	printf("\n");                               // 7. Newline (multi-line format için)
	while (node)                                // 8. Her redirection için
	{
		redirect = (t_redirect *)node->content; // 9. Redirection'ı al
		
		// 10. Redirection türüne göre yazdır
		if (redirect->type == T_REDIRECT_IN)    // 11. Input redirection
			printf("    < $%s$\n", redirect->filename);
		else if (redirect->type == T_REDIRECT_OUT) // 12. Output redirection
			printf("    > $%s$\n", redirect->filename);
		else if (redirect->type == T_APPEND)    // 13. Append redirection
			printf("    >> $%s$\n", redirect->filename);
		else if (redirect->type == T_HEREDOC)   // 14. Heredoc
			printf("    << $%s$\n", redirect->filename);
		else                                    // 15. Bilinmeyen tür
			printf("    Unknown redirect\n");
			
		node = node->next;                      // 16. Sonraki redirection'a geç
	}
}
```

### Kod Analizi - create_command()

```c
t_command	*create_command(void)
{
	t_command	*cmd;                           // 1. Yeni command pointer'ı

	cmd = halloc(sizeof(t_command));            // 2. Command struct'ı için memory allocate et
	if (!cmd)                                   // 3. Allocation başarısız mı?
		return (NULL);                          // 4. Null dön
		
	// 5. Tüm field'ları başlat
	cmd->args = NULL;                           // 6. Args array'i boş
	cmd->redirections = NULL;                   // 7. Redirection listesi boş
	cmd->pipe_fd[0] = -1;                       // 8. Pipe read FD invalid
	cmd->pipe_fd[1] = -1;                       // 9. Pipe write FD invalid
	cmd->pid = -1;                              // 10. Process ID invalid
	cmd->next = NULL;                           // 11. Next pointer null
	
	return (cmd);                               // 12. Başlatılan command'ı dön
}
```

**Debug Output Formatı:**
```
Command 1:
  Args: $cat$, $file.txt$
  Redirections: 
    > $output.txt$
  Pipe: YES
  Pid: -1

Command 2:
  Args: $grep$, $pattern$
  Redirections: (none)
  Pipe: NO
  Pid: -1
```

## 3. DEBUG.C - TOKEN DEBUGGING VE UTILITY

### Kod Analizi - print_tokens_advanced()

```c
void	print_tokens_advanced(t_list *tokens)
{
	t_list		*current;                       // 1. Token list traversal pointer'ı
	t_token_new	*token;                         // 2. Token struct pointer'ı
	int			i;                              // 3. Token counter

	printf("\n" GREEN "=== ADVANCED TOKENS DEBUG ===" RESET "\n"); // 4. Debug header
	current = tokens;                           // 5. Token listesinin başından başla
	i = 0;                                      // 6. Counter'ı sıfırla
	while (current)                             // 7. Her token için
	{
		token = (t_token_new *)current->content; // 8. Token'ı al
		// 9. Token bilgilerini formatla ve yazdır
		printf("Token[%d]: Type=%-12s Value=$%s$ Len=%d\n", i,
			get_token_type_name(token->type), token->value, token->len);
		current = current->next;                // 10. Sonraki token'a geç
		i++;                                    // 11. Counter'ı artır
	}
	printf(GREEN "=== END TOKENS DEBUG ===" RESET "\n\n"); // 12. Debug footer
}
```

### Kod Analizi - get_token_type_name()

```c
static char	*get_token_type_name(t_token_types type)
{
	if (type == T_WORD)                         // 1. Word token
		return ("WORD");
	else if (type == T_PIPE)                    // 2. Pipe token
		return ("PIPE");
	else if (type == T_CMD)                     // 3. Command token
		return ("T_CMD");
	else if (type == T_REDIRECT_IN)             // 4. Input redirection
		return ("REDIRECT_IN");
	else if (type == T_REDIRECT_OUT)            // 5. Output redirection
		return ("REDIRECT_OUT");
	else if (type == T_APPEND)                  // 6. Append redirection
		return ("APPEND");
	else if (type == T_HEREDOC)                 // 7. Heredoc
		return ("HEREDOC");
	else if (type == T_SINGLE_QUOTE)            // 8. Single quote
		return ("SINGLE_QUOTE");
	else if (type == T_DOUBLE_QUOTE)            // 9. Double quote
		return ("DOUBLE_QUOTE");
	else if (type == T_ENV_VAR)                 // 10. Environment variable
		return ("ENV_VAR");
	else if (type == T_EOF)                     // 11. End of file
		return ("EOF");
	else if (type == T_WHITESPACE)              // 12. Whitespace
		return ("T_WHITESPACE");
	return ("UNKNOWN");                         // 13. Bilinmeyen tür
}
```

### Kod Analizi - expand_env_var()

```c
char	*expand_env_var(char *var_name, t_global *global)
{
	t_env	*env_node;                          // 1. Environment node pointer'ı

	env_node = find_env_node(global->env_list, var_name); // 2. Variable'ı environment'ta ara
	if (env_node)                               // 3. Variable bulundu mu?
		return (ft_strdup(env_node->value));    // 4. Value'yu kopyala ve dön
	return (ft_strdup(""));                     // 5. Bulunamadıysa boş string dön
}
```

### Kod Analizi - is_special_char()

```c
int	is_special_char(char c)
{
	// 1. Shell'de özel anlama sahip karakterleri kontrol et
	if (c == '|' || c == '<' || c == '>' || c == '\'' || c == '"' || c == ' '
		|| c == '\t' || c == '\n')
		return (1);                             // 2. Özel karakter
	return (0);                                 // 3. Normal karakter
}
```

### Kod Analizi - debug_print()

```c
void	debug_print(char *msg)
{
	printf(GREEN "[DEBUG] %s" RESET "\n", msg); // 1. Renkli debug mesajı yazdır
}
```

**Token Debug Output Formatı:**
```
=== ADVANCED TOKENS DEBUG ===
Token[0]: Type=WORD         Value=$cat$ Len=3
Token[1]: Type=REDIRECT_OUT  Value=$>$ Len=1
Token[2]: Type=WORD         Value=$file.txt$ Len=8
Token[3]: Type=PIPE         Value=$|$ Len=1
Token[4]: Type=WORD         Value=$grep$ Len=4
Token[5]: Type=WORD         Value=$pattern$ Len=7
=== END TOKENS DEBUG ===
```

## DEBUGGING WORKFLOW

### Development Debugging:
1. **Token Analysis**: `print_tokens_advanced()` ile lexer output'unu kontrol
2. **Command Structure**: `print_commands_debug()` ile parser output'unu kontrol
3. **FD Tracking**: `register_heredoc_fd()` ile FD leak'leri takip
4. **State Monitoring**: `debug_print()` ile execution flow takibi

### Production Usage:
- Debug fonksiyonları preprocessor directive ile disable edilebilir
- Performance impact minimal
- Memory overhead sadece debug build'lerde

## FD MANAGEMENT STRATEGY

### Heredoc FD Tracking:
```c
#define MAX_HEREDOC_FDS 1024    // Maximum simultaneous heredoc count

// FD Registration
register_heredoc_fd(fd);        // Track opened heredoc FD
close_all_heredoc_fds();        // Cleanup all tracked FDs
```

### FD Lifecycle:
1. **Creation**: `handle_heredoc()` temp file oluşturur
2. **Registration**: `register_heredoc_fd()` ile track eder
3. **Usage**: Pipeline/redirection kullanır
4. **Cleanup**: Signal handler veya exit'te kapatır

### Leak Prevention:
- Array capacity check (MAX_HEREDOC_FDS)
- Automatic close on signal (SIGINT/SIGQUIT)
- Process exit cleanup
- Force close if array full

## GLOBAL STATE ACCESS PATTERN

### Singleton Pattern:
```c
// Global state access
t_global *global = get_global();
if (!global) {
    // Handle error - global state not initialized
    return ERROR;
}

// Safe access
if (global->in_child) {
    // Child process behavior
} else {
    // Parent process behavior
}
```

### State Synchronization:
- Parent-child communication via waitpid()
- Signal handlers update global state
- Exit status propagation through processes
- Environment changes reflected immediately

## MEMORY MANAGEMENT

### Allocation Strategy:
- `halloc()` ile garbage collected allocation
- Automatic cleanup on process exit
- Signal-safe cleanup routines
- No manual free() required

### Resource Tracking:
- Heredoc FD array management
- Environment list management
- Command structure allocation
- Token list allocation

## ERROR HANDLING

### Graceful Degradation:
```c
// FD registration failure
if (global->heredoc_fd_count >= MAX_HEREDOC_FDS) {
    close(fd);  // Prevent leak
    // Continue execution with warning
}

// Memory allocation failure
if (!cmd) {
    return NULL;  // Propagate error
}
```

### Debug Information:
- Color-coded debug messages
- Structured output format
- FD tracking information
- State transition logging

## PERFORMANCE CONSIDERATIONS

### Debug Impact:
- **Debug Build**: Verbose output, FD tracking overhead
- **Release Build**: Minimal overhead, optimized execution
- **Conditional Compilation**: Debug code can be disabled

### Memory Usage:
- **FD Array**: Fixed size (MAX_HEREDOC_FDS * sizeof(int))
- **Debug Strings**: Minimal impact on heap
- **Global State**: Single instance, shared across modules

### Real-time Constraints:
- Signal handlers use minimal utils
- FD cleanup operations are fast
- No blocking operations in critical paths

Bu modül, minishell projesi için essential utility functions sağlar ve development sürecinde debugging, production'da ise resource management için kritik rol oynar.
