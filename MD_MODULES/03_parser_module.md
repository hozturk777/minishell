# 🏗️ PARSER MODULE - Sözdizimi Analiz Modülü

## 📋 İçindekiler
1. [parser.c - Ana Parser Motoru](#parserc---ana-parser-motoru)
2. [parser_is.c - Token Tanıma Fonksiyonları](#parser_isc---token-tanıma-fonksiyonları)
3. [command_utils.c - Komut Yardımcı Fonksiyonları](#command_utilsc---komut-yardımcı-fonksiyonları)
4. [create_command - Komut Yapısı Oluşturucu](#create_command---komut-yapısı-oluşturucu)

---

## 🎯 Parser Modülünün Amacı

**Parser (Sözdizimi Analiz)** modülü, lexer'dan gelen token'ları anlamlı komut yapılarına dönüştürür. Bu compiler design'ın **syntax analysis** fazıdır.

### 🔄 Parser İşlem Akışı
```
Token List (from Lexer)
        ↓
    Syntax Validation
        ↓
    Command Structure Building
        ↓
    Pipeline Recognition
        ↓
    Redirection Parsing
        ↓
    Command Chain Creation
        ↓
Command Structure List (to Executor)
```

### 🎪 Parser'ın Tanıdığı Yapılar
```bash
# Simple Command
ls -l

# Pipeline  
cat file.txt | grep pattern | wc -l

# Redirections
cat < input.txt > output.txt

# Heredoc
cat << EOF

# Complex Mixed
echo "hello" | grep 'pattern' >> log.txt
```

---

## 📄 parser.c - Ana Parser Motoru

### 🎯 Dosyanın Amacı
Token listesini command structure'lara dönüştüren ana parsing logic'ini içerir.

### 📝 Satır Satır Kod Analizi

#### 🚀 Ana Parsing Fonksiyonu

```c
t_command *parse_tokens_to_commands(t_list *tokens, t_global *global)
{
    t_command   *head;      // Command chain başlangıcı
    t_command   *current;   // Mevcut işlenen command
    t_list      *token_node; // Token list iterator
    
    // 1. INITIALIZATION
    head = NULL;            // Command chain henüz yok
    current = NULL;         // Mevcut command henüz yok  
    token_node = tokens;    // Token listesinin başından başla
    
    // 2. MAIN PARSING LOOP
    while (token_node)      // Tüm token'ları gez
    {
        // 3. COMMAND START DETECTION
        if (is_command_start(token_node)) // Token command başlangıcı mı?
        {
            // 4. SINGLE COMMAND PARSING
            current = parse_single_command(&token_node, global);
            if (!current)   // Parsing başarısız
            {
                return (NULL);
            }
            
            // 5. COMMAND CHAIN BUILDING
            if (!head)      // İlk command
                head = current;
            else           // Subsequent commands - pipeline
                append_command_to_chain(head, current);
        }
        else              // Command start değil
            token_node = token_node->next; // Sonraki token'a geç
    }
    
    return (head);        // Command chain'i döndür
}
```

**Açıklama:**
- **Line 7-9**: Temel initialization - command chain ve iterator setup
- **Line 12**: Ana parsing loop - her token'ı kontrol et
- **Line 15**: Command başlangıcı detection (T_CMD, T_WORD, etc.)
- **Line 18**: Tek command'ı parse et (pipe'a kadar)
- **Line 24-27**: Command chain building - pipeline structure
- **Line 29**: Non-command token'ları atla
- **Line 32**: Final command chain

**Kullanım Senaryosu:**
```c
// Input tokens: [T_CMD:"ls"] [T_WORD:"-l"] [T_PIPE:"|"] [T_CMD:"grep"] [T_WORD:".c"]
// Result: 
// Command 1: {args: ["ls", "-l"], next: Command 2}
// Command 2: {args: ["grep", ".c"], next: NULL}
```

---

#### ✅ Syntax Validation

```c
int check_syntax(t_list **token_node)
{
    t_list      *token_temp;
    t_token_new *token_test;
    
    token_temp = *token_node;
    
    while (token_temp)      // Tüm token'ları kontrol et
    {
        token_test = (t_token_new *)token_temp->content;
        
        // 1. REDIRECTION SYNTAX CHECK
        if (token_test->type == T_REDIRECT_IN || token_test->type == T_REDIRECT_OUT || 
            token_test->type == T_APPEND)
        {
            token_temp = token_temp->next;  // Redirection sonrası token
            
            // EOF check - redirection sonrası dosya adı olmalı
            if (!token_temp)
            {
                printf("minishell: syntax error near unexpected token `newline'\n");
                return (1);    // Syntax error
            }
            
            token_test = (t_token_new *)token_temp->content;
            
            // Whitespace skip
            if (token_test->type == T_WHITESPACE)
            {
                token_temp = token_temp->next;
                token_test = (t_token_new *)token_temp->content;
            }
            
            // Filename validation
            if (token_test->type != T_WORD)
            {
                printf("minishell: syntax error near unexpected token `%s'\n", 
                       token_test->value);
                return (1);    // Syntax error
            }
            continue;
        }
        
        // 2. HEREDOC SYNTAX CHECK
        else if (token_test->type == T_HEREDOC)
        {
            token_temp = token_temp->next;  // Heredoc sonrası token
            
            // EOF check - heredoc sonrası delimiter olmalı
            if (!token_temp)
            {
                printf("minishell: syntax error near unexpected token `newline'\n");
                return (1);    // Syntax error
            }
            
            token_test = (t_token_new *)token_temp->content;
            
            // Delimiter validation
            if (token_test->type != T_WORD && token_test->type != T_CMD)
            {
                printf("minishell: syntax error near unexpected token `%s'\n", 
                       token_test->value);
                return (1);    // Syntax error
            }
            continue;
        }
        
        token_temp = token_temp->next;
    }
    
    return (0);            // Syntax OK
}
```

**Syntax Kuralları:**
1. **Redirection**: `<`, `>`, `>>` sonrası mutlaka filename olmalı
2. **Heredoc**: `<<` sonrası mutlaka delimiter olmalı
3. **EOF**: Incomplete redirection/heredoc olmamalı

**Error Örnekleri:**
```bash
cat >           # Error: missing filename
cat <<          # Error: missing delimiter  
cat > |         # Error: invalid token after redirection
```

---

#### 🔧 Tek Komut Parsing

```c
t_command *parse_single_command(t_list **token_node, t_global *global)
{
    t_command   *cmd;       // Oluşturulacak komut
    t_list      *args_list; // Argüman listesi (geçici)
    t_list      *current;   // Token iterator
    
    // 1. COMMAND STRUCTURE INITIALIZATION
    cmd = create_command(); // Boş komut yapısı oluştur
    if (!cmd)              // Memory allocation başarısız
        return (NULL);
    
    args_list = NULL;      // Argüman listesi başlat
    current = *token_node; // Current token iterator
    
    // 2. SYNTAX VALIDATION
    if (check_syntax(token_node)) // Syntax kontrol et
    {
        return (NULL);     // Syntax error varsa fail
    }
    
    // 3. TOKEN PROCESSING LOOP (Pipe'a kadar)
    while (current && !is_pipe_token(current))
    {
        // 4. REDIRECTION PROCESSING
        if (is_redirect_token(current))
            parse_redirection(cmd, &current, global); // Redirect'i cmd'e ekle
        
        // 5. ARGUMENT PROCESSING
        else if (is_word_token(current))
            collect_command_arg(&args_list, current); // Arg'ı listeye ekle
        
        current = current->next; // Sonraki token
    }
    
    // 6. ARGUMENT LIST TO ARRAY CONVERSION
    cmd->args = convert_list_to_array(args_list); // List → Array
    
    // 7. TOKEN ITERATOR UPDATE
    *token_node = current;  // Pipeline'daki sonraki command için
    
    // 8. CLEANUP
    ft_lstclear(&args_list, free); // Temporary list'i temizle
    
    // 9. VARIABLE EXPANSION
    expand_command_args(cmd, global); // $VAR expansion işle
    
    return (cmd);
}
```

**İşlem Adımları:**
1. **Command Struct**: Boş komut yapısı oluştur
2. **Syntax Check**: Token dizilimi doğru mu?
3. **Token Loop**: Pipe'a kadar token'ları işle
4. **Categorization**: Redirect vs Argument ayrımı
5. **Collection**: Arguments'ları geçici listeye topla
6. **Conversion**: List'i array'e çevir (execve için)
7. **Expansion**: Variable expansion uygula
8. **Cleanup**: Geçici yapıları temizle

**Kullanım Senaryosu:**
```c
// Tokens: [T_CMD:"ls"] [T_WORD:"-l"] [T_REDIRECT_OUT:">"] [T_WORD:"output.txt"]
// Result Command:
// {
//   args: ["ls", "-l", NULL],
//   redirections: [{type: T_REDIRECT_OUT, filename: "output.txt"}],
//   next: NULL
// }
```

---

#### 🔄 Redirection Parsing

```c
void parse_redirection(t_command *cmd, t_list **token_node, t_global *global)
{
    t_redirect  *redirect;  // Oluşturulacak redirection
    t_token_new *token;     // Redirection token
    t_token_new *file_token; // Filename token
    
    (void)global;          // Şimdilik kullanılmıyor
    
    // 1. REDIRECTION TOKEN EXTRACTION
    token = (t_token_new *)(*token_node)->content;
    
    // 2. REDIRECTION STRUCT CREATION
    redirect = halloc(sizeof(t_redirect));
    if (!redirect)
        return;
    
    // 3. REDIRECTION PROPERTIES
    redirect->type = token->type;    // <, >, >>, <<
    redirect->fd = -1;              // Default FD
    redirect->next = NULL;          // List linkage
    
    // 4. MOVE TO FILENAME TOKEN
    *token_node = (*token_node)->next;
    
    if (*token_node)
    {
        file_token = (t_token_new *)(*token_node)->content;
        
        // 5. WHITESPACE SKIP
        if (file_token->type == T_WHITESPACE)
        {
            *token_node = (*token_node)->next;
            file_token = (t_token_new *)(*token_node)->content;
        }
        
        // 6. FILENAME EXTRACTION
        redirect->filename = ft_strdup(file_token->value);
    }
    
    // 7. ADD TO COMMAND
    add_redirect_to_command(cmd, redirect);
}
```

**Redirection Types:**
- `T_REDIRECT_IN` (`<`): Input redirection
- `T_REDIRECT_OUT` (`>`): Output redirection
- `T_APPEND` (`>>`): Append redirection
- `T_HEREDOC` (`<<`): Here document

**Kullanım Senaryosu:**
```c
// Tokens: [T_REDIRECT_OUT:">"] [T_WHITESPACE:" "] [T_WORD:"file.txt"]
// Result Redirection:
// {
//   type: T_REDIRECT_OUT,
//   filename: "file.txt",
//   fd: -1
// }
```

---

#### 📝 Argüman Toplama

```c
void collect_command_arg(t_list **args_list, t_list *token_node)
{
    t_token_new *token;
    char        *arg_copy;
    char        *temp;
    
    token = (t_token_new *)token_node->content;
    
    // 1. SINGLE QUOTE HANDLING
    if (token->type == T_SINGLE_QUOTE)
    {
        // Preserve quotes for expansion phase
        temp = ft_strjoin("\'", token->value);
        arg_copy = ft_strjoin(temp, "\'");
        // free(temp); // garbage collector kullanıldığı için gerek yok
    }
    
    // 2. DOUBLE QUOTE HANDLING
    else if (token->type == T_DOUBLE_QUOTE)
    {
        // Preserve quotes for expansion phase
        temp = ft_strjoin("\"", token->value);
        arg_copy = ft_strjoin(temp, "\"");
        // free(temp);
    }
    
    // 3. REGULAR WORD HANDLING
    else
    {
        // Direct copy for regular words
        arg_copy = ft_strdup(token->value);
    }
    
    // 4. ADD TO ARGUMENT LIST
    ft_lstadd_back(args_list, ft_lstnew(arg_copy));
}
```

**Quote Preservation Logic:**
- **Single Quote**: `'hello world'` → Literal string, no expansion
- **Double Quote**: `"hello $USER"` → Expansion allowed
- **Regular Word**: `hello` → Normal processing

**Kullanım Senaryosu:**
```c
// Tokens: [T_CMD:"echo"] [T_SINGLE_QUOTE:"hello"] [T_DOUBLE_QUOTE:"world $USER"]
// Result args_list:
// ["echo"] → ["'hello'"] → ["\"world $USER\""] → NULL
```

---

## 📄 parser_is.c - Token Tanıma Fonksiyonları

### 🎯 Dosyanın Amacı
Parser'ın karar verme sürecinde kullandığı token classification fonksiyonları.

### 📝 Satır Satır Kod Analizi

#### 🚀 Command Start Detection

```c
int is_command_start(t_list *token_node)
{
    t_token_new *token;
    
    if (!token_node)       // NULL check
        return (0);
    
    token = (t_token_new *)token_node->content;
    
    // Command start token types
    return (token->type == T_WORD || token->type == T_CMD || 
            token->type == T_HEREDOC || token->type == T_REDIRECT_OUT || 
            token->type == T_REDIRECT_IN);
}
```

**Command Start Kriterleri:**
- `T_WORD` / `T_CMD`: Normal komutlar
- `T_HEREDOC` / `T_REDIRECT_*`: Redirection ile başlayan komutlar

**Kullanım Senaryosu:**
```c
// [T_CMD:"ls"] → is_command_start() = 1 (TRUE)
// [T_PIPE:"|"] → is_command_start() = 0 (FALSE) 
// [T_REDIRECT_OUT:">"] → is_command_start() = 1 (TRUE)
```

---

#### 🔀 Pipe Token Detection

```c
int is_pipe_token(t_list *token_node)
{
    t_token_new *token;
    t_global    *global;
    
    global = get_global();
    
    if (!token_node)       // NULL check
        return (0);
    
    token = (t_token_new *)token_node->content;
    
    // Echo flag reset on pipe
    if (token->type == T_PIPE)
        global->echo_flag = 0; // Pipe sonrası echo mode sıfırla
    
    return (token->type == T_PIPE);
}
```

**Echo Flag Logic:**
Pipeline'da echo komutu sadece kendi segment'inde geçerli. Pipe sonrası reset edilir.

---

#### ↔️ Redirection Token Detection

```c
int is_redirect_token(t_list *token_node)
{
    t_token_new *token;
    
    if (!token_node)       // NULL check
        return (0);
    
    token = (t_token_new *)token_node->content;
    
    // All redirection types
    return (token->type == T_REDIRECT_IN || token->type == T_REDIRECT_OUT ||
            token->type == T_APPEND || token->type == T_HEREDOC);
}
```

**Redirection Types:**
- `T_REDIRECT_IN`: `<`
- `T_REDIRECT_OUT`: `>`
- `T_APPEND`: `>>`
- `T_HEREDOC`: `<<`

---

#### 📝 Word Token Detection (Echo Special Case)

```c
int is_word_token(t_list *token_node)
{
    t_token_new *token;
    t_global    *global;
    
    global = get_global();
    
    if (!token_node)       // NULL check
        return (0);
    
    token = (t_token_new *)token_node->content;
    
    // ECHO COMMAND DETECTION
    if (!ft_strcmp(token->value, "echo"))
        global->echo_flag = 1; // Echo mode aktif
    
    // ECHO MODE SPECIAL HANDLING
    if (global->echo_flag)
    {
        // Echo mode'da whitespace'ler de argüman sayılır
        return (token->type == T_WORD || token->type == T_SINGLE_QUOTE || 
                token->type == T_DOUBLE_QUOTE || token->type == T_WHITESPACE || 
                token->type == T_CMD);
    }
    
    // NORMAL MODE
    return (token->type == T_WORD || token->type == T_SINGLE_QUOTE || 
            token->type == T_DOUBLE_QUOTE || token->type == T_CMD);
}
```

**Echo Special Handling:**
- **Normal Commands**: Whitespace'ler token'lar arası separator
- **Echo Command**: Whitespace'ler preserve edilir (output'ta görünür)

**Kullanım Senaryosu:**
```bash
# Normal mode
ls -l          # args: ["ls", "-l"]

# Echo mode  
echo  hello    # args: ["echo", " ", " ", "hello"] (spaces preserved)
```

---

## 📄 command_utils.c - Komut Yardımcı Fonksiyonları

### 🎯 Dosyanın Amacı
Command structure manipulation için yardımcı utility fonksiyonları.

### 📝 Satır Satır Kod Analizi

#### 🔄 List to Array Conversion

```c
char **convert_list_to_array(t_list *args_list)
{
    char    **args_array;   // Result array
    int     size;           // Array size
    int     i;              // Iterator
    t_list  *current;       // List iterator
    
    // 1. SIZE CALCULATION
    size = ft_lstsize(args_list);
    
    // 2. ARRAY ALLOCATION (size + 1 for NULL terminator)
    args_array = halloc(sizeof(char *) * (size + 1));
    if (!args_array)
        return (NULL);
    
    // 3. LIST TO ARRAY CONVERSION
    i = 0;
    current = args_list;
    
    while (current)
    {
        // String duplication for each argument
        args_array[i] = ft_strdup((char *)current->content);
        current = current->next;
        i++;
    }
    
    // 4. NULL TERMINATION (execve requirement)
    args_array[i] = NULL;
    
    return (args_array);
}
```

**Execve Requirement:**
`execve()` fonksiyonu NULL-terminated string array bekler:
```c
char *args[] = {"ls", "-l", NULL}; // Correct format
```

**Kullanım Senaryosu:**
```c
// Input list: ["echo"] → ["hello"] → ["world"] → NULL
// Output array: {"echo", "hello", "world", NULL}
```

---

#### 🔗 Command Chain Building

```c
void append_command_to_chain(t_command *head, t_command *new_cmd)
{
    t_command *current;
    
    current = head;         // Chain'in başından başla
    
    // Chain'in sonunu bul
    while (current->next)
        current = current->next;
    
    // Yeni command'ı chain'in sonuna ekle
    current->next = new_cmd;
}
```

**Pipeline Structure:**
```c
// Pipeline: ls | grep .c | wc -l
// Command 1 → Command 2 → Command 3 → NULL
```

**Kullanım Senaryosu:**
```c
t_command *cmd1 = create_command(); // ls
t_command *cmd2 = create_command(); // grep .c

append_command_to_chain(cmd1, cmd2);
// Result: cmd1->next = cmd2
```

---

#### ➕ Redirection Addition

```c
void add_redirect_to_command(t_command *cmd, t_redirect *redirect)
{
    // Add redirection to command's redirection list
    ft_lstadd_back(&cmd->redirections, ft_lstnew(redirect));
}
```

**Multiple Redirections:**
```bash
cat < input.txt > output.txt 2> error.txt
# Command has multiple redirections in list
```

---

## 🏗️ create_command - Komut Yapısı Oluşturucu

### 📝 Satır Satır Kod Analizi

```c
t_command *create_command(void)
{
    t_command *cmd;
    
    // 1. MEMORY ALLOCATION
    cmd = halloc(sizeof(t_command));
    if (!cmd)              // Allocation failed
        return (NULL);
    
    // 2. FIELD INITIALIZATION
    cmd->args = NULL;              // No arguments yet
    cmd->redirections = NULL;      // No redirections yet
    cmd->pipe_fd[0] = -1;         // Input pipe FD
    cmd->pipe_fd[1] = -1;         // Output pipe FD  
    cmd->pid = -1;                // Process ID (after fork)
    cmd->next = NULL;             // Next command in pipeline
    
    return (cmd);
}
```

**Default Values:**
- **args**: NULL (will be populated by parser)
- **redirections**: NULL (will be populated if redirections exist)
- **pipe_fd**: -1 (will be set up during execution)
- **pid**: -1 (will be set after fork)
- **next**: NULL (will be linked for pipelines)

---

## 🔗 Parser Modülü Fonksiyon Çağrı Hiyerarşisi

```
parse_tokens_to_commands()
├── is_command_start()                 [parser_is.c]
├── parse_single_command()             [parser.c]
│   ├── create_command()               [debug_utils.c]
│   ├── check_syntax()                 [parser.c]
│   ├── is_pipe_token()                [parser_is.c]
│   ├── is_redirect_token()            [parser_is.c]
│   ├── is_word_token()                [parser_is.c]
│   ├── parse_redirection()            [parser.c]
│   │   └── add_redirect_to_command()  [command_utils.c]
│   ├── collect_command_arg()          [parser.c]
│   ├── convert_list_to_array()        [command_utils.c]
│   └── expand_command_args()          [expander module]
└── append_command_to_chain()          [command_utils.c]
```

---

## 🧪 Kapsamlı Test Senaryoları

### 1. **Simple Command**
```c
// Input tokens: [T_CMD:"ls"] [T_WORD:"-l"]
// Result:
t_command {
    args: ["ls", "-l", NULL],
    redirections: NULL,
    next: NULL
}
```

### 2. **Pipeline**
```c
// Input: "ls -l | grep .c"
// Tokens: [T_CMD:"ls"] [T_WORD:"-l"] [T_PIPE:"|"] [T_CMD:"grep"] [T_WORD:".c"]
// Result:
Command 1: {
    args: ["ls", "-l", NULL],
    next: Command 2
}
Command 2: {
    args: ["grep", ".c", NULL], 
    next: NULL
}
```

### 3. **Redirections**
```c
// Input: "cat < input.txt > output.txt"
// Result:
t_command {
    args: ["cat", NULL],
    redirections: [
        {type: T_REDIRECT_IN, filename: "input.txt"},
        {type: T_REDIRECT_OUT, filename: "output.txt"}
    ],
    next: NULL
}
```

### 4. **Heredoc**
```c
// Input: "cat << EOF"
// Result:
t_command {
    args: ["cat", NULL],
    redirections: [
        {type: T_HEREDOC, filename: "EOF"}
    ],
    next: NULL
}
```

### 5. **Mixed Complex**
```c
// Input: "echo 'hello world' | grep 'hello' > result.txt"
// Result:
Command 1: {
    args: ["echo", "'hello world'", NULL],
    next: Command 2
}
Command 2: {
    args: ["grep", "'hello'", NULL],
    redirections: [{type: T_REDIRECT_OUT, filename: "result.txt"}],
    next: NULL
}
```

### 6. **Echo Special Case**
```c
// Input: "echo  hello   world"  (multiple spaces)
// Result (echo_flag=1):
t_command {
    args: ["echo", " ", " ", "hello", " ", " ", " ", "world", NULL],
    next: NULL
}
```

---

## ⚠️ Error Handling & Syntax Validation

### 1. **Missing Filename**
```bash
cat >               # Missing filename after redirection
# Error: "syntax error near unexpected token `newline'"
```

### 2. **Invalid Token After Redirection**
```bash
cat > |             # Pipe after redirection
# Error: "syntax error near unexpected token `|'"
```

### 3. **Missing Heredoc Delimiter**
```bash
cat <<              # Missing delimiter
# Error: "syntax error near unexpected token `newline'"
```

### 4. **Memory Allocation Failure**
```c
if (!cmd)           // create_command() failed
    return (NULL);

if (!args_array)    // convert_list_to_array() failed
    return (NULL);
```

---

## 🎯 Parser Modülü Özeti

**Parser modülü** lexer'dan gelen **token stream**'i **executable command structures**'a dönüştürür:

### 🔧 Ana Sorumluluklar:
1. **Syntax Validation**: Token diziliminin doğruluğunu kontrol eder
2. **Command Recognition**: Komut boundaries'ini tanır
3. **Pipeline Building**: Command chain'leri oluşturur
4. **Redirection Parsing**: I/O redirection'ları ayrıştırır
5. **Argument Collection**: Command arguments'ları toplar
6. **Quote Preservation**: Quote bilgilerini expansion için saklar

### 🎨 Design Patterns:
- **Recursive Descent Parser**: Top-down parsing approach
- **Token Classification**: Type-based processing
- **Chain of Responsibility**: Command pipeline building
- **Factory Pattern**: Command structure creation

### 🚀 Performance Features:
- **Single Pass**: Token list'i tek geçişte işler
- **Memory Efficient**: Minimal temporary allocations
- **Error Recovery**: Graceful error handling
- **Modular Design**: Clear separation of concerns

Bu modül **compiler pipeline**'ının kritik bir komponenti olarak, shell'in **command understanding** capability'sini sağlar ve executor modülü için **structured input** hazırlar.

**Key Outputs:**
- ✅ Validated command structures
- ✅ Pipeline relationships  
- ✅ Redirection specifications
- ✅ Preserved quote information
- ✅ Error diagnostics
