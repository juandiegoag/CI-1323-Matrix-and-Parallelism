#include <mpi.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>

using namespace std;


int main (int argc,  char *argv[] ){
  int id, numproc, *matriz, *rbuf, *aparicionesFila, *totalColumnas, *conteoFilas, *sumaColumnas;

  MPI_Init( &argc , &argv );
  MPI_Comm_size( MPI_COMM_WORLD , &numproc );
  MPI_Comm_rank( MPI_COMM_WORLD , &id );

  int a = 0, filas = 0, columnas = 0, cantidad=0;

  if( id == 0 ){
    //Desde el proceso 0 solicita los valores al usuario
    cout << "Ingrese el multiplo del numero de filas: " << endl;
    cin >> a;
    cout << "Ingrese el numero de columnas: " << endl;
    cin >> columnas;

    filas = a * numproc;
    cantidad = a * columnas;//cantidad de elementos por proceso
    // Llenado de matriz desde proceso 0
    matriz = ( int * ) malloc ( filas * columnas * sizeof( int ) );
    aparicionesFila = new int[5*filas]; //matriz final de apariciones en las filas
    totalColumnas = new int[columnas];

    for( int i = 0; i < filas; i++ ){
      for( int j = 0; j < columnas; j++ ){
         *(matriz + i*columnas + j) = rand() % 5;
      }
    }

    for (int i = 0; i < columnas; i++) {
      totalColumnas[i]=0;
    }

  }
  MPI_Bcast(&a,        1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&filas,    1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&columnas, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&cantidad, 1, MPI_INT, 0, MPI_COMM_WORLD);
  // Repartición de la matriz entre los procesos pertenecientes al MPI_COMM_WORLD
  // a cada proceso se le repartirá a filas de c enteros ==> a * C

  rbuf = new int[cantidad];
  MPI_Scatter(matriz, cantidad, MPI_INT, rbuf, cantidad, MPI_INT ,0 ,MPI_COMM_WORLD);

  conteoFilas  = new int[5*a]; //arreglo que contiene el numero de apariciones por procesos
  //a cada proceso le corresponde un arreglo
  sumaColumnas = new int[columnas];//contiene la suma de todas las columnas, correspondientes proceso,
  //es de tamano del numero de elementos en una fila
  int offset = 0;//se usa para llevar la cuenta de las apariciones de un numero en una fila, cuando se termina de leer una fila,
  //y se pasa a la otra el offset se incrementa

  for (int j = 0; j < 5*a; j++) {
    conteoFilas[j]=0;//inicializa el array en 0
  }

  for (int j = 0; j < columnas; j++) {
    sumaColumnas[j]=0;//inicializa el array en 0
  }

  for (int i = 0; i < cantidad; i++) {
    sumaColumnas[i%columnas]+=rbuf[i];//va sumando los valores de las columnas, en el vector sumaColumnas

    if (i%columnas == 0 && i>0) {
      offset+=5;//actualiza el offset si ya se leyo una fila totalmente
    }

    switch (rbuf[i]) {//dependiendo del elemento que se lea se incrementa uno de los campos de conteoFilas
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

  MPI_Gather(conteoFilas, 5*a, MPI_INT, aparicionesFila, 5*a, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Reduce(sumaColumnas, totalColumnas, columnas, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD); //espera a que todos lleguen a este punto
  delete conteoFilas;
  delete rbuf;
  delete sumaColumnas;

  if (id == 0) {
    free(matriz);
    int numFila=0;
    cout << "PROCESO 0:" <<endl;
    cout<<endl;

    for (int i = 0; i < 5*filas; i++) {
      int posicion=i%5;
      if(posicion==0){
        numFila++;
        cout<<"[FILA "<<numFila<<"]: "<<endl;
      }
      cout<<"\t"<<"Numero de "<<posicion<<": "<<aparicionesFila[i]<<endl;
    }

    cout<<endl;
    
    for (int i = 0; i < columnas; i++) {
      cout<<"La suma de elementos de la columna "<<i<<" es:"<<endl;
      cout<<"\t"<<totalColumnas[i]<<endl;
    }

    delete aparicionesFila;
    delete totalColumnas;
  }

  MPI_Finalize();
  return 0;
}
