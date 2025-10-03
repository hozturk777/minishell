# EXECUTER MODÜLDOKÜMANTASYONU

## GENEL BAKIŞ

Executer modülü, minishell'de komutların çalıştırılmasından sorumlu olan core modüldür. Parser modülünden gelen komut yapılarını alır ve bunları sistem çağrıları kullanarak çalıştırır.

## MODÜLÜNDEKİ DOSYALAR

1. **executor_utils.c** - Ana executor fonksiyonları ve yardımcı işlevler
2. **execute_external.c** - Dış komutların çalıştırılması
3. **pipeline_execution.c** - Pipeline (|) komutlarının çalıştırılması
4. **executor.c** - Executor yardımcı fonksiyonları ve özel durumlar

## 1. EXECUTOR_UTILS.C - ANA EXECUTOR FONKSİYONLARI

### Ana İşlev Akışı

```
execute_commands()
├── Single Command → execute_single_command()
│   ├── Redirect Only → execute_redirect_command()
│   ├── Built-in → execute_builtin()
│   └── External → execute_external_command()
└── Pipeline → execute_pipeline()
```

### Kod Analizi - execute_commands()

```c
int	execute_commands(t_command *commands, t_global *global)
{
	if (!commands)                               // 1. Komut listesi boş mu kontrol et
		return (0);                              // 2. Boşsa başarı dön
	if (commands->next)                          // 3. Birden fazla komut var mı? (pipeline)
		return (execute_pipeline(commands, global)); // 4. Pipeline çalıştır
	else                                         // 5. Tek komut varsa
		return (execute_single_command(commands, global)); // 6. Tek komut çalıştır
}
```

**Akış Açıklaması:**
1. **Null Check**: Komut listesi boş ise 0 döner (başarı)
2. **Pipeline Detection**: `commands->next` varsa pipeline olduğunu anlar
3. **Route Decision**: Pipeline veya tek komut olarak uygun fonksiyona yönlendirir

**Örnek Senaryolar:**
- `ls -la` → Tek komut → execute_single_command()
- `ls -la | grep txt` → Pipeline → execute_pipeline()
- `""` → Boş → return 0

### Kod Analizi - execute_single_command()

```c
int	execute_single_command(t_command *cmd, t_global *global)
{
	// 1. Sadece redirection olan komutları kontrol et (args yok ama redirection var)
	if ((!cmd || !cmd->args || !cmd->args[0]) && cmd->redirections)
	{
		return (execute_redirect_command(cmd, global)); // 2. Redirection-only çalıştır
	}
	
	// 3. Hiç argüman yoksa hata dön
	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	
	// 4. Built-in komutlar için stdin/stdout'u yedekle
	int originals[2];
	originals[0] = dup(STDIN_FILENO);           // 5. Stdin'i yedekle
	originals[1] = dup(STDOUT_FILENO);          // 6. Stdout'u yedekle
	
	// 7. Built-in komut mu kontrol et
	if (is_builtin(cmd->args[0]))
		return (execute_builtin(cmd, global, originals)); // 8. Built-in çalıştır
	
	// 9. External komut için yedekleri kapat
	close(originals[0]);
	close(originals[1]);
	return (execute_external_command(cmd, global)); // 10. External command çalıştır
}
```

**Akış Açıklaması:**
1. **Redirection-Only Check**: `> file` gibi sadece redirection olan komutları yakalar
2. **Null Command Check**: Argüman yoksa hata döner
3. **FD Backup**: Built-in komutlar için stdin/stdout'u yedekler
4. **Command Type Detection**: Built-in mi external mi belirler
5. **Execution**: Uygun fonksiyona yönlendirir

**Örnek Senaryolar:**
- `> output.txt` → Redirection-only → execute_redirect_command()
- `echo hello` → Built-in → execute_builtin() 
- `ls -la` → External → execute_external_command()

### Kod Analizi - execute_redirect_command()

```c
static void	handle_redirect_child_process(t_command *cmd, t_global *global)
{
    setup_child_signals();                      // 1. Child process sinyallerini ayarla
    global->in_child = 1;                       // 2. Child process flag'ini set et
    setup_redirections(cmd);                    // 3. Redirection'ları uygula
    cleanup_and_exit();                         // 4. Cleanup yap
    exit(0);                                    // 5. Child'dan çık
}

static int	wait_for_redirect_process(pid_t pid)
{
	int	status;

    waitpid(pid, &status, 0);                   // 1. Child process'i bekle
    
    if (WIFSIGNALED(status))                    // 2. Signal ile mi öldü kontrol et
    {
		int signal_num = WTERMSIG(status);      // 3. Signal numarasını al
        if (signal_num == SIGINT)               // 4. SIGINT (Ctrl+C) kontrolü
		return (130);                           // 5. SIGINT exit code: 130
        else if (signal_num == SIGQUIT)         // 6. SIGQUIT (Ctrl+\) kontrolü  
		return (131);                           // 7. SIGQUIT exit code: 131
        else                                    // 8. Diğer sinyaller
		return (128 + signal_num);              // 9. 128 + signal numarası
    }
    return (WEXITSTATUS(status));               // 10. Normal exit status dön
}

int	execute_redirect_command(t_command *cmd, t_global *global)
{
	pid_t	pid;

	pid = fork();                               // 1. Child process oluştur
	if (pid == 0)                               // 2. Child process'te miyiz?
	{
		handle_redirect_child_process(cmd, global); // 3. Child işlemlerini yap
	}
	else if (pid > 0)                           // 4. Parent process'te miyiz?
	{
		return (wait_for_redirect_process(pid)); // 5. Child'ı bekle ve sonucu dön
	}
	return (1);                                 // 6. Fork hatası - hata dön
}
```

**Akış Açıklaması:**
1. **Fork Process**: Yeni bir child process oluşturur
2. **Child Setup**: Child'da sinyalleri ayarlar ve redirection'ları uygular
3. **Parent Wait**: Parent child'ı bekler ve exit status'unu döner
4. **Signal Handling**: Child signal ile öldürülürse uygun exit code döner

**Örnek Senaryolar:**
- `> output.txt` → Dosya oluşturur/temizler
- `>> append.txt` → Dosyaya append eder
- `< input.txt` → Input'u dosyadan okur

## 2. EXECUTE_EXTERNAL.C - DIŞ KOMUTLARIN ÇALIŞTIRILMASI

### Kod Analizi - execute_external_command()

```c
static void	handle_external_child_process(t_command *cmd, t_global *global, char *path)
{
    setup_child_signals();                      // 1. Child sinyallerini default'a çevir
    global->in_child = 1;                       // 2. Child process flag'ini set et
    setup_redirections(cmd);                    // 3. Redirection'ları uygula
    
    // 4. Komutu execve ile çalıştır
    execve(path, cmd->args, env_list_to_array(global->env_list));
    
    // 5. execve başarısızsa buraya gelir
    perror("execve");                           // 6. Hata mesajı yazdır
    cleanup_and_exit();                         // 7. Cleanup yap
    exit(127);                                  // 8. Command not found exit code
}

static int	wait_for_external_process(pid_t pid, char *path)
{
	int	status;
	(void) path;                                // 1. Path kullanılmıyor (void cast)

    waitpid(pid, &status, 0);                   // 2. Child process'i bekle
    
    if (WIFSIGNALED(status))                    // 3. Signal ile mi öldürüldü?
    {
        int signal_num = WTERMSIG(status);      // 4. Signal numarasını al
        if (signal_num == SIGINT)               // 5. SIGINT kontrolü
		return (130);                           // 6. SIGINT exit code
        else if (signal_num == SIGQUIT)         // 7. SIGQUIT kontrolü
		return (131);                           // 8. SIGQUIT exit code
        else                                    // 9. Diğer sinyaller
		return (128 + signal_num);              // 10. 128 + signal number
    }
    return (WEXITSTATUS(status));               // 11. Normal exit status
}

int	execute_external_command(t_command *cmd, t_global *global)
{
	char	*path;
	pid_t	pid;

	// 1. PATH environment'ta komutu ara
	path = find_command_path(cmd->args[0], global->env_list);
	if (!path)                                  // 2. Komut bulunamadı mı?
	{
		printf("minishell: %s: command not found\n", cmd->args[0]); // 3. Hata mesajı
		return (127);                           // 4. Command not found exit code
	}
	
	pid = fork();                               // 5. Child process oluştur
	if (pid == 0)                               // 6. Child process'te miyiz?
	{
		handle_external_child_process(cmd, global, path); // 7. Child işlemlerini yap
		exit (127);                             // 8. Fallback exit
	}
	else if (pid > 0)                           // 9. Parent process'te miyiz?
	{
		return (wait_for_external_process(pid, path)); // 10. Child'ı bekle
	}
	else                                        // 11. Fork hatası
	{
		perror("fork");                         // 12. Fork hata mesajı
		free(path);                             // 13. Path'i serbest bırak
		clear_garbage();                        // 14. Garbage collection
		return (1);                             // 15. Hata kodu dön
	}
}
```

**Akış Açıklaması:**
1. **Path Resolution**: PATH environment'ta komutu arar
2. **Fork Process**: Child process oluşturur
3. **Child Execution**: Child'da execve ile komutu çalıştırır
4. **Parent Wait**: Parent child'ı bekler ve sonucu döner
5. **Error Handling**: Komut bulunamama ve fork hatalarını işler

**Örnek Senaryolar:**
- `ls -la` → /bin/ls bulur ve çalıştırır
- `nonexistent` → Command not found (127)
- `cat file.txt` → /bin/cat ile file.txt'yi okur

## 3. PIPELINE_EXECUTION.C - PIPELINE KOMUTLARININ ÇALIŞTIRILMASI

### Pipeline İşlev Akışı

```
execute_pipeline()
├── preprocess_heredocs()     # Heredoc'ları önceden işle
├── count_pipeline_commands() # Komut sayısını say
├── execute_all_pipeline_commands() # Tüm komutları async çalıştır
│   └── execute_pipeline_command_async() # Her komut için
└── wait_for_all_pipeline_processes() # Tüm process'leri bekle
```

### Kod Analizi - execute_pipeline()

```c
int	execute_pipeline(t_command *commands, t_global *global)
{
    pid_t	*pids;                              // 1. Process ID'leri için array
    int		cmd_count;                          // 2. Komut sayısı
    int		last_status;                        // 3. Son komutun exit status'u

    // 4. Heredoc'ları main process'te önceden işle
    if (preprocess_heredocs(commands, global) == -1)
        return (1);                             // 5. Heredoc hatası varsa dön
        
    cmd_count = count_pipeline_commands(commands); // 6. Komut sayısını say
    pids = halloc(sizeof(pid_t) * cmd_count);   // 7. PID array'i allocate et
    if (!pids)                                  // 8. Allocation hatası kontrolü
        return (1);
        
    // 9. Tüm pipeline komutlarını çalıştır
    if (execute_all_pipeline_commands(commands, global, pids, cmd_count) == -1)
        return (1);                             // 10. Çalıştırma hatası varsa dön
        
    // 11. Tüm process'leri bekle ve son status'u al
    last_status = wait_for_all_pipeline_processes(pids, cmd_count);
    return (last_status);                       // 12. Son komutun exit status'unu dön
}
```

### Kod Analizi - execute_all_pipeline_commands()

```c
static int	execute_all_pipeline_commands(t_command *commands, t_global *global, pid_t *pids, int cmd_count)
{
    t_command	*current;                       // 1. Mevcut komut pointer'ı
    int			pipe_fd[2];                     // 2. Pipe file descriptor'ları
    int			prev_fd;                        // 3. Önceki pipe'ın read end'i
    int			i;                              // 4. Index counter

    current = commands;                         // 5. İlk komuttan başla
    prev_fd = 0;                               // 6. İlk komut stdin'den okur
    i = 0;                                     // 7. Index'i sıfırla
    
    while (current && i < cmd_count)           // 8. Her komut için döngü
    {
        // 9. Sonraki komut varsa pipe oluştur
        if (current->next && pipe(pipe_fd) == -1)
        {
            perror("pipe");                     // 10. Pipe hatası
            return (-1);
        }
        
        // 11. Komutu async olarak çalıştır
        pids[i] = execute_pipeline_command_async(current, global, prev_fd, pipe_fd);
        if (pids[i] == -1)                     // 12. Çalıştırma hatası kontrolü
            return (-1);
            
        if (prev_fd != 0)                      // 13. Önceki pipe'ı kapat
            close(prev_fd);
            
        if (current->next)                     // 14. Sonraki komut varsa
        {
            close(pipe_fd[1]);                 // 15. Write end'i kapat
            prev_fd = pipe_fd[0];              // 16. Read end'i bir sonraki için sakla
        }
        
        current = current->next;               // 17. Sonraki komuta geç
        i++;                                   // 18. Index'i artır
    }
    return (0);                                // 19. Başarı dön
}
```

### Kod Analizi - wait_for_all_pipeline_processes()

```c
static int	wait_for_all_pipeline_processes(pid_t *pids, int cmd_count)
{
    int	i;                                     // 1. Index counter
    int	status;                                // 2. Wait status
    int	last_status;                           // 3. Son komutun status'u

    i = 0;                                     // 4. Index'i sıfırla
    last_status = 0;                           // 5. Default status
    
    while (i < cmd_count)                      // 6. Her process için döngü
    {
        if (pids[i] > 0)                       // 7. Geçerli PID kontrolü
        {
            waitpid(pids[i], &status, 0);      // 8. Process'i bekle
            if (i == cmd_count - 1)            // 9. Son komut mu?
            {
                last_status = get_process_exit_status(status); // 10. Exit status'u al
            }
        }
        i++;                                   // 11. Sonraki process'e geç
    }
    return (last_status);                      // 12. Son komutun status'unu dön
}
```

**Pipeline Akış Açıklaması:**
1. **Preparation**: Heredoc'ları önceden işler ve komut sayısını sayar
2. **Pipe Creation**: Her komut arası için pipe oluşturur
3. **Async Execution**: Tüm komutları paralel olarak başlatır
4. **Process Management**: Tüm child process'leri bekler
5. **Status Collection**: Son komutun exit status'unu döner

**Örnek Senaryolar:**
- `ls -la | grep txt` → ls çıktısını grep'e pipe'lar
- `cat file | head -5 | tail -2` → 3-stage pipeline
- `echo hello | cat | cat` → Basit pipeline chain

## 4. EXECUTOR.C - ÖZEL DURUMLAR VE YARDIMCI FONKSİYONLAR

Bu dosya çoğunlukla commented out kod içeriyor ve development sürecindeki eski implementasyonları saklar.

### Önemli Fonksiyonlar:

#### preprocess_heredocs()
```c
int	preprocess_heredocs(t_command *commands, t_global *global)
{
	t_command	*current;                       // 1. Mevcut komut
	t_list		*redirect_node;                 // 2. Redirection node'u
	t_redirect	*redirect;                      // 3. Redirection yapısı

	current = commands;                         // 4. İlk komuttan başla
	while (current)                            // 5. Her komut için
	{
		if (current->redirections)             // 6. Redirection var mı?
		{
			redirect_node = current->redirections; // 7. Redirection listesi
			while (redirect_node)              // 8. Her redirection için
			{
				redirect = (t_redirect *)redirect_node->content; // 9. Cast et
				if (redirect && redirect->type == T_HEREDOC) // 10. Heredoc mu?
				{
					// 11. Heredoc'u main process'te işle
					redirect->fd = handle_heredoc(redirect);
					if (redirect->fd == -1)    // 12. Hata kontrolü
						return (-1);
				}
				redirect_node = redirect_node->next; // 13. Sonraki redirection
			}
		}
		current = current->next;               // 14. Sonraki komut
	}
	return (0);                                // 15. Başarı
}
```

#### execute_pipeline_command_async()
Bu fonksiyon pipeline'daki her komutu ayrı bir child process'te çalıştırır:

1. **Fork**: Child process oluşturur
2. **Setup**: Child'da sinyalleri ve pipe'ları ayarlar
3. **Execution**: Built-in veya external komut çalıştırır
4. **Return**: Parent'a PID döner

## KULLANIM SENARYOLARI

### 1. Tek Komut Çalıştırma
```bash
minishell$ ls -la
# execute_commands() → execute_single_command() → execute_external_command()
```

### 2. Built-in Komut
```bash
minishell$ echo hello world
# execute_commands() → execute_single_command() → execute_builtin()
```

### 3. Pipeline Komutlar
```bash
minishell$ ls -la | grep txt | wc -l
# execute_commands() → execute_pipeline() → 3 async process
```

### 4. Redirection
```bash
minishell$ > output.txt
# execute_commands() → execute_single_command() → execute_redirect_command()
```

### 5. Heredoc ile Pipeline
```bash
minishell$ cat <<EOF | grep hello
# preprocess_heredocs() → execute_pipeline()
```

## HATA YÖNETİMİ

### Exit Kodları:
- **0**: Başarı
- **1**: Genel hata
- **127**: Komut bulunamadı
- **130**: SIGINT (Ctrl+C)
- **131**: SIGQUIT (Ctrl+\)
- **128+N**: Signal N ile sonlandırıldı

### Hata Durumları:
1. **Command Not Found**: PATH'te komut bulunamadığında
2. **Fork Error**: Process oluşturulamadığında
3. **Pipe Error**: Pipeline oluşturulamadığında
4. **Execve Error**: Komut çalıştırılamadığında
5. **Heredoc Error**: Heredoc işlenemediğinde

## BELLEK YÖNETİMİ

1. **Path Allocation**: find_command_path() ile allocate edilen path'ler
2. **PID Arrays**: Pipeline için allocate edilen PID array'leri
3. **FD Management**: Pipe ve redirection FD'lerinin doğru kapatılması
4. **Garbage Collection**: cleanup_and_exit() ve clear_garbage() çağrıları

## SINYAL İŞLEME

### Parent Process:
- SIGINT/SIGQUIT'i ignore eder
- Child'lardan gelen sinyalleri exit code'a çevirir

### Child Process:
- setup_child_signals() ile default davranışa döner
- SIGINT/SIGQUIT'i normal şekilde işler

## GÜVENLIK VE STABİLİTE

1. **Process Isolation**: Her komut ayrı process'te çalışır
2. **FD Leaks**: Kullanılmayan FD'ler kapatılır
3. **Signal Safety**: Child process'ler signal-safe
4. **Error Recovery**: Hata durumlarında temizlik yapılır

Bu modül, minishell'in komut çalıştırma engine'idir ve UNIX process management, IPC (pipes), ve signal handling konularının pratik uygulamasını gösterir.
