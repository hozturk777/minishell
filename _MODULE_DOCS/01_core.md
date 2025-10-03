# 🎯 CORE Modülü Detaylı Analizi

## 📋 İçindekiler
1. [Modül Genel Bakış](#modül-genel-bakış)
2. [main.c - Ana Program Dosyası](#mainc---ana-program-dosyası)
3. [print_welcome.c - Karşılama Ekranı](#print_welcomec---karşılama-ekranı)
4. [token_handlers_advanced.c - Gelişmiş Token İşleyiciler](#token_handlers_advancedc---gelişmiş-token-i̇şleyiciler)
5. [Akış Şeması](#akış-şeması)
6. [Örnek Senaryolar](#örnek-senaryolar)

---

## 🎯 Modül Genel Bakış

Core modülü, minishell programının **kalbi**dir. Bu modül:
- Ana program giriş noktasını (`main`) içerir
- Shell döngüsünü yönetir
- Kullanıcı girdi işleme sürecini koordine eder
- Diğer tüm modülleri organize eder

### 📁 Dosyalar
- `main.c` - Ana program mantığı
- `print_welcome.c` - Karşılama ekranı
- `token_handlers_advanced.c` - Gelişmiş token işleyiciler

---

## 📄 main.c - Ana Program Dosyası

### 🔧 Fonksiyon Hiyerarşisi
```
main()
├── get_global()
├── init_global()
├── setup_signals()
├── print_welcome_advanced()
└── run_shell_loop()
    └── process_input()
        ├── tokenize_advanced()
        ├── parse_tokens_to_commands()
        └── execute_commands()
```

### 📝 Satır Satır Kod Analizi

#### 1. Header ve Include'lar (1-13. satırlar)
```c
/* Header bilgileri ve copyright */
#include "../../lib/minishell.h"  // Ana header dosyası
```

**Açıklama:**
- Standart 42 header formatı kullanılmış
- Tek include yeterli çünkü `minishell.h` tüm gerekli header'ları içerir

#### 2. process_input Fonksiyonu (15-62. satırlar)

```c
static int	process_input(char *input, t_global *global)
{
	t_list		*tokens;     // Token listesi için pointer
	t_command	*commands;   // Komut listesi için pointer

	// ADIM 1: Boş girdi kontrolü
	if (!input || ft_strlen(input) == 0)
		return (0);          // Boşsa hiçbir şey yapma
	
	// ADIM 2: History'ye ekle (readline kütüphanesi)
	add_history(input);
	
	// ADIM 3: String'i temizle (başında/sonunda boşluk varsa)
	input = ft_strtrim(input, " \t\n");  // Tab, space, newline karakterlerini temizle
	
	// ADIM 4: Global state'e girdiyi kaydet
	global->input_line = ft_strdup(input);  // Girdiyi kopyala ve global'e ata
	
	// ADIM 5: LEXER - Girdiyi token'lara çevir
	tokens = tokenize_advanced(input, global);
	if (!tokens)  // Tokenize başarısızsa
	{
		global->exit_status = 2;  // Syntax error kodu
		return (0);
	}
	
	// ADIM 6: Debug - Token'ları yazdır
	printf("\n=== TOKENS ===\n");
	print_tokens_advanced(tokens);
	
	// ADIM 7: PARSER - Token'ları komut yapılarına çevir
	commands = parse_tokens_to_commands(tokens, global);
	if (!commands)  // Parse başarısızsa
	{
		global->exit_status = 2;  // Syntax error kodu
		return (0);
	}
	
	// ADIM 8: Global state'e token ve komutları kaydet
	global->tokens = tokens;
	global->commands = commands;
	
	// ADIM 9: EXECUTOR - Komutları çalıştır
	if (commands)
		global->exit_status = execute_commands(commands, global);
	
	// ADIM 10: Temizlik
	if (global->input_line)
		global->input_line = NULL;	
	
	return (0);  // Başarılı tamamlama
}
```

**🎯 Fonksiyon Amacı:**
Kullanıcıdan gelen ham string girdiyi alıp, onu işleyerek komut olarak çalıştırır.

**📊 İşlem Akışı:**
1. **Girdi Doğrulama** → Boş mu kontrol et
2. **History Yönetimi** → Readline history'sine ekle
3. **String Temizleme** → Gereksiz boşlukları kaldır
4. **Lexical Analysis** → String'i token'lara ayır
5. **Parsing** → Token'ları komut yapılarına çevir
6. **Execution** → Komutları çalıştır

#### 3. run_shell_loop Fonksiyonu (64-82. satırlar)

```c
static void	run_shell_loop(t_global *global)
{
	char	*input;       // Kullanıcı girdisi için pointer
	int		should_exit;  // Çıkış flag'i

	should_exit = 0;
	
	// ANA SHELL DÖNGÜSÜ
	while (!should_exit && !global->should_exit)
	{
		// ADIM 1: Kullanıcıdan girdi al (readline ile)
		input = readline(PROMPT);  // PROMPT = "minishell$ "
		
		// ADIM 2: EOF (Ctrl+D) kontrolü
		if (!input)  // readline NULL döndürürse EOF demektir
		{
			handle_eof();        // EOF işleme fonksiyonu
			cleanup_and_exit();  // Temizlik ve çıkış
			break ;
		}
		
		// ADIM 3: Girdiyi işle
		should_exit = process_input(input, global);
		
		// ADIM 4: Memory cleanup
		free(input);  // readline malloc kullanır, free etmek gerekir
	}
}
```

**🎯 Fonksiyon Amacı:**
Minishell'in ana döngüsüdür. Sürekli kullanıcıdan komut alır ve işler.

**🔄 Döngü Mantığı:**
1. **Prompt Göster** → `minishell$ ` yazdır
2. **Girdi Al** → readline() ile kullanıcıdan string al
3. **EOF Kontrol** → Ctrl+D basıldı mı?
4. **İşle** → process_input() ile girdiyi işle
5. **Temizle** → Malloc'ed memory'yi free et
6. **Tekrarla** → Döngüye devam et

#### 4. get_global Fonksiyonu (84-89. satırlar)

```c
t_global	*get_global(void)
{
	static t_global	minishell;  // Static değişken - program boyunca yaşar

	return (&minishell);  // Her zaman aynı instance'ın adresini döndür
}
```

**🎯 Fonksiyon Amacı:**
Singleton pattern implementasyonu. Tüm program boyunca tek bir global state instance'ı kullanır.

**⚡ Singleton Avantajları:**
- Memory tasarrufu
- Global state'e her yerden erişim
- Consistency garantisi

#### 5. main Fonksiyonu (91-126. satırlar)

```c
int	main(int argc, char **argv, char **envp)
{
	// DEBUG: Process ID yazdır
	printf("\nDEBUG: Main process PID: %d\n", getpid());

	t_global	*global;  // Global state pointer

	// ADIM 1: Parametreleri ignore et (minishell parametre almaz)
	(void)argc;   // Unused parameter warning'ini engelle
	(void)argv;

	// ADIM 2: Global state'i al ve başlat
	global = get_global();                    // Singleton instance al
	global = init_global(envp, global);       // Environment ile başlat
	if (!global)  // Başlatma başarısızsa
	{
		printf("Error: Failed to initialize global state\n");
		cleanup_and_exit();  // FD'leri kapat ve çık
		return (1);
	}

	// ADIM 3: Signal handler'ları ayarla
	setup_signals();  // SIGINT, SIGQUIT, SIGPIPE için handler'lar

	// ADIM 4: Karşılama ekranını göster
	print_welcome_advanced();

	// ADIM 5: Debug mesajları
	debug_print("Global state initialized successfully");
	debug_print("Signal handlers set up successfully");

	// ADIM 6: Ana shell döngüsüne gir
	run_shell_loop(global);

	// ADIM 7: Çıkış mesajı ve temizlik
	printf(GREEN "Goodbye!" RESET "\n");
	cleanup_and_exit();  // FD'leri kapat ve garbage collect
	
	return (0);  // Başarılı çıkış
}
```

**🎯 Fonksiyon Amacı:**
Programın giriş noktası. Tüm sistemi başlatır ve koordine eder.

**🚀 Başlatma Sırası:**
1. **Debug Info** → Process ID göster
2. **Global Init** → Environment'ı yükle
3. **Signal Setup** → Keyboard interrupt'ları handle et
4. **Welcome Screen** → ASCII art göster
5. **Main Loop** → Shell döngüsüne gir
6. **Cleanup** → Program sonunda temizlik yap

---

## 📄 print_welcome.c - Karşılama Ekranı

### 📝 Satır Satır Kod Analizi

```c
#include "../../lib/minishell.h"

void	print_welcome_advanced(void)
{
	// ADIM 1: Ekranı temizle
	printf("\033[2J\033[H"); // ANSI escape codes: 2J=clear screen, H=cursor home
	
	// ADIM 2: Cyan bold renk ayarla
    printf("\033[1;36m"); // 1=bold, 36=cyan
    
    // ADIM 3: ASCII art ile başlık çiz
    printf("╔═══════════════════════════════════════════════════════════════════════════════════════════╗\n");
	usleep(20000);  // 20ms bekle (typing effect için)
	
    printf("║                                                                                           ║\n");
	usleep(20000);
	
    // MINISHELL yazısı (purple color)
    printf("║             \033[1;35m███╗   ███╗██╗███╗   ██╗██╗███████╗██╗  ██╗███████╗██╗     ██╗\033[1;36m                ║\n");
	usleep(20000);
    printf("║             \033[1;35m████╗ ████║██║████╗  ██║██║██╔════╝██║  ██║██╔════╝██║     ██║\033[1;36m                ║\n");
	usleep(20000);
    printf("║             \033[1;35m██╔████╔██║██║██╔██╗ ██║██║███████╗███████║█████╗  ██║     ██║\033[1;36m                ║\n");
	usleep(20000);
    printf("║             \033[1;35m██║╚██╔╝██║██║██║╚██╗██║██║╚════██║██╔══██║██╔══╝  ██║     ██║\033[1;36m                ║\n");
	usleep(20000);
    printf("║             \033[1;35m██║ ╚═╝ ██║██║██║ ╚████║██║███████║██║  ██║███████╗███████╗███████╗\033[1;36m           ║\n");
	usleep(20000);
    printf("║             \033[1;35m╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝\033[1;36m           ║\n");
	usleep(20000);
	
    printf("║                                                                                           ║\n");
	usleep(20000);
	
    // Yaratıcılar (yellow color)
    printf("║                             \033[1;33mCreated by: hasivaci && huozturk\033[1;36m                              ║\n");
	usleep(20000);
	
    // 42 School (green color)
    printf("║                                     \033[1;32m42 School Project\033[1;36m                                     ║\n");
	usleep(20000);
	
    printf("║                                                                                           ║\n");
	usleep(20000);
	
    // Selamlama (red color)
    printf("║                                   \033[1;31m🚀 SelamunAleyküm! 🚀\033[1;36m                                   ║\n");
	usleep(20000);
	
    printf("║                                                                                           ║\n");
	usleep(20000);
    printf("╚═══════════════════════════════════════════════════════════════════════════════════════════╝\033[0m\n");
	usleep(20000);
    printf("\n");
	usleep(20000);
}
```

**🎯 Fonksiyon Amacı:**
Program başlangıcında güzel bir karşılama ekranı gösterir.

**🎨 Özellikler:**
- **ANSI Colors**: Farklı renkler kullanır
- **ASCII Art**: MINISHELL yazısı
- **Typing Effect**: usleep() ile yavaş yazdırma efekti
- **Clear Screen**: Ekranı temizleyip başlangıç yapar

**🌈 Renk Kodları:**
- `\033[1;36m` → Cyan Bold
- `\033[1;35m` → Purple Bold
- `\033[1;33m` → Yellow Bold
- `\033[1;32m` → Green Bold
- `\033[1;31m` → Red Bold
- `\033[0m` → Reset (normal renk)

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

#### 6. handle_word_advanced Fonksiyonu (163-182. satırlar)

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

Bu core modülü, tüm minishell'in temelini oluşturur ve diğer tüm modülleri koordine eder.
