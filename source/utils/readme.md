### **📂 Dosyalar:**
```
utils/
├── debug.c                    [MEVCUT: source/debug.c]
├── debug_utils.c              [MEVCUT: source/debug_utils.c]
├── global_state.c             [MEVCUT: source/global_state.c]
├── string_utils.c             [YENİ: String işlemleri]
└── utils.h                    [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **debug.c:**
- `void debug_print(char *msg)`

#### **debug_utils.c:**
- `void print_tokens_advanced(t_list *tokens)`
- `void print_commands(t_list *commands)`
- `void print_commands_debug(t_command *commands)`

#### **global_state.c:**
- `t_global *init_global(char **envp)`
- `void free_global(t_global *global)`
- `void update_exit_status(t_global *global, int status)`

#### **string_utils.c:**
- `int is_special_char(char c)`
- String manipulation fonksiyonları

### **📋 Sorumluluklar:**
- Debug ve logging
- Global state yönetimi
- String utility fonksiyonları
- Memory management yardımcıları

---