//  segÃºn la versiÃ³n de gcc y opciones de optimizaciÃ³n usadas, tal vez haga falta
//  usar gcc â€“fno-omit-frame-pointer si gcc quitara el marco pila (%ebp)

#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

#define SIZE (1<<20)	// tamaÃ±o suficiente para tiempo apreciable
int lista[SIZE];
int resultado=0;

int popcount1(unsigned* array, int len) {
  unsigned x;
  int i, j, result = 0, tope = 8*sizeof(int);
  for (i = 0; i<len; ++i) {
    x = array[i];
    for (j = 0; j < tope; ++j) {
      result += x & 0x1;
      x >>= 1;
    }
  }
  return result;
}

int popcount2(unsigned* array, int len) {
  unsigned x;
  int i, result = 0;
  for (i=0; i<len; ++i) {
    x = array[i];
    do {
      result += x & 0x1;
      x >>= 1;
    } while (x);
  }
  return result;
}

int popcount3(unsigned* array, int len) {
  unsigned x;
  int i, result = 0;

  for (i = 0; i<len; ++i) {
    x = array[i];
    asm("\n"
      "salto : \n"
        "shr %[x] \n\t"
        "adc $0, %[r] \n\t"
        "cmp $0, %[x] \n\t"
        "jne salto \n\t"
      : [r] "+r" (result)
      : [x] "r" (x)
    );
  }
  return result;
}


int popcount4(unsigned* array, int len) {
  unsigned x;
  int i, j, result = 0, val;

  for (i = 0; i<len; ++i) {
    x = array[i];
    val = 0;

    for (j = 0; j < 8; ++j) {
      val += x & 0x01010101;
      x >>= 1;
    }

    val += (val >> 16);
    val += (val >> 8);
    result += val & 0xFF;
  }
  return result;
}


int popcount5 (unsigned* array, int len) {
  int i, val, result = 0;
  int SSE_mask[] = {0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f};
  int SSE_LUTb[] = {0x02010100, 0x03020201, 0x03020201, 0x04030302};
                // 3 2 1 0      7 6 5 4     11 10 9 8   15 14 13 12
  if (len & 0x3)
    printf ("leyendo 128b pero len no múltiplo de 4?\n");

  for (i = 0; i < len; i +=4) {
    asm("movdqu  %[x],   %%xmm0  \n\t"
        "movdqa  %%xmm0, %%xmm1 \n\t"
        "movdqu  %[m],   %%xmm6  \n\t"
        "psrlw   $4,     %%xmm1  \n\t"
        "pand    %%xmm6, %%xmm0  \n\t"
        "pand    %%xmm6, %%xmm1  \n\t"

        "movdqu  %[l],   %%xmm2  \n\t"
        "movdqa  %%xmm2, %%xmm3  \n\t"
        "pshufb  %%xmm0, %%xmm2  \n\t"
        "pshufb  %%xmm1, %%xmm3  \n\t"

        "paddb   %%xmm2, %%xmm3  \n\t"
        "pxor    %%xmm0, %%xmm0  \n\t"
        "psadbw  %%xmm0, %%xmm3  \n\t"
        "movhlps %%xmm3, %%xmm0  \n\t"
        "paddd   %%xmm3, %%xmm0  \n\t"
        "movd    %%xmm0, %[val]  \n\t"

      : [val] "=r"  (val)
      : [x]   "m"   (array[i]),
        [m]   "m"   (SSE_mask[0]),
        [l]   "m"   (SSE_LUTb[0])
    );
    result += val;
  }
  return result;
}


int popcount6(unsigned* array, int len) {
  int i, val, result = 0;
  unsigned x;

  for (i = 0; i < len; i++) {
    x = array[i];
    asm("popcnt %[x], %[val]"
      : [val] "=r"  (val)
      : [x]   "r"   (x)
    );
    result += val;
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
      : "edi"
    );
    result += val;
  }
  return result;
}

int p_r(unsigned x) {
  if (x == 0)
    return 0;
  else return
    (x & 1) + p_r(x >> 1);
}

int popcount8(unsigned* array, int len) {
  unsigned x;
  int i, result = 0;
  for (i = 0; i<len; ++i)
    result += p_r(array[i]);
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
    for (i=0; i<SIZE; i++)	// se queda en cache
	     lista[i]=i;

    crono(popcount1, "popcount1, con for\t\t");
    crono(popcount2, "popcount2, con do-while\t\t");
    crono(popcount3, "popcount3, con asm en el bucle\t");
    crono(popcount4, "popcount4, implementación de CSAPP");
    crono(popcount5, "popcount5, usando SSSE3 PSH HUFB");
    crono(popcount6, "popcount6, usando popcnt\t");
    crono(popcount7, "popcount7, usando popcnt mejorado");
    crono(popcount8, "popcount8, con recursividad\t");
    printf("\n");

    exit(0);
}
