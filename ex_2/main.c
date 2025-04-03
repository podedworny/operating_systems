#ifdef DYNAMIC
#include <dlfcn.h>
#else
#include "collatz.h"
#endif

#include <stdio.h>


int main(){
    int a, b, c, max_iter;
    printf("Podaj 3 liczby do testu: ");
    scanf("%d %d %d", &a, &b, &c);
    printf("Podaj maksymalna liczbe iteracji: ");
    scanf("%d", &max_iter);
    printf("Wyniki testu: \n");
    #ifdef DYNAMIC
    void *handle = dlopen("./libcollatz.so", RTLD_LAZY);
    if(!handle){
        printf("Nie udalo sie otworzyc biblioteki\n");
        return 1;
    }
    int (*test_collatz_convergence)(int, int) = dlsym(handle, "test_collatz_convergence");
    printf("%d %d %d\n", test_collatz_convergence(a, max_iter), test_collatz_convergence(b, max_iter), test_collatz_convergence(c, max_iter));
    dlclose(handle);
    #else
    printf("%d %d %d\n", test_collatz_convergence(a, max_iter), test_collatz_convergence(b, max_iter), test_collatz_convergence(c, max_iter));    
    #endif
    return 0;
}