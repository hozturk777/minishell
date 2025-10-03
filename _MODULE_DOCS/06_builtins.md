# BUILTINS MODÜL DOKÜMANTASYONU

## GENEL BAKIŞ

Builtins modülü, minishell'de dahili komutları (built-in commands) uygular. Bu komutlar shell'in kendisi tarafından çalıştırılır ve ayrı bir process oluşturulmaz. Bash'te bulunan temel built-in komutları taklit eder.

## MODÜLÜNDEKİ DOSYALAR

1. **builtins.c** - Ana built-in fonksiyonları ve dispatch sistemi
2. **builtins2.c** - CD built-in'inin implementasyonu
3. **echo_builtin.c** - Echo komutunun implementasyonu
4. **builtin_export.c** - Export komutunun implementasyonu  
5. **builtins_utils.c** - Yardımcı fonksiyonlar (exit, env, quotes)
6. **path_built.c** - CD için path resolution fonksiyonları

## BUILT-IN KOMUTLAR LİSTESİ

```
echo    - Metni ekrana yazdır
cd      - Dizin değiştir
pwd     - Mevcut dizini göster
export  - Environment variable set et
unset   - Environment variable sil
env     - Environment variable'ları listele
exit    - Shell'den çık
```

## 1. BUILTINS.C - ANA BUILT-IN SİSTEMİ

### İşlev Akışı

```
execute_builtin()
├── setup_redirections()     # Redirection'ları uygula
├── execute_builtin_command() # Komuta göre dispatch
└── restore_file_descriptors() # Orijinal FD'leri restore et
```

### Kod Analizi - is_builtin()

```c
int	is_builtin(char *command)
{
	if (!command)                               // 1. Null pointer kontrolü
		return (0);                             // 2. Null ise built-in değil
	if (ft_strcmp(command, "pwd") == 0)         // 3. PWD komutu mu?
		return (1);                             // 4. Evet, built-in
	if (ft_strcmp(command, "echo") == 0)        // 5. ECHO komutu mu?
		return (1);                             // 6. Evet, built-in
	if (ft_strcmp(command, "env") == 0)         // 7. ENV komutu mu?
		return (1);                             // 8. Evet, built-in
	if (ft_strcmp(command, "exit") == 0)        // 9. EXIT komutu mu?
		return (1);                             // 10. Evet, built-in
	if (ft_strcmp(command, "cd") == 0)          // 11. CD komutu mu?
		return (1);                             // 12. Evet, built-in
	if (ft_strcmp(command, "export") == 0)      // 13. EXPORT komutu mu?
		return (1);                             // 14. Evet, built-in
	if (ft_strcmp(command, "unset") == 0)       // 15. UNSET komutu mu?
		return (1);                             // 16. Evet, built-in
	return (0);                                 // 17. Hiçbiri değil, external komut
}
```

**Akış Açıklaması:**
1. **Null Check**: Komut string'i null ise false döner
2. **String Comparison**: Her desteklenen built-in ile karşılaştırır
3. **Boolean Return**: Built-in ise 1, değilse 0 döner

**Örnek Senaryolar:**
- `echo hello` → `is_builtin("echo")` → 1 (true)
- `ls -la` → `is_builtin("ls")` → 0 (false)
- `cd /home` → `is_builtin("cd")` → 1 (true)

### Kod Analizi - execute_builtin()

```c
int	execute_builtin(t_command *cmd, t_global *global, int *originals)
{
    int	result;                                 // 1. Sonuç değişkeni

    if (!cmd || !cmd->args || !cmd->args[0])    // 2. Geçerlilik kontrolü
        return (1);                             // 3. Geçersizse hata dön

    if (cmd->redirections)                      // 4. Redirection var mı?
    {
        setup_redirections(cmd);                // 5. Redirection'ları uygula
    }
    
    result = execute_builtin_command(cmd, global); // 6. Uygun built-in fonksiyonu çağır
    
    // 7. File descriptor'ları eski haline getir
    restore_file_descriptors(cmd, originals[1], originals[0]);
    
    return (result);                            // 8. Sonucu dön
}
```

**Akış Açıklaması:**
1. **Validation**: Komut ve argüman varlığını kontrol eder
2. **Redirection Setup**: Varsa input/output redirection'ları uygular
3. **Command Dispatch**: Uygun built-in fonksiyonunu çağırır
4. **FD Restoration**: stdin/stdout'u eski haline getirir

### Kod Analizi - execute_builtin_command()

```c
static int	execute_builtin_command(t_command *cmd, t_global *global)
{
    if (ft_strcmp(cmd->args[0], "pwd") == 0)                    // 1. PWD komutu
        return (builtin_pwd_global(global));                    // 2. PWD çalıştır
    else if (ft_strcmp(cmd->args[0], "echo") == 0)              // 3. ECHO komutu
        return (builtin_echo(cmd->args));                       // 4. ECHO çalıştır
    else if (ft_strcmp(cmd->args[0], "env") == 0)               // 5. ENV komutu
        return (builtin_env(global->env_list));                 // 6. ENV çalıştır
    else if (ft_strcmp(cmd->args[0], "exit") == 0)              // 7. EXIT komutu
        return (builtin_exit(cmd->args));                       // 8. EXIT çalıştır
    else if (ft_strcmp(cmd->args[0], "cd") == 0)                // 9. CD komutu
        return (builtin_cd(cmd->args, global));                 // 10. CD çalıştır
    else if (ft_strcmp(cmd->args[0], "export") == 0)            // 11. EXPORT komutu
        return (builtin_export(cmd->args, global));             // 12. EXPORT çalıştır
    else if (ft_strcmp(cmd->args[0], "unset") == 0)             // 13. UNSET komutu
        return (builtin_unset(cmd->args, global));              // 14. UNSET çalıştır
    else                                                        // 15. Bilinmeyen komut
        return (1);                                             // 16. Hata dön
}
```

**Akış Açıklaması:**
1. **Command Identification**: String karşılaştırması ile komut türünü belirler
2. **Function Dispatch**: Her komut için özel fonksiyonu çağırır
3. **Return Propagation**: Alt fonksiyonun return değerini yukarı iletir

### Kod Analizi - builtin_pwd_global()

```c
int	builtin_pwd_global(t_global *global)
{
	t_command	*cmd;                           // 1. Komut pointer'ı
	char		*pwd_env;                       // 2. PWD environment variable
	char		*cwd;                           // 3. Current working directory

	cmd = global->commands;                     // 4. Mevcut komut listesi
	while (cmd)                                 // 5. Her komut için
	{
		// 6. PWD komutu ve argüman var mı kontrol et
		if (cmd->args && !ft_strcmp(cmd->args[0], "pwd")  && cmd->args[1])
		{
			// 7. Geçersiz option kontrolü (- ile başlayıp devam ediyorsa)
			if (cmd->args[1][0] && cmd->args[1][0] == '-' && cmd->args[1][1])
			{
				printf("minishell: pwd: -%c: invalid option\n", cmd->args[1][1]);
				return (2);                     // 8. Geçersiz option hatası
			}
		}
		cmd = cmd->next;                        // 9. Sonraki komuta geç
	}
	
	pwd_env = get_env_value(global->env_list, "PWD"); // 10. PWD environment'ı al
	if (pwd_env)                                // 11. PWD environment var mı?
	{
		printf("%s\n", pwd_env);                // 12. PWD'yi yazdır
		return (0);                             // 13. Başarı dön
	}
	
	cwd = getcwd(NULL, 0);                      // 14. Sistem'den current directory al
	if (!cwd)                                   // 15. Sistem hatası kontrolü
	{
		printf("pwd: error retrieving current directory: No such file or directory\n");
		return (1);                             // 16. Hata dön
	}
	printf("%s\n", cwd);                        // 17. Current directory'yi yazdır
	return (0);                                 // 18. Başarı dön
}
```

**Akış Açıklaması:**
1. **Option Validation**: Geçersiz option'ları (-x gibi) yakalar
2. **Environment Check**: PWD environment variable'ını kontrol eder
3. **System Fallback**: PWD yoksa getcwd() ile sistem'den alır
4. **Error Handling**: Directory retrieve edilemezse hata mesajı verir

**Örnek Senaryolar:**
- `pwd` → `/home/user` (PWD environment'tan)
- `pwd -x` → Invalid option error
- PWD unset → getcwd() ile sistem'den al

## 2. ECHO_BUILTIN.C - ECHO KOMUTU

### Kod Analizi - builtin_echo()

```c
int	builtin_echo(char **args)
{
	int		i, j, b, next_g;                    // 1. Index değişkenleri
	int		newline, n_flag, enable_escape;     // 2. Flag değişkenleri
	char	*processed_arg;                     // 3. İşlenmiş argüman

	// 4. Değişkenleri başlat
	i = 1; j = 1; b = 0; n_flag = 0; next_g = 0;
	newline = 1; enable_escape = 0;
	
	// 5. -n flag kontrolü (newline suppress)
	if (args[1] && args[1][next_g] == '-')      // 6. İlk arg '-' ile mi başlıyor?
	{
		next_g++;                               // 7. '-' karakterini atla
		while (args[1][next_g])                 // 8. Geri kalan karakterleri kontrol et
		{
			if (args[1][next_g] == 'n')         // 9. 'n' karakteri mi?
				n_flag = 1;                     // 10. n_flag'i set et
			else                                // 11. Başka karakter varsa
			{
				n_flag = 0;                     // 12. n_flag'i sıfırla
				break;                          // 13. Loop'u kır
			}
			next_g++;                           // 14. Sonraki karaktere geç
		}

		if (n_flag)                             // 15. n_flag set edildi mi?
		{
			newline = 0;                        // 16. Newline'ı disable et
			i++;                                // 17. Argüman index'ini artır
			// 18. Whitespace handling (complex logic)
			if (args[i][0] != ' ')
			{
				newline = 1;
				i--;
			}
			while (!ft_strcmp(args[i], " ") || !ft_strcmp(args[i], "\t"))
			{
				i++;
			}
		}
	}
	
	// 19. Escape sequence kontrolü - tüm argümanlarda '\' var mı bak
	j = i;
	while (args[j])                             // 20. Her argüman için
	{
        b = 0;
		while (args[j][b])                      // 21. Argümanın her karakteri için
		{
			if (args[j][b] == '\\')             // 22. Backslash buldu mu?
				enable_escape = 1;              // 23. Escape processing'i enable et
			b++;                                // 24. Sonraki karaktere geç
		}
		j++;                                    // 25. Sonraki argümana geç
	}
	
	// 26. Argümanları yazdır
	j = i;
	while (args[j])                             // 27. Her argüman için
	{
		processed_arg = args[j];                // 28. Mevcut argümanı al
		if (processed_arg)                      // 29. Null değil mi?
		{
			if (enable_escape)                  // 30. Escape processing gerekli mi?
			{
				process_escape_sequences(processed_arg); // 31. Escape'leri işle
				enable_escape = 0;              // 32. Flag'i sıfırla
			}
			printf("%s", processed_arg);        // 33. Argümanı yazdır
		}
		j++;                                    // 34. Sonraki argümana geç
	}
	
	if (newline)                                // 35. Newline gerekli mi?
		printf("\n");                           // 36. Newline yazdır
	return (0);                                 // 37. Başarı dön
}
```

### Kod Analizi - process_escape_sequences()

```c
static void	process_escape_sequences(char *str)
{
	int	i, j;                                   // 1. Index değişkenleri

	i = 0; j = 0;                               // 2. Index'leri sıfırla
	while (str[i])                              // 3. String'in sonuna kadar
	{
		if (str[i] == '\\' && str[i + 1])       // 4. Backslash ve sonraki karakter var mı?
		{
			if (str[i + 1] == '\\')             // 5. Double backslash (\\)
			{
				str[i] = '\\';                  // 6. Single backslash yap
				str[j] = str[i];                // 7. Sonuç string'e koy
			}
			else                                // 8. Diğer escape'ler (\n, \t vs)
			{
				str[j] = str[i+1];              // 9. Escape edilmiş karakteri al
			}
			i++;                                // 10. Extra increment (2 karakter işlendi)
		}		
		else                                    // 11. Normal karakter
			str[j] = str[i];                    // 12. Olduğu gibi kopyala
		j++;                                    // 13. Output index'i artır
		i++;                                    // 14. Input index'i artır
	}
	str[j] = '\0';                              // 15. String'i sonlandır
}
```

**Echo Akış Açıklaması:**
1. **Flag Processing**: `-n` flag'ini kontrol eder (newline suppress)
2. **Escape Detection**: Tüm argümanlarda backslash arar
3. **Escape Processing**: Bulursa escape sequence'leri işler
4. **Output**: Argümanları sırayla yazdırır
5. **Newline**: `-n` yoksa sonuna newline ekler

**Örnek Senaryolar:**
- `echo hello world` → "hello world\n"
- `echo -n hello` → "hello" (no newline)
- `echo "hello\\nworld"` → "hello\nworld\n"

## 3. BUILTIN_EXPORT.C - EXPORT KOMUTU

### Kod Analizi - builtin_export()

```c
int	builtin_export(char **args, t_global *global)
{
	int i;                                      // 1. Index değişkeni
	char *equal_pos;                            // 2. '=' karakterinin pozisyonu

	if (!args[1])                               // 3. Argüman yok mu?
	{
		print_export_env(global->env_list);     // 4. Tüm export'ları listele
		return (0);                             // 5. Başarı dön
	}
	
	i = 1;                                      // 6. İlk argümandan başla
	while (args[i])                             // 7. Her argüman için
	{
		equal_pos = ft_strchr(args[i], '=');    // 8. '=' karakterini bul
		
		// 9. Geçerli key karakterleri kontrolü
        if (!is_valid_key_char(args[i], equal_pos))
        {
            printf("export: `%s': not a valid identifier\n", args[i]);
            return (2);                         // 10. Geçersiz identifier hatası
        }
        
		// 11. Argümanı işle ve environment'a ekle
		i = process_export_argument(args, i, global);
	}
	return (0);                                 // 12. Başarı dön
}
```

### Kod Analizi - handle_single_assignment()

```c
static void	handle_single_assignment(char *arg, t_global *global)
{
	char	*equal_sign;                        // 1. '=' pozisyonu
	char	*key;                               // 2. Key kısmı
	char	*value;                             // 3. Value kısmı

	equal_sign = ft_strchr(arg, '=');           // 4. '=' karakterini bul
	if (equal_sign)                             // 5. '=' var mı? (KEY=VALUE)
	{
		*equal_sign = '\0';                     // 6. String'i böl
		key = arg;                              // 7. Key kısmını al
		value = equal_sign + 1;                 // 8. Value kısmını al
		set_env_var(global, key, value);        // 9. Environment'a KEY=VALUE ekle
		*equal_sign = '=';                      // 10. String'i eski haline getir
	}
	else                                        // 11. '=' yok (sadece KEY)
	{
		key = arg;                              // 12. Tüm string key
		set_env_var(global, key, NULL);         // 13. Environment'a KEY (no value) ekle
	}
}
```

**Export Akış Açıklaması:**
1. **No Args**: Argüman yoksa tüm export'ları listeler
2. **Validation**: Key karakterlerinin geçerliliğini kontrol eder
3. **Assignment Detection**: KEY=VALUE mi yoksa sadece KEY mi belirler
4. **Environment Update**: Environment listesini günceller

**Örnek Senaryolar:**
- `export` → Tüm export'ları listele
- `export PATH=/usr/bin` → PATH'i set et
- `export MYVAR` → MYVAR'ı export et (value yok)
- `export -invalid` → Invalid identifier error

## 4. BUILTINS2.C - CD KOMUTU

### Kod Analizi - builtin_cd()

```c
int	builtin_cd(char **args, t_global *global)
{
    char	*path;                              // 1. Hedef path
    char	*old_pwd;                           // 2. Eski PWD
    char    *cwd;                               // 3. Current working directory

    old_pwd = get_env_value(global->env_list, "PWD"); // 4. Mevcut PWD'yi al
    path = determine_cd_path(args, global);     // 5. Hedef path'i belirle
    
    if (!path)                                  // 6. Path belirlenemedi mi?
        return (1);                             // 7. Hata dön
        
    if (args && args[1] && args[2])             // 8. Çok fazla argüman var mı?
    {
        printf("minishell: cd: too many arguments\n");
        return (1);                             // 9. Hata dön
    }
    
    if (chdir(path) != 0)                       // 10. Dizin değiştirme başarısız mı?
        return (handle_chdir_failure(path, old_pwd, global)); // 11. Hata işle
        
    cwd = getcwd(NULL, 0);                      // 12. Yeni current directory'yi al
    if (!cwd)                                   // 13. getcwd başarısız mı?
    {
        printf("cd: error retrieving current directory: getcwd: cannot access parent directories: No such file or directory\n");
        return (1);                             // 14. Hata dön
    }
    
	update_pwd_variables(old_pwd, path, global); // 15. PWD variable'ları güncelle
    set_env_var(global, "OLDPWD", old_pwd);     // 16. OLDPWD'yi set et
    set_env_var(global, "PWD", cwd);            // 17. PWD'yi set et
    return (0);                                 // 18. Başarı dön
}
```

### Kod Analizi - determine_cd_path()

```c
static char	*determine_cd_path(char **args, t_global *global)
{
    char	*home;                              // 1. HOME environment variable

    if (!args[1])                               // 2. Argüman yok mu? (sadece 'cd')
    {
        home = get_env_value(global->env_list, "HOME"); // 3. HOME'u al
        if (!home)                              // 4. HOME set edilmemiş mi?
        {
            printf("minishell: cd: HOME not set\n");
            return (NULL);                      // 5. Hata dön
        }
        return (home);                          // 6. HOME'u dön
    }
    else                                        // 7. Argüman var
        return (args[1]);                       // 8. Verilen path'i dön
}
```

**CD Akış Açıklaması:**
1. **Path Determination**: Argüman yoksa HOME, varsa verilen path kullanır
2. **Argument Validation**: Çok fazla argüman kontrolü yapar
3. **Directory Change**: chdir() ile sistem dizin değişimi yapar
4. **Environment Update**: PWD ve OLDPWD environment'larını günceller
5. **Error Handling**: Başarısızlık durumlarını işler

**Örnek Senaryolar:**
- `cd` → HOME directory'ye git
- `cd /usr/bin` → /usr/bin'e git
- `cd nonexistent` → No such file or directory
- `cd arg1 arg2` → Too many arguments

## 5. BUILTINS_UTILS.C - YARDIMCI FONKSİYONLAR

### Kod Analizi - builtin_exit()

```c
int	builtin_exit(char **args)
{
	int	exit_code;                              // 1. Çıkış kodu
	int i;                                      // 2. Index değişkeni

	exit_code = 0;                              // 3. Default exit code
	printf("exit\n");                           // 4. Exit mesajı yazdır
	i = 0;                                      // 5. Index'i sıfırla
	
	if (args[1])                                // 6. Exit code argümanı var mı?
	{
		while(args[1][i])                       // 7. Her karakter için
		{
			while (args[1][i] == '+' || args[1][i] == '-') // 8. Sign karakterlerini atla
				i++;
			if (!ft_isdigit(args[1][i]))        // 9. Digit olmayan karakter var mı?
			{
				printf("minishell: exit: %s: numeric argument required\n", args[1]);
				cleanup_and_exit();             // 10. Cleanup yap
				exit(2);                        // 11. Numeric error ile çık
			}
			i++;                                // 12. Sonraki karaktere geç
		}
		
		exit_code = ft_atoi(args[1]);           // 13. String'i integer'a çevir
		if (args[2])                            // 14. Çok fazla argüman var mı?
		{
			printf("minishell: exit: too many arguments\n");
			return (0);                         // 15. Hata mesajı ver ama çıkma
		}
	}
	
	cleanup_and_exit();                         // 16. Cleanup yap
	exit(exit_code);                            // 17. Belirtilen kod ile çık
}
```

### Kod Analizi - builtin_env()

```c
int	builtin_env(t_env *env_list)
{
	t_env		*current;                       // 1. Mevcut environment node'u
	t_global	*global;                        // 2. Global state

	current = env_list;                         // 3. Environment listesinin başından başla
	global = get_global();                      // 4. Global state'i al
	
	// 5. Geçersiz option kontrolü
	if (global->commands->args[1] && global->commands->args[1][0] == '-')
	{
		printf("env: invalid option -- '%c'\n", global->commands->args[1][1]);
		return (125);                           // 6. Invalid option error
	}
	// 7. File argümanı kontrolü (env komutunda desteklenmiyor)
	else if (global->commands->args[1])
	{
		printf("env: '%s': No such file or directory\n", global->commands->args[1]);	
		return (127);                           // 8. File not found error
	}
	
	// 9. Tüm environment variable'ları yazdır
	while (current)                             // 10. Her environment için
	{
		if (current->value)                     // 11. Value var mı?
			printf("%s=%s\n", current->key, current->value); // 12. KEY=VALUE formatında yazdır
		current = current->next;                // 13. Sonraki node'a geç
	}
	return (0);                                 // 14. Başarı dön
}
```

**Akış Açıklaması:**
1. **Exit Validation**: Numeric argüman kontrolü yapar
2. **Environment Listing**: Tüm environment variable'ları listeler
3. **Error Handling**: Geçersiz durumları yakalar

## 6. PATH_BUILT.C - PATH RESOLUTION

Bu dosya CD komutu için path resolution fonksiyonları içerir:

### resolve_logical_path()
- Relative path'leri absolute path'e çevirir
- `.` ve `..` directory'lerini işler
- Path normalization yapar

### find_existing_parent()
- Silinmiş directory'ler için parent bulur
- Access kontrolü yapar
- Fallback path sağlar

## KULLANIM SENARYOLARI

### 1. Echo Komutu
```bash
minishell$ echo hello world
hello world

minishell$ echo -n no newline
no newline%

minishell$ echo "hello\nworld"
hello
world
```

### 2. CD Komutu
```bash
minishell$ cd /usr/bin
minishell$ pwd
/usr/bin

minishell$ cd
minishell$ pwd
/home/user

minishell$ cd nonexistent
minishell: cd: nonexistent: No such file or directory
```

### 3. Export/Env Komutları
```bash
minishell$ export MYVAR=hello
minishell$ env | grep MYVAR
MYVAR=hello

minishell$ export NEWVAR
minishell$ env | grep NEWVAR
NEWVAR=
```

### 4. Exit Komutu
```bash
minishell$ exit 42
exit
# Process exits with code 42

minishell$ exit abc
minishell: exit: abc: numeric argument required
exit
# Process exits with code 2
```

## HATA YÖNETİMİ

### Exit Kodları:
- **0**: Başarı
- **1**: Genel hata
- **2**: Geçersiz argüman (exit, pwd, export)
- **125**: Geçersiz option (env)
- **127**: File not found (env)

### Hata Durumları:
1. **Invalid Options**: Desteklenmeyen flag'ler
2. **Missing Arguments**: Gerekli argümanlar eksik
3. **Too Many Arguments**: Fazla argüman
4. **Invalid Identifiers**: Export için geçersiz variable isimleri
5. **Directory Errors**: CD için erişim problemleri

## FİLE DESCRIPTOR YÖNETİMİ

Built-in komutlar redirection ile kullanılabilir:

```bash
echo hello > output.txt    # Built-in + output redirection
cat < input.txt            # Built-in + input redirection
export VAR=value 2>err.txt # Built-in + error redirection
```

### FD Restoration:
- `restore_file_descriptors()` ile orijinal FD'ler geri yüklenir
- Built-in'lar main process'te çalıştığı için FD management kritik
- Pipeline'da bile built-in'lar doğru FD kullanır

## ENVIRONMENT MANAGEMENT

### Environment Operations:
1. **get_env_value()**: Environment variable değerini al
2. **set_env_var()**: Environment variable set et
3. **print_export_env()**: Export formatında listele

### PWD Management:
- CD komutu PWD ve OLDPWD'yi otomatik günceller
- getcwd() fallback'i ile sistem synchronization
- Logical path resolution ile symbolic link'ler işlenir

Bu modül, minishell'in interactive shell davranışını sağlayan core component'tir ve POSIX shell standardlarına uygun built-in komut implementasyonu sunar.
