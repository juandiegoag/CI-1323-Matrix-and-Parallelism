#include <mpi.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

int main ( int argc,  char *argv[] )
{
  int id, numproc, * rbuf;

  MPI_Init( &argc , &argv );
  MPI_Comm_size( MPI_COMM_WORLD , &numproc );
  MPI_Comm_rank( MPI_COMM_WORLD , &id );

  int a = 0, filas = 0, columnas = 0;

  if( id == 0 )
  {
    //Desde el proceso 0 solicita los valores al usuario

    cout << "Ingrese el multiplo del numero de filas: " << endl;
    cin >> a;

    cout << "Ingrese el numero de columnas: " << endl;
    cin >> columnas;

    filas = a * numproc;

    int * matriz = ( int * ) malloc ( filas * columnas * sizeof( int ) );

    // Llenado de matriz desde proceso 0
    for( int i = 0; i < filas; i++ )
    {
      for( int j = 0; j < columnas; j++ )
      {
         * ( matriz + i * columnas + j ) = rand() % 5;
      }
    }

    // Repartición de la matriz entre los procesos pertenecientes al MPI_COMM_WORLD
    // a cada proceso se le repartirá a filas de c enteros ==> a * C

    int cantidad = a * columnas
    MPI_Scatter( matriz, cantidad, MPI_INT, rbuf, cantidad, MPI_INT ,0 ,MPI_COMM_WORLD);


/*
    for( int i = 0; i < filas; i++ )
    {
      for( int j = 0; j < columnas; j++ )
      {
         cout << * ( matriz + i * columnas + j ) << " ";
      }

      cout << endl;
    }
  */
}

MPI_Bcast(&a, 1, MPI_INT, 0, MPI_COMM_WORLD);
MPI_Bcast(&filas, 1, MPI_INT, 0, MPI_COMM_WORLD);
MPI_Bcast(&columnas, 1, MPI_INT, 0, MPI_COMM_WORLD);




  //MPI_Finalize();
  return 0;

}
