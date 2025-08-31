### **📂 Dosyalar:**
```
expander/
├── variable_expansion.c       [MEVCUT: source/variable_expansion.c]
├── quote_expansion.c          [MEVCUT: source/quote_expansion.c]
├── env_expansion.c            [YENİ: Çevre değişkeni genişletme]
└── expander.h                 [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **variable_expansion.c:**
- `char *expand_variables(char *input, t_global *global)`
- `char *handle_dollar_expansion(char *input, int *i, t_global *global)`
- `char *handle_regular_char(char *input, int *i)`
- `char *join_and_free(char *s1, char *s2)`

#### **quote_expansion.c:**
- `char *expand_with_quotes(char *input, t_global *global)`
- `void update_quote_state(char c, int *quote_state)`
- `char *remove_outer_quotes(char *input)`
- `int needs_expansion(char *str)`
- `void expand_command_args(t_command *cmd, t_global *global)`
- `char *remove_quotes(char *str)`

#### **env_expansion.c:**
- `char *expand_env_var(char *var_name, t_global *global)`

### **📋 Sorumluluklar:**
- $VAR değişken genişletme
- Tırnak içindeki metinleri işleme
- Çevre değişkenlerini çözümleme
- Quote removal işlemleri

---