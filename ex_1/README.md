1.  Zapoznaj się z koncepcją plików Makefile: [https://www.gnu.org/software/make/manual/html_node/Introduction.html](https://www.gnu.org/software/make/manual/html_node/Introduction.html)
2.  Zainstaluj/skonfiguruj IDE, w którym będziesz pracować przez resztę laboratoriów. (np. VS Code, Vim, etc.)
3.  Stwórz nowy projekt w IDE.
4.  Napisz prosty program countdown.c, który będzie w pętli odliczał od 10 do 0 i wypisywał aktualną liczbę na konsolę (każda liczba w nowej linii).
5.  Stwórz plik Makefile, za pomocą którego skompilujesz swój program.  
    Makefile powinien zawierać co najmniej trzy targety: all, countdown, clean.  
      
    all — powinien budować wszystkie targety (na razie tylko countdown, w kolejnych zadaniach będziemy dodawać nowe targety).  
    countdown — buduje program countdown.c  
    clean — usuwa wszystkie pliki binarne, czyści stan projektu  
      
    Użyj zmiennych CC oraz CFLAGS do zdefiniowania kompilatora (gcc) i flag compilacji (-Wall, -std=c17, ...).  
    Dodatkowo w Makefile powinien być specjalny target .PHONY.
6.  Skompiluj i uruchom program.
7.  Korzystając z gdb (lub lldb) zademonstruj poniższe:
    
    1.  zatrzymywanie programu (breakpoint) wewnątrz pętli
    2.  podejrzenie aktualnego indeks pętli
    3.  kontynuację wykonywania programu
    4.  wypisanie kolejnego indeksu
    5.  usunięnie breakpoint-a
    6.  kontynuowanie działania programu do końca
    
      
    [https://sourceware.org/gdb/current/onlinedocs/gdb.html/](https://sourceware.org/gdb/current/onlinedocs/gdb.html/)  
    [https://sourceware.org/gdb/current/onlinedocs/gdb.html/Sample-Session.html#Sample-Session](https://sourceware.org/gdb/current/onlinedocs/gdb.html/Sample-Session.html#Sample-Session)  
    [https://lldb.llvm.org/use/tutorial.html](https://lldb.llvm.org/use/tutorial.html)
8.  Skonfiguruj swoje IDE do współpracy z Makefile. Postaw breakpoint (graficznie, klikając) w środku pętli. Uruchom program w trybie Debug i zademonstruj wszystkie podpunkty z pkt. 7.
