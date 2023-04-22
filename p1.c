//Iván Álvarez Fernández
//Marcos Gómez Tejón

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void inicializaCadena(char *cadena, int n){
    int i;
    for(i=0; i<n/2; i++){
        cadena[i] = 'A';
    }
    for(i=n/2; i<3*n/4; i++){
        cadena[i] = 'C';
    }
    for(i=3*n/4; i<9*n/10; i++){
        cadena[i] = 'G';
    }
    for(i=9*n/10; i<n; i++){
        cadena[i] = 'T';
    }
    cadena[n] = '\0';
}

int main(int argc, char *argv[])
{
    int numprocs, rank, sum, ret_value;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(argc != 3){
        printf("Numero incorrecto de parametros\nLa sintaxis debe ser: program n L\n  program es el nombre del ejecutable\n  n es el tamaño de la cadena a generar\n  L es la letra de la que se quiere contar apariciones (A, C, G o T)\n");
        exit(1);
    }

    int i, n, count=0;
    char *cadena;
    char L;



    if(rank==0) {
        n = atoi(argv[1]);
        L = *argv[2];
        for(int j=1; j<numprocs; j++) {
            MPI_Send(&n, 1, MPI_INT, j, 0, MPI_COMM_WORLD);
            MPI_Send(&L, 1, MPI_CHAR, j, 0, MPI_COMM_WORLD);
        }
    } else{
            MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&L, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
   cadena = (char *) malloc((n+1)*sizeof(char));
   inicializaCadena(cadena, n);

    for(i=rank; i<n; i+=numprocs){
        if(cadena[i] == L){
            count++;
        }
    }
    if(rank!=0){
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else{
        sum+=count;
        for(int j=1; j<numprocs; j++) {
            MPI_Recv(&ret_value, 1, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += ret_value;
        }
        printf("La cadena es %s\n", cadena);
        printf("El numero de apariciones de la letra %c es %d\n", L, sum);
    }
    MPI_Finalize();
    free(cadena);
    exit(0);
}
