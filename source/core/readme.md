### **📂 Dosyalar:**
```
core/
├── main.c                     [MEVCUT: source/main.c]
├── shell_loop.c               [YENİ: Shell döngüsü]
├── input_handler.c            [YENİ: Input işleme]
└── core.h                     [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **main.c:**
- `int main(int argc, char **argv, char **envp)`
- `static void print_welcome_advanced(void)`

#### **shell_loop.c:**
- `static void run_shell_loop(t_global *global)`

#### **input_handler.c:**
- `static int process_input(char *input, t_global *global)`

### **📋 Sorumluluklar:**
- Program başlangıcı
- Ana shell döngüsü
- Input okuma ve işleme
- Program sonlandırma

---