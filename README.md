https://excalidraw.com/#room=d25f261f26c96d39c3f0,NgMwmAUaCjlhjK6nID2fzA


# 🐚 Minishell - Basit Shell Implementasyonu

**42 School Projesi** | **hasivaci** & **huozturk** tarafından oluşturuldu

C dilinde bash benzeri shell implementasyonu, 42 School kodlama standartları ve norm kurallarına uygun.

## 🎯 Mevcut Durum

### ✅ Tamamlanan Özellikler
- **Lexer**: Tamamen tamamlandı - tokenleştirme ve ayrıştırma
- **Parser**: Çalışıyor (token'lar → komutlar) - komut yapısı oluşturma  
- **Executor**: ✨ **YENİ** - Komut çalıştırma motoru implementasyonu
- **Built-ins**: ✨ **YENİ** - 7 built-in komut tamamen fonksiyonel
- **Environment**: Çevre değişkeni yönetimi
- **Redirections**: Giriş/Çıkış yönlendirme desteği
- **Pipelines**: Temel pipeline çalıştırma

### 🚧 Devam Eden / Sonraki Faz
- **Signal Handling**: SIGINT, SIGQUIT işleme
- **Heredoc**: `<<` implementasyonu  
- **Değişken Genişletme**: `$VAR` genişletme iyileştirmeleri
- **Quote İşleme**: Gelişmiş tırnak işleme
- **Hata Yönetimi**: Geliştirilmiş hata mesajları

## 🏗️ Built-in Komutlar

| Komut | Durum | Açıklama |
|-------|--------|----------|
| `pwd` | ✅ | Mevcut çalışma dizinini yazdır |
| `echo` | ✅ | Metin göster (`-n` flag desteği) |
| `env` | ✅ | Çevre değişkenlerini göster |
| `export` | ✅ | Çevre değişkeni ayarla |
| `unset` | ✅ | Çevre değişkenini kaldır |
| `cd` | ✅ | Dizin değiştir |
| `exit` | ✅ | Shell'den çık (durum koduyla) |

## 🔧 Derleme & Test

```bash
# Projeyi derle
make

# Minishell'i çalıştır
./minishell

# Built-in komutları test et
minishell$ pwd
minishell$ echo "Merhaba, Dünya!"
minishell$ echo -n "Yeni satır yok"
minishell$ env
minishell$ export BENIM_VAR=deger
minishell$ unset BENIM_VAR
minishell$ cd /tmp
minishell$ exit 0
```

## 📁 Proje Yapısı

```
├── executer/           ✨ YENİ - Çalıştırma motoru
│   ├── builtins.c     # Temel built-in'ler (pwd, echo, env, exit)
│   ├── builtins2.c    # Gelişmiş built-in'ler (cd, export, unset)
│   ├── executor.c     # Ana çalıştırma mantığı
│   ├── path_utils.c   # PATH çözümleme yardımcıları
│   └── redirections.c # G/Ç yönlendirme işleme
├── source/            # Temel shell mantığı
│   ├── main.c         # Giriş noktası (executor entegrasyonlu)
│   ├── lexer.c        # Tokenleştirme
│   ├── tokenizer.c    # Token işleme
│   └── ...
├── loop_dir/          
│   └── parser.c       # Token-komut ayrıştırıcı
├── lib/
│   └── minishell.h    # Tüm yapılar ve prototipleri
└── include/libft/     # Özel kütüphane fonksiyonları
```

## 🎯 Implementasyon Öne Çıkanları

### 42 Norm Uyumluluğu ✅
- Yasak yapılar yok: `for`, `do-while`, `switch`, `case`, `goto`
- Sadece izin verilen fonksiyonlar kullanıldı
- Düzgün bellek yönetimi
- 25 satır fonksiyon limiti uyumluluğu

### Mimari
- **Modüler Tasarım**: Her bileşen ayrı dosyalarda
- **Bellek Güvenliği**: Düzgün malloc/free yönetimi  
- **Hata İşleme**: `perror` ile kapsamlı hata kontrolü
- **Temiz Ayrım**: Lexer → Parser → Executor pipeline'ı

### Teknik Özellikler
- **Çevre Yönetimi**: Tam env değişken desteği
- **Path Çözümlemesi**: Akıllı komut path bulma
- **Pipeline Çalıştırma**: Çoklu komut pipeline desteği
- **G/Ç Yönlendirme**: Dosya giriş/çıkış işleme

## 🧪 Test Örnekleri

```bash
# Temel komutlar
minishell$ pwd
/sgoinfre/huozturk/minishell_repo_huso

# Çevre manipülasyonu
minishell$ export TEST=merhaba
minishell$ echo $TEST
merhaba
minishell$ unset TEST

# Dizin navigasyonu
minishell$ cd ..
minishell$ pwd
/sgoinfre/huozturk

# Harici komutlar (PATH çözümlemesi çalışırsa)
minishell$ ls -la
minishell$ cat dosya.txt
```

## 🎯 Sonraki Geliştirme Fazı

### Öncelik 1: Signal İşleme
- [ ] SIGINT (Ctrl+C) işleme
- [ ] SIGQUIT (Ctrl+\) işleme  
- [ ] Child process'lerde signal davranışı

### Öncelik 2: Gelişmiş Özellikler
- [ ] Heredoc (`<<`) implementasyonu
- [ ] Değişken genişletme iyileştirmeleri
- [ ] Quote işleme kenar durumları
- [ ] Wildcard desteği (`*`)

### Öncelik 3: Optimizasyon
- [ ] Performans iyileştirmeleri
- [ ] Bellek kullanımı optimizasyonu
- [ ] Hata mesajı geliştirmeleri
- [ ] Kod dokümantasyonu

## 🔍 Geliştirme Notları

Proje temiz bir mimari izler:
1. **Girdi** → Lexer (tokenleştirme)
2. **Token'lar** → Parser (komut yapısı)  
3. **Komutlar** → Executor (built-in'lerle çalıştırma)
4. **Çıktı** → Düzgün temizlikle sonuçlar

Built-in komutlar, optimal performans ve shell durumu yönetimi için harici komutlardan ayrı olarak işlenir.