# ENVIRONMENT MODÜL DOKÜMANTASYONU

## GENEL BAKIŞ

Environment modülü, minishell'de environment variable'ların yönetiminden sorumludur. UNIX shell'in environment davranışını taklit eder ve PATH resolution, variable setting/getting, ve system environment ile integration sağlar.

## MODÜLÜNDEKİ DOSYALAR

1. **environment.c** - Environment linked list yapısı ve temel işlemler
2. **env_utils.c** - Environment başlatma ve parsing fonksiyonları
3. **env_utils2.c** - Environment manipulation ve utility fonksiyonları
4. **path_utils.c** - PATH resolution ve command finding işlemleri

## ENVIRONMENT YAPISI

```c
typedef struct s_env
{
    char            *key;      // Environment variable adı (ör: "PATH")
    char            *value;    // Environment variable değeri (ör: "/usr/bin:/bin")
    struct s_env    *next;     // Sonraki environment node'u
} t_env;
```

## İŞLEV AKIŞI

```
Program Start
├── init_env_from_envp()     # System environment'ı al
├── process_envp_entries()   # Environment'ı parse et
└── create_env_nodes()       # Linked list oluştur

Runtime Operations
├── get_env_value()          # Variable değerini al
├── set_env_var()            # Variable set et
├── unset_env_var()          # Variable sil
└── find_command_path()      # PATH'te komut ara
```

## 1. ENVIRONMENT.C - TEMEL VERİ YAPISI YÖNETİMİ

### Kod Analizi - create_env_node()

```c
t_env	*create_env_node(char *key, char *value)
{
	t_env	*node;                              // 1. Yeni node pointer'ı

	node = (t_env *)halloc(sizeof(t_env));      // 2. Memory allocate et
	if (!node)                                  // 3. Allocation başarısız mı?
		return (NULL);                          // 4. Hata dön
		
	node->key = ft_strdup(key);                 // 5. Key'i kopyala
	node->value = ft_strdup(value);             // 6. Value'yu kopyala
	node->next = NULL;                          // 7. Next pointer'ı null yap
	
	if (!node->key)                             // 8. Key kopyalama başarısız mı?
	{
		return (NULL);                          // 9. Hata dön (memory leak!)
	}
	return (node);                              // 10. Yeni node'u dön
}
```

**Akış Açıklaması:**
1. **Memory Allocation**: Yeni t_env struct'ı için memory ayırır
2. **String Duplication**: Key ve value string'lerini kopyalar
3. **Initialization**: Next pointer'ı null yapar
4. **Error Handling**: Allocation hatalarını kontrol eder

**Örnek Senaryolar:**
- `create_env_node("PATH", "/usr/bin:/bin")` → PATH environment node'u
- `create_env_node("HOME", "/home/user")` → HOME environment node'u
- `create_env_node("SHELL", "/bin/bash")` → SHELL environment node'u

### Kod Analizi - add_env_node()

```c
void	add_env_node(t_env **env_list, t_env *new_node)
{
	t_env	*current;                           // 1. Mevcut node traversal pointer'ı

	if (!env_list || !new_node)                 // 2. Null pointer kontrolü
		return ;                                // 3. Geçersizse çık
		
	if (!*env_list)                             // 4. Liste boş mu?
	{
		*env_list = new_node;                   // 5. İlk node olarak ekle
		return ;                                // 6. İşlemi tamamla
	}
	
	current = *env_list;                        // 7. Liste başından başla
	while (current->next)                       // 8. Son node'a kadar git
		current = current->next;                // 9. Sonraki node'a geç
	current->next = new_node;                   // 10. Yeni node'u listenin sonuna ekle
}
```

**Akış Açıklaması:**
1. **Validation**: Parametre geçerliliğini kontrol eder
2. **Empty List**: Liste boşsa ilk element olarak ekler
3. **Traversal**: Liste sonuna kadar traverse eder
4. **Insertion**: Yeni node'u listenin sonuna bağlar

### Kod Analizi - find_env_node()

```c
t_env	*find_env_node(t_env *env_list, char *key)
{
	t_env	*current;                           // 1. Traversal pointer'ı

	current = env_list;                         // 2. Liste başından başla
	while (current)                             // 3. Liste sonuna kadar
	{
		// 4. Key'ler eşit mi? (tam uzunluk kontrolü ile)
		if (ft_strncmp(current->key, key, ft_strlen(key)) == 0
			&& ft_strlen(current->key) == ft_strlen(key))
			return (current);                   // 5. Bulduğu node'u dön
		current = current->next;                // 6. Sonraki node'a geç
	}
	return (NULL);                              // 7. Bulunamadı, null dön
}
```

**Akış Açıklaması:**
1. **Linear Search**: Linked list'te linear search yapar
2. **String Comparison**: Key string'lerini karşılaştırır
3. **Length Check**: Tam eşleşme için uzunluk kontrolü yapar
4. **Return**: Bulduğu node'u veya NULL döner

**Örnek Senaryolar:**
- `find_env_node(env_list, "PATH")` → PATH node'unu bulur
- `find_env_node(env_list, "NONEXISTENT")` → NULL döner
- `find_env_node(env_list, "HOME")` → HOME node'unu bulur

## 2. ENV_UTILS.C - ENVIRONMENT BAŞLATMA

### Kod Analizi - init_env_from_envp()

```c
t_env	*init_env_from_envp(char **envp)
{
	t_env	*env_list;                          // 1. Environment listesi

	env_list = NULL;                            // 2. Liste'yi null'la başlat
	if (!envp || !envp[0])                      // 3. System environment yok mu?
		return (create_minimal_env());          // 4. Minimal environment oluştur
	else                                        // 5. System environment var
		return (process_envp_entries(envp));    // 6. Environment'ı işle
}
```

### Kod Analizi - process_envp_entries()

```c
static t_env	*process_envp_entries(char **envp)
{
	t_env	*env_list;                          // 1. Environment listesi
	t_env	*new_node;                          // 2. Yeni node pointer'ı
	char	*key;                               // 3. Variable key'i
	char	*value;                             // 4. Variable value'su
	int		i;                                  // 5. Index counter

	env_list = NULL;                            // 6. Liste'yi null ile başlat
	i = 0;                                      // 7. Index'i sıfırla
	while (envp[i])                             // 8. Her environment string için
	{
		key = extract_env_key(envp[i]);         // 9. Key kısmını çıkar
		value = extract_env_value(envp[i]);     // 10. Value kısmını çıkar
		if (key && value)                       // 11. Her ikisi de geçerli mi?
		{
			new_node = create_env_node(key, value); // 12. Yeni node oluştur
			if (new_node)                       // 13. Node oluşturma başarılı mı?
				add_env_node(&env_list, new_node); // 14. Listeye ekle
		}
		i++;                                    // 15. Sonraki environment'a geç
	}
	return (env_list);                          // 16. Oluşturulan listeyi dön
}
```

### Kod Analizi - extract_env_key()

```c
static char	*extract_env_key(char *env_str)
{
	int		i;                                  // 1. Index counter
	char	*key;                               // 2. Çıkarılacak key

	i = 0;                                      // 3. Index'i sıfırla
	while (env_str[i] && env_str[i] != '=')     // 4. '=' karakterine kadar git
		i++;                                    // 5. Index'i artır
	key = ft_substr(env_str, 0, i);             // 6. Baştan '='e kadar substring al
	return (key);                               // 7. Key'i dön
}
```

### Kod Analizi - extract_env_value()

```c
static char	*extract_env_value(char *env_str)
{
	int		i;                                  // 1. Index counter
	char	*value;                             // 2. Çıkarılacak value

	i = 0;                                      // 3. Index'i sıfırla
	while (env_str[i] && env_str[i] != '=')     // 4. '=' karakterini bul
		i++;                                    // 5. Index'i artır
	if (env_str[i] == '=')                      // 6. '=' bulundu mu?
		i++;                                    // 7. '=' karakterini atla
	value = ft_strdup(&env_str[i]);             // 8. '='den sonrasını kopyala
	return (value);                             // 9. Value'yu dön
}
```

### Kod Analizi - create_minimal_env()

```c
static t_env	*create_minimal_env(void)
{
	t_env	*env_list;                          // 1. Environment listesi
	t_env	*new_node;                          // 2. Yeni node pointer'ı
	char	*cwd;                               // 3. Current working directory

	cwd = getcwd(NULL, 0);                      // 4. Mevcut dizini al
	if (cwd)                                    // 5. getcwd başarılı mı?
	{
		new_node = create_env_node("PWD", cwd); // 6. PWD node'u oluştur
		if (new_node)                           // 7. Node oluşturma başarılı mı?
			add_env_node(&env_list, new_node);  // 8. Listeye ekle
	}

	new_node = create_env_node("SHLVL", "0");   // 9. SHLVL node'u oluştur
	if (new_node)                               // 10. Node oluşturma başarılı mı?
		add_env_node(&env_list, new_node);      // 11. Listeye ekle

	new_node = create_env_node("_", "./minishell"); // 12. _ (last command) node'u
	if (new_node)                               // 13. Node oluşturma başarılı mı?
		add_env_node(&env_list, new_node);      // 14. Listeye ekle

	return (env_list);                          // 15. Minimal environment'ı dön
}
```

**Environment Başlatma Akışı:**
1. **System Check**: System environment var mı kontrol eder
2. **Parsing**: Environment string'lerini key=value'ya ayırır
3. **Node Creation**: Her variable için linked list node'u oluşturur
4. **Minimal Fallback**: System environment yoksa minimal set oluşturur

**Örnek Senaryolar:**
- Normal başlatma: System environment'dan tüm variable'ları alır
- Isolated başlatma: Sadece PWD, SHLVL, _ variable'larını oluşturur

## 3. ENV_UTILS2.C - ENVIRONMENT MANİPULASYON

### Kod Analizi - set_env_var()

```c
void	set_env_var(t_global *global, char *key, char *value)
{
	t_env	*existing;                          // 1. Mevcut node pointer'ı
	t_env	*new_node;                          // 2. Yeni node pointer'ı

	existing = find_env_node(global->env_list, key); // 3. Variable zaten var mı?
	if (existing)                               // 4. Mevcut variable bulundu mu?
	{
		existing->value = ft_strdup(value);     // 5. Value'yu güncelle
	}
	else                                        // 6. Yeni variable
	{
		new_node = create_env_node(key, value); // 7. Yeni node oluştur
		if (new_node)                           // 8. Node oluşturma başarılı mı?
			add_env_node(&global->env_list, new_node); // 9. Listeye ekle
	}
}
```

### Kod Analizi - unset_env_var()

```c
void	unset_env_var(t_global *global, char *key)
{
	t_env	*current;                           // 1. Mevcut node pointer'ı
	t_env	*prev;                              // 2. Önceki node pointer'ı

	current = global->env_list;                 // 3. Liste başından başla
	prev = NULL;                                // 4. Önceki'yi null yap
	while (current)                             // 5. Liste sonuna kadar
	{
		if (ft_strcmp(current->key, key) == 0)  // 6. Silinecek key bulundu mu?
		{
			if (prev)                           // 7. Önceki node var mı?
				prev->next = current->next;     // 8. Önceki'yi sonraki'ye bağla
			else                                // 9. İlk node siliniyorsa
				global->env_list = current->next; // 10. Liste başını güncelle
			return ;                            // 11. İşlemi tamamla
		}
		prev = current;                         // 12. Previous'u güncelle
		current = current->next;                // 13. Sonraki node'a geç
	}
}
```

### Kod Analizi - print_export_env()

```c
void	print_export_env(t_env *env_list)
{
	t_env	*current;                           // 1. Traversal pointer'ı

	current = env_list;                         // 2. Liste başından başla
	while (current)                             // 3. Her node için
	{
		// 4. Value var mı ve boş değil mi?
		if (current->value && ft_strlen(current->value) >= 0)
			printf("declare -x %s=\"%s\"\n", current->key, current->value);
		else                                    // 5. Value yok veya boş
			printf("declare -x %s\n", current->key); // 6. Sadece key yazdır
		current = current->next;                // 7. Sonraki node'a geç
	}
}
```

**Environment Manipulation Akışı:**
1. **Set Variable**: Mevcut variable'ı günceller veya yeni oluşturur
2. **Unset Variable**: Variable'ı linked list'ten kaldırır
3. **Print Export**: Export formatında tüm variable'ları listeler

**Örnek Senaryolar:**
- `set_env_var(global, "MYVAR", "hello")` → MYVAR=hello ekler/günceller
- `unset_env_var(global, "OLDVAR")` → OLDVAR'ı siler
- `print_export_env(env_list)` → `declare -x PATH="/usr/bin"` formatında yazdırır

## 4. PATH_UTILS.C - PATH RESOLUTION VE KOMUT ARAMA

### Kod Analizi - find_command_path()

```c
char	*find_command_path(char *command, t_env *env_list)
{
	char	*path_env;                          // 1. PATH environment variable

	if (!command)                               // 2. Komut geçerli mi?
		return (NULL);                          // 3. Geçersizse null dön
		
	if (ft_strchr(command, '/'))                // 4. Komut absolute/relative path mi?
	{
		if (access(command, F_OK) == 0)         // 5. Dosya mevcut mu?
			return (ft_strdup(command));        // 6. Path'i olduğu gibi dön
		return (NULL);                          // 7. Dosya yok, null dön
	}
	
	path_env = get_env_value(env_list, "PATH"); // 8. PATH environment'ını al
	if (!path_env)                              // 9. PATH tanımlı değil mi?
		return (NULL);                          // 10. Null dön
	return (search_in_path_directories(path_env, command)); // 11. PATH'te ara
}
```

### Kod Analizi - search_in_path_directories()

```c
static char	*search_in_path_directories(char *path_env, char *command)
{
	char	**paths;                            // 1. PATH directory'leri array'i
	char	*full_path;                         // 2. Tam path string'i
	int		i;                                  // 3. Index counter

	paths = ft_split(path_env, ':');            // 4. PATH'i ':' ile böl
	if (!paths)                                 // 5. Split başarısız mı?
		return (NULL);                          // 6. Null dön
		
	i = 0;                                      // 7. Index'i sıfırla
	while (paths[i])                            // 8. Her directory için
	{
		full_path = build_full_path(paths[i], command); // 9. Tam path oluştur
		if (full_path && access(full_path, F_OK) == 0) // 10. Dosya mevcut mu?
		{
			return (full_path);                 // 11. Bulunan path'i dön
		}
		if (full_path)                          // 12. Path oluşturulmuş ama dosya yok
			i++;                                // 13. Sonraki directory'e geç
	}
	return (NULL);                              // 14. Hiçbir directory'de bulunamadı
}
```

### Kod Analizi - build_full_path()

```c
char	*build_full_path(char *dir, char *command)
{
	char	*temp;                              // 1. Geçici string
	char	*full_path;                         // 2. Tam path

	if (!dir || !command)                       // 3. Parametreler geçerli mi?
		return (NULL);                          // 4. Geçersizse null dön
		
	temp = ft_strjoin(dir, "/");                // 5. Directory'ye '/' ekle
	if (!temp)                                  // 6. Join başarısız mı?
		return (NULL);                          // 7. Null dön
		
	full_path = ft_strjoin(temp, command);      // 8. '/'den sonra command ekle
	return (full_path);                         // 9. Tam path'i dön
}
```

### Kod Analizi - get_env_value()

```c
char	*get_env_value(t_env *env_list, char *key)
{
	t_env	*current;                           // 1. Traversal pointer'ı

	current = env_list;                         // 2. Liste başından başla
	while (current)                             // 3. Her node için
	{
		if (ft_strcmp(current->key, key) == 0)  // 4. Key eşleşiyor mu?
			return (current->value);            // 5. Value'yu dön
		current = current->next;                // 6. Sonraki node'a geç
	}
	return (NULL);                              // 7. Bulunamadı, null dön
}
```

### Kod Analizi - env_list_to_array()

```c
char	**env_list_to_array(t_env *env_list)
{
	t_env	*current;                           // 1. Traversal pointer'ı
	char	**env_array;                        // 2. Environment array'i
	char	*temp;                              // 3. Geçici string
	int		count;                              // 4. Node sayısı
	int		i;                                  // 5. Index counter

	count = count_env_nodes(env_list);          // 6. Toplam node sayısını say
	env_array = halloc(sizeof(char *) * (count + 1)); // 7. Array memory'si ayır
	if (!env_array)                             // 8. Allocation başarısız mı?
		return (NULL);                          // 9. Null dön
		
	current = env_list;                         // 10. Liste başından başla
	i = 0;                                      // 11. Index'i sıfırla
	while (current && i < count)                // 12. Her node için
	{
		temp = ft_strjoin(current->key, "=");   // 13. "KEY=" oluştur
		if (temp)                               // 14. Join başarılı mı?
			env_array[i] = ft_strjoin(temp, current->value); // 15. "KEY=VALUE" oluştur
		else                                    // 16. Join başarısız
			env_array[i] = NULL;                // 17. Null ekle
		current = current->next;                // 18. Sonraki node'a geç
		i++;                                    // 19. Index'i artır
	}
	env_array[i] = NULL;                        // 20. Array'i null ile sonlandır
	return (env_array);                         // 21. Array'i dön
}
```

**Path Resolution Akışı:**
1. **Path Type Detection**: Absolute/relative vs command name ayrımı
2. **Direct Access**: Path içeriyorsa direkt dosya kontrolü
3. **PATH Search**: PATH variable'ındaki her directory'de arama
4. **Command Assembly**: Directory + "/" + command birleştirme
5. **Access Check**: Dosya mevcudiyeti kontrolü

**Örnek Senaryolar:**
- `find_command_path("ls", env_list)` → "/bin/ls"
- `find_command_path("/bin/cat", env_list)` → "/bin/cat" (direkt)
- `find_command_path("nonexistent", env_list)` → NULL
- `find_command_path("./script.sh", env_list)` → "./script.sh" (relative)

## KULLANIM SENARYOLARI

### 1. Program Başlatma
```c
// main() fonksiyonunda
char **envp = /* system environment */;
t_env *env_list = init_env_from_envp(envp);
// Environment hazır
```

### 2. Komut Arama
```bash
minishell$ ls -la
# find_command_path("ls", env_list) → "/bin/ls"
# execve("/bin/ls", args, env_array)
```

### 3. Environment Variable İşlemleri
```bash
minishell$ export MYVAR=hello
# set_env_var(global, "MYVAR", "hello")

minishell$ echo $MYVAR
# get_env_value(env_list, "MYVAR") → "hello"

minishell$ unset MYVAR
# unset_env_var(global, "MYVAR")
```

### 4. Built-in ile Environment
```bash
minishell$ env
# print_export_env() format olmadan listeler

minishell$ export
# print_export_env() export formatında listeler
```

## MEMORY YÖNETİMİ

### Allocation Strategy:
1. **Node Creation**: Her variable için ayrı node
2. **String Duplication**: Key/value string'leri kopyalanır
3. **Linked List**: Dynamic boyutlu liste yapısı
4. **Garbage Collection**: halloc() ile otomatik cleanup

### Memory Layout:
```
env_list → [NODE1] → [NODE2] → [NODE3] → NULL
           ↓         ↓         ↓
           key1      key2      key3
           value1    value2    value3
```

## HATA YÖNETİMİ

### Hata Durumları:
1. **Memory Allocation**: halloc() başarısızlığı
2. **Missing Environment**: System environment yok
3. **Invalid PATH**: PATH variable tanımsız
4. **Command Not Found**: PATH'te komut bulunamama
5. **Access Denied**: Dosya mevcut ama erişim yok

### Error Recovery:
- **Minimal Environment**: System environment yoksa minimal set
- **Graceful Degradation**: PATH yoksa relative/absolute path çalışır
- **NULL Returns**: Hata durumlarında NULL döner

## PERFORMANCE CONSIDERATIONS

### Time Complexity:
- **find_env_node()**: O(n) - linear search
- **set_env_var()**: O(n) - find + set
- **unset_env_var()**: O(n) - find + delete
- **find_command_path()**: O(m×n) - m directories, n access calls

### Memory Usage:
- **Space Efficient**: Sadece kullanılan variable'lar için memory
- **String Duplication**: Her string kopyalanır (safety)
- **No Hash Table**: Simple linked list (small environments için yeterli)

## SHELL INTEGRATION

### POSIX Compliance:
- **Environment Inheritance**: Parent process environment'ı inherit eder
- **Variable Export**: Export edilen variable'lar child'lara geçer
- **PATH Resolution**: Standard UNIX PATH semantics
- **Special Variables**: PWD, OLDPWD, SHLVL yönetimi

### Shell Features:
- **Dynamic Updates**: Runtime'da variable değiştirme
- **Export Tracking**: Hangi variable'ların export edildiği
- **Command Search**: Efficient command finding
- **Environment Passing**: execve() için array conversion

Bu modül, minishell'in UNIX environment semantiğini doğru şekilde implement eden kritik bir component'tir ve shell'in system integration'ı için temel infrastructure sağlar.
