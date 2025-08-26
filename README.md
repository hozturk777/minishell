https://excalidraw.com/#room=d25f261f26c96d39c3f0,NgMwmAUaCjlhjK6nID2fzA


# 🐚 Minishell - Basit Shell Implementasyonu

**42 School Projesi** | **hasivaci** & **huozturk** tarafından oluşturuldu

C dilinde bash benzeri shell implementasyonu, 42 School kodlama standartları ve norm kurallarına uygun.

## 🎯 Mevcut Durum

### ✅ Tamamlanan Özellikler
- **Lexer**: Tamamen tamamlandı - tokenleştirme ve ayrıştırma
- **Parser**: Çalışıyor (token'lar → komutlar) - komut yapısı oluşturma  
- **Executor**: ✅ **TAMAMLANDI** - Komut çalıştırma motoru implementasyonu
- **Built-ins**: ✅ **TAMAMLANDI** - 7 built-in komut tamamen fonksiyonel
- **Environment**: Çevre değişkeni yönetimi
- **Redirections**: Giriş/Çıkış yönlendirme desteği
- **Pipelines**: Temel pipeline çalıştırma
- **Quote Processing**: ✅ **YENİ** - Tek ve çift tırnak işleme
- **Logical Path**: ✅ **YENİ** - cd .. davranışı silinen dizinler için
- **Heredoc**: ✅ **YENİ** - `<<` operatörü implementasyonu

### 🚧 Devam Eden / Sonraki Faz
- **Signal Handling**: SIGINT, SIGQUIT işleme
- **Değişken Genişletme**: `$VAR` genişletme iyileştirmeleri
- **External Commands**: PATH üzerinden harici komut çalıştırma
- **Pipeline Improvements**: Çoklu pipe zinciri iyileştirmeleri
- **Hata Yönetimi**: Geliştirilmiş hata mesajları

## 🏗️ Built-in Komutlar

| Komut | Durum | Açıklama |
|-------|--------|----------|
| `pwd` | ✅ | Mevcut çalışma dizinini yazdır (logical path desteği) |
| `echo` | ✅ | Metin göster (`-n` flag + quote processing) |
| `env` | ✅ | Çevre değişkenlerini göster |
| `export` | ✅ | Çevre değişkeni ayarla |
| `unset` | ✅ | Çevre değişkenini kaldır |
| `cd` | ✅ | Dizin değiştir (logical path + silinen parent handling) |
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
minishell$ echo 'Tek tırnak test'
minishell$ echo -n "Yeni satır yok"
minishell$ env
minishell$ export BENIM_VAR=deger
minishell$ echo $BENIM_VAR
minishell$ unset BENIM_VAR
minishell$ cd /tmp && pwd
minishell$ mkdir -p test/deep && cd test/deep && rm -rf ../deep && cd ..
minishell$ cat << EOF
> Bu bir heredoc test
> Birden fazla satır
> EOF
minishell$ exit 0
```

## 📁 Proje Yapısı

```
├── executer/           ✅ TAMAMLANDI - Çalıştırma motoru
│   ├── builtins.c     # Temel built-in'ler (pwd, echo, env, exit)
│   ├── builtins2.c    # Gelişmiş built-in'ler (cd, export, unset)
│   ├── executor.c     # Ana çalıştırma mantığı
│   ├── path_utils.c   # PATH çözümleme yardımcıları
│   └── redirections.c # G/Ç yönlendirme + heredoc
├── source/            # Temel shell mantığı
│   ├── main.c         # Giriş noktası (executor entegrasyonlu)
│   ├── lexer.c        # Tokenleştirme
│   ├── tokenizer.c    # Token işleme
│   └── token_handlers.c # Quote ve redirection handling
├── loop_dir/          
│   └── parser.c       # Token-komut ayrıştırıcı (quote aware)
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
- **G/Ç Yönlendirme**: Dosya giriş/çıkış işleme + heredoc
- **Quote Processing**: Tek/çift tırnak ile string işleme
- **Logical PWD**: Silinen dizinlerde bash-uyumlu cd davranışı

## 🧪 Test Örnekleri

```bash
# Temel komutlar
minishell$ pwd
/sgoinfre/huozturk/minishell_repo_huso

# Quote processing
minishell$ echo "hello world"
hello world
minishell$ echo 'single quotes'
single quotes

# Çevre manipülasyonu
minishell$ export TEST=merhaba
minishell$ echo $TEST
merhaba
minishell$ unset TEST

# Dizin navigasyonu (logical path)
minishell$ mkdir -p a/b/c && cd a/b/c
minishell$ rm -rf ../../b  # parent directory silme
minishell$ cd ..           # logical olarak a'ya geçer
minishell$ pwd
/sgoinfre/huozturk/minishell_repo_huso/a

# Heredoc
minishell$ cat << EOF
> İlk satır
> İkinci satır  
> EOF
İlk satır
İkinci satır

# Harici komutlar (PATH çözümlemesi gerektiğinde)
minishell$ ls -la
minishell$ cat dosya.txt
```

## 🎯 Sonraki Geliştirme Fazı

### Öncelik 1: External Commands
- [ ] PATH üzerinden komut arama ve çalıştırma
- [ ] execve() ile harici program çalıştırma
- [ ] Child process yönetimi

### Öncelik 2: Signal İşleme
- [ ] SIGINT (Ctrl+C) işleme
- [ ] SIGQUIT (Ctrl+\) işleme  
- [ ] Child process'lerde signal davranışı

### Öncelik 3: Gelişmiş Özellikler
- [ ] Pipeline zinciri iyileştirmeleri
- [ ] Değişken genişletme edge case'leri
- [ ] Wildcard desteği (`*`)
- [ ] Tab completion (bonus)

### Öncelik 4: Optimizasyon
- [ ] Performans iyileştirmeleri
- [ ] Bellek kullanımı optimizasyonu
- [ ] Hata mesajı geliştirmeleri
- [ ] Kod dokümantasyonu

## ✅ Bu Sprint'te Tamamlananlar

### Major Achievements
- **Quote Processing**: Tek ve çift tırnak tamamen çalışıyor
- **Logical PWD**: cd .. komutu silinen parent dizinlerde bash gibi davranıyor
- **Heredoc**: << operatörü implementasyonu tamamlandı
- **Built-ins Stability**: Tüm 7 built-in komut stabil çalışıyor

### Bug Fixes
- Parser quote token'larını tanıyordu (is_word_token düzeltildi)
- cd .. logical path resolution düzeltildi
- Echo quote removal implementasyonu eklendi
- Trailing slash handling cd komutunda

## 🔍 Geliştirme Notları

Proje temiz bir mimari izler:
1. **Girdi** → Lexer (tokenleştirme + quote handling)
2. **Token'lar** → Parser (komut yapısı + redirection parsing)  
3. **Komutlar** → Executor (built-in'ler + environment management)
4. **Çıktı** → Düzgün temizlikle sonuçlar

### Önemli Implementasyonlar
- **Quote Processing Pipeline**: Tokenizer → Parser → Echo removal
- **Logical PWD Management**: Environment tabanlı pwd tracking
- **Heredoc Temp Files**: Geçici dosya oluşturup cleanup
- **Modular Built-ins**: Her built-in ayrı fonksiyon olarak

Built-in komutlar, optimal performans ve shell durumu yönetimi için harici komutlardan ayrı olarak işlenir.

## 📊 Proje Metrikleri

- **Built-in Commands**: 7/7 ✅
- **Token Types**: 10+ farklı token türü
- **Quote Support**: Tek ve çift tırnak ✅
- **Redirection Types**: 4 tür (>, <, >>, <<) ✅
- **Code Quality**: 42 Norm uyumlu ✅