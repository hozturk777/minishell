### **📂 Dosyalar:**
```
parser/
├── parser.c                   [MEVCUT: loop_dir/parser.c]
├── command_builder.c          [YENİ: Komut oluşturma]
├── redirection_parser.c       [YENİ: Yönlendirme parsing]
└── parser.h                   [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **parser.c:**
- `t_command *parse_tokens_to_commands(t_list *tokens, t_global *global)`
- `t_command *parse_single_command(t_list **token_node, t_global *global)`
- `int is_command_start(t_list *token_node)`
- `int is_pipe_token(t_list *token_node)`
- `int is_redirect_token(t_list *token_node)`
- `int is_word_token(t_list *token_node)`

#### **command_builder.c:**
- `t_command *create_command(void)`
- `void free_command(t_command *cmd)`
- `void free_commands(t_list **commands)`
- `void free_commands_list(t_command *commands)`
- `void collect_command_arg(t_list **args_list, t_list *token_node)`
- `char **convert_list_to_array(t_list *args_list)`
- `void append_command_to_chain(t_command *head, t_command *new_cmd)`

#### **redirection_parser.c:**
- `void parse_redirection(t_command *cmd, t_list **token_node, t_global *global)`
- `void add_redirect_to_command(t_command *cmd, t_redirect *redirect)`

### **📋 Sorumluluklar:**
- Token listelerini komut yapılarına çevirme
- Pipeline'ları parse etme  
- Yönlendirmeleri parse etme
- Komut argümanlarını organize etme

---