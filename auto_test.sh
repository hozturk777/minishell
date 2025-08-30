#!/bin/bash

# Otomatik Minishell Test Script'i
echo "=== MİNİSHELL OTOMATİK TEST ==="
echo

# Test input dosyası oluştur
cat > minishell_test_input.txt << 'EOF'
pwd
echo "Hello World"
ls
export TEST_VAR=hello
echo $TEST_VAR
cd ..
pwd
cd minishell
env | grep TEST_VAR
exit
EOF

echo "📋 Temel komutları test ediyoruz..."
echo "Test input dosyası:"
cat minishell_test_input.txt
echo
echo "Minishell çıktısı:"
echo "==================="

# Minishell'i test input ile çalıştır
./minishell < minishell_test_input.txt

echo
echo "==================="
echo "✅ Test tamamlandı!"
echo

# Temizlik
rm -f minishell_test_input.txt

echo
echo "🧪 MANUEL SİNYAL TESTLERİ"
echo "Şimdi manuel olarak sinyal testlerini yapın:"
echo
echo "1. './minishell' komutunu çalıştırın"
echo "2. Aşağıdaki testleri yapın:"
echo
echo "   Test A - Ctrl+C Interactive:"
echo "   minishell$ [Ctrl+C tuşuna basın]"
echo "   Beklenen: Yeni prompt gösterilmeli"
echo
echo "   Test B - Ctrl+C Child Process:"
echo "   minishell$ sleep 5"
echo "   [2 saniye bekleyin, Ctrl+C'ye basın]"
echo "   Beklenen: sleep sonlanmalı, yeni prompt"
echo
echo "   Test C - Ctrl+D EOF:"
echo "   minishell$ [Ctrl+D tuşuna basın]"
echo "   Beklenen: 'exit' yazdırıp shell sonlanmalı"
echo
echo "   Test D - Exit Status:"
echo "   minishell$ sleep 10"
echo "   [Ctrl+C'ye basın]"
echo "   minishell$ echo \$?"
echo "   Beklenen: 130"
echo
