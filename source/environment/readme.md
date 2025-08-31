### **📂 Dosyalar:**
```
environment/
├── environment.c              [MEVCUT: source/environment.c]
├── env_utils.c                [MEVCUT: source/env_utils.c]
├── path_utils.c               [MEVCUT: executer/path_utils.c]
└── environment.h              [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **environment.c:**
- `t_env *init_env_from_envp(char **envp)`
- `t_env *create_env_node(char *key, char *value)`
- `void add_env_node(t_env **env_list, t_env *new_node)`
- `t_env *find_env_node(t_env *env_list, char *key)`
- `void free_env_list(t_env *env_list)`

#### **env_utils.c:**
- Environment utility fonksiyonları

#### **path_utils.c:**
- `char *find_command_path(char *command, t_env *env_list)`
- `char *build_full_path(char *dir, char *command)`
- `char *get_env_value(t_env *env_list, char *key)`
- `void free_string_array(char **array)`
- `char **env_list_to_array(t_env *env_list)`
- `int count_env_nodes(t_env *env_list)`

### **📋 Sorumluluklar:**
- Çevre değişkeni listesi yönetimi
- PATH çözümleme
- Environment array dönüşümleri
- Komut path bulma

---