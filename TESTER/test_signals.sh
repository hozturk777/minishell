#!/bin/bash

# Signal Testing Script for Minishell
# Test all signal behaviors (Ctrl+C, Ctrl+D, Ctrl+\)

echo "=== MINISHELL SIGNAL TESTING ==="
echo "Bu script minishell'in sinyal işlemlerini test eder."
echo
echo "Test edilecek sinyaller:"
echo "1. SIGINT (Ctrl+C) - Interactive modda yeni prompt"
echo "2. SIGQUIT (Ctrl+\) - Interactive modda ignore"
echo "3. EOF (Ctrl+D) - Shell'den çıkış"
echo "4. SIGINT child process'te - Process'i sonlandır"
echo
echo "NOT: Bu script'i çalıştırdıktan sonra manuel testler yapılacak."
echo

# Test dosyaları oluştur
echo "Test dosyaları oluşturuluyor..."
echo "Test file content" > signal_test.txt

echo
echo "🧪 Test 1: Temel Shell Başlatma"
echo "Minishell başlatılıyor..."
echo "./minishell komutunu çalıştırın ve şunları test edin:"
echo
echo "a) Normal komut çalıştırma:"
echo "   minishell$ ls"
echo "   minishell$ pwd"
echo "   minishell$ echo 'Hello World'"
echo
echo "b) SIGINT (Ctrl+C) Testi - Interactive Mode:"
echo "   minishell$ [Ctrl+C tuşlarına basın]"
echo "   Beklenen: Yeni prompt gösterilmeli, shell sonlanmamalı"
echo
echo "c) SIGQUIT (Ctrl+\\) Testi - Interactive Mode:"
echo "   minishell$ [Ctrl+\\ tuşlarına basın]"
echo "   Beklenen: Hiçbir şey olmamalı, shell devam etmeli"
echo
echo "d) EOF (Ctrl+D) Testi:"
echo "   minishell$ [Ctrl+D tuşlarına basın]"
echo "   Beklenen: Shell sonlanmalı, 'exit' mesajı gösterilmeli"
echo

echo "🧪 Test 2: Child Process Signal Handling"
echo "./minishell komutunu çalıştırın ve şunları test edin:"
echo
echo "a) Uzun çalışan komutta SIGINT:"
echo "   minishell$ sleep 10"
echo "   [2-3 saniye bekleyin, sonra Ctrl+C'ye basın]"
echo "   Beklenen: sleep sonlanmalı, yeni prompt gösterilmeli"
echo
echo "b) Cat komutu ile interaktif input:"
echo "   minishell$ cat"
echo "   [Birkaç satır yazın, sonra Ctrl+C'ye basın]"
echo "   Beklenen: cat sonlanmalı, yeni prompt gösterilmeli"
echo
echo "c) Cat komutu ile EOF:"
echo "   minishell$ cat"
echo "   [Birkaç satır yazın, sonra Ctrl+D'ye basın]"
echo "   Beklenen: cat sonlanmalı, yazdıklarınız ekrana basılmalı"
echo

echo "🧪 Test 3: Pipeline ile Signal Handling"
echo "./minishell komutunu çalıştırın ve şunları test edin:"
echo
echo "a) Pipeline'da SIGINT:"
echo "   minishell$ sleep 10 | cat"
echo "   [2-3 saniye bekleyin, sonra Ctrl+C'ye basın]"
echo "   Beklenen: Tüm pipeline sonlanmalı, yeni prompt gösterilmeli"
echo
echo "b) Uzun pipeline:"
echo "   minishell$ find / -name '*.txt' 2>/dev/null | head -20"
echo "   [Birkaç saniye bekleyin, sonra Ctrl+C'ye basın]"
echo "   Beklenen: Pipeline sonlanmalı, yeni prompt gösterilmeli"
echo

echo "🧪 Test 4: Built-in Commands ile Signal"
echo "./minishell komutunu çalıştırın ve şunları test edin:"
echo
echo "a) Export komutu sırasında SIGINT:"
echo "   minishell$ export TEST_VAR='Hello World'"
echo "   minishell$ [Ctrl+C'ye basın]"
echo "   Beklenen: Yeni prompt, TEST_VAR hala set olmalı"
echo
echo "b) CD komutu sırasında signal:"
echo "   minishell$ cd .."
echo "   minishell$ pwd"
echo "   minishell$ [Ctrl+C'ye basın]"
echo "   Beklenen: Yeni prompt, directory değişimi korunmalı"
echo

echo "🧪 Test 5: Redirection ile Signal"
echo "./minishell komutunu çalıştırın ve şunları test edin:"
echo
echo "a) Output redirection sırasında SIGINT:"
echo "   minishell$ sleep 5 > output.txt"
echo "   [2 saniye bekleyin, sonra Ctrl+C'ye basın]"
echo "   Beklenen: Sleep sonlanmalı, output.txt oluşturulmalı (muhtemelen boş)"
echo
echo "b) Heredoc sırasında SIGINT:"
echo "   minishell$ cat << EOF"
echo "   [Birkaç satır yazın, sonra Ctrl+C'ye basın]"
echo "   Beklenen: Heredoc sonlanmalı, yeni prompt gösterilmeli"
echo

echo "📋 Kontrol Listesi:"
echo "[ ] Ctrl+C: Interactive modda yeni prompt gösteriyor"
echo "[ ] Ctrl+C: Child process'i doğru sonlandırıyor"
echo "[ ] Ctrl+\\: Interactive modda ignore ediliyor"
echo "[ ] Ctrl+\\: Child process'te doğru davranıyor"
echo "[ ] Ctrl+D: Shell'i doğru sonlandırıyor"
echo "[ ] Ctrl+D: Cat gibi komutlarda doğru çalışıyor"
echo "[ ] Pipeline'larda sinyaller doğru çalışıyor"
echo "[ ] Built-in'lerde sinyaller doğru çalışıyor"
echo "[ ] Redirection'larda sinyaller doğru çalışıyor"
echo "[ ] Exit status'lar doğru ayarlanıyor (130 for SIGINT)"
echo

echo "✅ Test başlatmak için şunu çalıştırın:"
echo "   ./minishell"
echo
echo "💡 İpucu: Test sırasında exit status'u kontrol etmek için:"
echo "   minishell$ sleep 10"
echo "   [Ctrl+C'ye basın]"
echo "   minishell$ echo \$?"
echo "   Beklenen çıktı: 130"
echo

# Temizlik fonksiyonu
cleanup() {
    echo
    echo "Test dosyaları temizleniyor..."
    rm -f signal_test.txt output.txt
    echo "Temizlik tamamlandı."
}

trap cleanup EXIT
