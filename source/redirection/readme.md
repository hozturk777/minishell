### **📂 Dosyalar:**
```
redirection/
├── redirections.c             [MEVCUT: executer/redirections.c]
├── heredoc.c                  [YENİ: Here document işlemleri]
├── file_operations.c          [YENİ: Dosya işlemleri]
└── redirection.h              [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **redirections.c:**
- `void setup_redirections(t_command *cmd)`
- `void handle_single_redirection(t_redirect *redirect)`
- `void setup_pipeline_fds(t_command *cmd, int prev_fd, int *pipe_fd)`

#### **heredoc.c:**
- `int handle_heredoc(char *delimiter)`
- `char *generate_temp_filename(void)`

#### **file_operations.c:**
- Dosya açma/kapama/yazma işlemleri

### **📋 Sorumluluklar:**
- Input redirection (<)
- Output redirection (>)
- Append redirection (>>)
- Here document (<<)
- File descriptor yönetimi

---