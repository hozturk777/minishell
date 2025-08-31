# 🏗️ MINISHELL MODÜLER YAPI PLANI

## 📁 **Mevcut Dosya Yapısı ve Modüler Bölümlendirme**

Bu dokuman minishell projesinin hangi dosya ve fonksiyonların hangi modüle ait olduğunu detaylı olarak açıklar.

---

## 🎯 **1. LEXER MODÜLÜ** 
*Giriş metnini token'lara ayırma işlemleri*

### **📂 Dosyalar:**
```
lexer/
├── lexer.c                    [MEVCUT: source/lexer.c]
├── tokenizer.c                [MEVCUT: source/tokenizer.c]  
├── token_handlers.c           [MEVCUT: source/token_handlers.c]
└── lexer.h                    [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **lexer.c:**
- `t_lexer_new *init_lexer_advanced(char *input, t_global *global)`
- `void free_lexer_advanced(t_lexer_new *lexer)`
- `void advance_lexer(t_lexer_new *lexer)`

#### **tokenizer.c:**
- `t_list *tokenize_advanced(char *input, t_global *global)`
- `t_token_new *create_token_advanced(t_token_types type, char *value)`
- `void free_token_advanced(void *token)`
- `void free_tokens_advanced(t_list **tokens)`

#### **token_handlers.c:**
- `t_token_new *handle_pipe_advanced(t_lexer_new *lexer)`
- `t_token_new *handle_redirect_advanced(t_lexer_new *lexer)`
- `t_token_new *handle_word_advanced(t_lexer_new *lexer)`
- `t_token_new *handle_quotes_advanced(t_lexer_new *lexer)`

### **📋 Sorumluluklar:**
- Input string'i token'lara ayırma
- Token türlerini belirleme (WORD, PIPE, REDIRECT, vs.)
- Özel karakterleri işleme (|, >, <, >>, <<)
- Tırnak içindeki metinleri işleme

---

## 🎯 **2. PARSER MODÜLÜ**
*Token'ları komut yapılarına dönüştürme*

### **📂 Dosyalar:**
```
parser/
├── parser.c                   [MEVCUT: loop_dir/parser.c]
├── command_builder.c          [YENİ: Komut oluşturma]
├── redirection_parser.c       [YENİ: Yönlendirme parsing]
└── parser.h                   [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **parser.c:**
- `t_command *parse_tokens_to_commands(t_list *tokens, t_global *global)`
- `t_command *parse_single_command(t_list **token_node, t_global *global)`
- `int is_command_start(t_list *token_node)`
- `int is_pipe_token(t_list *token_node)`
- `int is_redirect_token(t_list *token_node)`
- `int is_word_token(t_list *token_node)`

#### **command_builder.c:**
- `t_command *create_command(void)`
- `void free_command(t_command *cmd)`
- `void free_commands(t_list **commands)`
- `void free_commands_list(t_command *commands)`
- `void collect_command_arg(t_list **args_list, t_list *token_node)`
- `char **convert_list_to_array(t_list *args_list)`
- `void append_command_to_chain(t_command *head, t_command *new_cmd)`

#### **redirection_parser.c:**
- `void parse_redirection(t_command *cmd, t_list **token_node, t_global *global)`
- `void add_redirect_to_command(t_command *cmd, t_redirect *redirect)`

### **📋 Sorumluluklar:**
- Token listelerini komut yapılarına çevirme
- Pipeline'ları parse etme  
- Yönlendirmeleri parse etme
- Komut argümanlarını organize etme

---

## 🎯 **3. EXPANDER MODÜLÜ**
*Değişken genişletme ve tırnak işleme*

### **📂 Dosyalar:**
```
expander/
├── variable_expansion.c       [MEVCUT: source/variable_expansion.c]
├── quote_expansion.c          [MEVCUT: source/quote_expansion.c]
├── env_expansion.c            [YENİ: Çevre değişkeni genişletme]
└── expander.h                 [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **variable_expansion.c:**
- `char *expand_variables(char *input, t_global *global)`
- `char *handle_dollar_expansion(char *input, int *i, t_global *global)`
- `char *handle_regular_char(char *input, int *i)`
- `char *join_and_free(char *s1, char *s2)`

#### **quote_expansion.c:**
- `char *expand_with_quotes(char *input, t_global *global)`
- `void update_quote_state(char c, int *quote_state)`
- `char *remove_outer_quotes(char *input)`
- `int needs_expansion(char *str)`
- `void expand_command_args(t_command *cmd, t_global *global)`
- `char *remove_quotes(char *str)`

#### **env_expansion.c:**
- `char *expand_env_var(char *var_name, t_global *global)`

### **📋 Sorumluluklar:**
- $VAR değişken genişletme
- Tırnak içindeki metinleri işleme
- Çevre değişkenlerini çözümleme
- Quote removal işlemleri

---

## 🎯 **4. EXECUTOR MODÜLÜ**
*Komut çalıştırma ve pipeline yönetimi*

### **📂 Dosyalar:**
```
executor/
├── executor.c                 [MEVCUT: executer/executor.c]
├── pipeline.c                 [YENİ: Pipeline özel işlemleri]
├── process_manager.c          [YENİ: Process yönetimi]
└── executor.h                 [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **executor.c:**
- `int execute_commands(t_command *commands, t_global *global)`
- `int execute_single_command(t_command *cmd, t_global *global)`
- `int execute_external_command(t_command *cmd, t_global *global)`

#### **pipeline.c:**
- `int execute_pipeline(t_command *commands, t_global *global)`
- `int execute_pipeline_command(t_command *cmd, t_global *global, int prev_fd, int *pipe_fd)`

#### **process_manager.c:**
- Process yaratma ve yönetme fonksiyonları

### **📋 Sorumluluklar:**
- Komut çalıştırma koordinasyonu
- Pipeline setup ve yönetimi
- External program çalıştırma
- Process yaratma ve beklemek

---

## 🎯 **5. BUILTINS MODÜLÜ**
*Built-in komutlar*

### **📂 Dosyalar:**
```
builtins/
├── builtins.c                 [MEVCUT: executer/builtins.c]
├── builtins2.c                [MEVCUT: executer/builtins2.c]
├── builtin_utils.c            [YENİ: Built-in yardımcı fonksiyonlar]
└── builtins.h                 [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **builtins.c:**
- `int is_builtin(char *command)`
- `int execute_builtin(t_command *cmd, t_global *global)`
- `int builtin_pwd(void)`
- `int builtin_pwd_global(t_global *global)`
- `int builtin_echo(char **args)`
- `int builtin_env(t_env *env_list)`
- `int builtin_exit(char **args, t_global *global)`

#### **builtins2.c:**
- `int builtin_cd(char **args, t_global *global)`
- `int builtin_export(char **args, t_global *global)`
- `int builtin_unset(char **args, t_global *global)`

#### **builtin_utils.c:**
- `void update_pwd_env(t_global *global)`
- `void print_export_env(t_env *env_list)`
- `void set_env_var(t_global *global, char *key, char *value)`
- `void unset_env_var(t_global *global, char *key)`
- `char *resolve_logical_path(char *current_pwd, char *path)`
- `char *find_existing_parent(char *path)`
- `char *try_parent_fallback(char *failed_path)`

### **📋 Sorumluluklar:**
- 7 mandatory built-in komut (echo, cd, pwd, export, unset, env, exit)
- Çevre değişkeni yönetimi
- Directory navigation
- Exit status yönetimi

---

## 🎯 **6. REDIRECTION MODÜLÜ**
*I/O yönlendirme işlemleri*

### **📂 Dosyalar:**
```
redirection/
├── redirections.c             [MEVCUT: executer/redirections.c]
├── heredoc.c                  [YENİ: Here document işlemleri]
├── file_operations.c          [YENİ: Dosya işlemleri]
└── redirection.h              [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **redirections.c:**
- `void setup_redirections(t_command *cmd)`
- `void handle_single_redirection(t_redirect *redirect)`
- `void setup_pipeline_fds(t_command *cmd, int prev_fd, int *pipe_fd)`

#### **heredoc.c:**
- `int handle_heredoc(char *delimiter)`
- `char *generate_temp_filename(void)`

#### **file_operations.c:**
- Dosya açma/kapama/yazma işlemleri

### **📋 Sorumluluklar:**
- Input redirection (<)
- Output redirection (>)
- Append redirection (>>)
- Here document (<<)
- File descriptor yönetimi

---

## 🎯 **7. ENVIRONMENT MODÜLÜ**
*Çevre değişkeni yönetimi*

### **📂 Dosyalar:**
```
environment/
├── environment.c              [MEVCUT: source/environment.c]
├── env_utils.c                [MEVCUT: source/env_utils.c]
├── path_utils.c               [MEVCUT: executer/path_utils.c]
└── environment.h              [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **environment.c:**
- `t_env *init_env_from_envp(char **envp)`
- `t_env *create_env_node(char *key, char *value)`
- `void add_env_node(t_env **env_list, t_env *new_node)`
- `t_env *find_env_node(t_env *env_list, char *key)`
- `void free_env_list(t_env *env_list)`

#### **env_utils.c:**
- Environment utility fonksiyonları

#### **path_utils.c:**
- `char *find_command_path(char *command, t_env *env_list)`
- `char *build_full_path(char *dir, char *command)`
- `char *get_env_value(t_env *env_list, char *key)`
- `void free_string_array(char **array)`
- `char **env_list_to_array(t_env *env_list)`
- `int count_env_nodes(t_env *env_list)`

### **📋 Sorumluluklar:**
- Çevre değişkeni listesi yönetimi
- PATH çözümleme
- Environment array dönüşümleri
- Komut path bulma

---

## 🎯 **8. SIGNALS MODÜLÜ**
*Sinyal işleme*

### **📂 Dosyalar:**
```
signals/
├── signal_handler.c           [MEVCUT: source/signal_handler.c]
├── signal_utils.c             [YENİ: Sinyal yardımcı fonksiyonlar]
└── signals.h                  [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **signal_handler.c:**
- `void setup_signals(void)`
- `void sigint_handler(int sig)`
- `void sigquit_handler(int sig)`
- `void restore_signals(void)`
- `void setup_child_signals(void)`
- `void handle_eof(void)`

#### **signal_utils.c:**
- Sinyal yardımcı fonksiyonları

### **📋 Sorumluluklar:**
- SIGINT (Ctrl+C) yönetimi
- SIGQUIT (Ctrl+\) yönetimi
- EOF (Ctrl+D) yönetimi
- Child process sinyal ayarları

---

## 🎯 **9. UTILS MODÜLÜ**
*Yardımcı fonksiyonlar ve debug*

### **📂 Dosyalar:**
```
utils/
├── debug.c                    [MEVCUT: source/debug.c]
├── debug_utils.c              [MEVCUT: source/debug_utils.c]
├── global_state.c             [MEVCUT: source/global_state.c]
├── string_utils.c             [YENİ: String işlemleri]
└── utils.h                    [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **debug.c:**
- `void debug_print(char *msg)`

#### **debug_utils.c:**
- `void print_tokens_advanced(t_list *tokens)`
- `void print_commands(t_list *commands)`
- `void print_commands_debug(t_command *commands)`

#### **global_state.c:**
- `t_global *init_global(char **envp)`
- `void free_global(t_global *global)`
- `void update_exit_status(t_global *global, int status)`

#### **string_utils.c:**
- `int is_special_char(char c)`
- String manipulation fonksiyonları

### **📋 Sorumluluklar:**
- Debug ve logging
- Global state yönetimi
- String utility fonksiyonları
- Memory management yardımcıları

---

## 🎯 **10. CORE MODÜLÜ**
*Ana program akışı*

### **📂 Dosyalar:**
```
core/
├── main.c                     [MEVCUT: source/main.c]
├── shell_loop.c               [YENİ: Shell döngüsü]
├── input_handler.c            [YENİ: Input işleme]
└── core.h                     [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **main.c:**
- `int main(int argc, char **argv, char **envp)`
- `static void print_welcome_advanced(void)`

#### **shell_loop.c:**
- `static void run_shell_loop(t_global *global)`

#### **input_handler.c:**
- `static int process_input(char *input, t_global *global)`

### **📋 Sorumluluklar:**
- Program başlangıcı
- Ana shell döngüsü
- Input okuma ve işleme
- Program sonlandırma

---

## 🎯 **11. INCLUDE MODÜLÜ**
*Header dosyaları ve kütüphaneler*

### **📂 Dosyalar:**
```
include/
├── minishell.h                [MEVCUT: lib/minishell.h - Ana header]
├── types.h                    [YENİ: Type definitions]
├── constants.h                [YENİ: Constants ve defines]
└── libft/                     [MEVCUT: libft kütüphanesi]
    ├── libft.h
    ├── *.c files
    └── Makefile
```

### **📋 Sorumluluklar:**
- Tüm type definitions
- Function prototypes
- Constants ve macros
- Libft kütüphanesi

---

## 📋 **MODÜLER AYIRMA PLANI**

### **🚀 Önerilen Adımlar:**

1. **Header Dosyaları Oluştur:**
   - Her modül için ayrı header dosyası
   - Function prototypes'ları modüllere ayır
   - Type definitions'ları organize et

2. **Dosyaları Taşı:**
   - Mevcut dosyaları uygun modül dizinlerine taşı
   - Fonksiyonları mantıklı dosyalara böl
   - Yeni utility dosyaları oluştur

3. **Makefile Güncelle:**
   - Modüler compilation rules
   - Her modül için ayrı compilation
   - Dependency management

4. **Include Yollarını Düzenle:**
   - Relative path'leri güncelle
   - Header dependencies'i çöz
   - Cross-module dependencies'i minimize et

### **🎯 Avantajları:**
- ✅ Daha temiz ve organize kod yapısı
- ✅ Kolay maintenance ve debugging
- ✅ Modüler test edilebilirlik
- ✅ Parallel development imkanı
- ✅ Kod reusability
- ✅ Better separation of concerns

### **⚠️ Dikkat Edilecek Noktalar:**
- Header file circular dependencies
- Global variable erişimi
- Module interface design
- Compilation order
- Memory management across modules

---

**🏆 Bu plan ile minishell projesi tamamen modüler ve professional bir yapıya kavuşacak!**
