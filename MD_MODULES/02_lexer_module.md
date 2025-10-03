# 🔤 LEXER MODULE - Leksikal Analiz Modülü

## 📋 İçindekiler
1. [lexer.c - Temel Lexer Fonksiyonları](#lexerc---temel-lexer-fonksiyonları)
2. [tokenizer.c - Ana Tokenization Motoru](#tokenizerc---ana-tokenization-motoru)
3. [token_handlers.c - Token İşleyici Fonksiyonlar](#token_handlersc---token-i̇şleyici-fonksiyonlar)
4. [token_handlers_advanced.c - Gelişmiş Token İşleyiciler](#token_handlers_advancedc---gelişmiş-token-i̇şleyiciler)

---

## 🎯 Lexer Modülünün Amacı

**Lexer (Leksikal Analiz)** modülü, ham string girdiyi anlamlı token'lara (sembollere) ayıran modüldür. Bu, compiler'ların ve interpreter'ların ilk aşamasıdır.

### 🔄 Lexer İşlem Akışı
```
Raw Input String
        ↓
    Lexer Init
        ↓
Character-by-Character Scanning
        ↓
Token Recognition & Creation
        ↓
Token List Output
```

### 🎪 Token Türleri
```c
T_WORD          // Normal kelimeler: "ls", "file.txt"
T_CMD           // İlk komut: komut başlangıcı
T_PIPE          // Pipe: "|"
T_REDIRECT_IN   // Input redirect: "<"
T_REDIRECT_OUT  // Output redirect: ">"
T_APPEND        // Append redirect: ">>"
T_HEREDOC       // Here document: "<<"
T_SINGLE_QUOTE  // Tek tırnak: 'text'
T_DOUBLE_QUOTE  // Çift tırnak: "text"
T_ENV_VAR       // Environment variable: $VAR
T_WHITESPACE    // Boşluk karakteri
T_EOF           // End of input
```

---

## 📄 lexer.c - Temel Lexer Fonksiyonları

### 🎯 Dosyanın Amacı
Lexer'ın temel yapı taşları olan initialization, navigation ve token creation fonksiyonlarını içerir.

### 📝 Satır Satır Kod Analizi

#### 🏗️ Lexer Başlatma Fonksiyonu

```c
t_lexer_new *init_lexer_advanced(char *input, t_global *global)
{
    t_lexer_new *lexer;
    
    // 1. MEMORY ALLOCATION
    lexer = (t_lexer_new *)halloc(sizeof(t_lexer_new));
    if (!lexer)                    // Allocation başarısız kontrolü
        return (NULL);
    
    // 2. BASIC INITIALIZATION
    lexer->input = input;          // Input string referansı
    lexer->pos = 0;                // Başlangıç pozisyonu
    lexer->len = ft_strlen(input); // Input uzunluğu
    lexer->global = global;        // Global state referansı
    
    // 3. CHARACTER & FLAG INITIALIZATION
    lexer->current_char = '\0';    // Default current char
    lexer->t_cmd_flag = 0;         // Command flag - pipe sonrası kontrol
    
    // 4. FIRST CHARACTER SETUP
    if (lexer->len > 0)
        lexer->current_char = input[0];  // İlk karakteri ayarla
    else
        lexer->current_char = '\0';      // Boş input durumu
    
    return (lexer);
}
```

**Açıklama:**
- **Line 5**: `halloc()` garbage collector kullanımı - memory leak önleme
- **Line 9-12**: Temel lexer state'i kurulumu
- **Line 15-16**: Flag initialization - özellikle pipe sonrası command detection
- **Line 19-22**: İlk karakter ayarlama - loop için hazırlık

**Kullanım Senaryosu:**
```c
char *input = "ls -l | grep .c";
t_lexer_new *lexer = init_lexer_advanced(input, global);

// lexer->input = "ls -l | grep .c"
// lexer->pos = 0
// lexer->current_char = 'l'
// lexer->len = 15
```

---

#### ➡️ Lexer İlerleme Fonksiyonu

```c
void advance_lexer(t_lexer_new *lexer)
{
    lexer->pos++;                       // Pozisyonu bir ileri al
    
    if (lexer->pos >= lexer->len)       // Input sonuna geldi mi?
        lexer->current_char = '\0';     // EOF marker
    else
        lexer->current_char = lexer->input[lexer->pos]; // Sonraki karakteri al
}
```

**Açıklama:**
- **Line 3**: Position counter'ı artır
- **Line 5-6**: Boundary check - input sonu kontrolü
- **Line 8**: Yeni current character güncelleme

**Kullanım Senaryosu:**
```c
// Input: "ls -l"
// pos=0, current_char='l'
advance_lexer(lexer);
// pos=1, current_char='s'
advance_lexer(lexer);
// pos=2, current_char=' '
```

---

#### 🏷️ Token Oluşturma Fonksiyonu

```c
t_token_new *create_token_advanced(t_token_types type, char *value)
{
    t_token_new *token;
    
    // 1. TOKEN MEMORY ALLOCATION
    token = (t_token_new *)halloc(sizeof(t_token_new));
    if (!token)
        return (NULL);
    
    // 2. BASIC TOKEN PROPERTIES
    token->type = type;                    // Token türünü ayarla
    token->value = ft_strdup(value);       // Value'yu kopyala
    
    // 3. VALUE VALIDATION
    if (!token->value)                     // strdup başarısız kontrolü
    {
        // free(token); // garbage collector kullanıldığı için gerek yok
        return (NULL);
    }
    
    // 4. ADDITIONAL PROPERTIES
    token->len = ft_strlen(value);         // Value uzunluğu
    token->quote_type = 0;                 // Tırnak türü (default: yok)
    token->expanded = 0;                   // Expansion flag (default: false)
    
    return (token);
}
```

**Açıklama:**
- **Line 5-7**: Memory allocation error handling
- **Line 10-11**: Temel token properties
- **Line 14-18**: Value validation ve error handling
- **Line 21-23**: Meta-data initialization

**Kullanım Senaryosu:**
```c
// Pipe token oluşturma
t_token_new *pipe_token = create_token_advanced(T_PIPE, "|");

// Word token oluşturma
t_token_new *word_token = create_token_advanced(T_WORD, "file.txt");

// Quote token oluşturma
t_token_new *quote_token = create_token_advanced(T_SINGLE_QUOTE, "hello world");
```

---

#### 🔲 Whitespace Atlama Fonksiyonu

```c
void skip_whitespace_advanced(t_lexer_new *lexer)
{
    // Whitespace karakterleri: space, tab, newline
    while (lexer->current_char == ' ' || lexer->current_char == '\t'
        || lexer->current_char == '\n')
        advance_lexer(lexer);              // Whitespace'leri atla
}
```

**Açıklama:**
- **Line 3-4**: Whitespace character kontrolü (space, tab, newline)
- **Line 5**: Continue advancing until non-whitespace

**Kullanım Senaryosu:**
```c
// Input: "ls    -l" (multiple spaces)
// pos=2, current_char=' '
skip_whitespace_advanced(lexer);
// pos=6, current_char='-' (all spaces skipped)
```

---

## 📄 tokenizer.c - Ana Tokenization Motoru

### 🎯 Dosyanın Amacı
Lexer'ın ana motor fonksiyonu olan `tokenize_advanced()` ve yardımcı fonksiyonları içerir.

### 📝 Satır Satır Kod Analizi

#### 🎭 Token Türü Belirleme Fonksiyonu

```c
static t_token_new *create_token_by_type(t_lexer_new *lexer)
{
    // PIPE TOKEN CHECK
    if (lexer->current_char == '|')
        return (handle_pipe_advanced(lexer));
    
    // WHITESPACE TOKEN CHECK  
    else if ((lexer->current_char == ' ' || lexer->current_char == '\t'))
        return (handle_whitespaces_advanced(lexer));
    
    // REDIRECTION TOKEN CHECK
    else if (lexer->current_char == '<' || lexer->current_char == '>')
        return (handle_redirect_advanced(lexer));
    
    // QUOTE TOKEN CHECK
    else if (lexer->current_char == '\'' || lexer->current_char == '"')
        return (handle_quotes_advanced(lexer));
    
    // DEFAULT: WORD TOKEN
    else
        return (handle_word_advanced(lexer, &lexer->first_word_check));
}
```

**Açıklama:**
- **Line 4**: Pipe character detection
- **Line 7**: Whitespace detection (space, tab)
- **Line 10**: Redirection operators (<, >, <<, >>)
- **Line 13**: Quote detection (single, double)
- **Line 16**: Default case - word/command tokens

**Kullanım Senaryosu:**
```c
// Input: "ls | grep .c"
// current_char='l' → handle_word_advanced()
// current_char='|' → handle_pipe_advanced()  
// current_char='g' → handle_word_advanced()
```

---

#### ⚙️ Token Sonrası İşleme

```c
static void handle_post_token_processing(t_lexer_new *lexer, t_token_new *token)
{
    // CMD veya PIPE token'ları için özel işlem
    if (token->type == T_CMD || token->type == T_PIPE)
    {
        skip_whitespace_advanced(lexer);    // Sonraki whitespace'leri atla
        
        // PIPE sonrası command flag ayarlama
        if (token->type == T_PIPE)
        {
            // Sonraki character printable ise command flag set et
            if (lexer->current_char >= 32 && lexer->current_char <= 127)
                lexer->t_cmd_flag = 1;      // Next word will be T_CMD
        }
    }
}
```

**Açıklama:**
- **Line 4**: Command veya pipe token sonrası işlem
- **Line 6**: Whitespace'leri temizle
- **Line 9-13**: Pipe sonrası sonraki word'ü T_CMD olarak işaretle

**Kullanım Senaryosu:**
```c
// Input: "ls | grep"
// "ls" → T_CMD
// "|" → T_PIPE → t_cmd_flag = 1
// "grep" → T_CMD (t_cmd_flag sayesinde)
```

---

#### 🔄 Sonraki Token Alma

```c
static t_token_new *get_next_token(t_lexer_new *lexer)
{
    t_token_new *token;
    
    token = NULL;
    lexer->first_word_check = 0;           // First word flag reset
    
    // INPUT END CHECK
    if (lexer->current_char == '\0')
        return (NULL);                     // No more tokens
    
    // TOKEN CREATION BY TYPE
    token = create_token_by_type(lexer);   // Appropriate handler'ı çağır
    if (!token)                            // Token creation failed
    {
        return (NULL);
    }
    
    // POST-PROCESSING
    handle_post_token_processing(lexer, token); // Token sonrası işlemler
    return (token);
}
```

**Açıklama:**
- **Line 6**: First word flag reset
- **Line 9-10**: End of input kontrolü
- **Line 13**: Type-specific token creation
- **Line 14-17**: Error handling
- **Line 20**: Post-processing (whitespace skip, flags)

---

#### ✅ Token İşleme ve Doğrulama

```c
static int process_token(t_list **tokens, t_token_new *token)
{
    // TOKEN VALIDATION
    if (!token)                            // Token creation başarısız
    {
        printf("Token Error\n");
        return (0);                        // Process failed
    }
    
    // VALUE VALIDATION
    else if (token->value)                 // Valid value var mı?
    {
        ft_lstadd_back(tokens, ft_lstnew(token)); // Token'ı listeye ekle
        return (1);                        // Process successful
    }
    
    return (0);                            // Process failed
}
```

**Açıklama:**
- **Line 4-8**: Token null check ve error reporting
- **Line 11-15**: Value validation ve list'e ekleme
- **Line 17**: Default failure case

---

#### 🚀 Ana Tokenization Fonksiyonu

```c
t_list *tokenize_advanced(char *input, t_global *global)
{
    t_lexer_new *lexer;
    t_list      *tokens;
    t_token_new *token;
    
    // 1. LEXER INITIALIZATION
    lexer = init_lexer_advanced(input, global);
    if (!lexer)                            // Initialization başarısız
        return (NULL);
    
    // 2. TOKEN LIST INITIALIZATION
    tokens = NULL;                         // Token listesi başlat
    
    // 3. MAIN TOKENIZATION LOOP
    while (lexer->current_char)            // Input sonuna kadar
    {
        token = get_next_token(lexer);     // Sonraki token'ı al
        global->echo_flag = 0;             // Echo flag reset
        
        if (!process_token(&tokens, token)) // Token'ı işle ve listeye ekle
            return (NULL);                 // İşlem başarısız
    }
    
    return (tokens);                       // Token listesini döndür
}
```

**Açıklama:**
- **Line 7-9**: Lexer initialization ve error check
- **Line 12**: Token list başlatma
- **Line 15-21**: Ana tokenization loop
- **Line 16**: Her iterasyonda yeni token al
- **Line 19**: Token processing ve list'e ekleme
- **Line 23**: Final token list return

**Kullanım Senaryosu:**
```c
char *input = "echo \"hello world\" | wc -l";
t_list *tokens = tokenize_advanced(input, global);

// Oluşan token listesi:
// [T_CMD:"echo"] → [T_DOUBLE_QUOTE:"hello world"] → [T_PIPE:"|"] → 
// [T_CMD:"wc"] → [T_WORD:"-l"] → NULL
```

---

## 📄 token_handlers.c - Token İşleyici Fonksiyonlar

### 🎯 Dosyanın Amacı
Farklı token türleri için özel işleme fonksiyonları. Her token türünün kendine özgü parsing logic'i vardır.

### 📝 Satır Satır Kod Analizi

#### 🔀 Pipe Token İşleyici

```c
t_token_new *handle_pipe_advanced(t_lexer_new *lexer)
{
    advance_lexer(lexer);                  // '|' karakterini atla
    return (create_token_advanced(T_PIPE, "|")); // Pipe token oluştur
}
```

**Açıklama:**
- **Line 3**: Current character '|' olduğu için bir ileri geç
- **Line 4**: PIPE token'ı oluştur ve döndür

**Kullanım Senaryosu:**
```c
// Input: "ls|grep"
// current_char='|' → handle_pipe_advanced()
// Result: T_PIPE token, current_char='g'
```

---

#### 📝 Word Token İşleyici

```c
t_token_new *handle_word_advanced(t_lexer_new *lexer, int *first_word_check)
{
    int             start;
    t_token_types   types;
    
    start = lexer->pos;                    // Word başlangıç pozisyonu
    types = T_WORD;                        // Default type
    
    // INPUT VALIDATION
    if (!lexer || !lexer->input || lexer->pos >= lexer->len)
        return (NULL);
    
    // TOKEN TYPE DETERMINATION
    // İlk word veya pipe sonrası word → T_CMD
    if (lexer->pos == 0 || lexer->t_cmd_flag == 1)
    {
        types = T_CMD;                     // Command type
        *first_word_check = 0;
        lexer->t_cmd_flag = 0;             // Flag'i reset et
    }
    else
    {
        types = T_WORD;                    // Regular word type
        *first_word_check = 1;
    }
    
    // WORD CONTENT PROCESSING
    return (process_word_content(lexer, start, types));
}
```

**Açıklama:**
- **Line 6-7**: Starting position ve default type
- **Line 10-11**: Input validation
- **Line 15-20**: Command vs Word type determination
- **Line 15**: İlk word veya pipe sonrası → T_CMD
- **Line 22-25**: Regular word case
- **Line 28**: Actual content processing

**Kullanım Senaryosu:**
```c
// Input: "ls -l | grep .c"
// "ls" → pos=0 → T_CMD
// "-l" → pos=3 → T_WORD  
// "grep" → t_cmd_flag=1 → T_CMD
// ".c" → regular position → T_WORD
```

---

#### 🔍 Word Content İşleme

```c
static t_token_new *process_word_content(t_lexer_new *lexer, int start, t_token_types types)
{
    int     len;
    char    *word;
    
    // SPECIAL CASE: $"..." pattern
    if (lexer->current_char == '$' && lexer->input[lexer->pos + 1] == '"')
    {
        advance_lexer(lexer);              // '$' karakterini atla
        advance_lexer(lexer);              // '"' karakterini atla
        
        // Read until closing quote
        while (lexer->current_char != '\0' && lexer->current_char != '"')
            advance_lexer(lexer);
        
        if (lexer->current_char == '"')
            advance_lexer(lexer);          // Closing quote'u atla
        
        len = lexer->pos - start;
        word = ft_substr(lexer->input, start + 2, len - 3); // $" ve " dışındaki kısım
        if (!word)
            return (NULL);
        return (create_token_advanced(types, word));
    }
    
    // REGULAR WORD PROCESSING
    return (extract_regular_word(lexer, start, types));
}
```

**Açıklama:**
- **Line 6**: Special pattern detection: $"text"
- **Line 8-9**: Skip $" characters
- **Line 12-13**: Read until closing quote
- **Line 15-16**: Skip closing quote
- **Line 18-22**: Extract content without quotes
- **Line 25**: Regular word processing için fallback

**Kullanım Senaryosu:**
```c
// Input: echo $"hello world"
// $"hello world" → extract "hello world" content
// Regular word: echo → normal extraction
```

---

#### ⚡ Regular Word Extraction

```c
static t_token_new *extract_regular_word(t_lexer_new *lexer, int start, t_token_types types)
{
    int     len;
    char    *word;
    
    // WORD BOUNDARY DETECTION
    while (lexer->current_char != '\0' && lexer->current_char != ' '
        && lexer->current_char != '\t' && lexer->current_char != '\n'
        && lexer->current_char != '|' && lexer->current_char != '<'
        && lexer->current_char != '>' && lexer->current_char != '\''
        && lexer->current_char != '"')
        advance_lexer(lexer);
    
    // WORD LENGTH CALCULATION
    len = lexer->pos - start;
    if (len > 0)
    {
        word = ft_substr(lexer->input, start, len); // Word'ü extract et
    }
    else
        return (NULL);                     // Empty word
    
    if (!word)
        return (NULL);                     // Extraction failed
    
    return (create_token_advanced(types, word)); // Token oluştur
}
```

**Açıklama:**
- **Line 6-11**: Word boundary characters - word'ün nerede bittiğini belirle
- **Line 14-18**: Length calculation ve substring extraction
- **Line 19-20**: Empty word kontrolü
- **Line 22-23**: Extraction failure kontrolü
- **Line 25**: Final token creation

**Word Boundary Characters:**
- Whitespace: ` `, `\t`, `\n`
- Operators: `|`, `<`, `>`
- Quotes: `'`, `"`
- EOF: `\0`

**Kullanım Senaryosu:**
```c
// Input: "filename.txt | grep"
// "filename.txt" → boundaries: space and |
// "grep" → boundary: EOF
```

---

## 📄 token_handlers_advanced.c - Gelişmiş Token İşleyiciler

### 🎯 Dosyanın Amacı
Redirection, quotes ve whitespace için özelleşmiş token handler'ları.

### 📝 Satır Satır Kod Analizi

#### ⬅️ Input Redirection Handler

```c
static t_token_new *handle_input_redirect(t_lexer_new *lexer)
{
    advance_lexer(lexer);                  // '<' karakterini atla
    
    // HEREDOC CHECK (<<)
    if (lexer->current_char == '<')
    {
        advance_lexer(lexer);              // İkinci '<' karakterini atla
        skip_whitespace_advanced(lexer);   // Whitespace'leri temizle
        return (create_token_advanced(T_HEREDOC, "<<")); // Heredoc token
    }
    
    // REGULAR INPUT REDIRECT (<)
    return (create_token_advanced(T_REDIRECT_IN, "<"));
}
```

**Açıklama:**
- **Line 3**: İlk '<' karakterini atla
- **Line 6-11**: Heredoc pattern (<<) kontrolü
- **Line 13**: Regular input redirect (<)

**Kullanım Senaryosu:**
```c
// Input: "cat < file.txt"     → T_REDIRECT_IN
// Input: "cat << EOF"         → T_HEREDOC
```

---

#### ➡️ Output Redirection Handler

```c
static t_token_new *handle_output_redirect(t_lexer_new *lexer)
{
    advance_lexer(lexer);                  // '>' karakterini atla
    
    // APPEND CHECK (>>)
    if (lexer->current_char == '>')
    {
        advance_lexer(lexer);              // İkinci '>' karakterini atla
        skip_whitespace_advanced(lexer);   // Whitespace'leri temizle
        return (create_token_advanced(T_APPEND, ">>")); // Append token
    }
    
    // REGULAR OUTPUT REDIRECT (>)
    return (create_token_advanced(T_REDIRECT_OUT, ">"));
}
```

**Açıklama:**
- **Line 3**: İlk '>' karakterini atla
- **Line 6-11**: Append pattern (>>) kontrolü
- **Line 13**: Regular output redirect (>)

**Kullanım Senaryosu:**
```c
// Input: "echo hello > file.txt"   → T_REDIRECT_OUT
// Input: "echo hello >> file.txt"  → T_APPEND
```

---

#### 🔀 Redirection Dispatcher

```c
t_token_new *handle_redirect_advanced(t_lexer_new *lexer)
{
    if (lexer->current_char == '<')
        return (handle_input_redirect(lexer));    // Input redirection
    else if (lexer->current_char == '>')
        return (handle_output_redirect(lexer));   // Output redirection
    return (NULL);
}
```

**Açıklama:**
- **Line 3**: '<' için input handler
- **Line 5**: '>' için output handler
- **Line 7**: Invalid character için NULL

---

#### 💬 Quote Handler

```c
t_token_new *handle_quotes_advanced(t_lexer_new *lexer)
{
    char    quote_char;
    int     start;
    int     len;
    char    *quoted_str;
    
    quote_char = lexer->current_char;      // Quote type (' veya ")
    advance_lexer(lexer);                  // Opening quote'u atla
    start = lexer->pos;                    // Content başlangıcı
    
    // FIND CLOSING QUOTE
    while (lexer->current_char != '\0' && lexer->current_char != quote_char)
        advance_lexer(lexer);
    
    // QUOTE CLOSED SUCCESSFULLY
    if (lexer->current_char == quote_char)
    {
        len = lexer->pos - start;          // Content length
        
        quoted_str = ft_substr(lexer->input, start, len); // Content extract
        advance_lexer(lexer);              // Closing quote'u atla
        
        // EMPTY QUOTE HANDLING
        if (len == 0)
            return(create_token_advanced(T_SINGLE_QUOTE, ft_strdup(""))); // Empty quote
        
        // TOKEN TYPE BY QUOTE TYPE
        if (quote_char == '\'')
            return (create_token_advanced(T_SINGLE_QUOTE, quoted_str)); // Single quote
        else
            return (create_token_advanced(T_DOUBLE_QUOTE, quoted_str)); // Double quote
    }
    
    return (NULL);                         // Unclosed quote error
}
```

**Açıklama:**
- **Line 7-9**: Quote type detection ve setup
- **Line 12-13**: Closing quote arama
- **Line 16-28**: Successful quote processing
- **Line 21**: Content extraction
- **Line 25-26**: Empty quote handling
- **Line 29-32**: Quote type based token creation
- **Line 35**: Unclosed quote error

**Kullanım Senaryosu:**
```c
// Input: echo 'hello world'    → T_SINGLE_QUOTE:"hello world"
// Input: echo "hello $USER"    → T_DOUBLE_QUOTE:"hello $USER"
// Input: echo ''               → T_SINGLE_QUOTE:""
// Input: echo 'unclosed        → NULL (error)
```

---

#### ⚪ Whitespace Handler

```c
t_token_new *handle_whitespaces_advanced(t_lexer_new *lexer)
{
    int     start;
    int     len;
    char    *whitespaces;
    
    start = lexer->pos;                    // Whitespace başlangıcı
    
    // CONSUME ALL CONSECUTIVE WHITESPACES
    while (lexer->current_char != '\0' && 
           (lexer->current_char == ' ' || lexer->current_char == '\t'))
    {
        advance_lexer(lexer);
    }
    
    len = lexer->pos - start;              // Whitespace uzunluğu
    whitespaces = " ";                     // Simplified whitespace token
    return(create_token_advanced(T_WHITESPACE, whitespaces));
}
```

**Açıklama:**
- **Line 6**: Whitespace başlangıç pozisyonu
- **Line 9-13**: Consecutive whitespace consumption
- **Line 15-16**: Simplified whitespace token (tek space olarak)

**Kullanım Senaryosu:**
```c
// Input: "ls    -l"  (multiple spaces)
// Result: [T_CMD:"ls"] → [T_WHITESPACE:" "] → [T_WORD:"-l"]
```

---

## 🔗 Modül İçi Fonksiyon Çağrı Hiyerarşisi

```
tokenize_advanced()
├── init_lexer_advanced()              [lexer.c]
├── get_next_token()                   [tokenizer.c]
│   ├── create_token_by_type()         [tokenizer.c]
│   │   ├── handle_pipe_advanced()     [token_handlers.c]
│   │   ├── handle_whitespaces_advanced() [token_handlers_advanced.c]
│   │   ├── handle_redirect_advanced() [token_handlers_advanced.c]
│   │   │   ├── handle_input_redirect()  [token_handlers_advanced.c]
│   │   │   └── handle_output_redirect() [token_handlers_advanced.c]
│   │   ├── handle_quotes_advanced()   [token_handlers_advanced.c]
│   │   └── handle_word_advanced()     [token_handlers.c]
│   │       └── process_word_content() [token_handlers.c]
│   │           └── extract_regular_word() [token_handlers.c]
│   └── handle_post_token_processing() [tokenizer.c]
├── process_token()                    [tokenizer.c]
└── ft_lstadd_back()                   [libft]
```

---

## 🧪 Kapsamlı Test Senaryoları

### 1. **Basit Komut**
```c
Input: "ls -l"
Tokens: [T_CMD:"ls"] → [T_WHITESPACE:" "] → [T_WORD:"-l"]
```

### 2. **Pipeline**
```c
Input: "cat file.txt | grep pattern | wc -l"
Tokens: [T_CMD:"cat"] → [T_WHITESPACE:" "] → [T_WORD:"file.txt"] → 
        [T_WHITESPACE:" "] → [T_PIPE:"|"] → [T_WHITESPACE:" "] → 
        [T_CMD:"grep"] → [T_WHITESPACE:" "] → [T_WORD:"pattern"] → 
        [T_WHITESPACE:" "] → [T_PIPE:"|"] → [T_WHITESPACE:" "] →
        [T_CMD:"wc"] → [T_WHITESPACE:" "] → [T_WORD:"-l"]
```

### 3. **Redirections**
```c
Input: "cat < input.txt > output.txt"
Tokens: [T_CMD:"cat"] → [T_WHITESPACE:" "] → [T_REDIRECT_IN:"<"] → 
        [T_WHITESPACE:" "] → [T_WORD:"input.txt"] → [T_WHITESPACE:" "] → 
        [T_REDIRECT_OUT:">"] → [T_WHITESPACE:" "] → [T_WORD:"output.txt"]
```

### 4. **Heredoc**
```c
Input: "cat << EOF"
Tokens: [T_CMD:"cat"] → [T_WHITESPACE:" "] → [T_HEREDOC:"<<"] → 
        [T_WHITESPACE:" "] → [T_WORD:"EOF"]
```

### 5. **Quotes**
```c
Input: "echo 'hello world' \"user: $USER\""
Tokens: [T_CMD:"echo"] → [T_WHITESPACE:" "] → [T_SINGLE_QUOTE:"hello world"] → 
        [T_WHITESPACE:" "] → [T_DOUBLE_QUOTE:"user: $USER"]
```

### 6. **Complex Mixed**
```c
Input: "echo \"file: $HOME/test.txt\" | grep 'pattern' >> log.txt"
Tokens: [T_CMD:"echo"] → [T_WHITESPACE:" "] → [T_DOUBLE_QUOTE:"file: $HOME/test.txt"] → 
        [T_WHITESPACE:" "] → [T_PIPE:"|"] → [T_WHITESPACE:" "] → 
        [T_CMD:"grep"] → [T_WHITESPACE:" "] → [T_SINGLE_QUOTE:"pattern"] → 
        [T_WHITESPACE:" "] → [T_APPEND:">>"] → [T_WHITESPACE:" "] → [T_WORD:"log.txt"]
```

---

## ⚠️ Error Handling

### 1. **Unclosed Quotes**
```c
Input: "echo 'unclosed quote
Result: NULL (tokenization fails)
Error: "Error: Unclosed quote detected"
```

### 2. **Memory Allocation Failure**
```c
// halloc() fails
if (!lexer)
    return (NULL);

if (!token->value)
    return (NULL);
```

### 3. **Invalid Input**
```c
Input: NULL or ""
Result: NULL or empty token list
```

---

## 🎯 Lexer Modülü Özeti

**Lexer modülü** minishell'in **ilk aşaması** olarak:

1. **Character-level Processing**: Her karakteri tek tek analiz eder
2. **Token Recognition**: Anlamlı symbol gruplarını tanır  
3. **Syntax Preparation**: Parser için uygun format hazırlar
4. **Quote Handling**: Tek ve çift tırnak içeriklerini ayırır
5. **Operator Detection**: Pipe, redirection operatörlerini tanır
6. **Command/Word Distinction**: İlk word'leri command olarak işaretler

Bu modül **compiler design**'in **lexical analysis** fazının tam implementasyonudur ve sonraki **parser** modülü için temel hazırlar.

**Key Features:**
- ✅ Quote balancing
- ✅ Operator recognition  
- ✅ Command/argument distinction
- ✅ Whitespace handling
- ✅ Error detection
- ✅ Memory management
