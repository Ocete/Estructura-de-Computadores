//  segÃºn la versiÃ³n de gcc y opciones de optimizaciÃ³n usadas,
//tal vez haga falta
//  usar gcc â€“fno-omit-frame-pointer si gcc quitara el marco pila (%ebp)

#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

#define SIZE (1<<20)	// tamaÃ±o suficiente para tiempo apreciable
int lista[SIZE];
int resultado = 0;

int parity1(unsigned* array, int len) {
  unsigned x;
  int i, j, result = 0, tope = 8*sizeof(int), val;
  for (i = 0; i < len; ++i) {
    x = array[i];
    val = 0;
    for (j = 0; j < tope; ++j) {
      val ^= (x & 0x1);
      x >>= 1;
    }
    result += val;
  }
  return result;
}

int parity2(unsigned* array, int len) {
  unsigned x;
  int i, result = 0, val;
  for (i = 0; i < len; ++i) {
    x = array[i];
    val = 0;
    while (x) {
      val ^= (x & 0x1);
      x >>= 1;
    }
    result += val;
  }
  return result;
}

int parity3(unsigned* array, int len) {
  unsigned x;
  int i, result = 0, val;
  for (i = 0; i < len; ++i) {
    x = array[i];
    val = 0;
    while (x) {
      val ^= x;
      x >>= 1;
    }
    result += (val & 0x1);
  }
  return result;
}

int parity4(int* array, int len) {
  unsigned x;
  int i, j, result = 0, val;

  for (i = 0; i < len; ++i) {
    x = array[i];
    val = 0;
    asm("salto:               \n\t"
          "xor %[x], %[v]     \n\t"
          "shr %[x]           \n\t"
          "jne salto          \n\t"
        : [v] "+r"  (val)
        : [x]  "r"  (x)
    );
    result += (val & 0x1);
  }
  return result;
}

int parity5(unsigned* array, int len) {
  unsigned x;
  int i, j, result = 0, val;
  for (i = 0; i < len; ++i) {
    x = array[i];
    val = 0;
    for (j = 16; j != 1 ; j /= 2) {
      val ^= x;
      x >>= j;
    }
    result += (val & 0x1);
  }
  return result;
}

int parity6(unsigned* array, int len) {
  unsigned x;
  int i, j, result = 0;
  for (i = 0; i < len; ++i) {
    x = array[i];
    for (j = 16; j != 1 ; j /= 2) {
      asm(
        "mov    %[x],   %%edx     \n\t"
        "shr    $16,    %%edx     \n\t"
        "xor    %[x],   %%edx     \n\t"
        "xor    %%dh,   %%dl      \n\t"
        "setpo  %%dl              \n\t"
        "movzx  %%dl,   %[x]      \n\t"
        : [x] "+r"  (x)
        :
        : "edx"
      );
    }
    result += x;
  }
  return result;
}

int popcount7 (unsigned* array, int len) {
  int i, val, result = 0;
  unsigned x1, x2;

  if (len & 0x1)
    printf("leer 64b y len impar?\n");

  for (i = 0; i < len; i += 2) {
    x1 = array[i];
    x2 = array[i+1];
    asm("popcnt %[x1], %[val] \n\t"
        "popcnt %[x2], %%edi  \n\t"
        "add    %%edi, %[val] \n\t"
      : [val] "=&r" (val)
      : [x1]  "r"   (x1),
        [x2]  "r"   (x2)
      : "edi");
    result += val;
  }
  return result;
}

void crono(int (*func)(), char* msg){
    struct timeval tv1,tv2;	// gettimeofday() secs-usecs
    long           tv_usecs;	// y sus cuentas

    gettimeofday(&tv1,NULL);
    resultado = func(lista, SIZE);
    gettimeofday(&tv2,NULL);

    tv_usecs=(tv2.tv_sec -tv1.tv_sec )*1E6+
             (tv2.tv_usec-tv1.tv_usec);
    printf("resultado = %d\t", resultado);
    printf("%s:%9ld us\n", msg, tv_usecs);
    //Para obtener los datos de formas más sencilla basta con ejecutar
    //el código con la siguiente línea y sin las dos anteriores
    //de la forma ./ej1 >> popcount.txt
    //printf("%9ld\t", tv_usecs);
}

int main() {
    int i;			// inicializar array
    for (i = 0; i < SIZE; i++)	// se queda en cache
	     lista[i] = i;

    crono(parity1, "parity1, con for\t\t");
    crono(parity2, "parity2, con while\t\t");
    crono(parity3, "parity3, con while eficiente\t");
    crono(parity4, "parity4, con asm\t\t");
    crono(parity5, "parity5, suma en árbol\t\t");
    crono(parity6, "parity6, suma en árbol con asm\t");
    printf("\n");

    exit(0);
}
