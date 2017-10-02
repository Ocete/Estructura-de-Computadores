 #include <stdio.h> //para printf()
#include <stdlib.h> //para exit()
#include <string.h> //para strncmp()/strlen()
#include <sys/time.h> //para gettimeofday(), struct timeval

char cadena[] = "Atención: movb y code";
char password[] = "miss me?\n";
int passcode  = 314;

void boom() {
  printf("***************\n");
  printf("*** BOOM (: ***\n");
  printf("***************\n");
  exit(-1);
}

void defused() {
  printf("·························\n");
  printf("··· Bomba desactivada ···\n");
  printf("·························\n");
  exit(0);
}

void pista() {
  printf("%s", cadena);
}

int code(int in) {
  int ret;

  if (1119299126 % in == 0 &&
      188495978 % in == 0 &&
      in % 7381 == 0 &&
      in % 6893 == 0 &&
      (in & 0x1) == 0)
    ret = 0;
  else
    ret = 1;

  return ret;
}

int main() {
#define SIZE 100
  char pass[SIZE];
  int pasv;


#define TLIM 7
  struct timeval tv1,tv2;   // gettimeofday() secs-usecs
  gettimeofday(&tv1, NULL);

  printf ("Introduce la contraseña: ");
  fgets (pass, SIZE, stdin);

  password[4] = '&';

  if (strncmp(pass, password, strlen(password)))
             boom();

  gettimeofday(&tv2, NULL);
  if (tv2.tv_sec - tv1.tv_sec > TLIM)
             boom();

  printf ("Introduce el código (el cual es único): ");
  scanf("%s", &pass);
  if (code(atoi(pass)) == 1)
    boom();

  gettimeofday(&tv1, NULL);
  if (tv1.tv_sec - tv2.tv_sec < TLIM)
    boom();

  defused();

  pista();
}
