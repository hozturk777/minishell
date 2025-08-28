# MİNİSHELL PROJESİ - DETAYLI KOD ANALİZİ VE AKIŞ ŞEMASI

## 📋 ÖZET

Bu minishell projesi, bash benzeri bir shell programıdır. Proje lexer, parser ve executor modüllerinden oluşur. Kullanıcıdan aldığı komutları analiz eder, built-in komutları çalıştırır veya harici programları başlatır. Pipeline, yönlendirme ve çevre değişkeni yönetimi gibi temel shell özelliklerini destekler.

### 🎯 Temel Bileşenler:
- **Lexer**: Kullanıcı girdisini token'lara ayırır
- **Parser**: Token'ları komut yapılarına dönüştürür
- **Executor**: Komutları çalıştırır (built-in veya external)
- **Built-ins**: pwd, echo, cd, export, unset, env, exit
- **Pipeline**: Pipe (|) operatörü desteği
- **Redirections**: <, >, >> yönlendirmeleri
- **Environment**: Çevre değişkeni yönetimi

---

## 🔄 ANA AKIŞ ŞEMASI

```
[BAŞLAT] → [INIT] → [MAIN LOOP] → [PROCESS INPUT] → [ÇIK]
    ↓         ↓           ↓               ↓           ↓
   main()  init_global()  readline()  tokenize()   free_global()
                             ↓           ↓
                        add_history()  parse()
                                       ↓
                                   execute()
                                       ↓
                              [built-in? / external?]
                                  ↓        ↓
                            builtin_*()  fork()+execve()
```

### Detaylı İşlem Akışı:

1. **BAŞLATMA (main.c)**
   ```
   main() → init_global() → print_welcome() → run_shell_loop()
   ```

2. **ANA DÖNGÜ (run_shell_loop)**
   ```
   readline() → process_input() → [komut analizi] → [sonuç döndür]
   ```

3. **KOMUT İŞLEME (process_input)**
   ```
   input → tokenize_advanced() → parse_tokens_to_commands() → execute_commands()
   ```

4. **ÇALIŞTIRMA (execute_commands)**
   ```
   commands → [tek komut mu? / pipeline mi?] → [built-in mi? / external mi?]
   ```

---

## 💻 DETAYLI KOD ANALİZİ

### 1. MAIN.C - PROGRAM GİRİŞ NOKTASI

```c
int main(int argc, char **argv, char **envp)
{
    t_global *global;                    // Global state yapısı tanımla
    
    (void)argc;                          // Kullanılmayan parametreleri işaretle  
    (void)argv;                          // (42 norm kuralı)
    
    global = init_global(envp);          // Global state'i başlat, çevre değişkenlerini yükle
    if (!global)                         // Başlatma başarısız mı?
    {
        printf("Error: Failed to initialize global state\n");
        return (1);                      // Hata kodu ile çık
    }
    
    print_welcome_advanced();            // Karşılama mesajını yazdır
    debug_print("Global state initialized successfully"); // Debug mesajı
    
    run_shell_loop(global);              // Ana shell döngüsünü başlat
    
    printf(GREEN "Goodbye!" RESET "\n"); // Çıkış mesajı
    free_global(global);                 // Belleği temizle
    return (0);                          // Başarı ile çık
}
```

#### `run_shell_loop()` - Ana Shell Döngüsü

```c
static void run_shell_loop(t_global *global)
{
    char *input;                         // Kullanıcı girdisi için pointer
    int should_exit;                     // Çıkış kontrolü için flag
    
    should_exit = 0;                     // Başlangıçta çıkış yok
    while (!should_exit)                 // Çıkış flag'i true olana kadar döngü
    {
        input = readline(PROMPT);        // "minishell$ " prompt'u ile girdi al
        if (!input)                      // CTRL+D (EOF) durumu
        {
            printf("\n");                // Yeni satır yazdır
            break;                       // Döngüden çık
        }
        should_exit = process_input(input, global); // Girdiyi işle
        free(input);                     // Girdi belleğini serbest bırak
    }
}
```

#### `process_input()` - Komut İşleme

```c
static int process_input(char *input, t_global *global)
{
    t_list *tokens;                      // Token listesi
    t_command *commands;                 // Komut listesi
    
    if (!input || ft_strlen(input) == 0) // Boş girdi kontrolü
        return (0);                      // İşlem yok, devam et
        
    add_history(input);                  // Girdiyi readline history'ye ekle
    global->input_line = ft_strdup(input); // Global state'e kaydet
    
    // 1. LEXER: Input'u token'lara çevir
    tokens = tokenize_advanced(input, global);
    if (!tokens)                         // Tokenizasyon başarısız mı?
    {
        printf("Error: Tokenization failed\n");
        return (0);
    }
    
    // 2. PARSER: Token'ları komut yapılarına çevir  
    commands = parse_tokens_to_commands(tokens, global);
    if (!commands)                       // Parsing başarısız mı?
    {
        printf("Error: Parsing failed\n");
        free_tokens_advanced(&tokens);   // Token belleğini temizle
        return (0);
    }
    
    // 3. EXECUTION: Komutları çalıştır
    if (commands)
    {
        global->exit_status = execute_commands(commands, global);
        debug_print("Commands executed successfully");
    }
    
    // 4. CLEANUP: Memory'yi temizle
    global->tokens = tokens;             // Global'e ata (cleanup için)
    global->commands = commands;
    free_tokens_advanced(&global->tokens); // Token'ları temizle
    free_commands_list(global->commands);   // Komutları temizle
    global->commands = NULL;
    
    if (global->input_line)              // Input line temizle
    {
        free(global->input_line);
        global->input_line = NULL;
    }
    
    if (ft_strncmp(input, "exit", 4) == 0) // Exit komutu mu?
        return (1);                      // Çıkış flag'i döndür
    return (0);                          // Devam et
}
```

---

### 2. LEXER.C - TOKENİZASYON

#### `init_lexer_advanced()` - Lexer Başlatma

```c
t_lexer_new *init_lexer_advanced(char *input, t_global *global)
{
    t_lexer_new *lexer;                  // Lexer yapısı
    
    lexer = (t_lexer_new *)halloc(sizeof(t_lexer_new)); // Bellek ayır
    if (!lexer)                          // Bellek ayırma başarısız mı?
        return (NULL);
        
    lexer->input = input;                // Girdi metnini ata
    lexer->pos = 0;                      // Başlangıç pozisyonu
    lexer->len = ft_strlen(input);       // Girdi uzunluğu
    lexer->global = global;              // Global state referansı
    
    if (lexer->len > 0)                  // Girdi var mı?
        lexer->current_char = input[0];  // İlk karakteri al
    else
        lexer->current_char = '\0';      // Boş girdi
        
    return (lexer);                      // Hazır lexer'ı döndür
}
```

#### `advance_lexer()` - Sonraki Karaktere Geçme

```c
void advance_lexer(t_lexer_new *lexer)
{
    lexer->pos++;                        // Pozisyonu artır
    if (lexer->pos >= lexer->len)        // Son karakteri geçtik mi?
        lexer->current_char = '\0';      // EOF işareti
    else
        lexer->current_char = lexer->input[lexer->pos]; // Sonraki karakter
}
```

#### `create_token_advanced()` - Token Oluşturma

```c
t_token_new *create_token_advanced(t_token_types type, char *value)
{
    t_token_new *token;                  // Yeni token
    
    token = (t_token_new *)halloc(sizeof(t_token_new)); // Bellek ayır
    if (!token)                          // Bellek ayırma başarısız mı?
        return (NULL);
        
    token->type = type;                  // Token türünü ata (T_WORD, T_PIPE, vb.)
    token->value = ft_strdup(value);     // Token değerini kopyala
    if (!token->value)                   // String kopyalama başarısız mı?
    {
        free(token);                     // Token belleğini serbest bırak
        return (NULL);
    }
    
    token->len = ft_strlen(value);       // Değer uzunluğu
    token->quote_type = 0;               // Tırnak türü (başlangıçta yok)
    token->expanded = 0;                 // Genişletme flag'i (başlangıçta yok)
    
    return (token);                      // Hazır token'ı döndür
}
```

---

### 3. BUILTINS.C - DAHİLİ KOMUTLAR

#### `is_builtin()` - Built-in Kontrol Fonksiyonu

```c
int is_builtin(char *command)
{
    if (!command)                        // NULL pointer kontrolü
        return (0);                      // Built-in değil
        
    // Her built-in komut için string karşılaştırması
    if (ft_strcmp(command, "pwd") == 0)    return (1); // Mevcut dizin
    if (ft_strcmp(command, "echo") == 0)   return (1); // Metin yazdırma
    if (ft_strcmp(command, "env") == 0)    return (1); // Çevre değişkenleri
    if (ft_strcmp(command, "exit") == 0)   return (1); // Çıkış
    if (ft_strcmp(command, "cd") == 0)     return (1); // Dizin değiştirme
    if (ft_strcmp(command, "export") == 0) return (1); // Değişken export
    if (ft_strcmp(command, "unset") == 0)  return (1); // Değişken silme
    
    return (0);                          // Built-in değil
}
```

#### `execute_builtin()` - Built-in Executor

```c
int execute_builtin(t_command *cmd, t_global *global)
{
    // NULL ve argüman kontrolü
    if (!cmd || !cmd->args || !cmd->args[0])
        return (1);                      // Hata
        
    // Switch-case benzeri yapı (42 norm uyumlu)
    if (ft_strcmp(cmd->args[0], "pwd") == 0)
        return (builtin_pwd_global(global));      // pwd komutunu çalıştır
    if (ft_strcmp(cmd->args[0], "echo") == 0)
        return (builtin_echo(cmd->args));         // echo komutunu çalıştır
    if (ft_strcmp(cmd->args[0], "env") == 0)
        return (builtin_env(global->env_list));   // env komutunu çalıştır
    if (ft_strcmp(cmd->args[0], "exit") == 0)
        return (builtin_exit(cmd->args, global)); // exit komutunu çalıştır
    if (ft_strcmp(cmd->args[0], "cd") == 0)
        return (builtin_cd(cmd->args, global));   // cd komutunu çalıştır
    if (ft_strcmp(cmd->args[0], "export") == 0)
        return (builtin_export(cmd->args, global)); // export komutunu çalıştır
    if (ft_strcmp(cmd->args[0], "unset") == 0)
        return (builtin_unset(cmd->args, global)); // unset komutunu çalıştır
        
    return (1);                          // Bilinmeyen komut
}
```

#### `builtin_pwd_global()` - PWD Komutu

```c
int builtin_pwd_global(t_global *global)
{
    char *pwd_env;                       // PWD çevre değişkeni
    char *cwd;                          // Mevcut çalışma dizini
    
    // Önce PWD çevre değişkenini kontrol et
    pwd_env = get_env_value(global->env_list, "PWD");
    if (pwd_env)                        // PWD değişkeni var mı?
    {
        printf("%s\n", pwd_env);        // PWD değişkenini yazdır
        return (0);                     // Başarı
    }
    
    // PWD yoksa getcwd() sistem çağrısını kullan
    cwd = getcwd(NULL, 0);              // Mevcut dizini al (bellek otomatik ayırır)
    if (!cwd)                           // getcwd() başarısız mı?
    {
        printf("pwd: error retrieving current directory: No such file or directory\n");
        return (1);                     // Hata
    }
    
    printf("%s\n", cwd);               // Mevcut dizini yazdır
    free(cwd);                         // getcwd() ile ayrılan belleği serbest bırak
    return (0);                        // Başarı
}
```

#### `builtin_echo()` - ECHO Komutu

```c
int builtin_echo(char **args)
{
    int i;                              // Argüman indeksi
    int newline;                        // Yeni satır flag'i
    char *processed_arg;                // İşlenmiş argüman
    
    i = 1;                              // args[0] = "echo", args[1]'den başla
    newline = 1;                        // Varsayılan: yeni satır ekle
    
    // -n flag kontrol et (yeni satır eklemeyi engeller)
    if (args[1] && ft_strcmp(args[1], "-n") == 0)
    {
        newline = 0;                    // Yeni satır ekleme
        i = 2;                          // args[2]'den başla
    }
    
    // Tüm argümanları yazdır
    while (args[i])
    {
        processed_arg = remove_quotes(args[i]); // Tırnakları kaldır
        if (processed_arg)              // İşlem başarılı mı?
        {
            printf("%s", processed_arg); // Argümanı yazdır
            free(processed_arg);        // İşlenmiş string'i serbest bırak
        }
        if (args[i + 1])                // Sonraki argüman var mı?
            printf(" ");                // Boşluk ekle
        i++;                            // Sonraki argümana geç
    }
    
    if (newline)                        // Yeni satır eklenecek mi?
        printf("\n");                   // Yeni satır yazdır
        
    return (0);                         // Başarı
}
```

#### `builtin_env()` - ENV Komutu

```c
int builtin_env(t_env *env_list)
{
    t_env *current;                     // Mevcut çevre değişkeni düğümü
    
    current = env_list;                 // Liste başından başla
    while (current)                     // Liste sonuna kadar
    {
        if (current->value)             // Değer var mı?
            printf("%s=%s\n", current->key, current->value); // KEY=VALUE formatında yazdır
        current = current->next;        // Sonraki düğüme geç
    }
    return (0);                         // Başarı
}
```

#### `builtin_exit()` - EXIT Komutu

```c
int builtin_exit(char **args, t_global *global)
{
    int exit_code;                      // Çıkış kodu
    
    exit_code = 0;                      // Varsayılan çıkış kodu
    printf("exit\n");                   // Çıkış mesajı yazdır
    
    if (args[1])                        // Çıkış kodu verilmiş mi?
    {
        exit_code = ft_atoi(args[1]);   // String'i integer'a çevir
        if (args[2])                    // Fazla argüman var mı?
        {
            printf("minishell: exit: too many arguments\n");
            return (1);                 // Hata döndür (çıkma)
        }
    }
    
    free_global(global);                // Global state'i temizle
    exit(exit_code);                    // Belirtilen kod ile çık
}
```

---

### 4. BUILTINS2.C - GELİŞMİŞ DAHİLİ KOMUTLAR

#### `builtin_cd()` - CD Komutu

```c
int builtin_cd(char **args, t_global *global)
{
    char *path;                         // Hedef dizin yolu
    char *home;                         // HOME dizini
    char *old_pwd;                      // Eski PWD
    char *logical_path;                 // Mantıksal yol
    char *existing_parent;              // Var olan parent dizin
    
    // Mevcut PWD'yi kaydet (OLDPWD için)
    old_pwd = get_env_value(global->env_list, "PWD");
    
    if (!args[1])                       // Argüman verilmemiş mi?
    {
        // HOME dizinine git
        home = get_env_value(global->env_list, "HOME");
        if (!home)                      // HOME tanımlı değil mi?
        {
            printf("minishell: cd: HOME not set\n");
            return (1);                 // Hata
        }
        path = home;                    // HOME'u hedef yap
    }
    else
        path = args[1];                 // Verilen yolu kullan
        
    // Dizini değiştirmeyi dene
    if (chdir(path) != 0)               // chdir() başarısız mı?
    {
        // ".." için özel işlem (silinmiş parent durumu)
        if (ft_strcmp(path, "..") == 0 && old_pwd)
        {
            logical_path = resolve_logical_path(old_pwd, path);
            if (logical_path)
            {
                // Var olan en yakın parent'ı bul
                existing_parent = find_existing_parent(logical_path);
                if (existing_parent)
                {
                    if (chdir(existing_parent) == 0) // Parent'a geçebildik mi?
                    {
                        if (old_pwd)
                            set_env_var(global, "OLDPWD", old_pwd);
                        set_env_var(global, "PWD", existing_parent);
                        free(logical_path);
                        free(existing_parent);
                        return (0);     // Başarı
                    }
                    free(existing_parent);
                }
                free(logical_path);
            }
        }
        perror("cd");                   // Hata mesajı yazdır
        return (1);                     // Hata
    }
    
    // Başarılı dizin değişimi
    if (old_pwd)
        set_env_var(global, "OLDPWD", old_pwd); // Eski PWD'yi kaydet
        
    // Yeni PWD'yi hesapla ve ayarla
    logical_path = resolve_logical_path(old_pwd, path);
    if (logical_path)
    {
        set_env_var(global, "PWD", logical_path); // Mantıksal PWD
        free(logical_path);
    }
    else
        update_pwd_env(global);         // Fiziksel PWD
        
    return (0);                         // Başarı
}
```

#### `builtin_export()` - EXPORT Komutu

```c
int builtin_export(char **args, t_global *global)
{
    int i;                              // Argüman indeksi
    char *key;                          // Değişken adı
    char *value;                        // Değişken değeri
    char *equal_sign;                   // '=' işaretinin pozisyonu
    
    if (!args[1])                       // Argüman yok mu?
    {
        print_export_env(global->env_list); // Tüm export'ları listele
        return (0);                     // Başarı
    }
    
    i = 1;                              // args[1]'den başla
    while (args[i])                     // Tüm argümanları işle
    {
        equal_sign = ft_strchr(args[i], '='); // '=' ara
        if (equal_sign)                 // KEY=VALUE formatı mı?
        {
            *equal_sign = '\0';         // String'i böl
            key = args[i];              // Anahtar kısmı
            value = equal_sign + 1;     // Değer kısmı
            set_env_var(global, key, value); // Çevre değişkenini ayarla
            *equal_sign = '=';          // String'i eski haline getir
        }
        else                            // Sadece KEY formatı
        {
            key = args[i];              // Anahtar
            set_env_var(global, key, ""); // Boş değer ile export et
        }
        i++;                            // Sonraki argüman
    }
    return (0);                         // Başarı
}
```

#### `builtin_unset()` - UNSET Komutu

```c
int builtin_unset(char **args, t_global *global)
{
    int i;                              // Argüman indeksi
    
    if (!args[1])                       // Argüman yok mu?
        return (0);                     // Hiçbir şey yapma
        
    i = 1;                              // args[1]'den başla
    while (args[i])                     // Tüm argümanları işle
    {
        unset_env_var(global, args[i]); // Çevre değişkenini sil
        i++;                            // Sonraki argüman
    }
    return (0);                         // Başarı
}
```

#### `set_env_var()` - Çevre Değişkeni Ayarlama

```c
void set_env_var(t_global *global, char *key, char *value)
{
    t_env *existing;                    // Mevcut düğüm
    t_env *new_node;                    // Yeni düğüm
    
    // Değişken zaten var mı?
    existing = find_env_node(global->env_list, key);
    if (existing)                       // Mevcut değişken bulundu
    {
        free(existing->value);          // Eski değeri serbest bırak
        existing->value = ft_strdup(value); // Yeni değeri ata
    }
    else                                // Yeni değişken
    {
        new_node = create_env_node(key, value); // Yeni düğüm oluştur
        if (new_node)                   // Oluşturma başarılı mı?
            add_env_node(&global->env_list, new_node); // Listeye ekle
    }
}
```

#### `unset_env_var()` - Çevre Değişkeni Silme

```c
void unset_env_var(t_global *global, char *key)
{
    t_env *current;                     // Mevcut düğüm
    t_env *prev;                        // Önceki düğüm
    
    current = global->env_list;         // Liste başından başla
    prev = NULL;                        // Önceki düğüm yok
    
    while (current)                     // Liste sonuna kadar
    {
        if (ft_strcmp(current->key, key) == 0) // Aranan anahtar mı?
        {
            // Düğümü listeden çıkar
            if (prev)                   // Önceki düğüm var mı?
                prev->next = current->next; // Öncekinin next'ini güncelle
            else
                global->env_list = current->next; // Liste başını güncelle
                
            // Belleği serbest bırak
            free(current->key);         // Anahtar string'i
            free(current->value);       // Değer string'i
            free(current);              // Düğüm yapısı
            return;                     // İşlem tamamlandı
        }
        prev = current;                 // Önceki düğümü güncelle
        current = current->next;        // Sonraki düğüme geç
    }
}
```

---

### 5. EXECUTOR.C - KOMUT ÇALIŞTIRICI

#### `execute_commands()` - Ana Executor

```c
int execute_commands(t_command *commands, t_global *global)
{
    if (!commands)                      // Komut yok mu?
        return (0);                     // Hiçbir şey yapma
        
    if (commands->next)                 // Birden fazla komut var mı?
        return (execute_pipeline(commands, global)); // Pipeline çalıştır
    else
        return (execute_single_command(commands, global)); // Tek komut çalıştır
}
```

#### `execute_single_command()` - Tek Komut Çalıştırma

```c
int execute_single_command(t_command *cmd, t_global *global)
{
    // NULL ve argüman kontrolü
    if (!cmd || !cmd->args || !cmd->args[0])
        return (1);                     // Hata
        
    // Built-in komut mu?
    if (is_builtin(cmd->args[0]))
        return (execute_builtin(cmd, global)); // Built-in çalıştır
        
    return (execute_external_command(cmd, global)); // Harici komut çalıştır
}
```

#### `execute_external_command()` - Harici Komut Çalıştırma

```c
int execute_external_command(t_command *cmd, t_global *global)
{
    pid_t pid;                          // Process ID
    int status;                         // Çıkış durumu
    char *path;                         // Komut yolu
    
    // Komutun tam yolunu bul
    path = find_command_path(cmd->args[0], global->env_list);
    if (!path)                          // Komut bulunamadı mı?
    {
        printf("minishell: %s: command not found\n", cmd->args[0]);
        return (127);                   // "Command not found" exit code
    }
    
    pid = fork();                       // Child process oluştur
    if (pid == 0)                       // Child process mu?
    {
        // CHILD PROCESS
        setup_redirections(cmd);        // Yönlendirmeleri ayarla
        execve(path, cmd->args, env_list_to_array(global->env_list));
        // execve() başarılı olursa buraya dönmez
        perror("execve");               // execve() hatası
        exit(127);                      // Child process'i sonlandır
    }
    else if (pid > 0)                   // Parent process mu?
    {
        // PARENT PROCESS  
        waitpid(pid, &status, 0);       // Child'ın bitmesini bekle
        free(path);                     // Path belleğini serbest bırak
        return (WEXITSTATUS(status));   // Child'ın exit code'unu döndür
    }
    else                                // fork() hatası
    {
        perror("fork");                 // Hata mesajı
        free(path);                     // Path belleğini serbest bırak
        return (1);                     // Hata
    }
}
```

#### `execute_pipeline()` - Pipeline Çalıştırma

```c
int execute_pipeline(t_command *commands, t_global *global)
{
    t_command *current;                 // Mevcut komut
    int pipe_fd[2];                     // Pipe file descriptors
    int prev_fd;                        // Önceki pipe'ın read end'i
    int last_status;                    // Son komutun durumu
    
    current = commands;                 // İlk komuttan başla
    prev_fd = 0;                        // İlk komut stdin'den okur
    last_status = 0;                    // Başlangıç durumu
    
    while (current)                     // Tüm komutları işle
    {
        // Sonraki komut varsa pipe oluştur
        if (current->next && pipe(pipe_fd) == -1)
        {
            perror("pipe");             // Pipe oluşturma hatası
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
            close(pipe_fd[1]);          // Write end'i kapat
            prev_fd = pipe_fd[0];       // Read end'i sakla
        }
        
        current = current->next;        // Sonraki komuta geç
    }
    
    return (last_status);               // Son komutun durumunu döndür
}
```

#### `execute_pipeline_command()` - Pipeline Komutu Çalıştırma

```c
int execute_pipeline_command(t_command *cmd, t_global *global, int prev_fd, int *pipe_fd)
{
    pid_t pid;                          // Process ID
    int status;                         // Çıkış durumu
    char *path;                         // Komut yolu
    
    // Built-in komut ve pipeline'ın son komutu ise özel işlem
    if (is_builtin(cmd->args[0]) && !cmd->next && prev_fd == 0)
        return (execute_builtin(cmd, global));
        
    // Harici komut için path bul
    path = find_command_path(cmd->args[0], global->env_list);
    if (!path)                          // Komut bulunamadı mı?
        return (127);
        
    pid = fork();                       // Child process oluştur
    if (pid == 0)                       // Child process mu?
    {
        // CHILD PROCESS
        setup_pipeline_fds(cmd, prev_fd, pipe_fd); // Pipe'ları ayarla
        setup_redirections(cmd);        // Yönlendirmeleri ayarla
        
        // Built-in komut mu?
        if (is_builtin(cmd->args[0]))
        {
            execute_builtin(cmd, global); // Built-in çalıştır
            exit(global->exit_status);   // Child'ı sonlandır
        }
        
        // Harici komut çalıştır
        execve(path, cmd->args, env_list_to_array(global->env_list));
        perror("execve");               // execve() hatası
        exit(127);                      // Child'ı sonlandır
    }
    else if (pid > 0)                   // Parent process mu?
    {
        // PARENT PROCESS
        waitpid(pid, &status, 0);       // Child'ın bitmesini bekle
        free(path);                     // Path belleğini serbest bırak
        return (WEXITSTATUS(status));   // Child'ın exit code'unu döndür
    }
    else                                // fork() hatası
    {
        perror("fork");                 // Hata mesajı
        free(path);                     // Path belleğini serbest bırak
        return (1);                     // Hata
    }
}
```

---

### 6. PATH_UTILS.C - YOL ÇÖZÜMLEMELERİ

#### `find_command_path()` - Komut Yolu Bulma

```c
char *find_command_path(char *command, t_env *env_list)
{
    char *path_env;                     // PATH çevre değişkeni
    char **paths;                       // PATH dizinleri dizisi
    char *full_path;                    // Tam yol
    int i;                              // Dizin indeksi
    
    if (!command)                       // NULL komut kontrolü
        return (NULL);
        
    // Komut zaten tam yol mu? (/ içeriyorsa)
    if (ft_strchr(command, '/'))
    {
        if (access(command, F_OK) == 0) // Dosya erişilebilir mi?
            return (ft_strdup(command)); // Kendisini döndür
        return (NULL);                  // Dosya yok
    }
    
    // PATH çevre değişkenini al
    path_env = get_env_value(env_list, "PATH");
    if (!path_env)                      // PATH tanımlı değil mi?
        return (NULL);
        
    // PATH'i ':' ile ayır
    paths = ft_split(path_env, ':');
    if (!paths)                         // Split başarısız mı?
        return (NULL);
        
    // Her PATH dizininde komut ara
    i = 0;
    while (paths[i])
    {
        full_path = build_full_path(paths[i], command); // path/command oluştur
        if (full_path && access(full_path, F_OK) == 0) // Dosya var mı?
        {
            free_string_array(paths);   // Paths dizisini temizle
            return (full_path);         // Bulunan yolu döndür
        }
        if (full_path)                  // Geçici path'i temizle
            free(full_path);
        i++;                            // Sonraki dizin
    }
    
    free_string_array(paths);           // Paths dizisini temizle
    return (NULL);                      // Komut bulunamadı
}
```

#### `build_full_path()` - Tam Yol Oluşturma

```c
char *build_full_path(char *dir, char *command)
{
    char *temp;                         // Geçici string
    char *full_path;                    // Tam yol
    
    if (!dir || !command)               // NULL kontrolü
        return (NULL);
        
    temp = ft_strjoin(dir, "/");        // "dir/" oluştur
    if (!temp)                          // Join başarısız mı?
        return (NULL);
        
    full_path = ft_strjoin(temp, command); // "dir/command" oluştur
    free(temp);                         // Geçici string'i temizle
    
    return (full_path);                 // Tam yolu döndür
}
```

#### `env_list_to_array()` - Çevre Listesini Array'e Çevirme

```c
char **env_list_to_array(t_env *env_list)
{
    t_env *current;                     // Mevcut düğüm
    char **env_array;                   // Çevre dizisi
    char *temp;                         // Geçici string
    int count, i;                       // Sayaçlar
    
    // Düğüm sayısını say
    count = count_env_nodes(env_list);
    
    // Array oluştur (son eleman NULL olacak)
    env_array = malloc(sizeof(char *) * (count + 1));
    if (!env_array)                     // Bellek ayırma başarısız mı?
        return (NULL);
        
    current = env_list;                 // Liste başından başla
    i = 0;                              // Array indeksi
    
    while (current && i < count)        // Tüm düğümleri işle
    {
        // "KEY=VALUE" formatında string oluştur
        temp = ft_strjoin(current->key, "=");
        if (temp)
        {
            env_array[i] = ft_strjoin(temp, current->value);
            free(temp);                 // Geçici string'i temizle
        }
        else
            env_array[i] = NULL;        // Hata durumu
            
        current = current->next;        // Sonraki düğüm
        i++;                            // Sonraki indeks
    }
    
    env_array[i] = NULL;                // Array sonlandırıcı
    return (env_array);                 // Hazır array'i döndür
}
```

---

### 7. REDIRECTIONS.C - YÖNLENDİRMELER

#### `setup_redirections()` - Yönlendirme Kurulumu

```c
void setup_redirections(t_command *cmd)
{
    t_list *current;                    // Mevcut yönlendirme düğümü
    t_redirect *redirect;               // Yönlendirme yapısı
    
    if (!cmd || !cmd->redirections)     // Komut veya yönlendirme yok mu?
        return;                         // Hiçbir şey yapma
        
    current = cmd->redirections;        // Yönlendirme listesi başı
    while (current)                     // Tüm yönlendirmeleri işle
    {
        redirect = (t_redirect *)current->content; // Yönlendirme verisi
        if (redirect)                   // Geçerli yönlendirme mi?
            handle_single_redirection(redirect); // Tek yönlendirmeyi işle
        current = current->next;        // Sonraki yönlendirme
    }
}
```

#### `handle_single_redirection()` - Tek Yönlendirme İşleme

```c
void handle_single_redirection(t_redirect *redirect)
{
    int fd;                             // Dosya tanımlayıcı
    
    if (redirect->type == T_REDIRECT_IN) // < (input redirection)
    {
        fd = open(redirect->filename, O_RDONLY); // Dosyayı okuma için aç
        if (fd == -1)                   // Dosya açılamadı mı?
        {
            perror(redirect->filename); // Hata mesajı
            exit(1);                    // Child process'i sonlandır
        }
        dup2(fd, STDIN_FILENO);         // stdin'i dosyaya yönlendir
        close(fd);                      // Orijinal fd'yi kapat
    }
    else if (redirect->type == T_REDIRECT_OUT) // > (output redirection)
    {
        // Dosyayı yazma için aç, oluştur, içeriği sil
        fd = open(redirect->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1)                   // Dosya açılamadı mı?
        {
            perror(redirect->filename); // Hata mesajı
            exit(1);                    // Child process'i sonlandır
        }
        dup2(fd, STDOUT_FILENO);        // stdout'u dosyaya yönlendir
        close(fd);                      // Orijinal fd'yi kapat
    }
    else if (redirect->type == T_APPEND) // >> (append redirection)
    {
        // Dosyayı yazma için aç, oluştur, sonuna ekle
        fd = open(redirect->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1)                   // Dosya açılamadı mı?
        {
            perror(redirect->filename); // Hata mesajı
            exit(1);                    // Child process'i sonlandır
        }
        dup2(fd, STDOUT_FILENO);        // stdout'u dosyaya yönlendir (append)
        close(fd);                      // Orijinal fd'yi kapat
    }
}
```

#### `setup_pipeline_fds()` - Pipeline File Descriptor Kurulumu

```c
void setup_pipeline_fds(t_command *cmd, int prev_fd, int *pipe_fd)
{
    // Önceki komuttan gelen input var mı?
    if (prev_fd != 0)                   // İlk komut değil
    {
        dup2(prev_fd, STDIN_FILENO);    // stdin'i önceki pipe'a yönlendir
        close(prev_fd);                 // Önceki fd'yi kapat
    }
    
    // Sonraki komut var mı?
    if (cmd->next)                      // Son komut değil
    {
        close(pipe_fd[0]);              // Read end'i kapat (kullanmayacağız)
        dup2(pipe_fd[1], STDOUT_FILENO); // stdout'u pipe'a yönlendir
        close(pipe_fd[1]);              // Write end'i kapat (dup2 sonrası gereksiz)
    }
}
```

---

## 🎯 ÖRNEK ÇALIŞMA SENARYOLARI

### Senaryo 1: Basit Built-in Komut

**Komut:** `pwd`

**Akış:**
```
1. main() → run_shell_loop() → readline("minishell$ ")
2. process_input("pwd") → tokenize_advanced() → [T_WORD: "pwd"]
3. parse_tokens_to_commands() → cmd{args: ["pwd", NULL]}
4. execute_commands() → execute_single_command()
5. is_builtin("pwd") → true → execute_builtin()
6. builtin_pwd_global() → get_env_value("PWD") veya getcwd()
7. printf("%s\n", pwd_value) → "/current/directory"
```

### Senaryo 2: Harici Komut

**Komut:** `ls -la`

**Akış:**
```
1. tokenize_advanced() → [T_WORD: "ls", T_WORD: "-la"]
2. parse_tokens_to_commands() → cmd{args: ["ls", "-la", NULL]}
3. execute_single_command() → is_builtin("ls") → false
4. execute_external_command() → find_command_path("ls")
5. PATH search: /usr/bin/ls bulunur
6. fork() → child process
7. Child: execve("/usr/bin/ls", ["ls", "-la"], env_array)
8. Parent: waitpid() → child'ın sonucunu bekle
```

### Senaryo 3: Pipeline

**Komut:** `cat file.txt | grep "hello" | wc -l`

**Akış:**
```
1. tokenize: [T_WORD: "cat", T_WORD: "file.txt", T_PIPE, T_WORD: "grep", ...]
2. parse: 3 komut oluştur, pipe ile bağla
3. execute_pipeline():
   
   Command 1 (cat):
   - pipe() → pipe_fd[0,1] oluştur
   - fork() → child
   - Child: stdout → pipe_fd[1], execve("cat")
   
   Command 2 (grep):
   - stdin ← pipe_fd[0], stdout → yeni_pipe_fd[1]
   - fork() → child
   - Child: execve("grep")
   
   Command 3 (wc):
   - stdin ← önceki_pipe_fd[0]
   - fork() → child
   - Child: execve("wc")
```

### Senaryo 4: Yönlendirme

**Komut:** `echo "Hello World" > output.txt`

**Akış:**
```
1. tokenize: [T_WORD: "echo", T_WORD: "Hello World", T_REDIRECT_OUT, T_WORD: "output.txt"]
2. parse: cmd{args: ["echo", "Hello World"], redirections: [>output.txt]}
3. execute_single_command() → is_builtin("echo") → true
4. setup_redirections() → open("output.txt", O_WRONLY|O_CREAT|O_TRUNC)
5. dup2(fd, STDOUT_FILENO) → stdout dosyaya yönlendir
6. builtin_echo() → printf("Hello World\n") → dosyaya yazılır
```

### Senaryo 5: Çevre Değişkeni Export

**Komut:** `export MY_VAR=hello`

**Akış:**
```
1. tokenize: [T_WORD: "export", T_WORD: "MY_VAR=hello"]
2. parse: cmd{args: ["export", "MY_VAR=hello"]}
3. execute_builtin() → builtin_export()
4. ft_strchr("MY_VAR=hello", '=') → '=' bulunur
5. key = "MY_VAR", value = "hello"
6. set_env_var() → find_env_node("MY_VAR") → bulunamaz
7. create_env_node("MY_VAR", "hello") → listeye ekle
```

### Senaryo 6: CD Komutu ve PWD Güncellemesi

**Komut:** `cd /tmp`

**Akış:**
```
1. builtin_cd([cd, /tmp]) → old_pwd = get_env_value("PWD")
2. chdir("/tmp") → sistem çağrısı başarılı
3. set_env_var("OLDPWD", old_pwd) → eski dizini kaydet
4. resolve_logical_path(old_pwd, "/tmp") → "/tmp" döndür
5. set_env_var("PWD", "/tmp") → yeni PWD ayarla
```

---

## 🔍 HATA YÖNETİMİ VE ÖZEL DURUMLAR

### 1. Bellek Yönetimi
- Her `malloc()` için karşılık gelen `free()`
- Token ve komut listelerinin düzgün temizlenmesi
- Global state cleanup `exit` komutunda

### 2. File Descriptor Yönetimi
- Pipe'ların doğru kapatılması
- Yönlendirme sonrası fd cleanup
- Child process'lerde fd inheritance

### 3. Process Yönetimi
- `fork()` hata kontrolü
- `waitpid()` ile zombie process önleme
- Child process'lerde `exit()` kullanımı

### 4. Error Handling
- NULL pointer kontrolü
- Sistem çağrısı hata kontrolü
- Graceful error messaging

Bu detaylı analiz, minishell projesinin tüm bileşenlerinin nasıl çalıştığını ve birbiriyle nasıl etkileşim kurduğunu gösterir. Her fonksiyon, satır satır açıklanmış ve gerçek kullanım senaryoları ile desteklenmiştir.
