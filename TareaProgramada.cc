#include <mpi.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

int main (int argc,  char *argv[] )
{
  int id, numproc, *rbuf;

  MPI_Init( &argc , &argv );
  MPI_Comm_size( MPI_COMM_WORLD , &numproc );
  MPI_Comm_rank( MPI_COMM_WORLD , &id );

  int a = 0, filas = 0, columnas = 0;

  if( id == 0 ){
    //Desde el proceso 0 solicita los valores al usuario

    cout << "Ingrese el multiplo del numero de filas: " << endl;
    cin >> a;

    cout << "Ingrese el numero de columnas: " << endl;
    cin >> columnas;

    filas = a * numproc;

    int * matriz = ( int * ) malloc ( filas * columnas * sizeof( int ) );

    // Llenado de matriz desde proceso 0
    for( int i = 0; i < filas; i++ ){
      for( int j = 0; j < columnas; j++ ){
         *( matriz + i * columnas + j ) = rand() % 5;
         cout<<*( matriz + i * columnas + j )<<" ";
      }
      cout<<endl;
    }
    // Repartición de la matriz entre los procesos pertenecientes al MPI_COMM_WORLD
    // a cada proceso se le repartirá a filas de c enteros ==> a * C

    int cantidad = a * columnas;
    MPI_Scatter( matriz, cantidad, MPI_INT, rbuf, cantidad, MPI_INT ,0 ,MPI_COMM_WORLD);

    int *conteoFilas  = (int*) malloc(5*a*sizeof(int));
    int *sumaColumnas = (int*) malloc(columnas*sizeof(int));
    int offset = 0;

    for (int j = 0; j < 5*a; j++) {
      conteoFilas[j]=0;
    }

    for (int j = 0; j < columnas; j++) {
      sumaColumnas[j]=0;
    }

    for (int i = 0; i < cantidad; i++) {
      sumaColumnas[i%columnas]+=matriz[i];

      if (i%columnas == 0 && i>0) {
        offset+=5;
      }

      switch (matriz[i]) {
        case 0:
          *(conteoFilas+offset)+=1;
        break;

        case 1:
          *(conteoFilas+offset+1)+=1;
        break;

        case 2:
          *(conteoFilas+offset+2)+=1;
        break;

        case 3:
          *(conteoFilas+offset+3)+=1;
        break;

        case 4:
          *(conteoFilas+offset+4)+=1;
        break;
      }

    }



  }


  MPI_Bcast(&a,        1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&filas,    1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&columnas, 1, MPI_INT, 0, MPI_COMM_WORLD);




  MPI_Finalize();
  return 0;

}

/*
    for( int i = 0; i < filas; i++ ){
      for( int j = 0; j < columnas; j++ ){
         cout << * ( matriz + i * columnas + j ) << " ";
      }
      cout << endl;
    }

    for( int j = 1; j <= 5*a; j++ ){
       cout<< conteoFilas[j-1]<<" ";
      if (j%5==0) {
      cout<<endl;
      }
    }
    for( int j = 1; j <= columnas; j++ ){
       cout<< sumaColumnas[j-1]<<" ";
    }
  */
