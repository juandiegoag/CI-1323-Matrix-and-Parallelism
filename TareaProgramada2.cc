#include <mpi.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>

using namespace std;


void blanquear(int a[], int size)
{ //recibe un array, y lo llena con 0
  for (int j = 0; j < size; j++)
  {
    a[j]=0;
  }
}

int main (int argc, char *argv[] )
{ // variables necesarias para el desarrollo del programa, tanto ints como buffers

  int id, numproc, *aparicionesFila, *totalColumnas, *conteoFilas, *sumaColumnas, *aparicionesColumnas, *conteoColumnas;
  int * zodape;
  // comandos MPI necesarios, tanto numproc como id son utilizados para el funcionamiento del programa
  MPI_Init( &argc , &argv );
  MPI_Comm_size( MPI_COMM_WORLD , &numproc );
  MPI_Comm_rank( MPI_COMM_WORLD , &id );

  // se convierten los argumentos recibidos a int para obtener las dimensiones de la matriz
  int a = atoi(argv[1]);
  int filas = a * numproc;
  int columnas = atoi(argv[2]);
  int cantidad = a * columnas;

  if( id == 0)
  {
    aparicionesFila = new int[5*filas]; //matriz final de apariciones en las filas
    totalColumnas = new int[columnas];
    aparicionesColumnas = new int[5*columnas];

    blanquear( aparicionesFila, 5 * filas);
    blanquear( aparicionesColumnas, 5 * columnas);
    blanquear( totalColumnas, columnas);

  }

  // Se inicializa el zodape de matriz en cada proceso
  //zodape = ( int * ) malloc ( cantidad * sizeof( int ) );
  zodape = new int[cantidad];

  // Se llena zodape dentro de cada proceso
  for ( int i = 0; i < cantidad; i++ )
  {
        zodape[i] = rand() % 5;
  }



  conteoFilas    = new int[ 5*a ]; //arreglo que contiene el numero de apariciones por procesos
                                //a cada proceso le corresponde un arreglo.
  conteoColumnas = new int[ 5 * columnas ];//contiene las apariciones de los numeros en las columnas.

  sumaColumnas   = new int[ columnas ];//contiene la suma de todas las columnas, correspondientes proceso,
                                       //es de tamano del numero de elementos en una fila

  blanquear(conteoFilas , 5 * a );//llena los vectores con cero.
  blanquear(conteoColumnas, 5 * columnas );
  blanquear(sumaColumnas , columnas );

  int offset = 0; //se usa para llevar la cuenta de las apariciones de un numero en una fila, cuando se termina de leer una fila, y se pasa a la otra el offset se incrementa
  int offsetCol = 0;

  //Accion que se realiza en cada proceso para el conteo de la zodape local de la matriz
  for ( int i = 0; i < cantidad; i++ )
  {
    //Arreglo de offsets para los conteos por filas y por columnas
    offsetCol = (i % columnas) * 5;

    if( i % columnas == 0 && i > 0) {
      offset += 5;
    }
    //Calcular apariciones por fila dentro de zodape
     *( conteoFilas + zodape[ i ] + offset ) += 1;

    //Calcular apariciones por columna dentro de zodape
     *( conteoColumnas + zodape[ i ] + offsetCol ) += 1;

    //Calcular suma por columna dentro de la zodape
     sumaColumnas[ i % columnas ] += zodape[ i ];

  }
  // se blanquean todos los vectores antes de empezar a sumar en ellos

  MPI_Gather(conteoFilas, 5*a, MPI_INT, aparicionesFila, 5*a, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Reduce(sumaColumnas , totalColumnas , columnas , MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Reduce(conteoColumnas, aparicionesColumnas, 5*columnas, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD); //espera a que todos lleguen a este punto

  //borra los arrays
  delete conteoFilas;
  delete sumaColumnas;
  delete conteoColumnas;
  delete zodape;

  // Impresion de resultados desde el proceso 0
  if (id == 0)
  { // Se libera el espacio reservado con malloc

    int numFila=0, numColumna = 0;

    cout << "PROCESO 0:" <<endl;
    cout<<endl;

    for (int i = 0; i < 5*filas; i++)
    {
      int posicion=i%5;

      if(posicion==0)
      {
        numFila++;
        cout<<"[FILA "<<numFila<<"]: "<<endl;
      }
      cout<<"\t"<<"Numero de "<<posicion<<": "<<aparicionesFila[i]<<endl;
    }
    cout<<endl;

    for (int i = 0; i < columnas; i++)
    {
      cout<<"La suma de elementos de la columna "<<i<<" es:"<<endl;
      cout<<"\t"<<totalColumnas[i]<<endl;
    }
    cout<<endl;

    for (int i = 0; i < 5*columnas; i++)
    {
      int posicion=i%5;
      if(posicion==0)
      {
        numColumna++;
        cout<<"[COLUMNA "<<numColumna<<"]: "<<endl;
      }
      cout<<"\t"<<"Numero de "<<posicion<<": "<<aparicionesColumnas[i]<<endl;
    }
    // Se eliminan el resto de vectores reservados antes
    delete aparicionesColumnas;
    delete aparicionesFila;
    delete totalColumnas;
  }

  MPI_Finalize();
  return 0;
}
