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