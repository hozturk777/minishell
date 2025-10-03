# 🔤 LEXER Modülü Detaylı Analizi

## 📋 İçindekiler
1. [Modül Genel Bakış](#modül-genel-bakış)
2. [lexer.c - Temel Lexer Fonksiyonları](#lexerc---temel-lexer-fonksiyonları)
3. [tokenizer.c - Ana Tokenization Mantığı](#tokenizerc---ana-tokenization-mantığı)
4. [token_handlers.c - Token İşleyici Fonksiyonları](#token_handlersc---token-i̇şleyici-fonksiyonları)
5. [token_handlers_advanced.c - Gelişmiş Token İşleyiciler](#token_handlers_advancedc---gelişmiş-token-i̇şleyiciler)
6. [Akış Şeması](#akış-şeması)
7. [Örnek Senaryolar](#örnek-senaryolar)

---

## 🎯 Modül Genel Bakış

Lexer modülü, minishell'in **ilk aşaması**dır. Bu modül:
- Ham string girdiyi **token'lara** ayırır
- Her karakteri analiz eder ve anlamını belirler
- Token türlerini ve değerlerini oluşturur
- Parser için hazır token listesi üretir

### 🎯 Lexical Analysis Nedir?
```
"ls -l | grep txt > output.txt"
        ↓
[T_WORD:"ls", T_WORD:"-l", T_PIPE:"|", T_WORD:"grep", T_WORD:"txt", 
 T_REDIRECT_OUT:">", T_WORD:"output.txt"]
```

### 📁 Dosyalar
- `lexer.c` - Temel lexer yapıları ve fonksiyonları
- `tokenizer.c` - Ana tokenization döngüsü
- `token_handlers.c` - Spesifik token işleyicileri
- `token_handlers_advanced.c` - Gelişmiş token işleyicileri

---

## 📄 lexer.c - Temel Lexer Fonksiyonları

### 🔧 Fonksiyon Hiyerarşisi
```
init_lexer_advanced()
├── halloc() - Memory allocation
├── ft_strlen() - Input length calculation
└── Character initialization

advance_lexer()
├── Position increment
└── Current character update

create_token_advanced()
├── halloc() - Token memory allocation
├── ft_strdup() - Value copy
└── Token properties setup

skip_whitespace_advanced()
└── advance_lexer() - Multiple calls
```

### 📝 Satır Satır Kod Analizi

#### 1. init_lexer_advanced Fonksiyonu (15-33. satırlar)

```c
t_lexer_new	*init_lexer_advanced(char *input, t_global *global)
{
	t_lexer_new	*lexer;

	// ADIM 1: Lexer için memory allocation
	lexer = (t_lexer_new *)halloc(sizeof(t_lexer_new));
	if (!lexer)  // Allocation başarısızsa
		return (NULL);
	
	// ADIM 2: Temel değerleri ayarla
	lexer->input = input;          // İşlenecek string
	lexer->pos = 0;                // Başlangıç pozisyonu
	lexer->len = ft_strlen(input); // Input uzunluğu
	lexer->global = global;        // Global state referansı
	
	// ADIM 3: Flagları sıfırla
	lexer->current_char = '\0';    // Mevcut karakter
    lexer->t_cmd_flag = 0;         // Command flag
	
	// ADIM 4: İlk karakteri ayarla
	if (lexer->len > 0)
		lexer->current_char = input[0];  // İlk karakter
	else
		lexer->current_char = '\0';      // Boş string
		
	return (lexer);
}
```

**🎯 Fonksiyon Amacı:**
Lexer yapısını başlatır ve tokenization için hazırlar.

**📊 Lexer Yapısı (t_lexer_new):**
- `input`: İşlenecek string
- `pos`: Mevcut pozisyon (0'dan başlar)
- `len`: String uzunluğu
- `current_char`: İşlenmekte olan karakter
- `global`: Global state referansı
- `t_cmd_flag`: Komut flag'i

#### 2. advance_lexer Fonksiyonu (35-42. satırlar)

```c
void	advance_lexer(t_lexer_new *lexer)
{
	// ADIM 1: Pozisyonu bir ileri al
	lexer->pos++;
	
	// ADIM 2: String sonuna gelindi mi kontrol et
	if (lexer->pos >= lexer->len)
		lexer->current_char = '\0';  // EOF marker
	else
		lexer->current_char = lexer->input[lexer->pos];  // Sonraki karakter
}
```

**🎯 Fonksiyon Amacı:**
Lexer'ı bir sonraki karaktere ilerletir. Tokenization'da sürekli kullanılır.

**⚡ Kullanım Örneği:**
```c
// "hello" string'i için
lexer->pos = 0, current_char = 'h'
advance_lexer(lexer)  // pos = 1, current_char = 'e'
advance_lexer(lexer)  // pos = 2, current_char = 'l'
// ...
advance_lexer(lexer)  // pos = 5, current_char = '\0' (EOF)
```

#### 3. create_token_advanced Fonksiyonu (44-60. satırlar)

```c
t_token_new	*create_token_advanced(t_token_types type, char *value)
{
	t_token_new	*token;

	// ADIM 1: Token için memory allocation
	token = (t_token_new *)halloc(sizeof(t_token_new));
	if (!token)
		return (NULL);
	
	// ADIM 2: Token tipini ayarla
	token->type = type;
	
	// ADIM 3: Değeri kopyala
	token->value = ft_strdup(value);
	if (!token->value)  // Strdup başarısızsa
	{
		// free(token); // Garbage collector var, gerek yok
		return (NULL);
	}
	
	// ADIM 4: Diğer özellikleri ayarla
	token->len = ft_strlen(value);  // Değer uzunluğu
	token->quote_type = 0;          // Tırnak türü (default: yok)
	token->expanded = 0;            // Expansion flag (default: hayır)
	
	return (token);
}
```

**🎯 Fonksiyon Amacı:**
Yeni bir token oluşturur ve özelliklerini ayarlar.

**📊 Token Yapısı (t_token_new):**
- `type`: Token türü (T_WORD, T_PIPE, vb.)
- `value`: Token değeri (string)
- `len`: Değer uzunluğu
- `quote_type`: Tırnak türü (0=yok, 1=tek, 2=çift)
- `expanded`: Expansion yapıldı mı flag'i

#### 4. skip_whitespace_advanced Fonksiyonu (61-66. satırlar)

```c
void	skip_whitespace_advanced(t_lexer_new *lexer)
{
	// Boşluk, tab, newline karakterlerini atla
	while (lexer->current_char == ' ' || lexer->current_char == '\t'
		|| lexer->current_char == '\n')
		advance_lexer(lexer);  // Sonraki karaktere geç
}
```

**🎯 Fonksiyon Amacı:**
Whitespace karakterlerini atlar. Token'lar arası boşlukları temizler.

**💡 Neden Önemli:**
- `"ls    -l"` → `"ls -l"` olarak işlenir
- Multiple space'ler tek space gibi davranır
- Tab ve newline karakterleri de handle edilir

---

## 📄 tokenizer.c - Ana Tokenization Mantığı

### 📝 Satır Satır Kod Analizi

#### 1. create_token_by_type Fonksiyonu (42-51. satırlar)

```c
static t_token_new	*create_token_by_type(t_lexer_new *lexer)
{
	// ADIM 1: Mevcut karaktere göre token oluştur
	if (lexer->current_char == '|')
		return (handle_pipe_advanced(lexer));           // Pipe token
	else if ((lexer->current_char == ' ' || lexer->current_char == '\t'))
		return (handle_whitespaces_advanced(lexer));    // Whitespace token
	else if (lexer->current_char == '<' || lexer->current_char == '>')
		return (handle_redirect_advanced(lexer));       // Redirection token
	else if (lexer->current_char == '\'' || lexer->current_char == '"')
		return (handle_quotes_advanced(lexer));         // Quote token
	else
		return (handle_word_advanced(lexer, &lexer->first_word_check));  // Word token
}
```

**🎯 Fonksiyon Amacı:**
Mevcut karaktere bakarak hangi tür token oluşturulacağını belirler.

**🔍 Karakter Analizi:**
- `|` → Pipe token
- ` ` veya `\t` → Whitespace token
- `<` veya `>` → Redirection token
- `'` veya `"` → Quote token
- Diğer → Word token

#### 2. handle_post_token_processing Fonksiyonu (53-64. satırlar)

```c
static void	handle_post_token_processing(t_lexer_new *lexer, t_token_new *token)
{
	// ADIM 1: Özel token'larda post-processing yap
	if (token->type == T_CMD || token->type == T_PIPE)
	{
		// ADIM 2: Sonraki boşlukları atla
		skip_whitespace_advanced(lexer);
		
		// ADIM 3: Pipe'dan sonra command flag ayarla
		if (token->type == T_PIPE)
		{
			// Sonraki karakter printable mi kontrol et
			if (lexer->current_char >= 32 && lexer->current_char <= 127)
				lexer->t_cmd_flag = 1;  // Sonraki word command olacak
		}
	}
}
```

**🎯 Fonksiyon Amacı:**
Token oluşturulduktan sonra gerekli flag'ları ve pozisyonu ayarlar.

**💡 t_cmd_flag Mantığı:**
- Pipe'dan sonra gelen ilk word'ü T_CMD olarak işaretle
- `ls | grep` → `grep` T_CMD olarak işaretlenir

#### 3. get_next_token Fonksiyonu (66-83. satırlar)

```c
static t_token_new	*get_next_token(t_lexer_new *lexer)
{
	t_token_new	*token;

	token = NULL;
	
	// ADIM 1: First word flag'ini sıfırla
	lexer->first_word_check = 0;
	
	// ADIM 2: EOF kontrolü
	if (lexer->current_char == '\0')
		return (NULL);
	
	// ADIM 3: Karakter tipine göre token oluştur
	token = create_token_by_type(lexer);
	if (!token)
	{
		return (NULL);  // Token oluşturulamadı
	}

	// ADIM 4: Post-processing yap
	handle_post_token_processing(lexer, token);
	
	return (token);
}
```

**🎯 Fonksiyon Amacı:**
Lexer'dan bir sonraki token'ı alır. Ana tokenization döngüsünde kullanılır.

#### 4. process_token Fonksiyonu (143-154. satırlar)

```c
static int	process_token(t_list **tokens, t_token_new *token)
{
	// ADIM 1: Token geçerli mi kontrol et
	if (!token)
	{
		printf("Token Error\n");
		return (0);  // Hata
	}
	// ADIM 2: Token değeri var mı kontrol et
	else if (token->value)
	{
		// ADIM 3: Token'ı listeye ekle
		ft_lstadd_back(tokens, ft_lstnew(token));
		return (1);  // Başarılı
	}
	return (0);  // Başarısız
}
```

**🎯 Fonksiyon Amacı:**
Token'ı doğrular ve token listesine ekler.

#### 5. tokenize_advanced Fonksiyonu (156-174. satırlar)

```c
t_list	*tokenize_advanced(char *input, t_global *global)
{
	t_lexer_new	*lexer;
	t_list		*tokens;
	t_token_new	*token;
	
	// ADIM 1: Lexer'ı başlat
	lexer = init_lexer_advanced(input, global);
	if (!lexer)
		return (NULL);
	
	// ADIM 2: Token listesini başlat
	tokens = NULL;
	
	// ADIM 3: ANA TOKENIZATION DÖNGÜSÜ
	while (lexer->current_char)
	{
		// ADIM 4: Sonraki token'ı al
		token = get_next_token(lexer);
		
		// ADIM 5: Echo flag'ini sıfırla
		global->echo_flag = 0;
		
		// ADIM 6: Token'ı işle ve listeye ekle
		if (!process_token(&tokens, token))
			return (NULL);  // Hata durumunda NULL döndür
	}
	
	return (tokens);  // Token listesini döndür
}
```

**🎯 Fonksiyon Amacı:**
Ana tokenization fonksiyonu. String'i tamamen token'lara ayırır.

**🔄 Döngü Mantığı:**
1. **Başlat** → Lexer ve token listesi
2. **Döngü** → Her karakter için token oluştur
3. **İşle** → Token'ı doğrula ve listeye ekle
4. **Bitir** → Tüm string işlenene kadar devam et

---

## 📄 token_handlers.c - Token İşleyici Fonksiyonları

### 📝 Satır Satır Kod Analizi

#### 1. handle_pipe_advanced Fonksiyonu (15-18. satırlar)

```c
t_token_new	*handle_pipe_advanced(t_lexer_new *lexer)
{
	// ADIM 1: Pipe karakterini atla
	advance_lexer(lexer);
	
	// ADIM 2: Pipe token'ı oluştur
	return (create_token_advanced(T_PIPE, "|"));
}
```

**🎯 Fonksiyon Amacı:**
Pipe operatörünü (`|`) token'a çevirir.

**⚡ Kullanım Örneği:**
```
Input: "ls | grep"
       ^
Pipe bulundu → T_PIPE:"|" token'ı oluştur
Lexer pozisyonu ' ' karakterine ilerle
```

#### 2. handle_word_advanced Fonksiyonu (163-182. satırlar)

```c
t_token_new	*handle_word_advanced(t_lexer_new *lexer, int *first_word_check)
{
    int				start;  // Başlangıç pozisyonu
    t_token_types	types;  // Token türü

    // ADIM 1: Başlangıç pozisyonunu kaydet
    start = lexer->pos;
    types = T_WORD;  // Default türü
    
    // ADIM 2: Lexer geçerli mi kontrol et
    if (!lexer || !lexer->input || lexer->pos >= lexer->len)
        return (NULL);
    
    // ADIM 3: Command token'ı mı belirle
    if (lexer->pos == 0 || lexer->t_cmd_flag == 1)
    {
        // İlk word veya pipe'dan sonraki word - COMMAND
        types = T_CMD;
        *first_word_check = 0;
        lexer->t_cmd_flag = 0;  // Flag'i sıfırla
    }
    else
    {
        // Normal word - ARGUMENT
        types = T_WORD;
        *first_word_check = 1;
    }
    
    // ADIM 4: Word içeriğini işle
    return (process_word_content(lexer, start, types));
}
```

**🎯 Fonksiyon Amacı:**
Normal word'leri handle eder. Command vs argument ayrımı yapar.

**🔍 Command vs Argument Mantığı:**
- **İlk word** → T_CMD (komut)
- **Pipe'dan sonraki ilk word** → T_CMD
- **Diğer word'ler** → T_WORD (argüman)

**💡 Örnek:**
```
"ls -l | grep txt"
├── "ls"   → T_CMD (ilk word)
├── "-l"   → T_WORD (argüman)
├── "grep" → T_CMD (pipe'dan sonra)
└── "txt"  → T_WORD (argüman)
```

---

## 📄 token_handlers_advanced.c - Gelişmiş Token İşleyiciler

### 📝 Satır Satır Kod Analizi

#### 1. handle_input_redirect Fonksiyonu (16-28. satırlar)

```c
static t_token_new	*handle_input_redirect(t_lexer_new *lexer)
{
	// ADIM 1: İlk '<' karakterini atla
	advance_lexer(lexer);
	
	// ADIM 2: İkinci '<' var mı kontrol et (heredoc için)
	if (lexer->current_char == '<')
	{
		advance_lexer(lexer);           // İkinci '<'yi de atla
		skip_whitespace_advanced(lexer); // Sonraki boşlukları temizle
		
		// HEREDOC token'ı oluştur ("<<")
		return (create_token_advanced(T_HEREDOC, "<<"));
	}
	
	// Tek '<' - normal input redirection
	return (create_token_advanced(T_REDIRECT_IN, "<"));
}
```

**🎯 Fonksiyon Amacı:**
Input redirection operatörlerini (`<` ve `<<`) handle eder.

**📊 Çıktı Türleri:**
- `<` → T_REDIRECT_IN token'ı
- `<<` → T_HEREDOC token'ı

#### 2. handle_output_redirect Fonksiyonu (30-42. satırlar)

```c
static t_token_new	*handle_output_redirect(t_lexer_new *lexer)
{
	// ADIM 1: İlk '>' karakterini atla
	advance_lexer(lexer);
	
	// ADIM 2: İkinci '>' var mı kontrol et (append için)
	if (lexer->current_char == '>')
	{
		advance_lexer(lexer);           // İkinci '>'yi de atla
		skip_whitespace_advanced(lexer); // Sonraki boşlukları temizle
		
		// APPEND token'ı oluştur (">>")
		return (create_token_advanced(T_APPEND, ">>"));
	}
	
	// Tek '>' - normal output redirection
	return (create_token_advanced(T_REDIRECT_OUT, ">"));
}
```

**🎯 Fonksiyon Amacı:**
Output redirection operatörlerini (`>` ve `>>`) handle eder.

**📊 Çıktı Türleri:**
- `>` → T_REDIRECT_OUT token'ı
- `>>` → T_APPEND token'ı

#### 3. handle_redirect_advanced Fonksiyonu (44-53. satırlar)

```c
t_token_new	*handle_redirect_advanced(t_lexer_new *lexer)
{
    // ADIM 1: Redirection türünü belirle
    if (lexer->current_char == '<')
        return (handle_input_redirect(lexer));   // Input redirection
    else if (lexer->current_char == '>')
        return (handle_output_redirect(lexer));  // Output redirection
    
    return (NULL);  // Geçersiz karakter
}
```

**🎯 Fonksiyon Amacı:**
Ana redirection dispatcher. Hangi redirection türünü handle edeceğini belirler.

#### 4. handle_quotes_advanced Fonksiyonu (55-82. satırlar)

```c
t_token_new	*handle_quotes_advanced(t_lexer_new *lexer)
{
	char	quote_char;  // Tırnak karakteri (' veya ")
	int		start;       // İçeriğin başlangıç pozisyonu
	int		len;         // İçerik uzunluğu
	char	*quoted_str; // Tırnak içi string

	// ADIM 1: Tırnak karakterini kaydet ve atla
	quote_char = lexer->current_char; 
	advance_lexer(lexer);
	
	// ADIM 2: İçeriğin başlangıç pozisyonunu kaydet
	start = lexer->pos;
	
	// ADIM 3: Kapanış tırnağını bul
	while (lexer->current_char != '\0' && lexer->current_char != quote_char)
		advance_lexer(lexer);
	
	// ADIM 4: Kapanış tırnağı bulundu mu kontrol et
	if (lexer->current_char == quote_char)
	{
		// ADIM 5: İçerik uzunluğunu hesapla
		len = lexer->pos - start;
		
		// ADIM 6: İçeriği çıkart
		quoted_str = ft_substr(lexer->input, start, len);
		
		// ADIM 7: Kapanış tırnağını da atla
		advance_lexer(lexer);
		
		// ADIM 8: Boş string özel durumu
		if (len == 0)
			return(create_token_advanced(T_SINGLE_QUOTE, ft_strdup("")));
		
		// ADIM 9: Tırnak türüne göre token oluştur
		if (quote_char == '\'')
			return (create_token_advanced(T_SINGLE_QUOTE, quoted_str));
		else
			return (create_token_advanced(T_DOUBLE_QUOTE, quoted_str));
	}
	
	return (NULL);  // Kapanmamış tırnak - hata
}
```

**🎯 Fonksiyon Amacı:**
Tek ve çift tırnakları handle eder. Tırnak içi içeriği çıkarır.

**📊 Çıktı Türleri:**
- `'content'` → T_SINGLE_QUOTE:"content"
- `"content"` → T_DOUBLE_QUOTE:"content"
- `''` → T_SINGLE_QUOTE:""
- `""` → T_DOUBLE_QUOTE:""

**⚠️ Önemli Detaylar:**
- Tırnak karakterleri token değerine dahil edilmez
- Sadece içerik çıkarılır
- Kapanmamış tırnak NULL döndürür

#### 5. handle_whitespaces_advanced Fonksiyonu (84-115. satırlar)

```c
t_token_new	*handle_whitespaces_advanced(t_lexer_new *lexer)
{
	int		start;       // Başlangıç pozisyonu
	int		len;         // Whitespace uzunluğu
	char	*whitepaces; // Whitespace string'i

	// ADIM 1: Başlangıç pozisyonunu kaydet
	start = lexer->pos;
	
	// ADIM 2: Tüm whitespace karakterlerini atla
	while (lexer->current_char != '\0' && 
		   (lexer->current_char == ' ' || lexer->current_char == '\t'))
	{
		advance_lexer(lexer);
	}
	
	// ADIM 3: Whitespace uzunluğunu hesapla
	len = lexer->pos - start;
	
	// ADIM 4: Sabit whitespace değeri ata
	whitepaces = " ";  // Tüm whitespace'ler tek space olarak
	
	// ADIM 5: Whitespace token'ı oluştur
	return(create_token_advanced(T_WHITESPACE, whitepaces));
}
```

**🎯 Fonksiyon Amacı:**
Whitespace karakterlerini (space, tab) handle eder.

**💡 Önemli Nokta:**
- Multiple space'ler tek space token'ına çevrilir
- `"   "` → T_WHITESPACE:" "
- Bu, parsing aşamasında ignore edilebilir

---

## 🔄 Akış Şeması

### Ana Tokenization Akışı
```mermaid
graph TD
    A[tokenize_advanced başlar] --> B[init_lexer_advanced]
    B --> C[while lexer->current_char]
    C --> D[get_next_token]
    D --> E[create_token_by_type]
    E --> F{Karakter türü?}
    F -->|'|'| G[handle_pipe_advanced]
    F -->|' ' veya tab| H[handle_whitespaces_advanced]
    F -->|'<' veya '>'| I[handle_redirect_advanced]
    F -->|''' veya '"'| J[handle_quotes_advanced]
    F -->|Diğer| K[handle_word_advanced]
    G --> L[handle_post_token_processing]
    H --> L
    I --> L
    J --> L
    K --> L
    L --> M[process_token]
    M --> N[ft_lstadd_back]
    N --> C
    C --> O[return tokens]
```

### Character Processing Detayı
```mermaid
graph TD
    A[Character geldi] --> B{Karakter türü?}
    B -->|'|'| C[Pipe Token]
    B -->|'<'| D{Sonraki '<' mi?}
    B -->|'>'| E{Sonraki '>' mi?}
    B -->|'''| F[Single Quote Content]
    B -->|'"'| G[Double Quote Content]
    B -->|Space/Tab| H[Whitespace Token]
    B -->|Diğer| I{İlk word mu?}
    
    D -->|Evet| J[Heredoc Token]
    D -->|Hayır| K[Input Redirect Token]
    E -->|Evet| L[Append Token]
    E -->|Hayır| M[Output Redirect Token]
    I -->|Evet| N[Command Token]
    I -->|Hayır| O[Word Token]
```

---

## 🎬 Örnek Senaryolar

### Senaryo 1: Basit Komut
```bash
Input: "ls -l"
```

**Tokenization Süreci:**
1. `'l'` → handle_word_advanced → T_CMD:"ls"
2. `' '` → handle_whitespaces_advanced → T_WHITESPACE:" "
3. `'-'` → handle_word_advanced → T_WORD:"-l"

**Sonuç:**
```
[T_CMD:"ls", T_WHITESPACE:" ", T_WORD:"-l"]
```

### Senaryo 2: Pipeline
```bash
Input: "cat file.txt | grep hello"
```

**Tokenization Süreci:**
1. `'c'` → handle_word_advanced → T_CMD:"cat" (ilk word)
2. `' '` → handle_whitespaces_advanced → T_WHITESPACE:" "
3. `'f'` → handle_word_advanced → T_WORD:"file.txt"
4. `' '` → handle_whitespaces_advanced → T_WHITESPACE:" "
5. `'|'` → handle_pipe_advanced → T_PIPE:"|" (t_cmd_flag = 1)
6. `' '` → handle_whitespaces_advanced → T_WHITESPACE:" "
7. `'g'` → handle_word_advanced → T_CMD:"grep" (pipe'dan sonra)
8. `' '` → handle_whitespaces_advanced → T_WHITESPACE:" "
9. `'h'` → handle_word_advanced → T_WORD:"hello"

**Sonuç:**
```
[T_CMD:"cat", T_WHITESPACE:" ", T_WORD:"file.txt", T_WHITESPACE:" ", 
 T_PIPE:"|", T_WHITESPACE:" ", T_CMD:"grep", T_WHITESPACE:" ", T_WORD:"hello"]
```

### Senaryo 3: Redirection
```bash
Input: "cat < input.txt >> output.txt"
```

**Tokenization Süreci:**
1. `'c'` → handle_word_advanced → T_CMD:"cat"
2. `' '` → handle_whitespaces_advanced → T_WHITESPACE:" "
3. `'<'` → handle_redirect_advanced → handle_input_redirect → T_REDIRECT_IN:"<"
4. `' '` → handle_whitespaces_advanced → T_WHITESPACE:" "
5. `'i'` → handle_word_advanced → T_WORD:"input.txt"
6. `' '` → handle_whitespaces_advanced → T_WHITESPACE:" "
7. `'>'` → handle_redirect_advanced → handle_output_redirect (2 kez) → T_APPEND:">>"
8. `' '` → handle_whitespaces_advanced → T_WHITESPACE:" "
9. `'o'` → handle_word_advanced → T_WORD:"output.txt"

**Sonuç:**
```
[T_CMD:"cat", T_WHITESPACE:" ", T_REDIRECT_IN:"<", T_WHITESPACE:" ", 
 T_WORD:"input.txt", T_WHITESPACE:" ", T_APPEND:">>", T_WHITESPACE:" ", 
 T_WORD:"output.txt"]
```

### Senaryo 4: Tırnaklar
```bash
Input: "echo 'hello world' \"test $HOME\""
```

**Tokenization Süreci:**
1. `'e'` → handle_word_advanced → T_CMD:"echo"
2. `' '` → handle_whitespaces_advanced → T_WHITESPACE:" "
3. `'''` → handle_quotes_advanced → T_SINGLE_QUOTE:"hello world"
4. `' '` → handle_whitespaces_advanced → T_WHITESPACE:" "
5. `'"'` → handle_quotes_advanced → T_DOUBLE_QUOTE:"test $HOME"

**Sonuç:**
```
[T_CMD:"echo", T_WHITESPACE:" ", T_SINGLE_QUOTE:"hello world", 
 T_WHITESPACE:" ", T_DOUBLE_QUOTE:"test $HOME"]
```

### Senaryo 5: Heredoc
```bash
Input: "cat << EOF"
```

**Tokenization Süreci:**
1. `'c'` → handle_word_advanced → T_CMD:"cat"
2. `' '` → handle_whitespaces_advanced → T_WHITESPACE:" "
3. `'<'` → handle_redirect_advanced → handle_input_redirect (2 kez) → T_HEREDOC:"<<"
4. `' '` → handle_whitespaces_advanced → T_WHITESPACE:" "
5. `'E'` → handle_word_advanced → T_WORD:"EOF"

**Sonuç:**
```
[T_CMD:"cat", T_WHITESPACE:" ", T_HEREDOC:"<<", T_WHITESPACE:" ", T_WORD:"EOF"]
```

---

## 🎯 Önemli Noktalar

### 1. Token Türleri ve Anlamları
```c
typedef enum e_token_types
{
    T_WORD,           // Normal argümanlar: "file.txt", "-l"
    T_CMD,            // Komutlar: "ls", "grep", "cat"
    T_PIPE,           // Pipe operatörü: "|"
    T_REDIRECT_IN,    // Input redirection: "<"
    T_REDIRECT_OUT,   // Output redirection: ">"
    T_APPEND,         // Append redirection: ">>"
    T_HEREDOC,        // Here document: "<<"
    T_SINGLE_QUOTE,   // Tek tırnaklı: 'content'
    T_DOUBLE_QUOTE,   // Çift tırnaklı: "content"
    T_ENV_VAR,        // Environment variable: $VAR
    T_WHITESPACE,     // Boşluk karakteri
    T_EOF             // End of file
} t_token_types;
```

### 2. Command vs Word Ayrımı
```c
// İlk word → Command
if (lexer->pos == 0 || lexer->t_cmd_flag == 1) {
    types = T_CMD;
} else {
    types = T_WORD;
}
```

### 3. Quote Handling
```c
// Tek tırnak → Literal (expansion yok)
T_SINGLE_QUOTE:"$HOME" → Parser: "$HOME" (literal)

// Çift tırnak → Expansion var  
T_DOUBLE_QUOTE:"$HOME" → Expander: "/home/user"
```

### 4. Whitespace Management
```c
// Multiple space → Single space token
"   " → T_WHITESPACE:" "
// Parser aşamasında ignore edilebilir
```

### 5. Error Handling
```c
// Kapanmamış tırnak
"hello → return (NULL)

// Geçersiz character
if (!ft_isprint(char)) → Potential error
```

### 6. Memory Management
```c
// halloc() kullanımı - Garbage collector
lexer = (t_lexer_new *)halloc(sizeof(t_lexer_new));
token = (t_token_new *)halloc(sizeof(t_token_new));

// ft_strdup() - Kopyalama
token->value = ft_strdup(value);
```

Bu lexer modülü, minishell'in temel taşıdır ve ham string'i anlamlı token'lara dönüştürerek parser için hazırlar.
