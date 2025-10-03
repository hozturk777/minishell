# 🔄 EXPANDER Modülü Detaylı Analizi

## 📋 İçindekiler
1. [Modül Genel Bakış](#modül-genel-bakış)
2. [variable_expansion.c - Değişken Genişletme](#variable_expansionc---değişken-genişletme)
3. [quote_expansion.c - Tırnak İşleme](#quote_expansionc---tırnak-i̇şleme)
4. [argument_processor.c - Argüman İşleme](#argument_processorc---argüman-i̇şleme)
5. [expansion_utils.c - Yardımcı Fonksiyonlar](#expansion_utilsc---yardımcı-fonksiyonlar)
6. [Akış Şeması](#akış-şeması)
7. [Örnek Senaryolar](#örnek-senaryolar)

---

## 🎯 Modül Genel Bakış

Expander modülü, minishell'in **üçüncü aşaması**dır. Bu modül:
- **Variable expansion** yapar (`$HOME`, `$?`)
- **Quote processing** gerçekleştirir (`'literal'`, `"expanded"`)
- **Argument filtering** yapar (boş argümanları temizler)
- **Environment variable** çözümlemesi yapar

### 🎯 Expansion Nedir?
```
Input:  echo "Hello $USER, exit code: $?"
                ↓
Output: echo "Hello hasivaci, exit code: 0"
```

### 📁 Dosyalar
- `variable_expansion.c` - Değişken genişletme mantığı
- `quote_expansion.c` - Tırnak işleme ve quote state yönetimi
- `argument_processor.c` - Komut argümanlarını işleme
- `expansion_utils.c` - Yardımcı utility fonksiyonları

---

## 📄 variable_expansion.c - Değişken Genişletme

### 🔧 Fonksiyon Hiyerarşisi
```
handle_dollar_expansion()
├── Special variables ($?, $1, $2, ...)
├── process_variable_name()
│   ├── try_partial_matches() - Kısmi eşleşme
│   └── get_env_value() - Environment değer alma
└── handle_regular_char() - Normal karakter işleme
```

### 📝 Satır Satır Kod Analizi

#### 1. try_partial_matches Fonksiyonu (42-60. satırlar)

```c
static char	*try_partial_matches(char *var_name, int *i, int start, t_global *global)
{
    int		len;      // Değişken adı uzunluğu
    int		j;        // Iterator
    char	*partial_name;  // Kısmi ad
    char	*partial_value; // Kısmi değer

    len = ft_strlen(var_name);
    j = len - 1;
    
    // ADIM 1: Sondan başlayarak kısalt ve dene
    while (j > 0)
    {
        // ADIM 2: Kısmi ad oluştur
        partial_name = ft_substr(var_name, 0, j);
        
        // ADIM 3: Bu kısmi ad environment'ta var mı?
        partial_value = get_env_value(global->env_list, partial_name);
        
        if (partial_value)
        {
            // ADIM 4: Kısmi eşleşme bulundu!
            *i = start + j;  // Position'ı güncelle
            return (ft_strdup(""));  // Boş string döndür
        }
        j--; // Bir karakter daha kısalt
    }
    return (NULL);  // Hiçbir eşleşme bulunamadı
}
```

**🎯 Fonksiyon Amacı:**
Uzun değişken adlarında kısmi eşleşme arar. `$HOMEPATH` → `$HOME` şeklinde.

**💡 Kısmi Eşleşme Örneği:**
```bash
export HOME="/home/user"
echo $HOMEPATH
# HOME bulunur, PATH kısmı literal olarak kalır
# Sonuç: "/home/userPATH"
```

#### 2. process_variable_name Fonksiyonu (62-84. satırlar)

```c
static char	*process_variable_name(char *input, int *i, t_global *global)
{
    char	*var_name;   // Değişken adı
    char	*var_value;  // Değişken değeri
    int		start;       // Başlangıç pozisyonu

    // ADIM 1: Başlangıç pozisyonunu kaydet
    start = *i;
    
    // ADIM 2: Değişken adını oku (alphanumeric + _)
    while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
        (*i)++;
        
    // ADIM 3: Değişken adını çıkart
    var_name = ft_substr(input, start, *i - start);
    
    // ADIM 4: Environment'tan değeri al
    var_value = get_env_value(global->env_list, var_name);
    
    // ADIM 5: Bulunamadıysa kısmi eşleşme dene
    if (!var_value && ft_strlen(var_name) > 1)
    {
        var_value = try_partial_matches(var_name, i, start, global);
        if (var_value)
            return (var_value);
    }
    
    // ADIM 6: Sonucu döndür
    if (var_value)
        return (ft_strdup(var_value));
    else
        return (ft_strdup(""));  // Bulunamadıysa boş string
}
```

**🎯 Fonksiyon Amacı:**
Normal değişken adlarını işler ve environment'tan değerlerini alır.

#### 3. handle_dollar_expansion Fonksiyonu (86-122. satırlar)

```c
char	*handle_dollar_expansion(char *input, int *i, t_global *global)
{
    // ADIM 1: '$' karakterini atla
    (*i)++;
    
    // ADIM 2: Özel değişken '$?' (exit status)
    if (input[*i] == '?')
    {
        (*i)++;
        return (ft_itoa(global->exit_status));  // Exit status'u string'e çevir
    }
    
    // ADIM 3: Pozisyonel parametreler ($1, $2, vs.)
    if (ft_isdigit(input[*i]))
    {
        (*i)++;
        return (ft_strdup(""));  // Minishell'de desteklenmiyor
    }
    
    // ADIM 4: Geçersiz karakter kontrolü
    if (!ft_isalpha(input[*i]) && input[*i] != '_')
        return (ft_strdup("$"));  // Literal '$' döndür
    
    // ADIM 5: Normal değişken işleme
    return (process_variable_name(input, i, global));
}
```

**🎯 Fonksiyon Amacı:**
Dollar sign'dan sonraki expansion'ı handle eder. Özel değişkenleri de işler.

**⚡ Desteklenen Özel Değişkenler:**
- `$?` → Son komutun exit status'u
- `$1`, `$2` → Pozisyonel parametreler (boş döndürür)
- `$VARIABLE` → Environment değişkeni

#### 4. handle_regular_char Fonksiyonu (180-189. satırlar)

```c
char	*handle_regular_char(char *input, int *i)
{
	char	*result;

	// ADIM 1: Tek karakter için memory allocate et
	result = halloc(2);  // Karakter + NULL terminator
	if (!result)
		return (NULL);
	
	// ADIM 2: Karakteri kopyala
	result[0] = input[*i];
	result[1] = '\0';
	
	// ADIM 3: Pozisyonu ilerlet
	(*i)++;
	
	return (result);
}
```

**🎯 Fonksiyon Amacı:**
Normal karakterleri string'e çevirir. Expansion'da tek karakter işleme için kullanılır.

---

## 📄 quote_expansion.c - Tırnak İşleme

### 📝 Satır Satır Kod Analizi

#### 1. process_character Fonksiyonu (42-58. satırlar)

```c
static char	*process_character(char *input, int *i, t_global *global, int *quote_state)
{
    char	*temp;

    // ADIM 1: $ expansion (tek tırnak dışında)
    if (input[*i] == '$' && !quote_state[0])
        temp = handle_dollar_expansion(input, i, global);
    // ADIM 2: Çift tırnak içinde tek tırnak (literal)
    else if (input[*i] == '\'' && quote_state[1])
        temp = handle_regular_char(input, i);
    // ADIM 3: Tek tırnak içinde çift tırnak (literal)
    else if (input[*i] == '"' && quote_state[0])
        temp = handle_regular_char(input, i);
    // ADIM 4: Normal karakter (tırnak değil)
    else if (input[*i] != '\'' && input[*i] != '"')
        temp = handle_regular_char(input, i);
    // ADIM 5: Tırnak karakteri (skip)
    else
    {
        (*i)++;
        return (NULL);  // Skip tırnak karakteri
    }
    return (temp);
}
```

**🎯 Fonksiyon Amacı:**
Her karakteri quote state'e göre işler. Expansion veya literal olarak.

#### 2. expand_with_quotes Fonksiyonu (60-83. satırlar)

```c
char	*expand_with_quotes(char *input, t_global *global)
{
    char	*result;      // Sonuç string'i
    char	*temp;        // Geçici string
    int		i;            // Pozisyon
    int		quote_state[2]; // [single_quote, double_quote]

    // ADIM 1: Boş sonuç string'i başlat
    result = ft_strdup("");
    if (!result)
        return (NULL);
    
    // ADIM 2: Değişkenleri başlat
    i = 0;
    quote_state[0] = 0;  // Single quote kapalı
    quote_state[1] = 0;  // Double quote kapalı
    
    // ADIM 3: Her karakteri işle
    while (input[i])
    {
        // ADIM 4: Quote state'i güncelle
        update_quote_state(input[i], quote_state);
        
        // ADIM 5: Karakteri işle
        temp = process_character(input, &i, global, quote_state);
        if (!temp)
            continue;  // Tırnak karakteri skip edildi
        
        // ADIM 6: Sonuca ekle
        result = ft_strjoin(result, temp);
        if (!result)
            return (NULL);
    }
    
    return (result);
}
```

**🎯 Fonksiyon Amacı:**
Ana quote expansion fonksiyonu. Tırnak durumunu takip ederek expansion yapar.

#### 3. expand_with_heredoc Fonksiyonu (119-141. satırlar)

```c
char	*expand_with_heredoc(char *input, t_global *global)
{
	char	*result;  // Sonuç string'i
	char	*temp;    // Geçici string
	int		i;        // Pozisyon

	// ADIM 1: Boş sonuç string'i başlat
	result = ft_strdup("");
	i = 0;

	// ADIM 2: Her karakteri işle (heredoc'ta tırnak yok)
	while (input[i])
	{
		// ADIM 3: $ expansion varsa işle
		if (input[i] == '$')
			temp = handle_dollar_expansion(input, &i, global);
		else
		{
			// ADIM 4: Normal karakter - direkt ekle
			result = ft_strjoin_char(result, input[i]);
			i++;
			continue;
		}
		
		// ADIM 5: Expansion sonucunu ekle
		result = ft_strjoin(result, temp);
		if (!result)
			return (NULL);
	}
	return (result);
}
```

**🎯 Fonksiyon Amacı:**
Heredoc içeriği için expansion yapar. Heredoc'ta tırnak işleme yok.

#### 4. update_quote_state Fonksiyonu (144-151. satırlar)

```c
void	update_quote_state(char c, int *quote_state)
{
	// ADIM 1: Tek tırnak (çift tırnak açık değilse)
	if (c == '\'' && !quote_state[1])
		quote_state[0] = !quote_state[0];  // Toggle single quote
	// ADIM 2: Çift tırnak (tek tırnak açık değilse)
	else if (c == '"' && !quote_state[0])
		quote_state[1] = !quote_state[1];  // Toggle double quote
}
```

**🎯 Fonksiyon Amacı:**
Quote state'i günceller. Nested quote'ları handle eder.

**💡 Quote State Mantığı:**
```bash
echo "He said 'hello' to me"
# Double quote açık → single quote literal
echo 'He said "hello" to me'
# Single quote açık → double quote literal
```

#### 5. remove_outer_quotes Fonksiyonu (153-171. satırlar)

```c
char	*remove_outer_quotes(char *input)
{
	int		len;     // String uzunluğu
	char	*result; // Sonuç string'i

	// ADIM 1: Geçerlilik kontrolü
	if (!input)
		return (NULL);
	len = ft_strlen(input);
	if (len < 2)
		return (ft_strdup(input));  // Çok kısa, tırnak olamaz
	
	// ADIM 2: Dış tırnakları kontrol et
	if ((input[0] == '\'' && input[len - 1] == '\'')
		|| (input[0] == '"' && input[len - 1] == '"'))
	{
		// ADIM 3: Dış tırnakları kaldır
		result = ft_substr(input, 1, len - 2);
		return (result);
	}
	
	// ADIM 4: Tırnak yok, olduğu gibi döndür
	return (ft_strdup(input));
}
```

**🎯 Fonksiyon Amacı:**
String'in dış tırnaklarını kaldırır. Parser'dan gelen tırnaklı string'leri temizler.

---

## 📄 argument_processor.c - Argüman İşleme

### 📝 Satır Satır Kod Analizi

#### 1. expand_command_args Fonksiyonu (16-50. satırlar)

```c
void	expand_command_args(t_command *cmd, t_global *global)
{
	int		i;         // Argüman index'i
	char	*expanded; // Genişletilmiş string
	char	*clean_str; // Temizlenmiş string

	// ADIM 1: Geçerlilik kontrolü
	if (!cmd || !cmd->args)
		return;
	
	i = 0;
	// ADIM 2: Her argümanı işle
	while (cmd->args[i])
	{
		// ADIM 3: Çift tırnaklı mı? (expansion gerekli)
		if (is_double_quoted_literal(cmd->args[i]))
		{
			expanded = expand_with_quotes(cmd->args[i], global);
			if (expanded)
				cmd->args[i] = expanded;
		}
		// ADIM 4: Tek tırnaklı mı? (literal, expansion yok)
		else if (is_single_quoted_literal(cmd->args[i]))
		{
			clean_str = extract_single_quote_content(cmd->args[i]);
			cmd->args[i] = clean_str;
		}
		// ADIM 5: $ var mı? (expansion gerekli)
		else if (needs_expansion(cmd->args[i]))
		{
			expanded = expand_with_quotes(cmd->args[i], global);
			if (expanded)
				cmd->args[i] = expanded;
		}
		// ADIM 6: Normal string (sadece tırnak temizle)
		else
		{
			expanded = remove_outer_quotes(cmd->args[i]);
			if (expanded)
				cmd->args[i] = expanded;
		}
		i++;
	}
	
	// ADIM 7: Boş argümanları filtrele
	filter_empty_args(cmd);
}
```

**🎯 Fonksiyon Amacı:**
Komutun tüm argümanlarını işler. Quote türüne göre expansion yapar veya yapmaz.

#### 2. filter_empty_args Fonksiyonu (52-76. satırlar)

```c
void	filter_empty_args(t_command *cmd)
{
	int		i, j;        // Iterator'lar
	char	**new_args;  // Yeni argüman array'i
	int		count;       // Boş olmayan argüman sayısı

	// ADIM 1: Geçerlilik kontrolü
	if (!cmd || !cmd->args)
		return;
	
	// ADIM 2: Boş olmayan argüman sayısını say
	count = count_non_empty_args(cmd->args);
	
	// ADIM 3: Yeni array oluştur
	new_args = halloc(sizeof(char *) * (count + 1));
	if (!new_args)
		return;
	
	// ADIM 4: Boş olmayan argümanları kopyala
	i = 0;
	j = 0;
	while (cmd->args[i])
	{
		if (cmd->args[i][0] != '\0')  // Boş değilse
		{
			new_args[j] = cmd->args[i];
			cmd->args[i] = NULL;  // Eski pointer'ı temizle
			j++;
		}
		i++;
	}
	
	// ADIM 5: NULL terminator ekle ve değiştir
	new_args[j] = NULL;
	cmd->args = new_args;
}
```

**🎯 Fonksiyon Amacı:**
Expansion sonucu boş kalan argümanları kaldırır.

#### 3. count_non_empty_args Fonksiyonu (78-94. satırlar)

```c
int	count_non_empty_args(char **args)
{
	int	i;      // Iterator
	int	count;  // Sayaç

	// ADIM 1: Geçerlilik kontrolü
	if (!args)
		return (0);
	
	// ADIM 2: Boş olmayan argümanları say
	i = 0;
	count = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')  // Boş değilse
			count++;
		i++;
	}
	return (count);
}
```

**🎯 Fonksiyon Amacı:**
Boş olmayan argüman sayısını hesaplar.

---

## 📄 expansion_utils.c - Yardımcı Fonksiyonlar

### 📝 Satır Satır Kod Analizi

#### 1. needs_expansion Fonksiyonu (17-35. satırlar)

```c
int	needs_expansion(char *str)
{
	int	i;                // Iterator
	int	in_single_quote;  // Tek tırnak durumu

	// ADIM 1: Geçerlilik kontrolü
	if (!str)
		return (0);
	
	// ADIM 2: Değişkenleri başlat
	i = 0;
	in_single_quote = 0;
	
	// ADIM 3: String'i tara
	while (str[i])
	{
		// ADIM 4: Tek tırnak durumunu güncelle
		if (str[i] == '\'' && !in_single_quote)
			in_single_quote = 1;    // Tek tırnak açıldı
		else if (str[i] == '\'' && in_single_quote)
			in_single_quote = 0;    // Tek tırnak kapandı
		// ADIM 5: $ bulundu ve tek tırnak dışında
		else if (str[i] == '$' && !in_single_quote)
			return (1);  // Expansion gerekli
		i++;
	}
	return (0);  // Expansion gerekmiyor
}
```

**🎯 Fonksiyon Amacı:**
String'de expansion gerekip gerekmediğini kontrol eder.

#### 2. is_single_quoted_literal Fonksiyonu (37-43. satırlar)

```c
int	is_single_quoted_literal(char *str)
{
	// String'de single quote marker var mı kontrol et
	if (!str)
		return (0);
	return (ft_strnstr(str, "__SINGLE_QUOTE__", ft_strlen(str)) != NULL);
}
```

**🎯 Fonksiyon Amacı:**
String'in tek tırnaklı literal olup olmadığını kontrol eder.

#### 3. is_double_quoted_literal Fonksiyonu (45-51. satırlar)

```c
int	is_double_quoted_literal(char *str)
{
	// String'de double quote marker var mı kontrol et
	if (!str)
		return (0);
	return (ft_strnstr(str, "\"", ft_strlen(str)) != NULL);
}
```

**🎯 Fonksiyon Amacı:**
String'in çift tırnaklı olup olmadığını kontrol eder.

#### 4. extract_single_quote_content Fonksiyonu (53-76. satırlar)

```c
char	*extract_single_quote_content(char *str)
{
	char	*start;       // İçerik başlangıcı
	char	*end;         // İçerik sonu
	int		content_len;  // İçerik uzunluğu
	char	*result;      // Sonuç

	// ADIM 1: Geçerlilik kontrolü
	if (!str)
		return (NULL);
	
	// ADIM 2: Başlangıç marker'ını bul
	start = ft_strnstr(str, "__SINGLE_QUOTE__", ft_strlen(str));
	if (!start)
		return (ft_strdup(str));
	
	start += ft_strlen("__SINGLE_QUOTE__");
	
	// ADIM 3: Bitiş marker'ını bul
	end = ft_strnstr(start, "__END_SINGLE_QUOTE__", ft_strlen(start));
	if (!end)
		return (ft_strdup(start));
	
	// ADIM 4: İçeriği çıkart
	content_len = end - start;
	result = ft_substr(start, 0, content_len);
	return (result);
}
```

**🎯 Fonksiyon Amacı:**
Tek tırnakli marker'lardan içeriği çıkarır.

#### 5. ft_strjoin_char Fonksiyonu (78-96. satırlar)

```c
char	*ft_strjoin_char(char const *s1, char const s2)
{
	char	*str;  // Sonuç string'i
	int		i;     // String uzunluğu

	// ADIM 1: Geçerlilik kontrolü
	if (!s1 && !s2)
		return (NULL);
	
	// ADIM 2: Mevcut string uzunluğu
	i = ft_strlen(s1);
	
	// ADIM 3: Yeni string için memory (+2: char + NULL)
	str = halloc(i + 2);
	if (!str)
	{
        clear_garbage();
		exit(1);
	}
	
	// ADIM 4: Mevcut string'i kopyala
	ft_memcpy(str, s1, i);
	
	// ADIM 5: Yeni karakteri ekle
	ft_memcpy(str + i, &s2, 1);
	
	// ADIM 6: NULL terminator
	*(str + i + 1) = '\0';
	
	return (str);
}
```

**🎯 Fonksiyon Amacı:**
String'e tek karakter ekler. Heredoc expansion'da kullanılır.

---

## 🔄 Akış Şeması

### Ana Expansion Akışı
```mermaid
graph TD
    A[expand_command_args başlar] --> B[cmd->args[i] al]
    B --> C{Quote türü?}
    C -->|Double Quote| D[expand_with_quotes]
    C -->|Single Quote| E[extract_single_quote_content]
    C -->|needs_expansion| F[expand_with_quotes]
    C -->|Normal| G[remove_outer_quotes]
    D --> H[filter_empty_args]
    E --> H
    F --> H
    G --> H
    H --> I[Sonraki argüman]
    I --> B
    B --> J[Tüm argümanlar bitti]
```

### Variable Expansion Detayı
```mermaid
graph TD
    A[handle_dollar_expansion] --> B[$ karakterini atla]
    B --> C{Sonraki karakter?}
    C -->|?| D[ft_itoa(exit_status)]
    C -->|Digit| E[return empty string]
    C -->|Alpha/_| F[process_variable_name]
    C -->|Diğer| G[return $]
    F --> H[get_env_value]
    H --> I{Bulundu?}
    I -->|Evet| J[return value]
    I -->|Hayır| K[try_partial_matches]
    K --> L{Kısmi eşleşme?}
    L -->|Evet| M[return partial]
    L -->|Hayır| N[return empty]
```

---

## 🎬 Örnek Senaryolar

### Senaryo 1: Basit Variable Expansion
```bash
Input: echo "Hello $USER"
Environment: USER="hasivaci"
```

**Expansion Süreci:**
1. `expand_command_args()` çağrılır
2. `"Hello $USER"` → `is_double_quoted_literal()` → true
3. `expand_with_quotes()` çağrılır:
   - `H` → `handle_regular_char()` → "H"
   - `e` → `handle_regular_char()` → "e"
   - `$` → `handle_dollar_expansion()`:
     - `USER` → `get_env_value()` → "hasivaci"
   - Result: "Hello hasivaci"

**Sonuç:**
```c
cmd->args = ["echo", "Hello hasivaci", NULL]
```

### Senaryo 2: Exit Status Expansion
```bash
Input: echo "Last exit: $?"
Global: exit_status = 127
```

**Expansion Süreci:**
1. `$?` bulundu → `handle_dollar_expansion()`
2. `input[i] == '?'` → true
3. `ft_itoa(global->exit_status)` → "127"
4. Result: "Last exit: 127"

**Sonuç:**
```c
cmd->args = ["echo", "Last exit: 127", NULL]
```

### Senaryo 3: Single Quote Literal
```bash
Input: echo '$HOME'
```

**Expansion Süreci:**
1. `'$HOME'` → `is_single_quoted_literal()` → true
2. `extract_single_quote_content()` çağrılır
3. Marker'lar kaldırılır → "$HOME"
4. No expansion (literal)

**Sonuç:**
```c
cmd->args = ["echo", "$HOME", NULL]
```

### Senaryo 4: Mixed Quotes
```bash
Input: echo "User: $USER, Home: '$HOME'"
```

**Expansion Süreci:**
1. `expand_with_quotes()` çağrılır
2. Quote state tracking:
   - `"` → double quote açık
   - `$USER` → expansion (çift tırnak içinde)
   - `'$HOME'` → literal (çift tırnak içinde tek tırnak)
   - `"` → double quote kapandı
3. Result: "User: hasivaci, Home: '$HOME'"

### Senaryo 5: Empty Variable
```bash
Input: echo "$NONEXISTENT"
```

**Expansion Süreci:**
1. `$NONEXISTENT` → `handle_dollar_expansion()`
2. `process_variable_name()` → `get_env_value()` → NULL
3. `return ft_strdup("")` → ""
4. `filter_empty_args()` → Argüman kaldırılır

**Sonuç:**
```c
cmd->args = ["echo", NULL]  // Boş argüman kaldırıldı
```

### Senaryo 6: Partial Match
```bash
Input: echo "$HOMEPATH"
Environment: HOME="/home/user"
```

**Expansion Süreci:**
1. `$HOMEPATH` → `process_variable_name()`
2. `get_env_value("HOMEPATH")` → NULL
3. `try_partial_matches()`:
   - `HOMEPA` → NULL
   - `HOMEP` → NULL  
   - `HOME` → "/home/user" ✓
4. Position updated, "PATH" kısmı literal kalır
5. Result: "/home/userPATH"

### Senaryo 7: Heredoc Expansion
```bash
Input: cat << EOF
Hello $USER
EOF
```

**Expansion Süreci:**
1. `expand_with_heredoc()` çağrılır
2. Tırnak state yok, direkt expansion
3. `$USER` → `handle_dollar_expansion()` → "hasivaci"
4. Result: "Hello hasivaci"

---

## 🎯 Önemli Noktalar

### 1. Quote State Yönetimi
```c
int quote_state[2]; // [single_quote, double_quote]
// Single quote içinde expansion yok
// Double quote içinde expansion var
// Nested quote'lar literal
```

### 2. Expansion Öncelikleri
```c
// Öncelik sırası:
1. Single quote → Literal (expansion yok)
2. Double quote → Expansion var
3. needs_expansion() → $ var mı?
4. Normal → Sadece outer quote temizle
```

### 3. Özel Değişkenler
```c
$?  → exit_status          // Destekleniyor
$1  → positional params    // Boş döndürür
$$  → process_id           // Comment'li (desteklenmiyor)
```

### 4. Memory Management
```c
// halloc() kullanımı
char *result = halloc(size);
// Garbage collector otomatik temizlik yapar
```

### 5. Error Handling
```c
// NULL check'ler
if (!str) return (0);
if (!result) return (NULL);

// Empty string handling
return (ft_strdup(""));  // Bulunamayan variable'lar için
```

### 6. Performance Optimizations
```c
// Partial matching için backward search
// Quote state caching
// Boş argüman filtering
```

Bu expander modülü, bash-compatible variable expansion ve quote processing sağlar.
