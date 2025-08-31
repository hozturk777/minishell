### **📂 Dosyalar:**
```
signals/
├── signal_handler.c           [MEVCUT: source/signal_handler.c]
├── signal_utils.c             [YENİ: Sinyal yardımcı fonksiyonlar]
└── signals.h                  [YENİ: Header dosyası]
```

### **🔧 Fonksiyonlar:**

#### **signal_handler.c:**
- `void setup_signals(void)`
- `void sigint_handler(int sig)`
- `void sigquit_handler(int sig)`
- `void restore_signals(void)`
- `void setup_child_signals(void)`
- `void handle_eof(void)`

#### **signal_utils.c:**
- Sinyal yardımcı fonksiyonları

### **📋 Sorumluluklar:**
- SIGINT (Ctrl+C) yönetimi
- SIGQUIT (Ctrl+\) yönetimi
- EOF (Ctrl+D) yönetimi
- Child process sinyal ayarları

---