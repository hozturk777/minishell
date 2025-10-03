# 🏗️ MİNİSHELL PROJESİ - KAPSAMLI KOD AKIŞ ANALİZİ

## 📋 **PROJE GENEL BAKIŞ**

Minishell projesi, bash benzeri bir shell uygulamasıdır. Kullanıcıdan komut alır, bu komutları analiz eder ve çalıştırır. Proje modüler bir yapıya sahiptir ve her modül belirli bir sorumluluğu üstlenir.

### **🎯 Ana Bileşenler:**
```
[USER INPUT] → [LEXER] → [PARSER] → [EXPANDER] → [EXECUTOR] → [OUTPUT]
      ↓           ↓         ↓           ↓            ↓           ↓
   "ls -la"   [TOKENS]  [COMMANDS]  [EXPANDED]   [EXECUTION] "file1..."
```

### **📁 Proje Yapısı:**
```
minishell/
├── include/
│   ├── libft/           # Yardımcı C fonksiyonları
│   └── minishell.h      # Ana header dosyası
├── source/
│   ├── core/            # Ana program akışı
│   ├── lexer/           # Token analizi
│   ├── parser/          # Syntax analizi
│   ├── expander/        # Variable ve quote expansion
│   ├── executer/        # Komut çalıştırma
│   ├── builtins/        # Built-in komutlar
│   ├── environment/     # Environment yönetimi
│   ├── redirection/     # Input/Output yönlendirme
│   ├── signal/          # Signal handling
│   └── utils/           # Yardımcı fonksiyonlar
└── objects/             # Compile edilmiş dosyalar
```

---

## 🔄 **ANA AKIŞ ŞEMASI**

### **1. Program Başlatma Akışı**
```
main()                                    // Program giriş noktası
├── init_global(envp)                    // Global state başlatma
│   ├── halloc(sizeof(t_global))         // Global struct oluştur
│   ├── init_env_from_envp(envp)         // Environment variables yükle
│   │   ├── extract_env_key()            // KEY=VALUE'dan KEY çıkar
│   │   ├── extract_env_value()          // KEY=VALUE'dan VALUE çıkar
│   │   └── create_env_node()            // Env node oluştur
│   ├── update_shlvl()                   // SHLVL değerini artır
│   └── return global
├── setup_signal_handlers()              // Signal handler'ları ayarla
│   ├── sigaction(SIGINT)                // Ctrl+C handler
│   └── sigaction(SIGQUIT)               // Ctrl+\ handler
├── print_welcome_advanced()             // Karşılama mesajı
└── run_shell_loop(global)               // Ana shell döngüsü
    └── while(1) readline() döngüsü      // Kullanıcı input'u bekle
```

### **2. Komut İşleme Akışı (Ana Pipeline)**
```
process_input(input, global)
├── tokenize_advanced(input, global)     // 1. LEXER PHASE
│   ├── init_lexer_advanced()           // Lexer state başlat
│   ├── check_quote_balance()           // Quote dengesini kontrol et
│   └── while(current_char != '\0')     // Token extraction loop
│       ├── get_next_token()            // Bir sonraki token'ı al
│       │   ├── handle_pipe_advanced()  // Pipe (|) token
│       │   ├── handle_redirect_advanced() // Redirect (<, >, <<, >>) token
│       │   ├── handle_word_advanced()  // Word token
│       │   └── handle_whitespaces_advanced() // Whitespace skip
│       └── ft_lstadd_back(tokens)      // Token'ı listeye ekle
├── parse_tokens_to_commands(tokens)     // 2. PARSER PHASE
│   ├── parse_single_command()          // Her komut için
│   │   ├── create_command()            // Command struct oluştur
│   │   ├── collect_command_arg()       // Argümanları topla
│   │   ├── parse_redirection()         // Redirection'ları parse et
│   │   └── convert_list_to_array()     // Lista'dan array'e çevir
│   └── return command_list
├── expand_command_args(commands)        // 3. EXPANSION PHASE
│   ├── needs_expansion()               // Expansion gerekli mi?
│   ├── expand_with_quotes()            // Quote expansion
│   │   ├── handle_dollar_expansion()   // $VAR expansion
│   │   ├── update_quote_state()        // Quote durumunu güncelle
│   │   └── process_character()         // Karakter işle
│   ├── remove_outer_quotes()           // Dış tırnakları kaldır
│   └── filter_empty_args()             // Boş argümanları filtrele
└── execute_commands(commands, global)   // 4. EXECUTION PHASE
    ├── is_builtin() ? builtin : external
    ├── setup_redirections()            // FD yönlendirme
    ├── setup_pipes()                   // Pipe kurulumu
    └── fork() + execve() veya builtin_*()
```

---

## 💻 **DETAYLI MODÜL ANALİZİ**

### **📁 1. CORE MODÜLÜ (main.c)**

#### **`main()` - Program Giriş Noktası**
```c
int main(int argc, char **argv, char **envp)
{
    t_global *global;                    // Global state container
    
    (void)argc; (void)argv;              // Unused parameters (42 norm)
    
    // PHASE 1: Global state initialization
    global = init_global(envp);          // Environment ve state setup
    if (!global)                         // Memory allocation check
    {
        write(2, "Error: Init failed\n", 19);
        return (1);
    }
    
    // PHASE 2: Signal setup
    setup_signal_handlers();            // SIGINT, SIGQUIT handler'ları
    
    // PHASE 3: Welcome message
    print_welcome_advanced();           // Kullanıcıya karşılama
    
    // PHASE 4: Main shell loop
    run_shell_loop(global);             // Ana döngü başlat
    
    // PHASE 5: Cleanup (normally unreachable)
    clear_garbage();                    // Memory cleanup
    return (0);
}
```

#### **`run_shell_loop()` - Ana Shell Döngüsü**
```c
void run_shell_loop(t_global *global)
{
    char *input;                        // Kullanıcı input'u
    
    while (1)                          // Sonsuz döngü
    {
        // INPUT PHASE: Kullanıcıdan komut al
        input = readline(PROMPT);       // "minishell$ " prompt ile input iste
        
        // EOF CHECK: Ctrl+D kontrolü
        if (!input)                     // EOF (Ctrl+D) geldi
        {
            printf("\n");               // Yeni satır
            cleanup_and_exit(0);        // Temiz çıkış
        }
        
        // EMPTY INPUT CHECK: Boş input kontrolü
        if (ft_strlen(input) == 0)      // Boş string
        {
            free(input);                // Readline memory'sini free et
            continue;                   // Döngüye devam
        }
        
        // HISTORY: Komut geçmişine ekle
        add_history(input);             // Readline history'ye ekle
        
        // PROCESSING: Komutu işle
        if (process_input(input, global) == -1)  // Hata durumu
        {
            free(input);                // Memory cleanup
            cleanup_and_exit(1);        // Hata ile çıkış
        }
        
        // CLEANUP: Input memory'sini temizle
        free(input);                    // Readline system memory
    }
}
```

### **📁 2. LEXER MODÜLÜ (Tokenization)**

#### **`tokenize_advanced()` - Ana Tokenization Fonksiyonu**
```c
t_list *tokenize_advanced(char *input, t_global *global)
{
    t_lexer_new *lexer;                 // Lexer state
    t_list *tokens;                     // Token listesi
    t_token_new *token;                 // Tek token
    int single_quote_count;             // Quote sayacı
    
    // QUOTE BALANCE CHECK: Tırnak dengesi kontrolü
    single_quote_count = 0;
    if (!check_quote_balance(input, &single_quote_count))
    {
        printf("Error: Unbalanced quotes detected\n");
        return (NULL);
    }
    
    // LEXER INITIALIZATION: Lexer başlat
    lexer = init_lexer_advanced(input, global);
    if (!lexer)
        return (NULL);
    
    tokens = NULL;                      // Token listesi başlat
    
    // TOKENIZATION LOOP: Ana tokenization döngüsü
    while (lexer->current_char != '\0') // Input sonuna kadar
    {
        // TOKEN EXTRACTION: Bir sonraki token'ı çıkar
        token = get_next_token(lexer, single_quote_count);
        global->echo_flag = 0;          // Echo flag reset
        
        if (token && token->value)      // Valid token
        {
            ft_lstadd_back(&tokens, ft_lstnew(token)); // Listeye ekle
        }
        else if (!token)                // Error durumu
        {
            printf("Error: Unclosed quote detected\n");
            return (NULL);
        }
        else if (token && !token->value[0]) // Empty token - end
        {
            break;
        }
    }
    
    return (tokens);
}
```

#### **`get_next_token()` - Token Extraction**
```c
t_token_new *get_next_token(t_lexer_new *lexer, int single_quote_count)
{
    // WHITESPACE SKIP: Boşlukları atla
    if (is_whitespace(lexer->current_char))
    {
        return (handle_whitespaces_advanced(lexer));
    }
    
    // PIPE TOKEN: | karakteri
    else if (lexer->current_char == '|')
    {
        return (handle_pipe_advanced(lexer));
    }
    
    // REDIRECT TOKENS: <, >, <<, >> karakterleri
    else if (lexer->current_char == '<' || lexer->current_char == '>')
    {
        return (handle_redirect_advanced(lexer));
    }
    
    // WORD TOKEN: Diğer tüm karakterler (komut, argüman, dosya adı)
    else
    {
        return (handle_word_advanced(lexer, single_quote_count));
    }
}
```

#### **Token Types ve İşleme:**
```c
typedef enum e_token_type_new
{
    TOKEN_WORD,          // Komut, argüman, dosya adı
    TOKEN_PIPE,          // |
    TOKEN_REDIRECT_IN,   // <
    TOKEN_REDIRECT_OUT,  // >
    TOKEN_REDIRECT_APPEND, // >>
    TOKEN_HEREDOC,       // <<
    TOKEN_EOF            // End of input
} t_token_type_new;

// ÖRNEK TOKENIZATION:
// Input:  "echo hello | grep h > output.txt"
// Tokens: [WORD:"echo"] [WORD:"hello"] [PIPE] [WORD:"grep"] [WORD:"h"] [REDIRECT_OUT] [WORD:"output.txt"]
```

### **📁 3. PARSER MODÜLÜ (Syntax Analysis)**

#### **`parse_tokens_to_commands()` - Token'ları Command'lara Çevir**
```c
t_command *parse_tokens_to_commands(t_list *tokens)
{
    t_command *commands;                // Command listesi
    t_command *current_cmd;             // Şu anki command
    t_list *current_token;              // Şu anki token
    
    commands = NULL;                    // Liste başlat
    current_token = tokens;
    
    // COMMAND PARSING LOOP: Her command için
    while (current_token)
    {
        // SINGLE COMMAND PARSE: Bir command parse et
        current_cmd = parse_single_command(&current_token);
        
        if (!current_cmd)               // Parse hatası
            return (NULL);
        
        // COMMAND CHAIN: Command'ı listeye ekle
        if (!commands)                  // İlk command
            commands = current_cmd;
        else                           // Sonraki command'lar
            add_command_to_list(&commands, current_cmd);
    }
    
    return (commands);
}
```

#### **`parse_single_command()` - Tek Bir Command Parse Et**
```c
t_command *parse_single_command(t_list **tokens)
{
    t_command *cmd;                     // Yeni command
    t_list *args_list;                  // Argüman listesi
    t_token_new *token;
    
    // COMMAND CREATION: Command struct oluştur
    cmd = create_command();
    if (!cmd)
        return (NULL);
    
    args_list = NULL;                   // Argüman listesi başlat
    
    // TOKEN PROCESSING LOOP: Token'ları işle
    while (*tokens)
    {
        token = (t_token_new *)(*tokens)->content;
        
        // PIPE CHECK: Pipe geldi, command bitir
        if (token->type == TOKEN_PIPE)
        {
            *tokens = (*tokens)->next;  // Pipe'ı atla
            break;
        }
        
        // REDIRECTION: Yönlendirme işle
        else if (is_redirection_token(token))
        {
            if (parse_redirection(cmd, tokens) == -1)
                return (NULL);
        }
        
        // ARGUMENT: Normal argüman
        else if (token->type == TOKEN_WORD)
        {
            collect_command_arg(&args_list, token->value);
            *tokens = (*tokens)->next;
        }
        
        else                           // Bilinmeyen token
        {
            *tokens = (*tokens)->next;
        }
    }
    
    // ARGS CONVERSION: Lista'dan array'e çevir
    cmd->args = convert_list_to_array(args_list);
    
    // EXPANSION: Argümanları expand et
    expand_command_args(cmd, g_global);
    
    return (cmd);
}

// ÖRNEK PARSE:
// Tokens: [WORD:"echo"] [WORD:"hello"] [REDIRECT_OUT] [WORD:"file.txt"] [PIPE] [WORD:"cat"]
// Command 1: {args: ["echo", "hello"], redirections: [OUTPUT > "file.txt"]}
// Command 2: {args: ["cat"], redirections: []}
```

### **📁 4. EXPANDER MODÜLÜ (Variable & Quote Expansion)**

#### **`expand_command_args()` - Argüman Expansion**
```c
void expand_command_args(t_command *cmd, t_global *global)
{
    int i;                              // Argüman index
    char *expanded;                     // Expand edilmiş string
    char *clean_str;                    // Temizlenmiş string
    
    if (!cmd || !cmd->args)
        return;
    
    i = 0;
    // ARGUMENT LOOP: Her argüman için
    while (cmd->args[i])
    {
        // SINGLE QUOTE CHECK: Single quote içindeyse expansion yok
        if (is_single_quoted_literal(cmd->args[i]))
        {
            // Single quote marker'ını kaldır, içeriği koru
            clean_str = extract_single_quote_content(cmd->args[i]);
            cmd->args[i] = clean_str;
        }
        
        // EXPANSION NEEDED: Variable expansion gerekli
        else if (needs_expansion(cmd->args[i]))
        {
            expanded = expand_with_quotes(cmd->args[i], global);
            if (expanded)
                cmd->args[i] = expanded;
        }
        
        // QUOTE REMOVAL: Sadece dış tırnakları kaldır
        else
        {
            expanded = remove_outer_quotes(cmd->args[i]);
            if (expanded)
                cmd->args[i] = expanded;
        }
        
        i++;
    }
    
    // EMPTY FILTER: Boş argümanları filtrele
    filter_empty_args(cmd);
}
```

#### **`expand_with_quotes()` - Quote ve Variable Expansion**
```c
char *expand_with_quotes(char *input, t_global *global)
{
    char *result;                       // Sonuç string
    char *temp;                         // Geçici string
    int i;                              // Index
    int quote_state[2];                 // [single_quote, double_quote]
    
    // INITIALIZATION: Başlangıç değerleri
    result = ft_strdup("");             // Boş string ile başla
    if (!result)
        return (NULL);
    
    i = 0;
    quote_state[0] = 0;                 // Single quote kapalı
    quote_state[1] = 0;                 // Double quote kapalı
    
    // CHARACTER LOOP: Her karakter için
    while (input[i])
    {
        // QUOTE STATE UPDATE: Quote durumunu güncelle
        update_quote_state(input[i], quote_state);
        
        // CHARACTER PROCESSING: Karakteri işle
        temp = process_character(input, &i, global, quote_state);
        
        if (!temp)                      // Quote karakteri, atla
            continue;
        
        // RESULT JOIN: Sonuca ekle
        result = ft_strjoin(result, temp);
        if (!result)
            return (NULL);
    }
    
    return (result);
}

// EXPANSION ÖRNEKLERİ:
// Input: "echo $USER"           → Output: "echo hasivaci"
// Input: "echo '$USER'"         → Output: "echo $USER" (single quote içinde expansion yok)
// Input: "echo \"$USER\""       → Output: "echo hasivaci" (double quote içinde expansion var)
// Input: "echo $USER$HOME"      → Output: "echo hasivaci/home/hasivaci"
```

#### **`handle_dollar_expansion()` - $ Variable Expansion**
```c
char *handle_dollar_expansion(char *input, int *i, t_global *global)
{
    char *var_name;                     // Variable adı
    char *var_value;                    // Variable değeri
    int start;                          // Başlangıç pozisyonu
    
    (*i)++;                            // $ karakterini atla
    start = *i;
    
    // SPECIAL VARIABLES: Özel değişkenler
    if (input[*i] == '?')              // $? - Exit status
    {
        (*i)++;
        return (ft_itoa(global->last_exit_code));
    }
    
    // VARIABLE NAME EXTRACTION: Variable adını çıkar
    while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
        (*i)++;
    
    // EXTRACT VAR NAME: Variable adını al
    var_name = ft_substr(input, start, *i - start);
    if (!var_name)
        return (ft_strdup(""));
    
    // GET VALUE: Environment'tan değeri al
    var_value = get_env_value(global->env_list, var_name);
    if (var_value)
        return (ft_strdup(var_value));
    
    // NOT FOUND: Bulunamadı, boş string döndür
    return (ft_strdup(""));
}
```

### **📁 5. EXECUTOR MODÜLÜ (Command Execution)**

#### **`execute_commands()` - Ana Execution Fonksiyonu**
```c
int execute_commands(t_command *commands, t_global *global)
{
    t_command *current;                 // Şu anki command
    int exit_code;                      // Çıkış kodu
    
    if (!commands)
        return (0);
    
    // SINGLE vs PIPELINE: Tek command mı pipeline mı?
    if (!commands->next)                // Tek command
    {
        return (execute_single_command(commands, global));
    }
    else                               // Pipeline
    {
        return (execute_pipeline(commands, global));
    }
}
```

#### **`execute_single_command()` - Tek Command Çalıştır**
```c
int execute_single_command(t_command *cmd, t_global *global)
{
    // BUILTIN CHECK: Built-in command mı?
    if (is_builtin(cmd->args[0]))
    {
        // BUILTIN EXECUTION: Built-in çalıştır
        return (execute_builtin(cmd, global));
    }
    else
    {
        // EXTERNAL EXECUTION: External command çalıştır
        return (execute_external_command(cmd, global));
    }
}
```

#### **`execute_external_command()` - External Command Çalıştır**
```c
int execute_external_command(t_command *cmd, t_global *global)
{
    pid_t pid;                          // Process ID
    int status;                         // Wait status
    char *path;                         // Executable path
    char **env_array;                   // Environment array
    
    // PATH FINDING: Executable path bul
    path = find_command_path(cmd->args[0], global->env_list);
    if (!path)
    {
        printf("minishell: %s: command not found\n", cmd->args[0]);
        return (127);
    }
    
    // ENVIRONMENT PREPARATION: Environment array oluştur
    env_array = env_list_to_array(global->env_list);
    
    // FORK: Child process oluştur
    pid = fork();
    if (pid == -1)                     // Fork hatası
    {
        perror("fork");
        return (1);
    }
    else if (pid == 0)                 // Child process
    {
        // CHILD SETUP: Child process konfigürasyonu
        setup_child_signals();         // Signal handler'ları sıfırla
        global->in_child = 1;          // Child flag
        
        // REDIRECTIONS: Input/Output yönlendirmeleri
        if (setup_redirections(cmd) == -1)
        {
            clear_garbage();
            exit(1);
        }
        
        // EXECUTION: Program çalıştır
        execve(path, cmd->args, env_array);
        
        // EXECVE FAILED: execve başarısız
        perror("execve");
        clear_garbage();
        exit(127);
    }
    else                               // Parent process
    {
        // PARENT WAIT: Child'ın bitmesini bekle
        waitpid(pid, &status, 0);
        
        // EXIT CODE CALCULATION: Çıkış kodunu hesapla
        if (WIFSIGNALED(status))       // Signal ile sonlandı
        {
            int signal_num = WTERMSIG(status);
            if (signal_num == SIGINT)
                return (130);          // Ctrl+C
            else if (signal_num == SIGQUIT)
                return (131);          // Ctrl+\
            else
                return (128 + signal_num);
        }
        
        return (WEXITSTATUS(status));  // Normal exit code
    }
}
```

#### **`execute_pipeline()` - Pipeline Execution**
```c
int execute_pipeline(t_command *commands, t_global *global)
{
    t_command *current;                 // Şu anki command
    int pipe_fds[2];                    // Pipe file descriptors
    int prev_fd;                        // Önceki pipe'ın read end'i
    pid_t *child_pids;                  // Child process ID'leri
    int cmd_count;                      // Command sayısı
    int i;
    
    // COMMAND COUNT: Command sayısını say
    cmd_count = count_commands(commands);
    child_pids = halloc(sizeof(pid_t) * cmd_count);
    
    // HEREDOC PREPROCESSING: Heredoc'ları önceden işle
    if (preprocess_heredocs(commands, global) == -1)
        return (1);
    
    current = commands;
    prev_fd = -1;                      // İlk command için stdin
    i = 0;
    
    // PIPELINE LOOP: Her command için
    while (current)
    {
        // PIPE CREATION: Sonraki command varsa pipe oluştur
        if (current->next)
        {
            if (pipe(pipe_fds) == -1)
            {
                perror("pipe");
                return (1);
            }
        }
        
        // FORK: Child process oluştur
        child_pids[i] = fork();
        if (child_pids[i] == -1)
        {
            perror("fork");
            return (1);
        }
        else if (child_pids[i] == 0)   // Child process
        {
            // CHILD PIPE SETUP: Pipe bağlantıları
            global->in_child = 1;
            
            // INPUT: Önceki pipe'dan input al
            if (prev_fd != -1)
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            
            // OUTPUT: Sonraki pipe'a output ver
            if (current->next)
            {
                dup2(pipe_fds[1], STDOUT_FILENO);
                close(pipe_fds[1]);
                close(pipe_fds[0]);
            }
            
            // REDIRECTIONS: Redirection'ları uygula
            if (setup_redirections(current) == -1)
            {
                clear_garbage();
                exit(1);
            }
            
            // EXECUTION: Command'ı çalıştır
            if (is_builtin(current->args[0]))
            {
                int exit_code = execute_builtin(current, global);
                clear_garbage();
                exit(exit_code);
            }
            else
            {
                execute_external_command(current, global);
                clear_garbage();
                exit(127);
            }
        }
        
        // PARENT CLEANUP: Parent process pipe cleanup
        if (prev_fd != -1)
            close(prev_fd);             // Önceki pipe'ı kapat
        
        if (current->next)
        {
            close(pipe_fds[1]);         // Write end'i kapat
            prev_fd = pipe_fds[0];      // Read end'i sakla
        }
        
        current = current->next;
        i++;
    }
    
    // FINAL CLEANUP: Son pipe'ı kapat
    if (prev_fd != -1)
        close(prev_fd);
    
    // WAIT ALL CHILDREN: Tüm child'ları bekle
    int final_exit_code = 0;
    i = 0;
    while (i < cmd_count)
    {
        int status;
        waitpid(child_pids[i], &status, 0);
        
        // Son command'ın exit code'unu al
        if (i == cmd_count - 1)
        {
            if (WIFSIGNALED(status))
                final_exit_code = 128 + WTERMSIG(status);
            else
                final_exit_code = WEXITSTATUS(status);
        }
        i++;
    }
    
    return (final_exit_code);
}

// PIPELINE ÖRNEK:
// Input: "echo hello | grep h | wc -l"
// 
// Process 1: echo hello    (stdout -> pipe1)
// Process 2: grep h        (stdin <- pipe1, stdout -> pipe2)
// Process 3: wc -l         (stdin <- pipe2, stdout -> terminal)
//
// Execution akışı:
// 1. 3 child process oluştur
// 2. Pipe'ları bağla
// 3. Her child kendi command'ını çalıştır
// 4. Parent tüm child'ları bekle
// 5. Son command'ın exit code'unu döndür
```

### **📁 6. BUILTIN MODÜLÜ (Built-in Commands)**

#### **Built-in Commands List:**
```c
// BUILTIN COMMANDS:
// - echo     : Text yazdırma
// - cd       : Directory değiştirme
// - pwd      : Current directory
// - export   : Environment variable export
// - unset    : Environment variable silme
// - env      : Environment variables listesi
// - exit     : Shell'den çıkış
```

#### **`execute_builtin()` - Built-in Command Çalıştır**
```c
int execute_builtin(t_command *cmd, t_global *global)
{
    char *command = cmd->args[0];       // Command adı
    
    // COMMAND DISPATCH: Command'a göre fonksiyon çağır
    if (ft_strcmp(command, "echo") == 0)
        return (builtin_echo(cmd->args, global));
    else if (ft_strcmp(command, "cd") == 0)
        return (builtin_cd(cmd->args, global));
    else if (ft_strcmp(command, "pwd") == 0)
        return (builtin_pwd(cmd->args, global));
    else if (ft_strcmp(command, "export") == 0)
        return (builtin_export(cmd->args, global));
    else if (ft_strcmp(command, "unset") == 0)
        return (builtin_unset(cmd->args, global));
    else if (ft_strcmp(command, "env") == 0)
        return (builtin_env(cmd->args, global));
    else if (ft_strcmp(command, "exit") == 0)
        return (builtin_exit(cmd->args, global));
    
    return (1);                        // Bilinmeyen builtin
}
```

#### **`builtin_export()` - Export Command Implementation**
```c
int builtin_export(char **args, t_global *global)
{
    int i;                              // Argüman index
    char *key;                          // Variable key
    char *value;                        // Variable value
    char *equal_sign;                   // = işaretinin pozisyonu
    
    // NO ARGS: Argüman yoksa export listesini göster
    if (!args[1])
    {
        print_export_env_sorted(global);
        return (0);
    }
    
    i = 1;
    // ARGS LOOP: Her argüman için
    while (args[i])
    {
        // FORMAT VALIDATION: Format doğrulaması
        if (!validate_export_format(args[i]))
        {
            printf("minishell: export: `%s': not a valid identifier\n", args[i]);
            return (1);
        }
        
        // KEY=VALUE PARSE: = işaretini ara
        equal_sign = ft_strchr(args[i], '=');
        
        if (equal_sign)                // KEY=VALUE formatı
        {
            // SPLIT KEY-VALUE: Key ve value'yu ayır
            *equal_sign = '\0';
            key = args[i];
            value = equal_sign + 1;
            
            // ENV SET: Environment'a ekle
            set_env_var(global, key, value);
            
            // EXPORT CLEANUP: Export-only listesinden çıkar
            remove_from_export_only_list(global, key);
            
            *equal_sign = '=';         // String'i geri eski haline getir
        }
        else                          // Sadece KEY formatı
        {
            key = args[i];
            
            // EXPORT-ONLY: Sadece export listesine ekle (env'e değil)
            if (!is_in_env_list(global->env_list, key))
                add_to_export_only_list(global, key);
        }
        
        i++;
    }
    
    return (0);
}

// EXPORT BEHAVIOR EXAMPLES:
// export VAR        → VAR export listesinde, env'de YOK, echo $VAR → boş
// export VAR=value  → VAR hem export hem env'de, echo $VAR → value
// export VAR=""     → VAR hem export hem env'de boş değerle
```

### **📁 7. ENVIRONMENT MODÜLÜ**

#### **`init_env_from_envp()` - Environment Initialization**
```c
t_env *init_env_from_envp(char **envp)
{
    t_env *env_list;                    // Environment listesi
    t_env *new_node;                    // Yeni node
    char *key;                          // Variable key
    char *value;                        // Variable value
    int i;
    
    env_list = NULL;                    // Liste başlat
    
    // NULL CHECK: envp NULL kontrolü (env -i durumu)
    if (!envp)
    {
        // MINIMAL ENV: Minimal environment oluştur
        set_env_var_direct(&env_list, "PWD", getcwd(NULL, 0));
        set_env_var_direct(&env_list, "SHLVL", "1");
        return (env_list);
    }
    
    i = 0;
    // ENVP LOOP: Her environment variable için
    while (envp[i])
    {
        // KEY-VALUE EXTRACTION: Key ve value'yu çıkar
        key = extract_env_key(envp[i]);      // KEY=VALUE'dan KEY
        value = extract_env_value(envp[i]);  // KEY=VALUE'dan VALUE
        
        if (key && value)               // Valid key-value pair
        {
            // NODE CREATION: Environment node oluştur
            new_node = create_env_node(key, value);
            if (new_node)
                add_env_node(&env_list, new_node);
        }
        
        i++;
    }
    
    return (env_list);
}
```

#### **Environment Variable Access:**
```c
// GET VALUE: Environment variable değerini al
char *get_env_value(t_env *env_list, char *key)
{
    t_env *current = env_list;
    
    while (current)
    {
        if (ft_strcmp(current->key, key) == 0)
            return (current->value);
        current = current->next;
    }
    
    return (NULL);                     // Bulunamadı
}

// SET VALUE: Environment variable değerini set et
void set_env_var(t_global *global, char *key, char *value)
{
    t_env *current = global->env_list;
    
    // SEARCH EXISTING: Var olan variable'ı ara
    while (current)
    {
        if (ft_strcmp(current->key, key) == 0)
        {
            // UPDATE: Mevcut değeri güncelle
            current->value = halloc_strdup(value);
            return;
        }
        current = current->next;
    }
    
    // ADD NEW: Yeni variable ekle
    t_env *new_node = create_env_node(key, value);
    if (new_node)
        add_env_node(&global->env_list, new_node);
}
```

### **📁 8. REDIRECTION MODÜLÜ**

#### **`setup_redirections()` - Redirection Setup**
```c
int setup_redirections(t_command *cmd)
{
    t_list *redirect_node;              // Redirection node
    t_redirect *redirect;               // Redirection data
    int fd;                             // File descriptor
    
    if (!cmd->redirections)
        return (0);                    // Redirection yok
    
    redirect_node = cmd->redirections;
    
    // REDIRECTION LOOP: Her redirection için
    while (redirect_node)
    {
        redirect = (t_redirect *)redirect_node->content;
        
        // REDIRECTION TYPE: Redirection tipine göre işle
        if (redirect->type == TOKEN_REDIRECT_OUT)      // >
        {
            fd = open(redirect->filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd == -1)
            {
                perror("open");
                return (-1);
            }
            dup2(fd, STDOUT_FILENO);   // stdout'u file'a yönlendir
            close(fd);
        }
        else if (redirect->type == TOKEN_REDIRECT_APPEND)  // >>
        {
            fd = open(redirect->filename, O_CREAT | O_WRONLY | O_APPEND, 0644);
            if (fd == -1)
            {
                perror("open");
                return (-1);
            }
            dup2(fd, STDOUT_FILENO);   // stdout'u file'a append
            close(fd);
        }
        else if (redirect->type == TOKEN_REDIRECT_IN)      // <
        {
            fd = open(redirect->filename, O_RDONLY);
            if (fd == -1)
            {
                perror("open");
                return (-1);
            }
            dup2(fd, STDIN_FILENO);    // stdin'i file'dan al
            close(fd);
        }
        else if (redirect->type == TOKEN_HEREDOC)          // <<
        {
            fd = handle_heredoc(redirect, global);
            if (fd == -1)
                return (-1);
            dup2(fd, STDIN_FILENO);    // stdin'i heredoc'dan al
            close(fd);
        }
        
        redirect_node = redirect_node->next;
    }
    
    return (0);
}
```

#### **`handle_heredoc()` - Heredoc Implementation**
```c
int handle_heredoc(t_redirect *redirect, t_global *global)
{
    int pipe_fd[2];                     // Pipe file descriptors
    char *line;                         // Input line
    char *expanded_line;                // Expanded line
    
    // PIPE CREATION: Heredoc için pipe oluştur
    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        return (-1);
    }
    
    // HEREDOC INPUT LOOP: Delimiter'a kadar input al
    while (1)
    {
        line = readline("> ");          // Heredoc prompt
        
        // DELIMITER CHECK: Delimiter geldi mi?
        if (!line || ft_strcmp(line, redirect->filename) == 0)
        {
            if (line)
                free(line);
            break;                     // Heredoc bitir
        }
        
        // EXPANSION: Line'ı expand et (variable expansion)
        expanded_line = expand_with_heredoc(line, global);
        
        // WRITE TO PIPE: Pipe'a yaz
        write(pipe_fd[1], expanded_line, ft_strlen(expanded_line));
        write(pipe_fd[1], "\n", 1);
        
        // CLEANUP: Memory temizle
        free(line);
        // expanded_line garbage collector ile yönetiliyor
    }
    
    close(pipe_fd[1]);                 // Write end'i kapat
    return (pipe_fd[0]);               // Read end'i döndür
}

// HEREDOC ÖRNEK:
// cat <<EOF
// Bu bir heredoc
// $USER kullanıcısı
// EOF
//
// Çalışma:
// 1. Pipe oluştur
// 2. readline ile satır satır input al
// 3. Her satırı expand et ($USER → hasivaci)
// 4. Pipe'a yaz
// 5. EOF gelince pipe'ın read end'ini döndür
```

### **📁 9. SIGNAL MODÜLÜ**

#### **`setup_signal_handlers()` - Signal Handler Setup**
```c
void setup_signal_handlers(void)
{
    struct sigaction sa_int;            // SIGINT action
    struct sigaction sa_quit;           // SIGQUIT action
    
    // SIGINT SETUP: Ctrl+C handler
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);
    
    // SIGQUIT SETUP: Ctrl+\ handler (ignore)
    sa_quit.sa_handler = SIG_IGN;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);
}
```

#### **`sigint_handler()` - SIGINT Handler (Ctrl+C)**
```c
void sigint_handler(int sig)
{
    (void)sig;                          // Unused parameter
    
    // CHILD CHECK: Child process'te mi?
    if (g_global && g_global->in_child)
    {
        // CHILD HANDLING: Child process'te SIGINT
        write(STDOUT_FILENO, "\n", 1);  // Yeni satır
        clear_garbage();                // Memory cleanup
        exit(130);                      // SIGINT exit code
    }
    else
    {
        // PARENT HANDLING: Parent process'te SIGINT
        write(STDOUT_FILENO, "\n", 1);  // Yeni satır
        rl_on_new_line();              // Readline'a yeni satır bildir
        rl_replace_line("", 0);        // Mevcut line'ı temizle
        rl_redisplay();                // Prompt'u tekrar göster
        
        if (g_global)
            g_global->last_exit_code = 130; // Exit code güncelle
    }
}
```

---

## 🔗 **DATA STRUCTURES**

### **Core Structures:**
```c
// GLOBAL STATE: Tüm shell state'i
typedef struct s_global
{
    t_env           *env_list;          // Environment variables
    t_export_only   *export_only_list;  // Export-only variables
    int             last_exit_code;     // Son komutun exit code'u
    int             in_child;           // Child process flag
    int             echo_flag;          // Echo mode flag
} t_global;

// COMMAND STRUCTURE: Bir komut ve özellikleri
typedef struct s_command
{
    char            **args;             // Command arguments
    t_list          *redirections;      // Redirection listesi
    struct s_command *next;             // Pipeline'da sonraki command
} t_command;

// TOKEN STRUCTURE: Lexer token'ı
typedef struct s_token_new
{
    t_token_type_new type;              // Token tipi
    char            *value;             // Token değeri
} t_token_new;

// ENVIRONMENT NODE: Environment variable
typedef struct s_env
{
    char            *key;               // Variable adı
    char            *value;             // Variable değeri
    char            *full_var;          // KEY=VALUE formatı
    struct s_env    *next;              // Sonraki node
} t_env;

// REDIRECTION: Input/Output yönlendirme
typedef struct s_redirect
{
    t_token_type_new type;              // Redirection tipi
    char            *filename;          // Dosya adı
    int             fd;                 // File descriptor
} t_redirect;
```

---

## 🧩 **ÖRNEK EXECUTION FLOW**

### **Komut: `echo "Hello $USER" | grep Hello > output.txt`**

#### **1. TOKENIZATION:**
```
Input: echo "Hello $USER" | grep Hello > output.txt

Tokens:
[WORD: "echo"]
[WORD: "Hello $USER"]      // Tırnak içinde, expansion gerekli
[PIPE: "|"]
[WORD: "grep"]
[WORD: "Hello"]
[REDIRECT_OUT: ">"]
[WORD: "output.txt"]
```

#### **2. PARSING:**
```
Command 1:
├── args: ["echo", "Hello $USER"]
├── redirections: []
└── next: Command 2

Command 2:
├── args: ["grep", "Hello"]
├── redirections: [OUTPUT > "output.txt"]
└── next: NULL
```

#### **3. EXPANSION:**
```
Command 1 BEFORE: ["echo", "Hello $USER"]
Command 1 AFTER:  ["echo", "Hello hasivaci"]    // $USER expanded

Command 2 BEFORE: ["grep", "Hello"]
Command 2 AFTER:  ["grep", "Hello"]             // No expansion needed
```

#### **4. EXECUTION:**
```
Pipeline Execution:
├── Create pipe: pipe_fd[0] (read), pipe_fd[1] (write)
├── Fork Command 1 (echo):
│   ├── Setup: stdout → pipe_fd[1]
│   ├── Execute: echo "Hello hasivaci"
│   └── Output: "Hello hasivaci\n" → pipe
├── Fork Command 2 (grep):
│   ├── Setup: stdin ← pipe_fd[0], stdout → "output.txt"
│   ├── Execute: grep Hello
│   └── Output: "Hello hasivaci\n" → output.txt
└── Wait for both children
```

#### **5. RESULT:**
```
output.txt içeriği: "Hello hasivaci\n"
Shell prompt tekrar görünür
Exit code: 0 (success)
```

---

## 🚀 **MEMORY MANAGEMENT**

### **Garbage Collector System:**
```c
// GARBAGE COLLECTOR: Otomatik memory yönetimi
static t_list *garbage_collector = NULL;

// HALLOC: Tracked malloc
void *halloc(size_t size)
{
    void *ptr = malloc(size);           // Normal malloc
    if (!ptr)
    {
        clear_garbage();                // Cleanup on failure
        exit(1);                       // Critical failure
    }
    
    add_garbage(ptr);                  // Track pointer
    return (ptr);
}

// ADD GARBAGE: Pointer'ı track et
void *add_garbage(void *ptr)
{
    t_list *new_node = malloc(sizeof(t_list));
    new_node->content = ptr;
    new_node->next = garbage_collector;
    garbage_collector = new_node;
    return (ptr);
}

// CLEAR GARBAGE: Tüm tracked memory'yi free et
void clear_garbage(void)
{
    t_list *current = garbage_collector;
    t_list *next;
    
    while (current)
    {
        next = current->next;
        if (current->content)
            free(current->content);     // Actual data free
        free(current);                 // Node free
        current = next;
    }
    
    garbage_collector = NULL;          // Reset tracker
}
```

### **Memory Usage Pattern:**
```
Program Start:
├── init_global(): Environment loading
├── Main Loop:
│   ├── readline(): System malloc (user responsibility)
│   ├── tokenize(): halloc for tokens
│   ├── parse(): halloc for commands
│   ├── expand(): halloc for expanded strings
│   ├── execute(): halloc for temporary data
│   └── clear_garbage(): Free all halloc'ed memory
└── Program Exit: Final cleanup
```

---

## ⚠️ **ERROR HANDLING**

### **Error Categories:**
```c
// SYNTAX ERRORS: Parsing hatası
"Error: Unbalanced quotes detected"
"minishell: syntax error near unexpected token"

// COMMAND ERRORS: Komut bulunamadı
"minishell: command_name: command not found"

// SYSTEM ERRORS: System call hatası
"fork: Cannot allocate memory"
"pipe: Too many open files"

// BUILTIN ERRORS: Built-in komut hatası
"minishell: export: 'invalid_name': not a valid identifier"
"minishell: cd: No such file or directory"
```

### **Error Handling Strategy:**
```c
// GRACEFUL DEGRADATION: Hata durumunda güvenli devam
if (tokenize_error)
{
    printf("Syntax error\n");
    global->last_exit_code = 2;
    continue;                          // Shell'i crash ettirme
}

// CRITICAL ERRORS: Memory hatası
if (malloc_failed)
{
    clear_garbage();                   // Cleanup
    exit(1);                          // Program sonlandır
}

// COMMAND ERRORS: Komut hatası
if (command_not_found)
{
    printf("Command not found\n");
    return (127);                     // Bash compatible exit code
}
```

---

## 🔄 **LIFECYCLE SUMMARY**

### **Complete Program Lifecycle:**
```
STARTUP:
├── main() entry
├── init_global() → Environment setup
├── setup_signal_handlers() → Signal management
└── print_welcome() → User greeting

MAIN LOOP:
├── readline() → Get user input
├── tokenize_advanced() → Lexical analysis
├── parse_tokens_to_commands() → Syntax analysis
├── expand_command_args() → Variable/quote expansion
├── execute_commands() → Command execution
├── clear_garbage() → Memory cleanup
└── LOOP → Next command

SHUTDOWN:
├── Ctrl+D or exit command
├── cleanup_and_exit()
├── clear_garbage() → Final cleanup
├── rl_clear_history() → Readline cleanup
└── exit() → Program termination
```

Bu dokümantasyon, minishell projenizin tüm bileşenlerinin nasıl çalıştığını ve birbirleriyle nasıl etkileşim kurduğunu detaylı bir şekilde açıklamaktadır. Her modül, kendi sorumluluğu dahilinde çalışır ve diğer modüllerle defined interface'ler üzerinden iletişim kurar.
