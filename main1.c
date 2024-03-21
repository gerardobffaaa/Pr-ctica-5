#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#ifndef AYUDA_DBG
#define AYUDA_DBG 1
#endif

#if AYUDA_DBG > 0
#define IMPRIME_DBG( ... ) do{ fprintf( stderr, "DBG:" __VA_ARGS__ ); } while( 0 )
#else
#define IMPRIME_DBG( ... ) ;
#endif

static int h_division( int clave, size_t m ) {
   return clave % m;
}

static int h_msd( int clave, size_t m ) {
   return clave / m;
}

static int sondeo_lineal( int clave, size_t i ) {
   return i;
}

static int sondeo_cuadratico( int clave, size_t i ) {
   return i * i;
}

enum {
   CELDA_VACIA =   -1,
   CELDA_ELIMINADA = -2,
};

typedef struct {
   char* nombre;
   float salario;
} Entrada_tabla;

typedef struct {
   Entrada_tabla* tabla;
   size_t  capacidad;
   size_t  longitud;
   int (*h)(int, size_t);
   int (*sondeo)(int, size_t);
} Tabla_hash;

// Contadores globales de colisiones
int col_ins_g = 0;
int col_bus_g = 0;

static void imprime_tabla_hash( const Tabla_hash* th ) {
   printf( "----------------------------------------\n" );
   printf( "TH.Capacidad: %ld\n", th->capacidad );
   printf( "TH.Longitud: %ld\n", th->longitud );
   printf( "TH.Tabla:\n" );
   for( size_t i = 0; i < th->capacidad; ++i ) {
      printf( "[%02ld] (%s, %0.2f)\n", i, th->tabla[ i ].nombre, th->tabla[ i ].salario );
   }
   printf( "----------------------------------------\n\n" );
}

Tabla_hash* TH_Nueva( size_t capacidad, int (*h)(int, size_t), int (*sondeo)(int, size_t) ) {
   Tabla_hash* th = ( Tabla_hash* )malloc( sizeof( Tabla_hash ) );
   if( NULL != th ) {
      th->longitud = 0;
      th->capacidad = capacidad;
      th->tabla = ( Entrada_tabla* ) malloc( capacidad * sizeof( Entrada_tabla ) );
      th->h = h;
      th->sondeo = sondeo;
      if( NULL != th->tabla ) {
         for( int i = 0; i < th->capacidad; ++i ) {
            th->tabla[ i ].nombre = NULL;
            th->tabla[ i ].salario = 0.0;
         }
      }
      else {
         free( th );
         th = NULL;
      }
   }
   return th;
}

void TH_Elimina( Tabla_hash** th ) {
   assert( th );
   for( int i = 0; i < (*th)->capacidad; ++i ) {
      free( (*th)->tabla[ i ].nombre );
   }
   free( (*th)->tabla );
   free( *th );
   *th = NULL;
}

bool TH_Inserta( Tabla_hash* th, char* nombre, float salario ) {
   assert( th );
   assert( th->longitud < th->capacidad );
   int pos;
   int inicio = pos = th->h( nombre[0], th->capacidad );
   IMPRIME_DBG( "TH_Inserta: Calculé el valor hash: %d para el nombre: %s\n", pos, nombre );
   int i = 0;
   while( th->tabla[ pos ].nombre != NULL && strcmp(th->tabla[ pos ].nombre, nombre) != 0 ) {
      pos = ( inicio + th->sondeo( nombre[0], i ) ) % th->capacidad;
      ++i;
      IMPRIME_DBG( "TH_Inserta: Recalculé el valor hash: %d para el nombre: %s\n", pos, nombre );
      col_ins_g++; // Incrementa el contador de colisiones de inserción
   }
   if(th->tabla[ pos ].nombre != NULL && strcmp(th->tabla[ pos ].nombre, nombre) == 0) {
      // Si el nombre ya existe, actualizamos el salario
      th->tabla[ pos ].salario = salario;
      printf("Salario actualizado para el empleado con nombre=%s a: $%0.2f\n", nombre, salario);
   } else {
      // Si el nombre no existe, insertamos una nueva entrada
      th->tabla[ pos ].nombre = strdup(nombre);
      th->tabla[ pos ].salario = salario;
      ++th->longitud;
      printf("Factor de carga: %f, Colisiones de inserción: %d\n", (float)th->longitud / th->capacidad, col_ins_g);
   }
   return true;
}

bool TH_Busca( const Tabla_hash* th, char* nombre, float *salario ) {
   assert( th );
   assert( th->longitud > 0 );
   int inicio = th->h( nombre[0], th->capacidad );
   int pos = inicio;
   IMPRIME_DBG( "TH_Busca: Calculé el valor hash: %d para el nombre: %s\n", pos, nombre );
   bool encontrado = false;
   int colisiones = 0;
   if( th->tabla[ pos ].nombre != NULL && strcmp(th->tabla[ pos ].nombre, nombre) == 0 ) {
      encontrado = true;
   }
   else if( th->tabla[ pos ].nombre == NULL ) {
      encontrado = false;
   }
   else {
      int i = 0;
      while( th->tabla[ pos ].nombre != NULL && encontrado == false ) {
         if( th->tabla[ pos ].nombre == NULL ) {
            colisiones++;
         }
         pos = ( inicio + th->sondeo( nombre[0], i ) ) % th->capacidad;
         ++i;
         IMPRIME_DBG( "TH_Busca: Recalculé el valor hash: %d para el nombre: %s\n", pos, nombre );
         if( th->tabla[ pos ].nombre != NULL && strcmp(th->tabla[ pos ].nombre, nombre) == 0 ) {
            encontrado = true;
         }
         col_bus_g++; // Incrementa el contador de colisiones de búsqueda
      }
   }
   printf("Colisiones en la búsqueda: %d\n", colisiones);
   bool ret_val = false;
   if( encontrado ) {
      *salario = th->tabla[ pos ].salario;
      ret_val = true;
   }
   return ret_val;
}

bool TH_Elimina_Entrada( Tabla_hash* th, char* nombre ) {
   assert( th );
   assert( th->longitud > 0 );
   int inicio = th->h( nombre[0], th->capacidad );
   int pos = inicio;
   IMPRIME_DBG( "TH_Elimina_Entrada: Calculé el valor hash: %d para el nombre: %s\n", pos, nombre );
   bool encontrado = false;
   int slots_borrados_visitados = 0;
   if( th->tabla[ pos ].nombre != NULL && strcmp(th->tabla[ pos ].nombre, nombre) == 0 ) {
      encontrado = true;
   }
   else if( th->tabla[ pos ].nombre == NULL ) {
      encontrado = false;
   }
   else {
      int i = 0;
      while( th->tabla[ pos ].nombre != NULL && encontrado == false ) {
         if( th->tabla[ pos ].nombre == NULL ) {
            slots_borrados_visitados++;
         }
         pos = ( inicio + th->sondeo( nombre[0], i ) ) % th->capacidad;
         ++i;
         IMPRIME_DBG( "TH_Elimina_Entrada: Recalculé el valor hash: %d para el nombre: %s\n", pos, nombre );
         if( th->tabla[ pos ].nombre != NULL && strcmp(th->tabla[ pos ].nombre, nombre) == 0 ) {
            encontrado = true;
         }
      }
   }
   if( encontrado ) {
      free(th->tabla[ pos ].nombre);
      th->tabla[ pos ].nombre = NULL;
      th->tabla[ pos ].salario = 0.0;
      --th->longitud;
   }
   printf("Slots borrados visitados: %d\n", slots_borrados_visitados);
   return encontrado;
}

int Longitud( Tabla_hash* th ) {
    assert(th);
    return th->longitud;
}

float FactorCarga( Tabla_hash* th ) {
    assert(th);
    return (float)th->longitud / th->capacidad;
}

#define TAMANO_TABLA_HASH 10

int main() {
   Tabla_hash* por_salario = TH_Nueva( TAMANO_TABLA_HASH, h_division, sondeo_lineal );
   TH_Inserta( por_salario, "Juan", 13500.0 );
   TH_Inserta( por_salario, "Pedro", 14650.0 );
   TH_Inserta( por_salario, "Ana", 16560.0 );
   TH_Inserta( por_salario, "Maria", 19876.0 );
   TH_Inserta( por_salario, "Carlos", 11000.0 );
   TH_Inserta( por_salario, "Luis", 13500.0 );
   imprime_tabla_hash( por_salario );
   char* nombre = "Maria";
   float salario;
   bool ret_val = TH_Busca( por_salario, nombre, &salario );
   if( ret_val ) {
      printf( "El salario del empleado con nombre=%s es: $%0.2f\n", nombre, salario );
   }
   else {
      printf( "El empleado con nombre=%s no está en mis registros\n", nombre );
   }
   nombre = "Pedro";
   printf( "El empleado con nombre=%s [%s] está en mis registros\n", nombre,
         TH_Busca( por_salario, nombre, &salario ) ? "SÍ" : "NO" );
   if( TH_Elimina_Entrada( por_salario, "Juan" ) ) {
      printf( "Elemento eliminado\n" );
   }
   else {
      printf( "El elemento no pudo ser eliminado porque no existe\n" );
   }
   imprime_tabla_hash( por_salario );
   TH_Elimina( &por_salario );
}

