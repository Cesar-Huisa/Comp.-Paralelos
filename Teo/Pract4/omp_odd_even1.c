#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#ifdef DEBUG
const int RMAX = 100;
#else
const int RMAX = 10000000;
#endif

int thread_count;

void Usage(char* prog_name);
void Get_args(int argc, char* argv[], int* n_p, char* g_i_p);
void Generate_list(int a[], int n);
void Print_list(int a[], int n, char* title);
void Read_list(int a[], int n);
void Odd_even(int a[], int n);

// Funcion principal
int main(int argc, char* argv[]) {
   int  n;
   char g_i;
   int* a;
   double start, finish;

   Get_args(argc, argv, &n, &g_i);
   a = malloc(n*sizeof(int));
   if (g_i == 'g') {
      Generate_list(a, n);
#     ifdef DEBUG
      Print_list(a, n, "Antes de ordenar");
#     endif
   } else {
      Read_list(a, n);
   }

   start = omp_get_wtime();
   Odd_even(a, n);
   finish = omp_get_wtime();

#  ifdef DEBUG
   Print_list(a, n, "Despues de ordenar");
#  endif
   
   printf("Tiempo transcurrido = %e segundos\n", finish - start);

   free(a);
   return 0;
}

void Usage(char* prog_name) {
   fprintf(stderr, "uso:   %s <numero de hilos> <n> <g|i>\n", prog_name);
   fprintf(stderr, "   n:   numero de elementos de la lista\n");
   fprintf(stderr, "  'g':  genra la lista usando un generador aleatorio\n");
   fprintf(stderr, "  'i':  lista ingresada por usuario\n");
}

void Get_args(int argc, char* argv[], int* n_p, char* g_i_p) {
   if (argc != 4 ) {
      Usage(argv[0]);
      exit(0);
   }
   thread_count = strtol(argv[1], NULL, 10);
   *n_p = strtol(argv[2], NULL, 10);
   *g_i_p = argv[3][0];

   if (*n_p <= 0 || (*g_i_p != 'g' && *g_i_p != 'i') ) {
      Usage(argv[0]);
      exit(0);
   }
} 


// Genera la lista de ser requerida 
void Generate_list(int a[], int n) {
   int i;

   srandom(1);
   for (i = 0; i < n; i++)
      a[i] = random() % RMAX;
} 

// Muestra la lista
void Print_list(int a[], int n, char* title) {
   int i;

   printf("%s:\n", title);
   for (i = 0; i < n; i++)
      printf("%d ", a[i]);
   printf("\n\n");
} 

// Lee la lista generada
void Read_list(int a[], int n) {
   int i;

   printf("Please enter the elements of the list\n");
   for (i = 0; i < n; i++)
      scanf("%d", &a[i]);
}


// Funcion odd-even sort por el primer metodo del libro
void Odd_even(int a[], int n) {
   int phase, i, tmp;
#  ifdef DEBUG
   char title[100];
#  endif

   for (phase = 0; phase < n; phase++) {
      if (phase % 2 == 0)
#        pragma omp parallel for num_threads(thread_count) \
            default(none) shared(a, n) private(i, tmp)
         for (i = 1; i < n; i += 2) {
            if (a[i-1] > a[i]) {
               tmp = a[i-1];
               a[i-1] = a[i];
               a[i] = tmp;
            }
         }
      else
#        pragma omp parallel for num_threads(thread_count) \
            default(none) shared(a, n) private(i, tmp)
         for (i = 1; i < n-1; i += 2) {
            if (a[i] > a[i+1]) {
               tmp = a[i+1];
               a[i+1] = a[i];
               a[i] = tmp;
            }
         }
#     ifdef DEBUG
      sprintf(title, "After phase %d", phase);
      Print_list(a, n, title);
#     endif
   }
}

