# SIGNAL MODÜL DOKÜMANTASYONU

## GENEL BAKIŞ

Signal modülü, minishell'de UNIX signal handling sistemini implement eder. Interactive shell davranışını sağlar ve child process'lerin signal'lara doğru tepki vermesini garanti eder. Bash benzeri signal semantiği sunar.

## MODÜLÜNDEKİ DOSYALAR

1. **signal_handler.c** - Ana signal setup ve handler fonksiyonları
2. **sigint.c** - SIGINT (Ctrl+C) özel implementation'u

## SIGNAL TÜRLERİ

```
SIGINT  (2)  - Interrupt (Ctrl+C)
SIGQUIT (3)  - Quit (Ctrl+\)
SIGPIPE (13) - Broken pipe (pipeline'larda)
EOF     (-)  - End of File (Ctrl+D, readline NULL)
```

## SIGNAL DAVRANIŞI

### Interactive Shell (Parent Process):
- **SIGINT**: Mevcut command'ı iptal et, yeni prompt göster
- **SIGQUIT**: Ignore et (hiçbir şey yapma)
- **EOF**: Graceful shell exit

### Child Process:
- **SIGINT**: Process'i terminate et (exit 130)
- **SIGQUIT**: Process'i terminate et (exit 131)
- **Signal Propagation**: Parent'a exit status iletir

## İŞLEV AKIŞI

```
Program Start
├── setup_signals()          # Interactive shell signals
└── Signal Event
    ├── Parent Process
    │   ├── sigint_handler()  # Readline reset + new prompt
    │   └── EOF handling      # Graceful exit
    └── Child Process
        ├── setup_child_signals() # Default signal behavior
        ├── Signal received       # Process termination
        └── cleanup_and_exit()    # Resource cleanup
```

## 1. SIGNAL_HANDLER.C - ANA SIGNAL SİSTEMİ

### Kod Analizi - setup_signals()

```c
void	setup_signals(void)
{
	struct sigaction	sa_int;                 // 1. SIGINT action structure
	struct sigaction	sa_quit;                // 2. SIGQUIT action structure

	// 3. SIGINT (Ctrl+C) handler'ını ayarla
	sa_int.sa_handler = sigint_handler;         // 4. Custom handler function
	sigemptyset(&sa_int.sa_mask);               // 5. Signal mask'ı temizle
	sa_int.sa_flags = SA_RESTART;               // 6. System call'ları restart et
	sigaction(SIGINT, &sa_int, NULL);           // 7. SIGINT handler'ını register et

	// 8. SIGQUIT (Ctrl+\) handler'ını ayarla
	sa_quit.sa_handler = SIG_IGN;               // 9. Ignore handler (default ignore)
	sigemptyset(&sa_quit.sa_mask);              // 10. Signal mask'ı temizle
	sa_quit.sa_flags = 0;                       // 11. No special flags
	sigaction(SIGQUIT, &sa_quit, NULL);         // 12. SIGQUIT handler'ını register et
}
```

**Signal Setup Akışı:**
1. **sigaction Structure**: Modern POSIX signal API kullanır
2. **Custom Handlers**: SIGINT için özel handler, SIGQUIT ignore
3. **SA_RESTART**: Interrupted system call'ları otomatik restart eder
4. **Signal Mask**: Handler çalışırken başka signal'ları bloklamaz

**SA_RESTART Neden Önemli:**
- readline() gibi system call'lar signal ile interrupt edilebilir
- SA_RESTART bu call'ları otomatik olarak yeniden başlatır
- User experience iyileşir (readline bozulmaz)

### Kod Analizi - sigquit_handler()

```c
void	sigquit_handler(int sig)
{
	(void)sig;                                  // 1. Signal number'ı ignore et
	t_global *g_global;                         // 2. Global state pointer'ı
	
	g_global = get_global();                    // 3. Global state'i al
	if (g_global && g_global->in_child)         // 4. Child process'te miyiz?
	{
		cleanup_and_exit();                     // 5. Resource cleanup yap
		exit(131);                              // 6. SIGQUIT exit code (128 + 3)
	}
	// 7. Parent process'te hiçbir şey yapma (ignore)
}
```

**SIGQUIT Behavior:**
- **Interactive Mode**: Ignore edilir (Bash behavior)
- **Child Process**: Process terminate edilir with core dump
- **Exit Code**: 131 (128 + SIGQUIT signal number)

### Kod Analizi - setup_child_signals()

```c
void	setup_child_signals(void)
{
	struct sigaction	sa_int;                 // 1. SIGINT action structure
	struct sigaction	sa_quit;                // 2. SIGQUIT action structure

	// 3. Child'da da aynı handler'ları kullan
	sa_int.sa_handler = sigint_handler;         // 4. SIGINT handler (farklı davranış)
	sigemptyset(&sa_int.sa_mask);               // 5. Signal mask temizle
	sa_int.sa_flags = SA_RESTART;               // 6. System call restart
	sigaction(SIGINT, &sa_int, NULL);           // 7. SIGINT'i register et

	sa_quit.sa_handler = SIG_IGN;               // 8. SIGQUIT ignore
	sigemptyset(&sa_quit.sa_mask);              // 9. Signal mask temizle
	sa_quit.sa_flags = 0;                       // 10. No flags
	sigaction(SIGQUIT, &sa_quit, NULL);         // 11. SIGQUIT'i register et
}
```

**Child Signal Setup:**
- **Same Handlers**: Parent ile aynı handler fonksiyonları
- **Different Behavior**: Handler'lar g_global->in_child'a göre davranır
- **Signal Safety**: Child process'te de signal safety garantisi

### Kod Analizi - handle_eof()

```c
void	handle_eof(void)
{
	t_global *g_global;                         // 1. Global state pointer'ı

	g_global = get_global();                    // 2. Global state'i al
	printf("exit\n");                           // 3. Exit mesajı yazdır
	
	if (g_global)                               // 4. Global state geçerli mi?
	{
		g_global->should_exit = 1;              // 5. Exit flag'ini set et
		g_global->exit_status = 0;              // 6. Success exit code
	}
}
```

**EOF Handling:**
- **Graceful Exit**: Shell'i temiz şekilde sonlandırır
- **Exit Message**: "exit" mesajı yazdırır (Bash behavior)
- **Flag Setting**: Main loop'un exit etmesini sağlar
- **Resource Cleanup**: Main'de yapılır

## 2. SIGINT.C - SIGINT HANDLER İMPLEMENTASYONU

### Kod Analizi - sigint_handler()

```c
void	sigint_handler(int sig)
{
	t_global	*g_global;                      // 1. Global state pointer'ı

	(void)sig;                                  // 2. Signal number'ı ignore et
	g_global = get_global();                    // 3. Global state'i al
	
	if (g_global && g_global->in_child)         // 4. Child process'te miyiz?
	{
		if (g_global->commands)                 // 5. Commands mevcut mu?
			sigint_handler_child_cleanup(g_global->commands); // 6. Child cleanup
		cleanup_and_exit();                     // 7. General cleanup
		exit(130);                              // 8. SIGINT exit code (128 + 2)
	}
	
	// 9. Parent process'te de FD'leri kapat
	close_all_heredoc_fds();                   // 10. Heredoc FD'leri kapat
	
	// 11. Interactive shell davranışı
	write(STDOUT_FILENO, "\n", 1);              // 12. Yeni satır yazdır
	rl_on_new_line();                           // 13. Readline'a yeni satır bildir
	rl_replace_line("", 0);                     // 14. Input buffer'ı temizle
	rl_redisplay();                             // 15. Prompt'u yeniden göster
	
	if (g_global)                               // 16. Global state geçerli mi?
		g_global->exit_status = 130;            // 17. Exit status'u SIGINT yap
}
```

### Kod Analizi - sigint_handler_child_cleanup()

```c
void	sigint_handler_child_cleanup(t_command *cmd)
{
	t_redirect	*redirect;                      // 1. Redirection pointer'ı
	t_list		*node;                          // 2. List node pointer'ı

	node = cmd->redirections;                   // 3. Redirection listesinin başı
	while (node)                                // 4. Her redirection için
	{
		redirect = (t_redirect *)node->content; // 5. Redirection'ı al
		if (redirect->fd > 0)                   // 6. Geçerli FD var mı?
			close(redirect->fd);                // 7. FD'yi kapat
		node = node->next;                      // 8. Sonraki node'a geç
	}
}
```

**SIGINT Handler Akışı:**
1. **Process Type Detection**: Child mi parent mi belirler
2. **Child Cleanup**: Child'da FD'leri kapatır ve exit eder
3. **Parent Behavior**: Interactive shell davranışı gösterir
4. **Readline Integration**: Prompt'u yeniden gösterir
5. **Exit Status**: SIGINT exit code'unu set eder

**Readline Integration:**
- `rl_on_new_line()`: Readline'a cursor'un yeni satırda olduğunu bildirir
- `rl_replace_line("", 0)`: Input buffer'ı temizler
- `rl_redisplay()`: Prompt'u yeniden çizer

## SIGNAL SAFETY VE THREAD SAFETY

### Signal Handler Constraints:
- **Async-Signal-Safe Functions**: Handler'da sadece safe fonksiyonlar
- **No malloc()**: Signal handler'da memory allocation yok
- **Minimal Work**: Handler'da minimum iş yapılır
- **Global State**: Atomic operations veya simple flag'ler

### Safe Functions Used:
```c
write()          // System call - always safe
close()          // System call - safe for FDs
exit()           // Process termination - safe
rl_on_new_line() // Readline function - documented as safe
rl_replace_line() // Readline function - documented as safe
rl_redisplay()   // Readline function - documented as safe
```

### Unsafe Functions Avoided:
- `printf()` → `write()` kullanılır
- `malloc()` → Pre-allocated structures
- `free()` → Cleanup main process'te yapılır

## PROCESS HIERARCHY VE SIGNAL PROPAGATION

### Signal Flow:
```
Terminal (Ctrl+C)
├── Parent Process (Interactive Shell)
│   ├── sigint_handler() → New prompt
│   └── Continue main loop
└── Child Process (Command Execution)
    ├── sigint_handler() → Cleanup & exit(130)
    └── Parent waitpid() → Gets exit status
```

### Exit Status Mapping:
```c
// Normal exit
exit(0)    → Success

// Signal termination
exit(130)  → SIGINT (128 + 2)
exit(131)  → SIGQUIT (128 + 3)
exit(139)  → SIGSEGV (128 + 11)
```

### Child Process Signal Handling:
1. **Signal Reception**: Child process signal alır
2. **Handler Execution**: sigint_handler() çalışır
3. **Resource Cleanup**: FD'ler ve memory temizlenir
4. **Process Exit**: Appropriate exit code ile çıkar
5. **Parent Notification**: waitpid() exit status'u alır

## INTERACTIVE SHELL DAVRANIŞI

### Ctrl+C (SIGINT) Scenario:
```bash
minishell$ ls -la^C
# Signal received → handler called
# Readline reset → new prompt shown
minishell$ _
```

### Ctrl+\ (SIGQUIT) Scenario:
```bash
minishell$ ls -la^\
# Signal ignored in interactive mode
# No action taken
minishell$ ls -la  # Command continues
```

### Ctrl+D (EOF) Scenario:
```bash
minishell$ ^D
exit
# Shell exits gracefully
```

### Child Process Signal:
```bash
minishell$ sleep 10^C
# Child process (sleep) receives SIGINT
# Child exits with 130
# Parent shows new prompt
minishell$ echo $?
130
```

## PIPELINE VE SIGNAL HANDLING

### Pipeline Signal Propagation:
```bash
minishell$ cat file | head -5 | tail -2^C
# All child processes receive SIGINT
# Each child cleans up its resources
# Parent waits for all children
# Last child's exit status (130) returned
```

### Process Group Management:
- **Single Process Group**: Tüm pipeline processes aynı group'ta
- **Signal Broadcasting**: Terminal signal'ı tüm group'a gönderir
- **Coordinated Cleanup**: Her process kendi cleanup'ını yapar

## HEREDOC VE SIGNAL INTERACTION

### Heredoc Signal Handling:
```bash
minishell$ cat <<EOF^C
# Heredoc input interrupted
# Temp files cleaned up
# Process exits with 130
```

### FD Cleanup Priority:
1. **Heredoc FDs**: Temp file FD'leri önce kapatılır
2. **Redirection FDs**: Normal redirection FD'leri
3. **Pipe FDs**: Pipeline FD'leri
4. **Standard FDs**: stdin/stdout/stderr korunur

## DEBUGGING VE DEVELOPMENT

### Signal Handler Debugging:
```c
// Debug output (signal-safe)
write(STDERR_FILENO, "SIGINT received\n", 16);

// FD tracking
printf("DEBUG: Closing FD %d\n", fd);  // NOT signal-safe!
```

### Common Issues:
1. **Readline Corruption**: Prompt bozulması
2. **FD Leaks**: Signal handler'da FD kapatılmaması
3. **Race Conditions**: Signal timing problems
4. **Double Free**: Memory cleanup problems

### Testing Scenarios:
- Ctrl+C during command execution
- Ctrl+C during heredoc input
- Ctrl+C in pipeline
- Multiple rapid Ctrl+C
- Ctrl+D for EOF
- Ctrl+\ (should be ignored)

## PERFORMANCE IMPACT

### Signal Handler Overhead:
- **Minimal**: Handler'lar çok hızlı çalışır
- **No Blocking**: Handler'lar blocking operation yapmaz
- **Resource Efficient**: Minimal resource usage

### Real-time Constraints:
- **Fast Response**: User Ctrl+C'ye hızlı response
- **No Delay**: Signal processing gecikmesi yok
- **Predictable**: Deterministic behavior

## POSIX COMPLIANCE

### POSIX Signal Standards:
- **sigaction()**: Modern POSIX signal API
- **Signal Numbers**: Standard UNIX signal numbers
- **Exit Codes**: POSIX shell exit code conventions
- **Behavior**: POSIX shell signal semantics

### Bash Compatibility:
- **Interactive Behavior**: Bash ile aynı interactive behavior
- **Exit Status**: Bash ile uyumlu exit status'lar
- **Signal Propagation**: Bash ile aynı signal flow
- **Error Messages**: Bash-compatible error handling

Bu modül, minishell'in user interaction ve process management açısından kritik bir component'idir ve UNIX shell'in signal handling semantiğini doğru şekilde implement eder.
