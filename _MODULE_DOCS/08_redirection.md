# REDIRECTION MODÜL DOKÜMANTASYONU

## GENEL BAKIŞ

Redirection modülü, minishell'de input/output yönlendirme işlemlerini yönetir. UNIX shell'in redirection semantiğini implement eder: dosyaya/dosyadan okuma, yazma, append ve heredoc işlemleri.

## MODÜLÜNDEKİ DOSYALAR

1. **redirections.c** - Ana redirection setup ve multiple handling
2. **redirections2.c** - Heredoc implementation ve pipeline FD setup

## REDIRECTION TÜRLERİ

```
>   T_REDIRECT_OUT  - Output redirection (truncate)
>>  T_APPEND        - Output redirection (append)
<   T_REDIRECT_IN   - Input redirection
<<  T_HEREDOC       - Here document
|   T_PIPE          - Pipeline (başka modülde)
```

## REDIRECTION YAPISI

```c
typedef struct s_redirect
{
    int                 type;       // Redirection türü (T_REDIRECT_OUT, etc.)
    char               *filename;   // Hedef dosya adı veya heredoc delimiter
    int                 fd;         // File descriptor (heredoc için)
    struct s_redirect  *next;       // Sonraki redirection
} t_redirect;
```

## İŞLEV AKIŞI

```
setup_redirections()
├── handle_multiple_heredocs()    # Heredoc'ları önce işle
└── handle_single_redirection()   # Diğer redirection'ları işle
    ├── open_redirection_file()   # Dosya aç
    └── dup2()                    # FD'yi redirect et

Heredoc Flow
├── create_temp_filename()        # Geçici dosya adı oluştur
├── readline() loop               # Kullanıcıdan input al
├── expand_with_heredoc()         # Variable expansion
└── unlink() temp file           # Geçici dosyayı sil
```

## 1. REDIRECTIONS.C - ANA REDIRECTION SİSTEMİ

### Kod Analizi - setup_redirections()

```c
void	setup_redirections(t_command *cmd)
{
	t_list		*current;                       // 1. Redirection listesi traversal
	t_redirect	*redirect;                      // 2. Mevcut redirection

	if (!cmd || !cmd->redirections)             // 3. Geçerlilik kontrolü
		return ;                                // 4. Redirection yoksa çık

	handle_multiple_heredocs(cmd);              // 5. Önce tüm heredoc'ları işle
	
	current = cmd->redirections;                // 6. Redirection listesinin başı
	while (current)                             // 7. Her redirection için
	{
		redirect = (t_redirect *)current->content; // 8. Redirection'ı al
		if (redirect && redirect->type != T_HEREDOC) // 9. Heredoc değilse
		{
			handle_single_redirection(redirect); // 10. Normal redirection işle
		}
		current = current->next;                // 11. Sonraki redirection'a geç
	}
}
```

**Akış Açıklaması:**
1. **Validation**: Komut ve redirection listesi varlığını kontrol eder
2. **Heredoc Priority**: Heredoc'ları önce işler (özel handling gerekir)
3. **Sequential Processing**: Diğer redirection'ları sırayla işler
4. **Type Filtering**: Heredoc'ları atlar (zaten işlenmiş)

**Redirection Sıralaması Neden Önemli:**
- Heredoc'lar temporary file oluşturur, önce hazır olmalı
- Multiple redirection'larda son redirection geçerli olur
- Pipeline'da FD management kritik

### Kod Analizi - handle_single_redirection()

```c
static int	open_redirection_file(t_redirect *redirect)
{
	int	fd;                                     // 1. File descriptor

	if (redirect->type == T_REDIRECT_IN)        // 2. Input redirection
		fd = open(redirect->filename, O_RDONLY); // 3. Read-only aç
	else if (redirect->type == T_REDIRECT_OUT)  // 4. Output redirection
		fd = open(redirect->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644); // 5. Write/create/truncate
	else if (redirect->type == T_APPEND)        // 6. Append redirection
		fd = open(redirect->filename, O_WRONLY | O_CREAT | O_APPEND, 0644); // 7. Write/create/append
	else                                        // 8. Bilinmeyen tür
		fd = -1;                                // 9. Hata
		
	if (fd == -1)                               // 10. Dosya açma hatası
	{
		perror(redirect->filename);             // 11. Hata mesajı yazdır
	}
	return (fd);                                // 12. FD'yi dön
}

void	handle_single_redirection(t_redirect *redirect)
{
	int	fd;                                     // 1. File descriptor

	fd = open_redirection_file(redirect);       // 2. Dosyayı aç
	
	if (redirect->type == T_REDIRECT_IN)        // 3. Input redirection ise
		dup2(fd, STDIN_FILENO);                 // 4. Stdin'i dosyaya yönlendir
	else                                        // 5. Output redirection ise
		dup2(fd, STDOUT_FILENO);                // 6. Stdout'u dosyaya yönlendir
		
	if (fd > 2)                                 // 7. Standard FD'ler değilse
	{
		close(fd);                              // 8. Orijinal FD'yi kapat
	}	
}
```

**Akış Açıklaması:**
1. **File Opening**: Redirection türüne göre dosyayı uygun modda açar
2. **FD Duplication**: dup2() ile stdin/stdout'u dosyaya yönlendirir
3. **Resource Cleanup**: Kullanılmayan FD'leri kapatır
4. **Error Handling**: Dosya açma hatalarını yakalar

**File Open Modes:**
- **T_REDIRECT_IN**: `O_RDONLY` - Sadece okuma
- **T_REDIRECT_OUT**: `O_WRONLY | O_CREAT | O_TRUNC` - Yazma, oluştur, truncate
- **T_APPEND**: `O_WRONLY | O_CREAT | O_APPEND` - Yazma, oluştur, append

### Kod Analizi - handle_multiple_heredocs()

```c
static int	process_heredoc_redirects(t_list *cur)
{
	t_redirect	*redirect;                      // 1. Mevcut redirection
	int			last_heredoc_fd;                // 2. Son heredoc FD'si

	last_heredoc_fd = -1;                       // 3. FD'yi başlat
	while (cur)                                 // 4. Her redirection için
	{
		redirect = (t_redirect *)cur->content;  // 5. Redirection'ı al
		if (redirect && redirect->type == T_HEREDOC) // 6. Heredoc mu?
		{
			// 7. Önceki heredoc FD'sini kapat (sadece en son geçerli)
			if (last_heredoc_fd != -1 && last_heredoc_fd != redirect->fd)
				close(last_heredoc_fd);         // 8. Eski FD'yi kapat
			
			if (redirect->fd > 0)               // 9. Pre-processed heredoc
			{
				last_heredoc_fd = redirect->fd; // 10. Mevcut FD'yi kullan
			}
			else                                // 11. Yeni heredoc
				last_heredoc_fd = handle_heredoc(redirect); // 12. Heredoc'u işle
		}
		cur = cur->next;                        // 13. Sonraki redirection'a geç
	}
	return (last_heredoc_fd);                   // 14. Son heredoc FD'sini dön
}

void	handle_multiple_heredocs(t_command *cmd)
{
	int	fd;                                     // 1. Heredoc FD'si

	if (!cmd->redirections)                     // 2. Redirection yok mu?
		return ;                                // 3. Çık
		
	fd = process_heredoc_redirects(cmd->redirections); // 4. Heredoc'ları işle
	if (fd != -1)                               // 5. Geçerli FD var mı?
	{
		dup2(fd, STDIN_FILENO);                 // 6. Stdin'i heredoc'a yönlendir
		close(fd);                              // 7. Orijinal FD'yi kapat
	}
}
```

**Multiple Heredoc Mantığı:**
1. **Last Wins**: Birden fazla heredoc varsa son geçerli olur
2. **Resource Management**: Önceki heredoc FD'leri kapatılır
3. **Pipeline Awareness**: Pre-processed heredoc'ları tanır
4. **Stdin Redirection**: Son heredoc stdin'e yönlendirilir

**Örnek Senaryolar:**
- `cat <<EOF1 <<EOF2` → Sadece EOF2 heredoc'u geçerli
- Pipeline'da heredoc → Pre-processed FD kullanılır
- Single heredoc → Normal heredoc processing

## 2. REDIRECTIONS2.C - HEREDOC İMPLEMENTASYONU

### Kod Analizi - handle_heredoc()

```c
int	handle_heredoc(t_redirect *redirect)
{
	char		*temp_filename;                 // 1. Geçici dosya adı
	static int	heredoc_count = 0;              // 2. Heredoc sayacı (unique names için)
	t_global	*global;                        // 3. Global state

	global = get_global();                      // 4. Global state'i al
	if (!redirect || redirect->type != T_HEREDOC) // 5. Geçerlilik kontrolü
		return (-1);                            // 6. Geçersizse hata dön
		
	temp_filename = create_temp_filename(heredoc_count++); // 7. Unique temp dosya adı
	if (!temp_filename)                         // 8. Dosya adı oluşturulamadı mı?
		return (-1);                            // 9. Hata dön
		
	// 10. Temp dosyayı yazma modunda aç
	redirect->fd = open(temp_filename, O_CREAT | O_WRONLY | O_TRUNC, 0600);
	if (redirect->fd == -1)                     // 11. Dosya açma hatası
		return (-1);                            // 12. Hata dön
		
	process_heredoc_lines(redirect, redirect->fd, global); // 13. User input'u işle
	close(redirect->fd);                        // 14. Write FD'yi kapat
	
	redirect->fd = open(temp_filename, O_RDONLY); // 15. Read modunda yeniden aç
	unlink(temp_filename);                      // 16. Temp dosyayı filesystem'den sil
	
	// 17. Heredoc FD'yi global state'e kaydet (debug)
	if (redirect->fd > 2)
	{
		register_heredoc_fd(redirect->fd);      // 18. FD tracking için kaydet
		printf("DEBUG: Registered heredoc FD %d\n", redirect->fd);
	}
	
	return (redirect->fd);                      // 19. Read FD'yi dön
}
```

### Kod Analizi - process_heredoc_lines()

```c
static void	process_heredoc_lines(t_redirect *redirect, int fd, t_global *global)
{
	char	*line;                              // 1. Kullanıcı input satırı

	while ((line = add_garbage(readline("> "))) != NULL) // 2. Prompt ile input al
	{
		if (ft_strcmp(line, redirect->filename) == 0) // 3. Delimiter bulundu mu?
			break ;                             // 4. Heredoc'u sonlandır
			
		line = expand_with_heredoc(line, global); // 5. Variable expansion uygula
		write(fd, line, ft_strlen(line));       // 6. Satırı dosyaya yaz
		write(fd, "\n", 1);                     // 7. Newline ekle
	}
}
```

### Kod Analizi - create_temp_filename()

```c
static char	*create_temp_filename(int heredoc_count)
{
	char	*pid_str;                           // 1. Process ID string'i
	char	*count_str;                         // 2. Count string'i
	char	*temp1, *temp2, *result;            // 3. Geçici string'ler

	pid_str = ft_itoa(getpid());                // 4. Process ID'yi string'e çevir
	count_str = ft_itoa(heredoc_count);         // 5. Count'u string'e çevir
	if (!pid_str || !count_str)                 // 6. Conversion başarısız mı?
	{
		return (NULL);                          // 7. Hata dön
	}
	
	// 8. String concatenation: "/tmp/heredoc_" + PID + "_" + COUNT
	temp1 = ft_strjoin("/tmp/heredoc_", pid_str);
	temp2 = ft_strjoin(temp1, "_");
	result = ft_strjoin(temp2, count_str);
	
	return (result);                            // 9. Final filename'i dön
}
```

**Heredoc İşlem Akışı:**
1. **Temp File Creation**: Unique temp dosya oluşturur
2. **User Input Loop**: Delimiter'a kadar user input alır
3. **Variable Expansion**: Her satırda variable expansion yapar
4. **File Writing**: Input'u temp dosyaya yazar
5. **File Reopen**: Read modunda yeniden açar
6. **Cleanup**: Temp dosyayı filesystem'den siler

**Unique Filename Strategy:**
- Format: `/tmp/heredoc_<PID>_<COUNT>`
- PID: Process uniqueness sağlar
- COUNT: Aynı process'te multiple heredoc support

### Kod Analizi - setup_pipeline_fds()

```c
void	setup_pipeline_fds(t_command *cmd, int prev_fd, int *pipe_fd)
{
	if (prev_fd != 0)                           // 1. Önceki pipe'dan input var mı?
	{
		dup2(prev_fd, STDIN_FILENO);            // 2. Stdin'i önceki pipe'a yönlendir
		close(prev_fd);                         // 3. Orijinal FD'yi kapat
	}
	
	if (cmd->next)                              // 4. Sonraki komut var mı? (pipeline devam ediyor)
	{
		close(pipe_fd[0]);                      // 5. Read end'i kapat (kullanmayacağız)
		dup2(pipe_fd[1], STDOUT_FILENO);        // 6. Stdout'u pipe'ın write end'ine yönlendir
		close(pipe_fd[1]);                      // 7. Orijinal write FD'yi kapat
	}
}
```

**Pipeline FD Setup Mantığı:**
1. **Input Setup**: Önceki komutun output'unu stdin'e bağlar
2. **Output Setup**: Sonraki komut için output'u pipe'a yönlendirir
3. **FD Cleanup**: Kullanılmayan FD'leri kapatır
4. **Conditional**: Son komut pipe output kurmazı

## KULLANIM SENARYOLARI

### 1. Input Redirection
```bash
minishell$ cat < input.txt
# handle_single_redirection() → T_REDIRECT_IN
# open("input.txt", O_RDONLY) → fd
# dup2(fd, STDIN_FILENO) → stdin dosyadan okur
```

### 2. Output Redirection
```bash
minishell$ echo hello > output.txt
# handle_single_redirection() → T_REDIRECT_OUT
# open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644) → fd
# dup2(fd, STDOUT_FILENO) → stdout dosyaya yazar
```

### 3. Append Redirection
```bash
minishell$ echo world >> output.txt
# handle_single_redirection() → T_APPEND
# open("output.txt", O_WRONLY | O_CREAT | O_APPEND, 0644) → fd
# dup2(fd, STDOUT_FILENO) → stdout dosyaya append eder
```

### 4. Heredoc
```bash
minishell$ cat <<EOF
> Hello $USER
> This is a test
> EOF
# handle_heredoc() → temp file oluştur
# readline() loop → kullanıcı input'u al
# expand_with_heredoc() → $USER expand edilir
# dup2(heredoc_fd, STDIN_FILENO) → stdin heredoc'tan okur
```

### 5. Multiple Redirections
```bash
minishell$ cat < input.txt > output.txt
# setup_redirections() → her redirection'ı sırayla işler
# İlk: stdin dosyadan okur
# İkinci: stdout dosyaya yazar

minishell$ echo test <<EOF1 <<EOF2
> data1
> EOF1
> data2
> EOF2
# handle_multiple_heredocs() → sadece son heredoc (EOF2) geçerli
```

### 6. Pipeline ile Redirection
```bash
minishell$ cat file.txt | grep pattern > result.txt
# Pipeline: setup_pipeline_fds() her komut için
# Redirection: setup_redirections() grep için output redirection
```

## FİLE DESCRIPTOR YÖNETİMİ

### FD Lifecycle:
1. **Open**: Dosya açma ile FD oluşturma
2. **Duplicate**: dup2() ile stdin/stdout'a yönlendirme
3. **Close**: Orijinal FD'yi kapatma
4. **Cleanup**: Process sonunda tüm FD'leri kapatma

### FD Tracking:
- **Standard FDs**: 0 (stdin), 1 (stdout), 2 (stderr)
- **File FDs**: 3+ dosya operations için
- **Pipe FDs**: Pipeline operations için geçici
- **Heredoc FDs**: Temporary file operations için

### Memory ve Resource Management:
```c
// Heredoc temp files
unlink(temp_filename);    // Filesystem'den sil ama FD açık kalır

// FD cleanup
if (fd > 2)              // Standard FD'leri kapama
    close(fd);

// Pipeline FD management
close(pipe_fd[0]);       // Kullanılmayan pipe end'leri kapat
```

## HATA YÖNETİMİ

### Hata Durumları:
1. **File Not Found**: Input redirection'da dosya yok
2. **Permission Denied**: Dosya erişim izni yok
3. **Disk Full**: Output redirection'da disk dolu
4. **FD Exhaustion**: Çok fazla açık dosya
5. **Temp File Creation**: Heredoc temp dosya oluşturulamıyor

### Error Recovery:
```c
if (fd == -1) {
    perror(redirect->filename);  // System error message
    // Process genelde devam eder, komut fail olur
}
```

### Exit Codes:
- **0**: Başarı
- **1**: Genel redirection hatası
- **126**: Permission denied
- **127**: File not found

## GÜVENLIK CONSIDERATIONS

### File Permissions:
- **Heredoc**: 0600 (owner read/write only)
- **Output Files**: 0644 (owner rw, group/other r)
- **Temp Directory**: /tmp (world writable ama sticky bit)

### Race Conditions:
- **Temp Files**: PID + counter ile unique naming
- **Signal Safety**: Heredoc süresinde signal handling
- **FD Leaks**: Proper cleanup her durumda

## PERFORMANCE CONSIDERATIONS

### Heredoc Optimization:
- **Temp Files**: Memory yerine disk kullanır (büyük heredoc için)
- **Immediate Unlink**: Filesystem entry silinir ama FD açık kalır
- **Variable Expansion**: Her satır için expansion (costly)

### FD Efficiency:
- **Minimal FDs**: Sadece gerekli FD'ler açık tutulur
- **Early Close**: Kullanılmayan FD'ler hemen kapatılır
- **Pipe Efficiency**: Pipeline'da FD sharing optimize edilir

## BASH COMPATIBILITY

### Redirection Semantics:
- **Order Independence**: `cmd >file <input` ve `cmd <input >file` aynı
- **Multiple Same Type**: Son redirection geçerli (`cmd >file1 >file2`)
- **Error Handling**: Bash ile uyumlu error messages
- **Heredoc Expansion**: Variable expansion bash ile aynı

### Special Cases:
- **Heredoc Quoting**: Delimiter quote'lanırsa expansion yapılmaz
- **Noclobber**: Üzerine yazma kontrolü (minishell'de yok)
- **File Descriptor Redirection**: `cmd 2>&1` (minishell'de limited)

Bu modül, minishell'in UNIX I/O redirection semantiğini doğru bir şekilde implement eden kritik bir component'tir ve shell'in file system integration'ı için temel infrastructure sağlar.
