#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#define MAX 100
  typedef struct {
    int data[MAX];
    int numar_elemente;

  }COADA;
COADA *initializare(COADA *coada) {
  coada->numar_elemente = 0;
  return coada;
}
COADA *enqueue(COADA *coada,int element) {
  if (coada->numar_elemente < MAX) {
    coada->data[coada->numar_elemente] = element;
    coada->numar_elemente++;
  }
  else {
    printf("S-a depasit capacitatea maxima");
  }
  return coada;
}
void afisare_coada(COADA *coada) {

  if (coada->numar_elemente ==0 ) {
    printf("Coada este goala");
  }
  else {
    printf("Varful este:%d \n", coada->data[0]);
    for (int i = 0;i<coada->numar_elemente;i++) {
      printf("%d ", coada->data[i]) ;
    }
  }
  printf("\n");
}
COADA *dequeue(COADA *coada) {
  for (int i=0;i<coada->numar_elemente;i++) {
    coada->data[i] = coada->data[i+1];
  }
  coada->numar_elemente--;
  return coada;
}
void isEmpty(COADA *coada) {
  if (coada->numar_elemente == 0) {
    printf("Coada este goala");
  }
  else {
    printf("Coada nu este goala");
  }
  printf("\n");
}
int main(void) {
  //ex1
  /*
  int a = 0,b=0,c=0;
  printf("Inserati primul numar: ");
  scanf("%d",&a);
  printf("Inserati al doilea numar: ");
  scanf("%d",&b);
  printf("Inserati al treilea numar: ");
  scanf("%d",&c);
  int maximul = 0;
  if (a>b) {
    if (a>c) {
      maximul = a;
    }
    else
      maximul = c;
  }
  else if (b<c) {
    maximul = c;
  }
  else
    maximul = b;
  printf("Cel mai mare numar este: ");
  printf("%d",maximul);
  */
  //ex 2
  /*
  long int numar = 0;
  int prim = 1, prim_2 = 1;
  printf("Introduceti un numar: ");
  scanf("%ld",&numar);
  if (numar == 0 || numar == 1) {
    prim = 0;
    prim_2 = 0;
  }
  for (int d = 2;d<(numar-1);d++)
    {
      if (numar %d == 0) {
        prim = 0;
        break;
      }
    }
    printf("Prima varianta: \n");
    if (prim) {

      printf("Numarul este prim \n");
    }
    else {
      printf("Numarul nu este prim \n");
    }
  for (int i = 2;i<=sqrt(numar);i++) {
    if (numar%i == 0) {
      prim_2 = 0;
      break;
    }
  }
  printf("A doua varianta: \n");
  if (prim_2) {
    printf("Numarul este prim \n");
  }
  else {
    printf("Numarul nu este prim \n");
  }
  */
  //ex 3
  COADA *coada = malloc(sizeof(COADA));
  if (coada == NULL) {
    printf("eroare la alocare dinamica");
    exit(EXIT_FAILURE);
  }
  coada = initializare(coada);
  isEmpty(coada);
  coada = enqueue(coada,12);
  coada = enqueue(coada,13);
  coada = enqueue(coada,14);
  coada = enqueue(coada,15);
  coada = enqueue(coada,16);
  coada = enqueue(coada,17);
  afisare_coada(coada);
  coada = dequeue(coada);
  afisare_coada(coada);
  isEmpty(coada);






  return 0;
}