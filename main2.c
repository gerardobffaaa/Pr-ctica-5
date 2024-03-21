#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#define EMPTY_CELL -1
#define NOT_FOUND -2
#define MAX_PRODUCTS 5
#define HASH_TABLE_SIZE 7

typedef struct
{
   int bar_code;
   char    name[ 32 ];
   float   price;
} Product;

typedef struct
{
   char key[32];
   int index;
} Table_entry;

typedef struct
{
	Table_entry* table;
	size_t  size;
	size_t  len;
} Hash_table;

// Función hash Gonneth99 para cadenas
int Gonneth99(char* str, int m) {
    int hash = 0;
    int c;

    while (c = *str++)
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash % m;
}

// Resolución de colisiones mediante sondeo lineal
int probe(int i) {
    return i;
}

Hash_table* HT_New( size_t size )
{
   Hash_table* ht = ( Hash_table* )malloc( sizeof( Hash_table ) );
   if( NULL != ht )
   {
      ht->len = 0;
      ht->size = size;

      ht->table = ( Table_entry* ) malloc( size * sizeof( Table_entry ) );
      if( NULL == ht->table )
      {
         free( ht );
         ht = NULL;
      }
      else 
      {
         for( int i = 0; i < ht->size; ++i )
         {
            ht->table[ i ].index = EMPTY_CELL;
            ht->table[ i ].key[0] = '\0';
         }
      }
   }

   return ht;
}

void HT_Delete( Hash_table** ht )
{
   assert( ht );

   free( (*ht)->table );
   free( *ht );
   *ht = NULL;
}

bool HT_InsertByName( Hash_table* ht, char* name, int idx )
{
   assert( ht );
   assert( ht->len < ht->size );

   int pos;
   int home = pos = Gonneth99( name, ht->size );

   size_t i = 0;

   while( ht->table[ pos ].index != EMPTY_CELL )
   {
      if( strcmp(ht->table[ pos ].key, name) == 0 )
      {
         printf( "Error: Llave duplicada\n" );
         return false;
      }

      pos = ( home + probe( i ) ) % ht->size;
      ++i;
   }

   strcpy(ht->table[ pos ].key, name);
   ht->table[ pos ].index = idx;

   ++ht->len;

   return true;
}

int HT_SearchByName( const Hash_table* ht, char* name )
{
   assert( ht );
   assert( ht->len > 0 );

   int home = Gonneth99( name, ht->size );
   int pos = home;

   size_t i = 0;

   if( ht->table[ pos ].index == EMPTY_CELL ) return EMPTY_CELL;

   while( strcmp(ht->table[ pos ].key, name) != 0 )
   {
      pos = ( home + probe( i ) ) % ht->size;
      ++i;

      if( pos == home ) return NOT_FOUND;
   }

   return ht->table[ pos ].index;
}

bool HT_IsEmpty( const Hash_table* ht )
{
   return ht->len == 0;
}

bool HT_IsFull( const Hash_table* ht )
{
   return ht->len == ht->size;
}

float HT_LoadFactor( const Hash_table* ht )
{
   return (float)ht->len / ht->size;
}

void print_table_hash(Hash_table* ht)
{
    for (int i = 0; i < ht->size; i++)
    {
        printf("Posición %d: ", i);
        if (ht->table[i].index != EMPTY_CELL)
        {
            printf("Key: %s, Index: %d\n", ht->table[i].key, ht->table[i].index);
        }
        else
        {
            printf("Vacía\n");
        }
    }
}

int main()
{
   Product productos[ MAX_PRODUCTS ] =
   {
      { 1010,     "Gansito",     9.0 },
      { 2021,     "Crema",      16.5 },
      { 3032,     "Arroz",      28.5 },
      { 4043,     "Papitas",    14.0 },
      { 5054,     "Detergente", 25.0 },
   };

   Hash_table* tabla = HT_New( HASH_TABLE_SIZE );

   for( size_t i = 0; i < MAX_PRODUCTS && !HT_IsFull( tabla ); ++i )
   {
     HT_InsertByName( 
         tabla,
         productos[ i ].name,
         i );
   }

   print_table_hash(tabla);

   int idx = HT_SearchByName( tabla, "Detergente" );
   printf("Índice de 'Detergente': %d\n", idx);
   assert( idx == 4 );

   HT_Delete( &tabla );
}

