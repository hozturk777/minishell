# 📋 Minishell Parser Detaylı Analizi

Bu dosya, `parser.c` dosyasındaki fonksiyonları satır satır analiz eder ve parser'ın nasıl çalıştığını açıklar.

## 🎯 Parser'ın Genel Amacı

Parser, lexer tarafından üretilen token'ları alır ve bunları çalıştırılabilir komut yapılarına (`t_command`) dönüştürür. Bu süreç şu adımları içerir:

1. **Token stream'ini komutlara ayırma**
2. **Her komut için argümanları toplama** 
3. **Yönlendirmeleri (redirections) parse etme**
4. **Pipeline chain'i oluşturma**

---

## 📊 Fonksiyon Hiyerarşisi

```
parse_tokens_to_commands()           ← Ana parser fonksiyonu
    ├── is_command_start()           ← Token kontrol fonksiyonu
    ├── parse_single_command()       ← Tek komut parser'ı
    │   ├── create_command()         ← Komut yapısı oluşturucu
    │   ├── is_pipe_token()          ← Pipe token kontrolü
    │   ├── is_redirect_token()      ← Redirect token kontrolü
    │   ├── is_word_token()          ← Word token kontrolü
    │   ├── parse_redirection()      ← Yönlendirme parser'ı
    │   ├── collect_command_arg()    ← Argüman toplama
    │   └── convert_list_to_array()  ← Liste → dizi dönüşümü
    └── append_command_to_chain()    ← Pipeline zincirleme
```

---

## 🔍 Fonksiyon Detaylı Analizleri

### 1. `parse_tokens_to_commands()` - Ana Parser Fonksiyonu

```c
t_command	*parse_tokens_to_commands(t_list *tokens, t_global *global)
{
    t_command	*head;        // Pipeline'ın başı
    t_command	*current;     // Şu anki komut
    t_list		*token_node;  // Token üzerinde iterator

    head = NULL;              // Pipeline başlangıçta boş
    current = NULL;
    token_node = tokens;      // Token listesinin başından başla
    
    while (token_node)        // Tüm token'ları tara
    {
        if (is_command_start(token_node))  // Komut başlangıcı mı?
        {
            // Tek komut parse et
            current = parse_single_command(&token_node, global);
            
            if (!head)        // İlk komut mu?
                head = current;
            else             // Zincire ekle
                append_command_to_chain(head, current);
        }
        else
            token_node = token_node->next;  // Sonraki token'a geç
    }
    return (head);           // Pipeline'ın başını döndür
}
```

**Ne Yapar:**
- Token listesini tarar
- Her komut başlangıcında `parse_single_command()` çağırır
- Komutları pipeline chain'inde birbirine bağlar
- **Örnek:** `ls -l | grep txt | wc -l` → 3 ayrı komut oluşturur

---

### 2. `parse_single_command()` - Tek Komut Parser'ı

```c
t_command	*parse_single_command(t_list **token_node, t_global *global)
{
    t_command	*cmd;         // Oluşturulacak komut
    t_list		*args_list;   // Argümanlar için geçici liste
    t_list		*current;     // Mevcut token

    cmd = create_command();   // Boş komut yapısı oluştur
    if (!cmd)
        return (NULL);
    
    args_list = NULL;         // Argüman listesi başlangıçta boş
    current = *token_node;    // Token pointer'ını al
    
    // Pipe'a kadar olan tüm token'ları işle
    while (current && !is_pipe_token(current))
    {
        if (is_redirect_token(current))           // Yönlendirme mi?
            parse_redirection(cmd, &current, global);
        else if (is_word_token(current))          // Kelime mi?
            collect_command_arg(&args_list, current);
        
        current = current->next;  // Sonraki token'a geç
    }
    
    cmd->args = convert_list_to_array(args_list);  // Liste → dizi
    *token_node = current;                         // Token pointer'ını güncelle
    ft_lstclear(&args_list, free);                 // Geçici listeyi temizle
    return (cmd);
}
```

**Ne Yapar:**
- Tek komutun tüm bileşenlerini parse eder
- Argümanları toplar (ls, -l, -a)
- Yönlendirmeleri işler (>, <, >>)
- Pipe'a gelince durur
- **Örnek:** `ls -l > output.txt` → args=["ls", "-l"], redirect="> output.txt"

---

### 3. `parse_redirection()` - Yönlendirme Parser'ı

```c
void	parse_redirection(t_command *cmd, t_list **token_node, t_global *global)
{
    t_redirect		*redirect;    // Yönlendirme yapısı
    t_token_new		*token;       // Mevcut token (>, <, >>)
    t_token_new		*file_token;  // Dosya adı token'ı

    token = (t_token_new *)(*token_node)->content;  // Token'ı al
    redirect = malloc(sizeof(t_redirect));          // Yönlendirme oluştur
    if (!redirect)
        return ;
    
    redirect->type = token->type;       // Yönlendirme türünü set et
    redirect->fd = -1;                  // File descriptor henüz yok
    redirect->next = NULL;              // Liste sonu
    
    *token_node = (*token_node)->next;  // Dosya adı token'ına geç
    
    if (*token_node)                    // Dosya adı var mı?
    {
        file_token = (t_token_new *)(*token_node)->content;
        redirect->filename = ft_strdup(file_token->value);  // Dosya adını kopyala
    }
    
    add_redirect_to_command(cmd, redirect);  // Komuta yönlendirme ekle
}
```

**Ne Yapar:**
- `>`, `<`, `>>`, `<<` operatörlerini işler
- Dosya adını yakalar
- Yönlendirme yapısını oluşturur
- **Örnek:** `> output.txt` → type=T_REDIRECT_OUT, filename="output.txt"

---

### 4. Token Kontrol Fonksiyonları

#### `is_command_start()` - Komut Başlangıcı Kontrolü
```c
int	is_command_start(t_list *token_node)
{
    t_token_new	*token;

    if (!token_node)
        return (0);
    token = (t_token_new *)token_node->content;
    return (token->type == T_WORD);  // Sadece kelime token'ları komut başlatır
}
```

#### `is_pipe_token()` - Pipe Token Kontrolü
```c
int	is_pipe_token(t_list *token_node)
{
    t_token_new	*token;

    if (!token_node)
        return (0);
    token = (t_token_new *)token_node->content;
    return (token->type == T_PIPE);  // | operatörü kontrolü
}
```

#### `is_redirect_token()` - Yönlendirme Token Kontrolü
```c
int	is_redirect_token(t_list *token_node)
{
    t_token_new	*token;

    if (!token_node)
        return (0);
    token = (t_token_new *)token_node->content;
    return (token->type == T_REDIRECT_IN || token->type == T_REDIRECT_OUT
        || token->type == T_APPEND || token->type == T_HEREDOC);
}
```

**Bu fonksiyonlar:** Token türlerini kontrol ederek parser'ın doğru yolu seçmesini sağlar.

---

### 5. `collect_command_arg()` - Argüman Toplama

```c
void	collect_command_arg(t_list **args_list, t_list *token_node)
{
    t_token_new	*token;
    char		*arg_copy;

    token = (t_token_new *)token_node->content;  // Token'ı al
    arg_copy = ft_strdup(token->value);          // Değeri kopyala
    ft_lstadd_back(args_list, ft_lstnew(arg_copy));  // Listeye ekle
}
```

**Ne Yapar:**
- Word token'larını alır
- String değerini kopyalar  
- Geçici argüman listesine ekler
- **Örnek:** "ls", "-l", "-a" token'larını args_list'e toplar

---

### 6. `convert_list_to_array()` - Liste → Dizi Dönüşümü

```c
char	**convert_list_to_array(t_list *args_list)
{
    char	**args_array;     // Sonuç dizisi
    int		size;             // Liste boyutu
    int		i;
    t_list	*current;

    size = ft_lstsize(args_list);                    // Liste boyutunu öğren
    args_array = malloc(sizeof(char *) * (size + 1)); // +1 NULL terminator için
    if (!args_array)
        return (NULL);
    
    i = 0;
    current = args_list;
    while (current)                                  // Liste üzerinde döngü
    {
        args_array[i] = ft_strdup((char *)current->content);  // String kopyala
        current = current->next;
        i++;
    }
    args_array[i] = NULL;                           // NULL terminator ekle
    return (args_array);
}
```

**Ne Yapar:**
- Linked list'i string dizisine çevirir
- execve() için gerekli format
- **Örnek:** ["ls", "-l", "-a", NULL] dizisi oluşturur

---

### 7. `append_command_to_chain()` - Pipeline Zincirleme

```c
void	append_command_to_chain(t_command *head, t_command *new_cmd)
{
    t_command	*current;

    current = head;
    while (current->next)        // Zincirin sonunu bul
        current = current->next;
    current->next = new_cmd;     // Yeni komutu ekle
}
```

**Ne Yapar:**
- Pipeline zincirinin sonuna yeni komut ekler
- **Örnek:** `cmd1 | cmd2 | cmd3` → cmd1->next=cmd2, cmd2->next=cmd3

---

### 8. `add_redirect_to_command()` - Yönlendirme Ekleme

```c
void	add_redirect_to_command(t_command *cmd, t_redirect *redirect)
{
    ft_lstadd_back(&cmd->redirections, ft_lstnew(redirect));
}
```

**Ne Yapar:**
- Yönlendirme yapısını komutun redirection listesine ekler
- Bir komutun birden fazla yönlendirmesi olabilir

---

## 🎯 Parser İş Akışı Örneği

### Girdi: `ls -l > output.txt | grep test`

#### **Adım 1:** Token'lar
```
[WORD:"ls"] [WORD:"-l"] [REDIRECT_OUT:">"] [WORD:"output.txt"] [PIPE:"|"] [WORD:"grep"] [WORD:"test"]
```

#### **Adım 2:** İlk Komut Parse
```c
// parse_single_command() çağrılır
cmd1 = {
    args: ["ls", "-l", NULL],
    redirections: [{ type: T_REDIRECT_OUT, filename: "output.txt" }],
    next: NULL
}
```

#### **Adım 3:** İkinci Komut Parse  
```c
// Pipe'dan sonra yeni parse_single_command() çağrılır
cmd2 = {
    args: ["grep", "test", NULL],
    redirections: [],
    next: NULL
}
```

#### **Adım 4:** Pipeline Oluşturma
```c
// append_command_to_chain() ile
cmd1->next = cmd2;
```

#### **Final Sonuç:**
```
Pipeline: cmd1 → cmd2
cmd1: ls -l > output.txt
cmd2: grep test
```

---

## ⚡ Parser'ın Güçlü Yanları

### ✅ **Modüler Tasarım**
- Her fonksiyon tek bir görevi yapar
- Kod tekrarı yok
- Test edilmesi kolay

### ✅ **Memory Yönetimi**
- Geçici listeler temizlenir
- String'ler kopyalanır (güvenli)
- Malloc hataları kontrol edilir

### ✅ **Extensibility**
- Yeni token türleri kolayca eklenebilir
- Yeni yönlendirme türleri desteklenebilir

---

## 🚨 Dikkat Edilmesi Gerekenler

### **1. Memory Leaks**
- `ft_strdup()` kullanılan her yerde free gerekli
- `malloc()` için NULL check yapılmalı

### **2. Token Pointer Yönetimi**
- `**token_node` double pointer doğru güncellenmeli
- Infinite loop'lardan kaçınılmalı

### **3. Error Handling**
- Syntax hataları handle edilmeli
- Incomplete command'lar kontrol edilmeli

---

## 🎯 Parser'ın Çıktısı

Parser başarıyla çalıştığında şu yapıyı üretir:

```c
t_command pipeline = {
    .args = ["ls", "-l", NULL],
    .redirections = [redirect_to_output_txt],
    .next = {
        .args = ["grep", "test", NULL], 
        .redirections = [],
        .next = NULL
    }
};
```

Bu yapı daha sonra executor tarafından alınır ve komutlar sırayla çalıştırılır! 🚀

---

## 📚 Öğrenilen Kavramlar

- **Abstract Syntax Tree (AST)** benzeri yapı oluşturma
- **Pipeline parsing** ve linked list yönetimi
- **Token stream processing** teknikleri
- **Memory management** ve pointer manipulation
- **Modular programming** prensipleri

Bu parser, minishell'in beynini oluşturur ve bash benzeri komut satırı işlemlerini mümkün kılar! 🧠
