#!/bin/bash


# ==============================================================================
#                      Minishell Değerlendirme Test Betiği
# ==============================================================================
# Bu betik, sağlanan değerlendirme kriterlerine göre minishell projesini
# test etmek için tasarlanmıştır.
# ==============================================================================


# Renk Kodları
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color


# Test sayaçları
PASSED=0
FAILED=0
TOTAL=0


# Test dosyaları için bir temizlik fonksiyonu
cleanup() {
   rm -f outfile infile errorfile test_dir/a_file
   rmdir test_dir
}


# Her testin başında ve sonunda temizlik yap
trap cleanup EXIT


# Testleri çalıştırmak için yardımcı fonksiyon
# Parametreler: 1:Test Adı, 2:Çalıştırılacak Komut, 3:Beklenen Çıktı (grep için), 4:Açıklama
run_test() {
   local test_name="$1"
   local command="$2"
   local expected="$3"
   local description="$4"
  
   TOTAL=$((TOTAL + 1))
   echo -e "\n${BLUE}--- Test $TOTAL: $test_name ---${NC}"
   echo -e "${YELLOW}Açıklama:${NC} $description"
   echo -e "${YELLOW}Komut:${NC} $command"
  
   # Komutu minishell içinde çalıştır ve çıktıyı al
   # 'exit' komutu ekleyerek minishell'in her testten sonra kapanmasını sağlıyoruz
   output=$(echo -e "$command\nexit" | timeout 5 ./minishell 2>&1)
  
   # minishell prompt'unu çıktıdan temizle
   output_clean=$(echo "$output" | grep -v "minishell>" | sed '/^exit$/d')


   if echo "$output_clean" | grep -qF -- "$expected"; then
       echo -e "${GREEN}✅ BAŞARILI${NC}"
       echo -e "Beklenen '$expected' dizesi çıktıda bulundu."
       PASSED=$((PASSED + 1))
   else
       echo -e "${RED}❌ BAŞARISIZ${NC}"
       echo -e "${RED}Beklenen:${NC} '$expected'"
       echo -e "${RED}Alınan:${NC}\n$output_clean"
       FAILED=$((FAILED + 1))
   fi
}


# Manuel testler için talimat fonksiyonu
manual_test() {
   echo -e "\n${BLUE}--- Manuel Test ---${NC}"
   echo -e "${YELLOW}Lütfen aşağıdaki adımları manuel olarak test edin:${NC}"
   echo -e "➡️  $1"
}


# --- Betik Başlangıcı ---


# Projeyi derle
echo -e "${BLUE}🔨 Minishell derleniyor...${NC}"
make re > /dev/null 2>&1
if [ ! -f "./minishell" ]; then
   echo -e "${RED}💥 Derleme başarısız! Betik durduruluyor.${NC}"
   exit 1
fi
echo -e "${GREEN}✅ Derleme başarılı!${NC}"


# 1. Mandatory Part
echo -e "\n\n${YELLOW}=============== 1. Derleme ve Temel Kontroller ===============${NC}"
manual_test "Makefile'ı kontrol et. 'make -n' komutu '-Wall -Wextra -Werror' flag'lerini gösteriyor mu?"
manual_test "Makefile'ın gereksiz yere yeniden link yapıp yapmadığını kontrol et ('make' komutunu tekrar çalıştır)."
manual_test "Projeyi yazan öğrenciye kaç tane global değişken kullandığını ve neden zorunlu olduğunu sor. Bu değişken sadece sinyal numarasını mı tutuyor?"


# 2. Simple Command
echo -e "\n\n${YELLOW}=============== 2. Basit Komutlar ===============${NC}"
run_test "Basit Komut" "/bin/ls" "minishell" "Argümansız basit bir komut çalıştırma (/bin/ls)."
run_test "Boş Komut" "" "" "Hiçbir şey yazmadan Enter'a basmak."
run_test "Sadece Boşluklar" "   " "" "Sadece boşluk veya tab karakterleri olan bir satır."


# 3. Arguments
echo -e "\n\n${YELLOW}=============== 3. Argümanlı Komutlar ===============${NC}"
run_test "Argümanlı Komut" "/bin/ls -l" "total" "Basit bir komutu argümanlarla çalıştırma."
run_test "Çoklu Argüman" "/bin/echo merhaba dünya test" "merhaba dünya test" "Birden fazla argümanlı komut."


# 4. echo
echo -e "\n\n${YELLOW}=============== 4. 'echo' Testleri ===============${NC}"
run_test "echo" "echo selam" "selam" "Temel echo komutu."
run_test "echo -n" "echo -n selam" "selam" "echo komutunun '-n' seçeneği."


# 5. exit
echo -e "\n\n${YELLOW}=============== 5. 'exit' Testleri ===============${NC}"
manual_test "'exit' komutunu çalıştırarak minishell'in düzgün kapanıp kapanmadığını kontrol et. Sonra minishell'i yeniden başlat."
manual_test "'exit 99' komutunu çalıştır, ardından 'echo $?' komutu ile bash'te çıkış kodunu kontrol et (99 olmalı)."


# 6. Return value ($?)
echo -e "\n\n${YELLOW}=============== 6. Dönüş Değeri ($?) Testleri ===============${NC}"
run_test "Başarılı Komut Dönüş Değeri" "ls > /dev/null\necho \$?" "0" "Başarılı bir komutun dönüş değeri 0 olmalı."
run_test "Başarısız Komut Dönüş Değeri" "ls non_existent_file\necho \$?" "1" "Başarısız bir komutun dönüş değeri 0'dan farklı olmalı. (Genellikle 1 veya 2)"


# 7. Signals
echo -e "\n\n${YELLOW}=============== 7. Sinyal (Signal) Testleri (MANUEL) ===============${NC}"
manual_test "Boş bir prompt'ta 'ctrl-C' yap. Yeni bir satırda yeni bir prompt gelmeli."
manual_test "Boş bir prompt'ta 'ctrl-\\' yap. Hiçbir şey olmamalı."
manual_test "Boş bir prompt'ta 'ctrl-D' yap. Minishell kapanmalı. (Yeniden başlat)"
manual_test "Prompt'a 'ls -l' yaz ama Enter'a basma. 'ctrl-C' yap. Yeni bir prompt gelmeli ve buffer temizlenmeli (Enter'a basınca 'ls -l' çalışmamalı)."
manual_test "'cat' gibi bloklayan bir komut çalıştırırken 'ctrl-C' yap. Komut sonlanmalı ve yeni prompt gelmeli."
manual_test "'cat' gibi bloklayan bir komut çalıştırırken 'ctrl-\\' yap. 'Quit (core dumped)' gibi bir mesajla komut sonlanmalı."


# 8. Double Quotes
echo -e "\n\n${YELLOW}=============== 8. Çift Tırnak (\") Testleri ===============${NC}"
run_test "Çift Tırnaklı Argüman" "echo \"selam dünya\"" "selam dünya" "Boşluk içeren çift tırnaklı argüman."
run_test "Çift Tırnak İçinde Pipe" "echo \"cat file.c | grep a\"" "cat file.c | grep a" "Çift tırnak içindeki özel karakterler yorumlanmamalı."
run_test "Çift Tırnak ve Değişken" "echo \"Kullanıcı: \$USER\"" "Kullanıcı: $(whoami)" "Çift tırnak içinde environment değişkeni genişletilmeli."


# 9. Single Quotes
echo -e "\n\n${YELLOW}=============== 9. Tek Tırnak (') Testleri ===============${NC}"
run_test "Tek Tırnaklı Argüman" "echo 'selam dünya'" "selam dünya" "Tek tırnaklı argüman."
run_test "Tek Tırnak ve Değişken" "echo '\$USER'" "\$USER" "Tek tırnak içinde hiçbir şey genişletilmemeli."
run_test "Tek Tırnak Boş Argüman" "echo ''" "" "Boş tek tırnaklı argüman."


# 10. env, export, unset
echo -e "\n\n${YELLOW}=============== 10. env, export, unset Testleri ===============${NC}"
run_test "env Komutu" "env" "PATH=" "'env' komutu ortam değişkenlerini listelemeli."
run_test "export Komutu" "export YENI_DEGISKEN=test\nenv" "YENI_DEGISKEN=test" "Yeni bir ortam değişkeni oluşturma."
run_test "unset Komutu" "export SILINECEK=sil\nunset SILINECEK\nenv" "SILINECEK=" "Bir ortam değişkenini silme (çıktıda olmamalı, grep tersi kontrol)."


# 11. cd, pwd
echo -e "\n\n${YELLOW}=============== 11. cd ve pwd Testleri ===============${NC}"
run_test "pwd Komutu" "pwd" "$(pwd)" "'pwd' mevcut dizini göstermeli."
run_test "cd Komutu" "mkdir test_dir\ncd test_dir\npwd" "test_dir" "'cd' ile dizin değiştirme."
run_test "cd .. Komutu" "cd .." "minishell_test" "cd .. ile bir üst dizine geçme."
run_test "cd Olmayan Dizin" "cd /nonexistent_directory" "No such file or directory" "Var olmayan bir dizine gitmeye çalışınca hata vermeli."


# 12. Relative and Absolute Path
echo -e "\n\n${YELLOW}=============== 12. Göreceli ve Mutlak Yol Testleri ===============${NC}"
run_test "Göreceli Yol ile Komut" "touch test_dir/a_file\nls test_dir" "a_file" "Göreceli yolla komut çalıştırma."
# Mutlak yol testi zaten "Basit Komut" testinde yapıldı (/bin/ls).


# 13. Environment path ($PATH)
echo -e "\n\n${YELLOW}=============== 13. $PATH Testleri ===============${NC}"
run_test "$PATH ile Komut" "ls" "minishell" "PATH değişkenini kullanarak komut bulma."
manual_test "'unset PATH' komutunu çalıştır. Ardından 'ls' gibi komutlar çalışmamalı. Sonra 'export PATH=/bin:/usr/bin' gibi bir komutla PATH'i geri getir."


# 14. Redirection
echo -e "\n\n${YELLOW}=============== 14. Yönlendirme (<, >, >>, <<) Testleri ===============${NC}"
run_test "Output Yönlendirme (>)" "echo selam > outfile\ncat outfile" "selam" "Standart çıktıyı dosyaya yazma."
run_test "Append Yönlendirme (>>)" "echo dünya >> outfile\ncat outfile" "selam\ndünya" "Dosyanın sonuna ekleme."
run_test "Input Yönlendirme (<)" "echo 'input test' > infile\ncat < infile" "input test" "Dosyadan standart girdi alma."
manual_test "Heredoc (<<) testini yap: 'cat << EOF' yazıp Enter'a bas. Birkaç satır yazdıktan sonra 'EOF' yazarak bitir."


# 15. Pipes
echo -e "\n\n${YELLOW}=============== 15. Pipe (|) Testleri ===============${NC}"
run_test "Basit Pipe" "echo 'merhaba dunya' | grep dunya" "merhaba dunya" "İki komutu birbirine bağlama."
run_test "Pipe ve Hatalı Komut" "ls non_existent_file | cat" "No such file or directory" "Pipe zincirindeki hatalı komutun hatası görünmeli."
run_test "Pipe ve Yönlendirme" "echo 'pipe ve yonlendirme' > infile\ncat < infile | grep pipe" "pipe ve yonlendirme" "Pipe ve yönlendirmeleri birlikte kullanma."


# 16. Go Crazy
echo -e "\n\n${YELLOW}=============== 16. Çılgın Testler ===============${NC}"
run_test "Bilinmeyen Komut" "asdfghjkl" "command not found" "Var olmayan bir komut hata vermeli ama çökmemeli."
manual_test "Yukarı/Aşağı ok tuşları ile komut geçmişinde gezinebiliyor musun?"
manual_test "'cat | cat | ls' gibi bir komut normal davranıyor mu? (İlk cat'e girdi beklemeli)"


# --- Test Sonu ---
echo -e "\n\n${YELLOW}=============== TEST SONUÇLARI ===============${NC}"
echo -e "${WHITE}Toplam Test: $TOTAL${NC}"
echo -e "${GREEN}✅ Başarılı: $PASSED${NC}"
echo -e "${RED}❌ Başarısız: $FAILED${NC}"


if [ $FAILED -eq 0 ]; then
   echo -e "\n${GREEN}🎉 Tüm otomatize testler başarılı! Manuel testleri de kontrol etmeyi unutma! 🎉${NC}"
else
   echo -e "\n${RED}💥 Bazı testler başarısız oldu. Hataları kontrol et. 💥${NC}"
fi


# Geçici dosyaları temizle
cleanup