# MINISHELL EXECUTOR MODÜLÜ - DETAYLI ANALİZ VE AKIŞ ŞEMASI

## 📋 İÇİNDEKİLER
1. [Genel Bakış](#genel-bakış)
2. [Dosya Yapısı](#dosya-yapısı)
3. [Ana Fonksiyonlar ve Akış Şeması](#ana-fonksiyonlar-ve-akış-şeması)
4. [Built-in Komutlar](#built-in-komutlar)
5. [External Komutlar](#external-komutlar)
6. [Pipeline İşlemleri](#pipeline-işlemleri)
7. [Yönlendirmeler (Redirections)](#yönlendirmeler)
8. [Çevre Değişkenleri](#çevre-değişkenleri)
9. [Örnek Senaryolar](#örnek-senaryolar)

---

## 🔍 GENEL BAKIŞ

Minishell'in executor modülü, parser tarafından analiz edilmiş komutları gerçek işlemlere dönüştüren kritik bileşendir. Bu modül 5 ana dosyadan oluşur:

```
executer/
├── executor.c      → Ana executor logic
├── builtins.c      → Built-in komutlar (pwd, echo, env, exit)
├── builtins2.c     → Diğer built-in komutlar (cd, export, unset)
├── path_utils.c    → Komut yolu çözümleme
└── redirections.c  → Giriş/çıkış yönlendirmeleri
```

---

## 📁 DOSYA YAPISI

### 1. **executor.c** - Ana Executor Motoru
```c
// Ana giriş noktası - komut listesini işler
int execute_commands(t_command *commands, t_global *global)

// Tek komut çalıştırma
int execute_single_command(t_command *cmd, t_global *global)

// Harici program çalıştırma
int execute_external_command(t_command *cmd, t_global *global)

// Pipeline (|) işlemleri
int execute_pipeline(t_command *commands, t_global *global)
int execute_pipeline_command(t_command *cmd, t_global *global, int prev_fd, int *pipe_fd)
```

### 2. **builtins.c** - Temel Built-in Komutlar
```c
// Built-in kontrol fonksiyonları
int is_builtin(char *command)
int execute_builtin(t_command *cmd, t_global *global)

// Temel built-in komutlar
int builtin_pwd(void)
int builtin_pwd_global(t_global *global)
int builtin_echo(char **args)
int builtin_env(t_env *env_list)
int builtin_exit(char **args, t_global *global)
```

### 3. **builtins2.c** - Gelişmiş Built-in Komutlar
```c
// Dizin değiştirme
int builtin_cd(char **args, t_global *global)

// Çevre değişkeni export/unset
int builtin_export(char **args, t_global *global)
int builtin_unset(char **args, t_global *global)

// Yardımcı fonksiyonlar
void update_pwd_env(t_global *global)
void print_export_env(t_env *env_list)
void set_env_var(t_global *global, char *key, char *value)
char *resolve_logical_path(char *current_pwd, char *path)
```

### 4. **path_utils.c** - Komut Yolu Çözümleme
```c
// Ana path çözümleme
char *find_command_path(char *command, t_env *env_list)
char *build_full_path(char *dir, char *command)

// Çevre değişkeni ve array işlemleri
char *get_env_value(t_env *env_list, char *key)
char **env_list_to_array(t_env *env_list)
int count_env_nodes(t_env *env_list)
```

### 5. **redirections.c** - Giriş/Çıkış Yönlendirmeleri
```c
// Yönlendirme kurulumu
void setup_redirections(t_command *cmd)
void handle_single_redirection(t_redirect *redirect)

// Pipeline için fd kurulumu
void setup_pipeline_fds(t_command *cmd, int prev_fd, int *pipe_fd)
```

---

## 🔄 ANA FONKSIYONLAR VE AKIŞ ŞEMASI

### Ana Akış Şeması

```
USER INPUT → LEXER → PARSER → EXECUTOR
                              ↓
                    ┌─────────────────┐
                    │ execute_commands │
                    └─────────────────┘
                              ↓
                         ┌─────────┐
                         │ Komut   │
                         │ Sayısı? │
                         └─────────┘
                         ↙        ↘
                   ┌─────────┐  ┌──────────┐
                   │ Tek     │  │ Pipeline │
                   │ Komut   │  │ (|)      │
                   └─────────┘  └──────────┘
                         ↓             ↓
            ┌──────────────────┐  ┌──────────────────┐
            │execute_single_   │  │execute_pipeline  │
            │command           │  │                  │
            └──────────────────┘  └──────────────────┘
                         ↓             ↓
                   ┌─────────┐  ┌──────────────┐
                   │Built-in?│  │Her komut için│
                   └─────────┘  │pipeline_cmd  │
                   ↙        ↘   └──────────────┘
          ┌─────────────┐ ┌──────────────┐
          │execute_     │ │execute_      │
          │builtin      │ │external_cmd  │
          └─────────────┘ └──────────────┘
```

### 1. **execute_commands()** - Ana Giriş Noktası

```c
int execute_commands(t_command *commands, t_global *global)
{
    // Komut yoksa 0 döndür
    if (!commands)
        return (0);
    
    // Pipeline var mı kontrol et
    if (commands->next)
        return (execute_pipeline(commands, global));  // Birden fazla komut → Pipeline
    else
        return (execute_single_command(commands, global)); // Tek komut
}
```

**Örnek:**
- `ls -la` → Tek komut → `execute_single_command()`
- `ls -la | grep txt` → Pipeline → `execute_pipeline()`

### 2. **execute_single_command()** - Tek Komut İşleyici

```c
int execute_single_command(t_command *cmd, t_global *global)
{
    // Komut ve argüman kontrolü
    if (!cmd || !cmd->args || !cmd->args[0])
        return (1);
    
    // Built-in komut mu kontrol et
    if (is_builtin(cmd->args[0]))
        return (execute_builtin(cmd, global));    // Built-in komut
    
    return (execute_external_command(cmd, global)); // Harici program
}
```

**Karar Verme Süreci:**
```
Komut: "pwd"
1. cmd->args[0] = "pwd"
2. is_builtin("pwd") → true
3. execute_builtin() çağrılır
4. builtin_pwd_global() çalıştırılır

Komut: "ls"
1. cmd->args[0] = "ls"
2. is_builtin("ls") → false
3. execute_external_command() çağrılır
4. PATH'te "ls" aranır ve /bin/ls bulunur
```

---

## 🔧 BUILT-IN KOMUTLAR

### Built-in Kontrol Sistemi

```c
int is_builtin(char *command)
{
    if (!command)
        return (0);
    
    // Her built-in komut için kontrol
    if (ft_strcmp(command, "pwd") == 0)    return (1);  // Mevcut dizin
    if (ft_strcmp(command, "echo") == 0)   return (1);  // Metin yazdırma
    if (ft_strcmp(command, "env") == 0)    return (1);  // Çevre değişkenleri
    if (ft_strcmp(command, "exit") == 0)   return (1);  // Çıkış
    if (ft_strcmp(command, "cd") == 0)     return (1);  // Dizin değiştirme
    if (ft_strcmp(command, "export") == 0) return (1);  // Değişken export
    if (ft_strcmp(command, "unset") == 0)  return (1);  // Değişken silme
    
    return (0);
}
```

### Built-in Executor

```c
int execute_builtin(t_command *cmd, t_global *global)
{
    // Switch-case benzeri yapı (42 norm uyumlu)
    if (ft_strcmp(cmd->args[0], "pwd") == 0)
        return (builtin_pwd_global(global));
    if (ft_strcmp(cmd->args[0], "echo") == 0)
        return (builtin_echo(cmd->args));
    // ... diğer komutlar
}
```

### Önemli Built-in Komutlar

#### 1. **PWD** - Mevcut Dizin
```c
int builtin_pwd_global(t_global *global)
{
    char *pwd_env;
    char *cwd;

    // Önce PWD çevre değişkenini kontrol et
    pwd_env = get_env_value(global->env_list, "PWD");
    if (pwd_env)
    {
        printf("%s\n", pwd_env);  // PWD değişkenini yazdır
        return (0);
    }
    
    // PWD yoksa getcwd() kullan
    cwd = getcwd(NULL, 0);
    if (!cwd)
    {
        printf("pwd: error retrieving current directory\n");
        return (1);
    }
    
    printf("%s\n", cwd);
    free(cwd);
    return (0);
}
```

#### 2. **CD** - Dizin Değiştirme
```c
int builtin_cd(char **args, t_global *global)
{
    char *path;
    char *home;
    char *old_pwd;

    // Mevcut PWD'yi kaydet
    old_pwd = get_env_value(global->env_list, "PWD");
    
    // Argüman yoksa HOME dizinine git
    if (!args[1])
    {
        home = get_env_value(global->env_list, "HOME");
        if (!home)
        {
            printf("cd: HOME not set\n");
            return (1);
        }
        path = home;
    }
    else
        path = args[1];  // Verilen yolu kullan

    // Dizini değiştir
    if (chdir(path) != 0)
    {
        perror("cd");
        return (1);
    }

    // Çevre değişkenlerini güncelle
    if (old_pwd)
        set_env_var(global, "OLDPWD", old_pwd);  // Eski yolu kaydet
    
    update_pwd_env(global);  // Yeni PWD'yi güncelle
    return (0);
}
```

**CD Komut Örnekleri:**
```bash
cd           # HOME dizinine git
cd /tmp      # /tmp dizinine git
cd ..        # Üst dizine git
cd -         # Önceki dizine git (OLDPWD kullanarak)
```

#### 3. **EXPORT** - Çevre Değişkeni Export
```c
int builtin_export(char **args, t_global *global)
{
    int i;
    char *key, *value;
    char *equal_sign;

    // Argüman yoksa tüm export edilmiş değişkenleri listele
    if (!args[1])
    {
        print_export_env(global->env_list);
        return (0);
    }

    i = 1;
    while (args[i])
    {
        equal_sign = ft_strchr(args[i], '=');
        if (equal_sign)
        {
            // KEY=VALUE formatı
            *equal_sign = '\0';
            key = args[i];
            value = equal_sign + 1;
            set_env_var(global, key, value);  // Değişkeni ayarla
        }
        else
        {
            // Sadece KEY (değer olmadan export)
            key = args[i];
            set_env_var(global, key, "");
        }
        i++;
    }
    return (0);
}
```

**Export Örnekleri:**
```bash
export                    # Tüm export edilmiş değişkenleri listele
export MY_VAR=hello       # MY_VAR=hello olarak export et
export PATH=$PATH:/new    # PATH'e yeni dizin ekle
```

---

## 🌐 EXTERNAL KOMUTLAR

### Harici Program Çalıştırma Süreci

```c
int execute_external_command(t_command *cmd, t_global *global)
{
    pid_t pid;
    int status;
    char *path;

    // 1. Komutun tam yolunu bul
    path = find_command_path(cmd->args[0], global->env_list);
    if (!path)
    {
        printf("minishell: %s: command not found\n", cmd->args[0]);
        return (127);  // Komut bulunamadı hatası
    }

    // 2. Child process oluştur
    pid = fork();
    if (pid == 0)
    {
        // CHILD PROCESS
        setup_redirections(cmd);  // Yönlendirmeleri ayarla
        execve(path, cmd->args, env_list_to_array(global->env_list));
        perror("execve");
        exit(127);
    }
    else if (pid > 0)
    {
        // PARENT PROCESS
        waitpid(pid, &status, 0);  // Child'ın bitmesini bekle
        free(path);
        return (WEXITSTATUS(status));  // Exit status'u döndür
    }
    else
    {
        // FORK HATASI
        perror("fork");
        free(path);
        return (1);
    }
}
```

### Komut Yolu Çözümleme

```c
char *find_command_path(char *command, t_env *env_list)
{
    char *path_env;
    char **paths;
    char *full_path;
    int i;

    // 1. Komut zaten tam yol mu? (/ içeriyorsa)
    if (ft_strchr(command, '/'))
    {
        if (access(command, F_OK) == 0)  // Dosya var mı?
            return (ft_strdup(command));
        return (NULL);
    }

    // 2. PATH çevre değişkenini al
    path_env = get_env_value(env_list, "PATH");
    if (!path_env)
        return (NULL);

    // 3. PATH'i ':' ile ayır
    paths = ft_split(path_env, ':');
    if (!paths)
        return (NULL);

    // 4. Her PATH dizininde komut ara
    i = 0;
    while (paths[i])
    {
        full_path = build_full_path(paths[i], command);
        if (full_path && access(full_path, F_OK) == 0)
        {
            free_string_array(paths);
            return (full_path);  // Komut bulundu!
        }
        if (full_path)
            free(full_path);
        i++;
    }
    
    free_string_array(paths);
    return (NULL);  // Komut bulunamadı
}
```

**Path Çözümleme Örneği:**
```
Komut: "ls"
PATH: "/usr/local/bin:/usr/bin:/bin"

1. "/usr/local/bin/ls" var mı? → Hayır
2. "/usr/bin/ls" var mı? → Hayır  
3. "/bin/ls" var mı? → Evet! → "/bin/ls" döndür
```

---

## ⚡ PIPELINE İŞLEMLERİ

Pipeline, birden fazla komutun çıkışını bir sonrakinin girişine bağlama işlemidir.

### Pipeline Akış Şeması

```
Komut: "cat file.txt | grep hello | wc -l"

┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│ cat file.txt│    │ grep hello  │    │   wc -l     │
│             │────│             │────│             │
│   stdout    │────│ stdin stdout│────│ stdin stdout│
└─────────────┘    └─────────────┘    └─────────────┘
      │                   │                   │
   pipe_fd[1]         pipe_fd[0]        normal stdout
```

### Pipeline Implementation

```c
int execute_pipeline(t_command *commands, t_global *global)
{
    t_command *current;
    int pipe_fd[2];      // Pipe file descriptors
    int prev_fd;         // Önceki komuttan gelen fd
    int last_status;

    current = commands;
    prev_fd = 0;         // İlk komut stdin'den okur
    last_status = 0;

    while (current)
    {
        // Sonraki komut varsa pipe oluştur
        if (current->next && pipe(pipe_fd) == -1)
        {
            perror("pipe");
            return (1);
        }

        // Komutu çalıştır
        last_status = execute_pipeline_command(current, global, prev_fd, pipe_fd);

        // Önceki pipe'ı kapat
        if (prev_fd != 0)
            close(prev_fd);

        // Sonraki iterasyon için pipe ayarla
        if (current->next)
        {
            close(pipe_fd[1]);  // Write end'i kapat
            prev_fd = pipe_fd[0];  // Read end'i sakla
        }

        current = current->next;
    }

    return (last_status);
}
```

### Pipeline Komut Çalıştırma

```c
int execute_pipeline_command(t_command *cmd, t_global *global, int prev_fd, int *pipe_fd)
{
    pid_t pid;
    int status;
    char *path;

    // Built-in komut ve pipeline'ın son komutu ise
    if (is_builtin(cmd->args[0]) && !cmd->next && prev_fd == 0)
        return (execute_builtin(cmd, global));

    // Harici komut için path bul
    path = find_command_path(cmd->args[0], global->env_list);
    if (!path)
        return (127);

    pid = fork();
    if (pid == 0)
    {
        // CHILD PROCESS
        setup_pipeline_fds(cmd, prev_fd, pipe_fd);  // Pipe'ları ayarla
        setup_redirections(cmd);                    // Yönlendirmeleri ayarla

        // Built-in komut mu?
        if (is_builtin(cmd->args[0]))
        {
            execute_builtin(cmd, global);
            exit(global->exit_status);
        }

        // Harici komut çalıştır
        execve(path, cmd->args, env_list_to_array(global->env_list));
        perror("execve");
        exit(127);
    }
    else if (pid > 0)
    {
        // PARENT PROCESS
        waitpid(pid, &status, 0);
        free(path);
        return (WEXITSTATUS(status));
    }
    else
    {
        perror("fork");
        free(path);
        return (1);
    }
}
```

### Pipeline FD Kurulumu

```c
void setup_pipeline_fds(t_command *cmd, int prev_fd, int *pipe_fd)
{
    // Önceki komuttan gelen input var mı?
    if (prev_fd != 0)
    {
        dup2(prev_fd, STDIN_FILENO);   // stdin'i önceki pipe'a yönlendir
        close(prev_fd);
    }

    // Sonraki komut var mı?
    if (cmd->next)
    {
        close(pipe_fd[0]);              // Read end'i kapat
        dup2(pipe_fd[1], STDOUT_FILENO); // stdout'u pipe'a yönlendir
        close(pipe_fd[1]);
    }
}
```

---

## 📤 YÖNLENDİRMELER (REDIRECTIONS)

Yönlendirmeler, komutların giriş/çıkışını dosyalara yönlendirme işlemidir.

### Yönlendirme Türleri

```c
// token_enum.h'dan
typedef enum e_token_types
{
    T_REDIRECT_IN,     // <  (input redirection)
    T_REDIRECT_OUT,    // >  (output redirection) 
    T_APPEND,          // >> (append redirection)
    T_HEREDOC,         // << (heredoc)
} t_token_types;
```

### Yönlendirme İşleme

```c
void setup_redirections(t_command *cmd)
{
    t_list *current;
    t_redirect *redirect;

    if (!cmd || !cmd->redirections)
        return;

    current = cmd->redirections;
    while (current)
    {
        redirect = (t_redirect *)current->content;
        if (redirect)
            handle_single_redirection(redirect);  // Her yönlendirmeyi işle
        current = current->next;
    }
}
```

### Tek Yönlendirme İşleyici

```c
void handle_single_redirection(t_redirect *redirect)
{
    int fd;

    if (redirect->type == T_REDIRECT_IN)
    {
        // < filename (input redirection)
        fd = open(redirect->filename, O_RDONLY);
        if (fd == -1)
        {
            perror(redirect->filename);
            exit(1);
        }
        dup2(fd, STDIN_FILENO);   // stdin'i dosyaya yönlendir
        close(fd);
    }
    else if (redirect->type == T_REDIRECT_OUT)
    {
        // > filename (output redirection)
        fd = open(redirect->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1)
        {
            perror(redirect->filename);
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);  // stdout'u dosyaya yönlendir
        close(fd);
    }
    else if (redirect->type == T_APPEND)
    {
        // >> filename (append redirection)
        fd = open(redirect->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1)
        {
            perror(redirect->filename);
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);  // stdout'u dosyaya yönlendir (append)
        close(fd);
    }
}
```

**Yönlendirme Örnekleri:**
```bash
ls > output.txt        # ls çıkışını output.txt'ye yaz
cat < input.txt        # input.txt'yi cat'e girdi olarak ver  
echo "hello" >> log.txt # "hello"yu log.txt'nin sonuna ekle
cat file.txt > out.txt 2>&1  # stdout ve stderr'i out.txt'ye yönlendir
```

---

## 🌱 ÇEVRE DEĞİŞKENLERİ

### Çevre Değişkeni Yapısı

```c
typedef struct s_env
{
    char *key;           // Değişken adı (örn: "PATH")
    char *value;         // Değişken değeri (örn: "/usr/bin:/bin")
    struct s_env *next;  // Sonraki düğüm
} t_env;
```

### Temel Çevre Değişkeni İşlemleri

#### 1. Değişken Alma
```c
char *get_env_value(t_env *env_list, char *key)
{
    t_env *current;

    current = env_list;
    while (current)
    {
        if (ft_strcmp(current->key, key) == 0)
            return (current->value);  // Değişken bulundu
        current = current->next;
    }
    return (NULL);  // Değişken bulunamadı
}
```

#### 2. Değişken Ayarlama
```c
void set_env_var(t_global *global, char *key, char *value)
{
    t_env *existing;
    t_env *new_node;

    // Değişken zaten var mı?
    existing = find_env_node(global->env_list, key);
    if (existing)
    {
        // Mevcut değeri güncelle
        free(existing->value);
        existing->value = ft_strdup(value);
    }
    else
    {
        // Yeni düğüm oluştur
        new_node = create_env_node(key, value);
        add_env_node(&global->env_list, new_node);
    }
}
```

#### 3. Değişken Silme
```c
void unset_env_var(t_global *global, char *key)
{
    t_env *current;
    t_env *prev;

    current = global->env_list;
    prev = NULL;

    while (current)
    {
        if (ft_strcmp(current->key, key) == 0)
        {
            // Düğümü listeden çıkar
            if (prev)
                prev->next = current->next;
            else
                global->env_list = current->next;

            // Belleği serbest bırak
            free(current->key);
            free(current->value);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}
```

#### 4. Çevre Array'ine Dönüştürme
```c
char **env_list_to_array(t_env *env_list)
{
    t_env *current;
    char **env_array;
    char *temp;
    int count, i;

    // Düğüm sayısını say
    count = count_env_nodes(env_list);
    
    // Array oluştur
    env_array = malloc(sizeof(char *) * (count + 1));
    if (!env_array)
        return (NULL);

    current = env_list;
    i = 0;
    while (current && i < count)
    {
        // "KEY=VALUE" formatında string oluştur
        temp = ft_strjoin(current->key, "=");
        if (temp)
        {
            env_array[i] = ft_strjoin(temp, current->value);
            free(temp);
        }
        else
            env_array[i] = NULL;
        
        current = current->next;
        i++;
    }
    env_array[i] = NULL;  // Array sonlandırıcı
    return (env_array);
}
```

---

## 🎯 ÖRNEK SENARYOLAR

### Senaryo 1: Basit Komut Çalıştırma
```bash
minishell$ ls -la
```

**Akış:**
1. `execute_commands()` → tek komut tespit edilir
2. `execute_single_command()` → "ls" built-in değil
3. `execute_external_command()` → 
   - `find_command_path("ls")` → "/bin/ls" bulunur
   - `fork()` → child process oluşur
   - Child: `execve("/bin/ls", ["ls", "-la"], env_array)`
   - Parent: `waitpid()` ile bekler

### Senaryo 2: Built-in Komut
```bash
minishell$ cd /tmp
```

**Akış:**
1. `execute_commands()` → tek komut
2. `execute_single_command()` → "cd" built-in
3. `execute_builtin()` → `builtin_cd()`
   - `chdir("/tmp")` sistem çağrısı
   - `set_env_var(global, "OLDPWD", old_pwd)`
   - `update_pwd_env(global)` → PWD güncelle

### Senaryo 3: Pipeline
```bash
minishell$ cat file.txt | grep "hello" | wc -l
```

**Akış:**
1. `execute_commands()` → pipeline tespit edilir
2. `execute_pipeline()` → 
   - **1. Komut (cat):**
     - `pipe()` oluştur → pipe_fd[0,1]
     - `fork()` → child process
     - Child: stdout'u pipe_fd[1]'e yönlendir
     - `execve("/bin/cat", ["cat", "file.txt"])`
   - **2. Komut (grep):**
     - Yeni `pipe()` oluştur
     - `fork()` → child process  
     - Child: stdin'i önceki pipe'dan, stdout'u yeni pipe'a
     - `execve("/bin/grep", ["grep", "hello"])`
   - **3. Komut (wc):**
     - `fork()` → child process
     - Child: stdin'i önceki pipe'dan
     - `execve("/usr/bin/wc", ["wc", "-l"])`

### Senaryo 4: Yönlendirme
```bash
minishell$ echo "Hello World" > output.txt
```

**Akış:**
1. `execute_single_command()` → "echo" built-in
2. `execute_builtin()` ama önce...
3. `setup_redirections()` →
   - `open("output.txt", O_WRONLY | O_CREAT | O_TRUNC)`
   - `dup2(fd, STDOUT_FILENO)` → stdout dosyaya yönlendirilir
4. `builtin_echo()` → "Hello World" yazdırır (dosyaya gider)

### Senaryo 5: Çevre Değişkeni Export
```bash
minishell$ export MY_VAR=hello
minishell$ echo $MY_VAR
```

**Akış:**
1. **Export komutu:**
   - `builtin_export(["export", "MY_VAR=hello"])`
   - "=" bulunur → key="MY_VAR", value="hello"
   - `set_env_var(global, "MY_VAR", "hello")`
   - Çevre listesine eklenir

2. **Echo komutu:**
   - Parser `$MY_VAR`'ı genişletir → "hello"
   - `builtin_echo(["echo", "hello"])` → "hello" yazdırır

---

## 🔍 HATA YÖNETİMİ VE EXIT STATUS'LAR

### Standart Exit Status'lar
```c
// Başarılı
#define SUCCESS 0

// Genel hatalar  
#define GENERAL_ERROR 1

// Komut bulunamadı
#define COMMAND_NOT_FOUND 127

// Çalıştırılamaz komut
#define COMMAND_NOT_EXECUTABLE 126
```

### Hata Yakalama Örnekleri

```c
// Komut bulunamadı
if (!path)
{
    printf("minishell: %s: command not found\n", cmd->args[0]);
    return (127);
}

// Fork hatası
if (pid < 0)
{
    perror("fork");
    return (1);
}

// Dosya açma hatası (redirection)
if (fd == -1)
{
    perror(redirect->filename);
    exit(1);
}
```

---

## 📝 ÖZET

Minishell'in executor modülü, aşağıdaki temel görevleri yerine getirir:

1. **Komut Tipini Belirleme:** Built-in mi harici komut mu?
2. **Built-in Komutlar:** pwd, echo, cd, export, unset, env, exit
3. **Harici Komutlar:** PATH'te arama, fork/exec döngüsü
4. **Pipeline İşlemleri:** Pipe'lar arası veri akışı
5. **Yönlendirmeler:** Dosya giriş/çıkış yönlendirmeleri  
6. **Çevre Değişkenleri:** Export, unset, değişken yönetimi

Her modül birbirleriyle sıkı entegre çalışarak tam bir shell deneyimi sunar. Kod 42 École norm kurallarına uygun olarak yazılmış ve sadece izin verilen sistem fonksiyonlarını kullanmaktadır.

---

## 🚀 PERFORMANs NOTLARI

- **Bellek Yönetimi:** Her malloc'a karşılık free
- **File Descriptor Yönetimi:** Pipe'lar ve dosyalar düzgün kapatılır
- **Process Yönetimi:** Zombie process'ler önlenir (waitpid)
- **Error Handling:** Her sistem çağrısı kontrol edilir

Bu detaylı analiz, minishell executor modülünün nasıl çalıştığını ve diğer modüllerle nasıl entegre olduğunu göstermektedir.
