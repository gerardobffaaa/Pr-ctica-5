#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Función hash de división
int32_t f_div(int key, int m)
{
    return key % m;
}

size_t myPow10( size_t n )
{
    size_t acum = 1;

    for( int i = n; i > 0; --i )
        acum *= 10;

    return acum;
}

#define SIGMA 2 //desplazamiento
#define POS_VAL 2 //esto es n, unidades y decenas

int32_t f_ms(int key, int m)
{
    int32_t res = (key*key) % (myPow10( (3*POS_VAL) / 2 + SIGMA) );
    fprintf( stderr, "k*k=%d\n", key*key); //nos muestra el cuadrado
    res = res/ myPow10( POS_VAL / 2 + SIGMA );
    res = res % m;
   
    return res;
}

uint32_t f_gonnet99( char *s, int m)
{
    uint32_t i;
   
    for( i = 0; *s; ++s)
    {
        i = 131*i + *s;
    }
   
    return i % m;
}

// Función hash de multiplicación
int h_multiplication(int key, int m) {
    float A = 0.6180339887; // constante 0<A<1
    float val = key * A;
    float frac_part = val - (int)val; // obtenemos la parte fraccionaria
    return m * frac_part; // multiplicamos por m
}

// Función hash para cadenas sencillas
int h_string(char* s, int m) {
    int hash = 0;
    int n = strlen(s);
    for (int i = 0; i < n; i++) {
        hash = (31 * hash + s[i]) % m;
    }
    return hash;
}

// Resolución de colisiones: Linear probing
int probe_linear(int key, int i) {
    return i;
}

// Resolución de colisiones: Quadratic probing
int probe_quadratic(int key, int i) {
    return i * i;
}

#define LIST_SIZE 5 //lista de numeros
#define TABLE_SIZE 17 //tamaño de la tabla

typedef struct {
    int id;
    float salary;
} Employee;

Employee employees[TABLE_SIZE]; // Tabla hash para empleados

int num_employees = 0; // Número de empleados actualmente en la tabla

// Función para calcular el factor de carga
float load_factor() {
    return (float)num_employees / TABLE_SIZE;
}

// Función para insertar un empleado en la tabla hash
void insert_employee(int id, float salary) {
    int index = h_multiplication(id, TABLE_SIZE); // Cambia esto a la función hash deseada
    int i = 0;
    int collisions = 0;

    while (employees[(index + probe_linear(id, i)) % TABLE_SIZE].id != 0) { // Cambia probe_linear a la técnica de resolución de colisiones deseada
        i++;
        collisions++;
    }

    employees[(index + probe_linear(id, i)) % TABLE_SIZE] = (Employee){.id = id, .salary = salary};
    num_employees++;


    printf("Empleado insertado con ID %d y salario %.2f\n", id, salary);
    printf("Factor de carga: %.2f\n", load_factor());
    printf("Colisiones: %d\n", collisions);
}

int main() {
    // Inicializa la tabla hash
    for (int i = 0; i < TABLE_SIZE; i++) {
        employees[i] = (Employee){.id = 0, .salary = 0.0};
    }

    // Inserta empleados con IDs que son múltiplos de 5
    printf("m= %d \n", TABLE_SIZE);
    for (int i = 1; i <= LIST_SIZE; i++) {
        insert_employee(i * 5, i * 1000.0);
    }

    int m = TABLE_SIZE; // Define m aquí

    char* list_names[5] =
    {
        "Flor Burnley",
        "Carma Vuong",  
        "Lynsey Zaragosa",  
        "Hilaria Laury",  
        "Stephane Mcnamara"
    };

    for(int i = 0; i < LIST_SIZE; ++i)
    {
        printf("Nombre: %s\n", list_names[i]);
        printf("Hash Gonnet99: %d\n", f_gonnet99(list_names[i], m));
        printf("Linear Probing: %d\n", probe_linear(f_gonnet99(list_names[i], m), i));
        printf("Quadratic Probing: %d\n\n", probe_quadratic(f_gonnet99(list_names[i], m), i));
    }
   
    int list[LIST_SIZE] = {3345, 7856, 1109, 3347, 8710};

    for(int i = 0; i < LIST_SIZE; ++i)
    {
        printf("Número: %d\n", list[i]);
        printf("Hash División: %d\n", f_div(list[i], m));
        printf("Hash Mid-Square: %d\n", f_ms(list[i], m));
        printf("Hash Multiplicación: %d\n", h_multiplication(list[i], m));
        printf("Linear Probing: %d\n", probe_linear(f_div(list[i], m), i));
        printf("Quadratic Probing: %d\n\n", probe_quadratic(f_div(list[i], m), i));
    }
}

