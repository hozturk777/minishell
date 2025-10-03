# 🏗️ CORE MODULE - Ana Program Akışı

## 📋 İçindekiler
1. [main.c - Ana Program Dosyası](#mainc---ana-program-dosyası)
2. [print_welcome.c - Karşılama Ekranı](#print_welcomec---karşılama-ekranı)

---

## 📄 main.c - Ana Program Dosyası

### 🎯 Dosyanın Amacı
`main.c` dosyası minishell programının ana entry point'idir. Programın başlatılması, ana döngünün çalıştırılması ve global state yönetiminden sorumludur.

### 🔄 Fonksiyon Akış Şeması
```
main() 
├── get_global()           // Global state singleton'ını al
├── init_global()          // Global state'i başlat  
├── setup_signals()        // Sinyal handler'ları ayarla
├── print_welcome_advanced() // Karşılama ekranını göster
└── run_shell_loop()       // Ana shell döngüsü
    ├── readline()         // Kullanıcıdan input al
    └── process_input()    // Input'u işle
        ├── tokenize_advanced()      // Lexer: Token'lara ayır
        ├── parse_tokens_to_commands() // Parser: Komutlara çevir  
        └── execute_commands()       // Executor: Komutları çalıştır
```

---

### 📝 Satır Satır Kod Analizi

#### 🔧 Global State Singleton Fonksiyonu

```c
t_global *get_global(void)
{
    static t_global minishell;  // Static değişken - sadece bir kez oluşturulur
    
    return (&minishell);        // Her çağrıda aynı instance'ı döndür
}
```

**Açıklama:**
- **Line 1**: `static` keyword ile global state'in sadece bir instance'ı olmasını garanti eder
- **Line 3**: Her çağrıda aynı memory adresini döndürür (Singleton Pattern)

**Kullanım Senaryosu:**
```c
t_global *global1 = get_global();
t_global *global2 = get_global();
// global1 == global2 (aynı memory adresi)
```

---

#### 🚀 Ana Fonksiyon (main)

```c
int main(int argc, char **argv, char **envp)
{
    printf("\nDEBUG: Main process PID: %d\n", getpid());
    
    t_global *global;
    
    (void)argc;     // Kullanılmayan parametreleri işaretle
    (void)argv;     // 42 norm kuralları gereği
    
    global = get_global();              // Global state'i al
    global = init_global(envp, global); // Environment ile başlat
    
    if (!global)    // Başlatma başarısız ise
    {
        printf("Error: Failed to initialize global state\n");
        cleanup_and_exit();
        return (1);
    }
    
    setup_signals();                    // SIGINT, SIGQUIT handler'larını ayarla
    print_welcome_advanced();           // Karşılama ekranını göster
    debug_print("Global state initialized successfully");
    debug_print("Signal handlers set up successfully");
    
    run_shell_loop(global);             // Ana shell döngüsünü başlat
    
    printf(GREEN "Goodbye!" RESET "\n");
    cleanup_and_exit();                 // Temizlik ve çıkış
    return (0);
}
```

**Açıklama:**
- **Line 3**: Process ID'yi debug için yazdır
- **Line 7-8**: Argc ve argv kullanılmadığı için void cast (42 norm kuralı)
- **Line 10-11**: Global state'i başlat ve environment'ı yükle
- **Line 13-18**: Error handling - başarısız initialization durumu
- **Line 20**: Signal handler'ları kurulum (Ctrl+C, Ctrl+\ için)
- **Line 21**: ASCII art karşılama ekranı
- **Line 25**: Ana shell döngüsünü başlat
- **Line 27-29**: Temiz çıkış

**Kullanım Senaryosu:**
```bash
./minishell
# Program başlar
# Karşılama ekranı gösterilir
# Shell prompt belirir: minishell$
```

---

#### 🔄 Ana Shell Döngüsü

```c
static void run_shell_loop(t_global *global)
{
    char *input;
    int should_exit;
    
    should_exit = 0;
    while (!should_exit && !global->should_exit)  // Çıkış koşulu kontrolü
    {
        input = readline(PROMPT);                  // Kullanıcıdan input al
        
        if (!input)  // EOF (Ctrl+D) algılandı
        {
            handle_eof();                          // EOF işleme
            cleanup_and_exit();                    // Temizlik ve çıkış
            break;
        }
        
        should_exit = process_input(input, global); // Input'u işle
        free(input);                               // Memory temizliği
    }
}
```

**Açıklama:**
- **Line 6**: Döngü koşulu - local veya global çıkış flag'i kontrol
- **Line 8**: `readline()` fonksiyonu ile prompt göster ve input al
- **Line 10-15**: Ctrl+D (EOF) durumu - graceful shutdown
- **Line 17**: Her input'u process_input() fonksiyonuna gönder
- **Line 18**: Memory leak önleme - input string'ini free et

**Kullanım Senaryosu:**
```bash
minishell$ ls -l        # readline() ile alınır
minishell$ echo "test"  # Her komut process_input()'a gönderilir
minishell$ exit         # should_exit = 1 olur, döngü biter
```

---

#### ⚙️ Input İşleme Fonksiyonu

```c
static int process_input(char *input, t_global *global)
{
    t_list *tokens;
    t_command *commands;
    
    // 1. GİRDİ KONTROLÜ ve HAZIRLIK
    if (!input || ft_strlen(input) == 0)          // Boş input kontrolü
        return (0);
    
    add_history(input);                           // Readline history'ye ekle
    input = ft_strtrim(input, " \t\n");          // Whitespace temizle
    global->input_line = ft_strdup(input);       // Global'e kaydet
    
    // 2. LEXER FAZISI - Tokenization
    tokens = tokenize_advanced(input, global);    // Input'u token'lara ayır
    if (!tokens)                                  // Tokenization başarısız
    {
        global->exit_status = 2;                  // Syntax error
        return (0);
    }
    
    printf("\n=== TOKENS ===\n");               // Debug output
    print_tokens_advanced(tokens);
    
    // 3. PARSER FAZISI - Syntax Analysis
    commands = parse_tokens_to_commands(tokens, global); // Token'ları komutlara çevir
    if (!commands)                                       // Parsing başarısız
    {
        global->exit_status = 2;                         // Syntax error
        return (0);
    }
    
    // 4. GLOBAL STATE GÜNCELLEME
    global->tokens = tokens;                      // Token'ları kaydet
    global->commands = commands;                  // Komutları kaydet
    
    // 5. EXECUTOR FAZISI - Command Execution
    if (commands)
        global->exit_status = execute_commands(commands, global); // Komutları çalıştır
    
    // 6. TEMİZLİK
    if (global->input_line)
        global->input_line = NULL;                // Input referansını temizle
    
    return (0);
}
```

**Açıklama:**

**📥 Input Hazırlama (Lines 6-11):**
- Boş input kontrolü
- Readline history'ye ekleme (↑ tuşu ile erişim için)
- Whitespace temizleme
- Global state'e backup

**🔤 Lexer Fazı (Lines 13-21):**
- Raw string'i anlamlı token'lara ayırma
- Syntax hataları için error handling
- Debug output (geliştirme aşamasında)

**🏗️ Parser Fazı (Lines 23-31):**
- Token'ları command structure'larına dönüştürme
- Pipeline ve redirection analizi
- Syntax validation

**💾 State Management (Lines 33-35):**
- İşlenmiş veriyi global state'e kaydetme
- Diğer modüllerin erişimi için

**⚡ Execution Fazı (Lines 37-39):**
- Komutları actual execution'a gönderme
- Exit status tracking

**Kullanım Senaryosu:**
```bash
minishell$ ls -l | grep .c > files.txt

# İşlem Adımları:
# 1. Input: "ls -l | grep .c > files.txt"
# 2. Lexer: [T_WORD:"ls", T_WORD:"-l", T_PIPE:"|", T_WORD:"grep", T_WORD:".c", T_REDIRECT_OUT:">", T_WORD:"files.txt"]
# 3. Parser: İki komut objesi + bir redirection
# 4. Executor: Pipeline execution with output redirection
```

---

### 🎯 Fonksiyon Çağrı Hiyerarşisi

```
main()
├── get_global()
├── init_global() 
│   ├── init_env_from_envp()      [environment/]
│   └── ft_bzero()                [libft/]
├── setup_signals()               [signal/]
├── print_welcome_advanced()      [core/]
└── run_shell_loop()
    └── process_input()
        ├── add_history()         [readline lib]
        ├── ft_strtrim()          [libft/]
        ├── tokenize_advanced()   [lexer/]
        ├── parse_tokens_to_commands() [parser/]
        └── execute_commands()    [executer/]
```

### 🚫 Error Handling

```c
// Memory allocation failure
if (!global) {
    cleanup_and_exit();
    return (1);
}

// Tokenization failure  
if (!tokens) {
    global->exit_status = 2;  // Syntax error code
    return (0);
}

// EOF handling (Ctrl+D)
if (!input) {
    handle_eof();
    cleanup_and_exit();
    break;
}
```

### 🧪 Test Senaryoları

1. **Normal Komut:**
   ```bash
   minishell$ echo "Hello World"
   # process_input() çalışır, başarılı execution
   ```

2. **Syntax Error:**
   ```bash
   minishell$ echo |
   # Tokenization başarısız, exit_status = 2
   ```

3. **EOF (Ctrl+D):**
   ```bash
   minishell$ ^D
   # handle_eof() çağrılır, graceful exit
   ```

4. **Empty Input:**
   ```bash
   minishell$ 
   # Boş input, hiçbir işlem yapılmaz
   ```

---

## 📄 print_welcome.c - Karşılama Ekranı

### 🎯 Dosyanın Amacı
Program başlatıldığında kullanıcıya görsel bir karşılama ekranı gösterir. ASCII art ve renkli output kullanarak professional bir görünüm sağlar.

### 📝 Satır Satır Kod Analizi

```c
void print_welcome_advanced(void)
{
    // 1. EKRAN TEMİZLEME ve POZİSYONLAMA
    printf("\033[2J\033[H");        // ANSI escape codes
    // \033[2J = Clear entire screen
    // \033[H  = Move cursor to home position (0,0)
    
    // 2. RENK AYARLAMA
    printf("\033[1;36m");           // Cyan bold color
    // \033[1 = Bold style
    // 36m    = Cyan color
    
    // 3. ASCII ART ÇERÇEVE
    printf("╔═══════════════════════════════════════════════════════════════════════════════════════════╗\n");
    usleep(20000);                  // 20ms delay - typing effect
    
    printf("║                                                                                           ║\n");
    usleep(20000);
    
    // 4. MINISHELL LOGO (Magenta renk)
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
    
    // 5. PROJE BİLGİLERİ
    printf("║                                                                                           ║\n");
    usleep(20000);
    printf("║                             \033[1;33mCreated by: hasivaci && huozturk\033[1;36m                              ║\n");
    // \033[1;33m = Yellow bold
    usleep(20000);
    printf("║                                     \033[1;32m42 School Project\033[1;36m                                     ║\n");
    // \033[1;32m = Green bold
    usleep(20000);
    
    // 6. KARŞILAMA MESAJI
    printf("║                                                                                           ║\n");
    usleep(20000);
    printf("║                                   \033[1;31m🚀 SelamunAleyküm! 🚀\033[1;36m                                   ║\n");
    // \033[1;31m = Red bold + emoji
    usleep(20000);
    printf("║                                                                                           ║\n");
    usleep(20000);
    
    // 7. ÇERÇEVE KAPATMA
    printf("╚═══════════════════════════════════════════════════════════════════════════════════════════╝\033[0m\n");
    // \033[0m = Reset all formatting
    usleep(20000);
    
    // 8. KULLANICI TALİMATLARI
    printf("\n");
    usleep(20000);
    printf("\033[1;32mType 'exit' to quit the shell.\033[0m\n");
    // \033[1;32m = Green bold
    usleep(20000);
    printf("\033[1;34mEnjoy coding! 💻\033[0m\n\n");
    // \033[1;34m = Blue bold
    usleep(20000);
}
```

### 🎨 ANSI Color Codes Kullanımı

| Kod | Renk | Kullanım Alanı |
|-----|------|---------------|
| `\033[1;36m` | Cyan Bold | Çerçeve |
| `\033[1;35m` | Magenta Bold | Logo |
| `\033[1;33m` | Yellow Bold | Yazarlar |
| `\033[1;32m` | Green Bold | Proje info |
| `\033[1;31m` | Red Bold | Karşılama |
| `\033[1;34m` | Blue Bold | Talimatlar |
| `\033[0m` | Reset | Format sıfırlama |

### ⏱️ Timing Effect

```c
usleep(20000);  // 20 milisaniye = 0.02 saniye bekleme
```
**Amaç:** Typewriter effect - satırların ard arda görünmesi için

### 🖥️ Terminal Control

```c
printf("\033[2J\033[H");  // Screen clear + cursor home
```
**Açıklama:**
- `\033[2J`: Tüm terminal ekranını temizle
- `\033[H`: Cursor'u sol üst köşeye (0,0) taşı

### 🎯 Fonksiyon Çağrı Zamanı

```
main()
├── init_global()
├── setup_signals()  
├── print_welcome_advanced()  ← Burada çağrılır
└── run_shell_loop()
```

### 📱 Görsel Çıktı Örneği

```
╔═══════════════════════════════════════════════════════════════════════════════════════════╗
║                                                                                           ║
║             ███╗   ███╗██╗███╗   ██╗██╗███████╗██╗  ██╗███████╗██╗     ██╗                ║
║             ████╗ ████║██║████╗  ██║██║██╔════╝██║  ██║██╔════╝██║     ██║                ║
║             ██╔████╔██║██║██╔██╗ ██║██║███████╗███████║█████╗  ██║     ██║                ║
║             ██║╚██╔╝██║██║██║╚██╗██║██║╚════██║██╔══██║██╔══╝  ██║     ██║                ║
║             ██║ ╚═╝ ██║██║██║ ╚████║██║███████║██║  ██║███████╗███████╗███████╗           ║
║             ╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝           ║
║                                                                                           ║
║                             Created by: hasivaci && huozturk                              ║
║                                     42 School Project                                     ║
║                                                                                           ║
║                                   🚀 SelamunAleyküm! 🚀                                   ║
║                                                                                           ║
╚═══════════════════════════════════════════════════════════════════════════════════════════╝

Type 'exit' to quit the shell.
Enjoy coding! 💻

minishell$ 
```

---

## 🔗 Modül Bağımlılıkları

### Import Edilen Dosyalar
```c
#include "../../lib/minishell.h"  // Ana header dosyası
```

### Kullanılan External Fonksiyonlar
- `readline()` - GNU Readline library
- `add_history()` - GNU Readline library  
- `printf()` - Standard C library
- `usleep()` - POSIX sleep function
- `getpid()` - Process ID alma

### Çağrılan Internal Fonksiyonlar
- `init_global()` - Environment initialization
- `setup_signals()` - Signal handler setup
- `tokenize_advanced()` - Lexer module
- `parse_tokens_to_commands()` - Parser module
- `execute_commands()` - Executor module
- `cleanup_and_exit()` - Cleanup utility

---

## 🧪 Test Örnekleri

### 1. Normal Başlatma
```bash
./minishell
# Karşılama ekranı gösterilir
# Shell prompt belirir
```

### 2. Environment Başlatma Hatası
```bash
./minishell  # envp bozuk olduğunda
# "Error: Failed to initialize global state"
# Program return code 1 ile çıkar
```

### 3. EOF ile Çıkış
```bash
minishell$ ^D  # Ctrl+D
# handle_eof() çağrılır
# "Goodbye!" mesajı
# Program temiz çıkış yapar
```

---

## 🎯 Özet

**Core modülü** minishell'in kalbidir ve şu sorumluluklara sahiptir:

1. **Program Lifecycle Management**: Başlatma, ana döngü, çıkış
2. **Global State Management**: Singleton pattern ile state yönetimi  
3. **Input Processing Pipeline**: Lexer → Parser → Executor koordinasyonu
4. **Error Handling**: Graceful error handling ve recovery
5. **User Experience**: Professional karşılama ekranı ve feedback

Bu modül diğer tüm modüllerin orchestrator'ı olarak çalışır ve programın genel akışını kontrol eder.
