#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "lib.h"
#include <unistd.h>
#define PATH_MAX 260

void menu()
{

    printf(" Alu:   Adrian Garrido Pantaleon\n");
    printf("@Alu:   Adrian.garrido01@goumh.umh.es\n\n");

    printf("--------------Comandos----------------\n");
    printf(">exit\n");
    printf(">new db <nombre_base_de_datos>\n");
    printf(">use db <nombre_base_de_datos>\n");
    printf(">new table <nombre_tabla> <campo> <tipo> [<campo> <tipo> ...]\n");
    printf(">use table <nombre_base_de_datos>\n");
    printf(">insert <valor> [<valor> ...]\n");
    printf(">select [<campo> * <valor>] [<orden>]\n");
}

/*
Esta funcion se encarga de comprobar si el nombre es el correcto
*/
int comprobar_caracteres(char *nombre_bd)
{
    char cadena[100] = "";
    int j = 0, error = 0;

    strcpy(cadena, nombre_bd); // aqui copiamos en cadena el parametro

    while (cadena[j] != '\0')
    { // Aqui vamos recorriendo la cadena caracter a caracter para comprobarlo

        if (cadena[j] == '_')
        {
            j++;
            continue;
        }
        if (cadena[j] >= '0' && cadena[j] <= '9')
        {
            j++;
            continue;
        }
        if (cadena[j] >= 'a' && cadena[j] <= 'z')
        {
            j++;
            continue;
        }
        if (cadena[j] >= 'A' && cadena[j] <= 'Z')
        {
            j++;
            continue;
        }

        printf("ERROR de caracteres\n");
        error++;
        break;
    }
    return error;
}

/*
Esta funcion se encarga crear la carpeta con el nombre indicado
*/
void crear_bd(char *nombre_bd)
{

    char aux1[15] = "\\meta.db";
    char aux2[50] = "";

    strcpy(aux2, nombre_bd);
    strcat(aux2, aux1);

    if (access(nombre_bd, F_OK) == 0)
    {
        printf("\n Carpeta ya creada\n");
    }
    else
    {
        mkdir(nombre_bd);            // creamos la carpeta
        crear_meta(nombre_bd, aux2); // llamamos a la funcion para crear el archivo meta.db
    }
}

/*
Esta funcion se encarga de crear el meta.db
*/
void crear_meta(char *nombre_bd, char *aux2)
{

    DIR *dir;
    if (dir = opendir(nombre_bd))
    {
        FILE *file;
        file = fopen(aux2, "w");
        if (file == NULL)
        {
            printf("El fichero est%c vac%co\n", 160, 161);
        }
        else
        {
            fprintf(file, "TABLAS: 0\n");
            fprintf(file, "-------------------------\n");

            fclose(file);
        }
    }
}

/*
Esta funcion se encarga de comprobar si existe una carpeta
*/
int entrar_dir(char *nombre_bd)
{

    DIR *dir;
    if (dir = opendir(nombre_bd))
    {
        return 0;
    }
    else // no existe la BD
    {
        printf("La BD %s no se ha encontrado\n", nombre_bd);
    }
}

/*
Esta funcion se encarga de validar el tipo de dato que sea correcto
*/
TYPE validar_tipo(char *valor)
{
    if (strcmp(valor, "NUM") == 0)
    {
        return NUM;
    }
    else if (strcmp(valor, "DATE") == 0)
    {
        return DATE;
    }
    else if (strcmp(valor, "EMAIL") == 0)
    {
        return EMAIL;
    }
    else if (strcmp(valor, "TEXT") == 0)
    {
        return TEXT;
    }

    return ERROR;
}

/*
Esta funcion se encarga inicializar las tablas dentro de la BD
*/
TABLE *inicializar_tabla(int num_campos, char *nombre_tabla, char **campos, TYPE *tipos)
{
    TABLE *tabla = malloc(sizeof(TABLE)); // Reservamos memoria para una tabla

    if (tabla == NULL)
    {
        return NULL;
    }

    strcpy(tabla->nombre, nombre_tabla); // Copiamos en la tabla el nombre de la tabla
    tabla->numCampos = num_campos;
    tabla->campos = campos;
    tabla->tipos = tipos;
    tabla->ant = NULL;
    tabla->sig = NULL;

    return tabla;
}

/*
Esta funcion se encarga crear las tablas dentro de la BD
*/
void crear_tabla(TABLE *tabla, char *nombre_bd, char *nombre_tabla)
{
    char aux1[5] = ".tab";
    char aux2[5] = "\\";
    char aux3[100] = "";

    strcpy(aux3, nombre_bd);
    strcat(aux3, aux2);
    strcat(aux3, nombre_tabla);
    strcat(aux3, aux1);

    if (access(aux3, F_OK) == 0)
    {

        printf("La tabla %s ya fue creada\n", nombre_tabla);
    }
    else
    {

        DIR *dir;
        if (dir = opendir(nombre_bd))
        {

            FILE *fichero;
            fichero = fopen(aux3, "w");

            if (fichero != NULL)
            {

                // editar el meta
                editar_meta(nombre_bd, tabla);
            }

            fclose(fichero);
        }
        else
        {
            printf("algo salio mal\n");
        }
    }
}

/*
Esta funcion se encarga de contar el numero de tablas de la carpeta
*/
int contar_tablas(char *nombre_bd)
{

    DIR *dir;
    struct dirent *file; // Habilita trabajar con los ficheros de la carpeta.
    int num = 0;

    if (dir = opendir(nombre_bd))
    {
        while (file = readdir(dir))
        {
            num++;
        }
    }
    num = num - 3;
    return num;
}

/*
Esta funcion se encarga de contar el numero de tablas de la carpeta
*/
void editar_meta(char *nombre_bd, TABLE *tabla)
{

    DIR *dir;
    char aux1[11] = "\\meta.db";
    char aux2[50] = "";
    char aux_tipos[50] = "";
    int i, numero_tablas = 0;

    strcpy(aux2, nombre_bd);
    strcat(aux2, aux1);
    numero_tablas = contar_tablas(nombre_bd);

    if (dir = opendir(nombre_bd))
    { // entramos al directorio

        FILE *file;              // creamos el meta
        file = fopen(aux2, "w"); // abrimos el txt
        if (file == NULL)
        {
            printf("El fichero est%c vac%co\n", 160, 161);
        }
        else
        {
            fprintf(file, "TABLAS: %d\n", numero_tablas);
            fprintf(file, "-------------------------\n");
    
            while (tabla != NULL)
            {

                fprintf(file, "%s: %d\n", tabla->nombre, tabla->numCampos);

                for (i = 0; i < tabla->numCampos; i++)
                {
                    if (tabla->tipos[i] == 0)
                    {
                        strcpy(aux_tipos, "NUM");
                    }
                    else if (tabla->tipos[i] == 1)
                    {
                        strcpy(aux_tipos, "DATE");
                    }
                    else if (tabla->tipos[i] == 2)
                    {
                        strcpy(aux_tipos, "EMAIL");
                    }
                    else if (tabla->tipos[i] == 3)
                    {
                        strcpy(aux_tipos, "TEXT");
                    }

                    fprintf(file, "%s: %s\n", tabla->campos[i], aux_tipos);
                }

                if (tabla->sig == NULL)
                {
                    break;
                }

                fprintf(file, "-------------------------\n");
                tabla = tabla->sig;
            }
        }
        fclose(file);
    }
}

/*
Esta funcion se encarga de recorrer la tabla_principal y se situa en la última posicion.
*/
void insertar_ultima(TABLE *tabla_principal, TABLE *tabla_auxiliar)
{

    TABLE *tabla_principal2 = tabla_principal;

    while (tabla_principal2->sig != NULL)
    {
        tabla_principal2 = tabla_principal2->sig;
    }

    tabla_principal2->sig = tabla_auxiliar;
    tabla_auxiliar->ant = tabla_principal2;
}

/*
Esta funcion se encarga de comprobar que no haya campos duplicados.
*/
int ver_duplicados(char **cadena, int num_arg)
{
    int i, j;

    for (i = 0; i < num_arg; i++)
    {
        for (j = i + 1; j < num_arg; j++)
        {
            if (strcmp(cadena[i], cadena[j]) == 0)
            {
                printf("Error, no se pueden repetir los campos\n");
                return 1;
            }
        }
    }

    return 0;
}

/*
Esta funcion se encarga de comprobar si una tabla existe.
*/
int entrar_tabla(char *nombre_bd, char *nombre_tabla)
{

    DIR *dir;

    char aux1[11] = ".tab";
    char aux2[5] = "\\";
    char aux3[50] = "";

    strcpy(aux3, nombre_bd);
    strcat(aux3, aux2);
    strcat(aux3, nombre_tabla);
    strcat(aux3, aux1);

    if (access(aux3, F_OK) == 0)
    {
        if (dir = opendir(nombre_bd))
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

/*
Esta funcion se encarga de comprobar las comillas, Si comillas activadas =1 y desactivadas =0
*/
char *MiStrTok(char *chorizo, char *sep, int comillas)
{

    static char *pt1 = NULL;
    static char *pt2 = NULL;
    static char *ptFinal;

    if (chorizo != NULL)
    {
        pt1 = chorizo;
        ptFinal = pt1 + strlen(chorizo);
    }
    else
    {
        pt1 = pt2 + 1;
    }

    if (comillas == 1)
    {

        while (pt1[0] != '\0' && pt1 < ptFinal && strchr(sep, pt1[0]) != NULL && pt1[0] != '"')
        {
            pt1++;
        }

        if (pt1[0] == '"')
        {
            pt1++;
            pt2 = pt1 + 1;
            while (pt2[0] != '\0' && pt2[0] != '"')
            {

                pt2++;
            }

            if (pt2[0] == '\0')
            {
                pt1 = pt2 = ptFinal = NULL;
                return NULL;
            }
            else
            {
                pt2[0] = '\0';
                return pt1;
            }
        }
    }
    else
    {

        while (pt1[0] != '\0' && pt1 < ptFinal && strchr(sep, pt1[0]) != NULL)
        {
            pt1++;
        }
    }

    if (pt1 >= ptFinal)
    {
        pt1 = pt2 = ptFinal = NULL;
        return NULL;
    }

    pt2 = pt1 + 1;

    while (pt2[0] != '\0' && strchr(sep, pt2[0]) == NULL)
    {
        pt2++;
    }

    pt2[0] = '\0';

    return pt1;
}

/*
Esta funcion se encarga de buscar las tablas
*/
TABLE *buscar_tabla(char *nombre_tabla, TABLE *tabla)
{
    TABLE *tabla_aux = tabla;

    if (tabla_aux != NULL)
    {
        while (tabla_aux != NULL)
        {
            if (strcmp(tabla_aux->nombre, nombre_tabla) == 0)
            {
                return tabla_aux;
            }

            tabla_aux = tabla_aux->sig;
        }
    }
    else
    {
        return tabla_aux;
    }
}

/*
Esta funcion se encarga de validar números, 1= OK y 0 =MAL
*/
int validar_decimal(char *cadena)
{

    int i, p = 0;

    if (cadena[0] == '+' || cadena[0] == '-')
    {
        i = 1;
    }
    else
    {
        i = 0;
    }

    if (strlen(cadena) == 1 && i == 1) // Solo escribe + o -
    {
        return 0;
    }

    if (strlen(cadena) == 0)
    {
        return 0;
    }

    if (i == 1 && strlen(cadena) == 2 && cadena[1] == '.') // Caso de +. o -.
    {
        return 0;
    }

    if (strlen(cadena) == 1 && i == 0 && cadena[0] == '.')
    {
        return 0;
    }

    for (i; i < strlen(cadena); i++)
    {
        if (cadena[i] < '0' || cadena[i] > '9') // Que no sea un numero la posición i
        {
            if (cadena[i] == '.')
            {
                p++;
            }
            else
            {
                return 0;
            }
        }

        if (p > 1) // Que el numero tenga mas de una coma
        {
            return 0;
        }
    }

    return 1; // correcto
}

/*
Esta funcion se encarga de validar fecha
*/
int validar_fecha_DMA(char *cadena)
{
    int i, d, m, a;

    if (strlen(cadena) != 10)
    {
        return 0;
    }

    for (i = 0; i < 10; i++)
    {

        if (i != 2 && i != 5)
        {

            if (cadena[i] < '0' || cadena[i] > '9')
            {
                return 0;
            }
        }
    }

    if ((cadena[2] != '/' && cadena[5] != '/') && (cadena[2] != '-' && cadena[5] != '-'))
    {

        return 0;
    }

    d = (cadena[0] - '0') * 10 + (cadena[1] - '0');
    m = (cadena[3] - '0') * 10 + (cadena[4] - '0');
    a = (cadena[6] - '0') * 1000 + (cadena[7] - '0') * 100 + (cadena[8] - '0') * 10 + (cadena[9] - '0');

    if (d < 1 || m < 1 || a < 1000 || d > 31 || m > 12 || a > 9999)
    {

        return 0;
    }
    else if ((m == 4 || m == 6 || m == 9 || m == 11) && d > 30)
    {

        return 0;
    }
    else if (m == 2 && a % 4 == 0 && d > 29)
    { // año bisiesto
        return 0;
    }
    else if (m == 2 && a % 4 != 0 && d > 28)
    { // año no bisiesto
        return 0;
    }

    return 1;
}

/*
Esta funcion se encarga de inicializar una linea
*/
ROW *inicializar_linea(char **valores)
{

    ROW *linea = malloc(sizeof(ROW));

    if (linea == NULL)
    {
        return NULL;
    }

    linea->valores = valores;
    linea->sig = NULL,
    linea->ant = NULL;

    return linea;
}

void insertar_ultima_linea(ROW *linea_principal, ROW *linea_auxiliar)
{

    ROW *fila = linea_principal;

    while (fila->sig != NULL)
    {
        fila = fila->sig;
    }

    fila->sig = linea_auxiliar;
    linea_auxiliar->ant = fila;
    linea_auxiliar->sig = NULL;
}

/*
Esta funcion se encarga de actualizar lineas
*/
void actualizar_lineas(char *nombre_bd, ROW *linea, TABLE *tabla)
{

    DIR *dir;

    char aux1[11] = ".tab";
    char aux2[50] = "\\";
    char aux3[50] = "";

    strcpy(aux3, nombre_bd);
    strcat(aux3, aux2);
    strcat(aux3, tabla->nombre);
    strcat(aux3, aux1);

    if (dir = opendir(nombre_bd))
    {

        FILE *fichero;
        fichero = fopen(aux3, "w");

        if (fichero != NULL)
        {

            // escribir todas las lineas
            while (linea != NULL)
            {

                for (int i = 0; i < tabla->numCampos; i++)
                {

                    if (tabla->tipos[i] == TEXT && i == tabla->numCampos - 1)
                    {
                        fprintf(fichero, "\"%s\"\n", linea->valores[i]);
                    }
                    else if (tabla->tipos[i] == TEXT)
                    {
                        fprintf(fichero, "\"%s\", ", linea->valores[i]);
                    }
                    else if (tabla->numCampos - 1 == i)
                    {
                        fprintf(fichero, "%s\n", linea->valores[i]);
                    }
                    else
                    {
                        fprintf(fichero, "%s, ", linea->valores[i]);
                    }
                }

                if (linea->sig == NULL)
                {
                    break;
                }

                linea = linea->sig;
            }

            fclose(fichero);
        }
        else
        {
            printf("algo salio mal\n");
        }
    }
    else
    {
        printf("Extra%co caso no nos deja entrar a la carpeta\n", 164);
    }
}

/*
Esta funcion se encarga de devolvernos la tabla principal
*/
TABLE *cargar_tablas(char *nombre_bd)
{

    TABLE *tabla = NULL, *tabla_nueva;
    FILE *meta;

    char aux1[25] = "\\meta.db";
    char aux2[100] = "";
    char linea[1000], valor[100];

    int num_tablas, num_campos, i, j;
    char *nombre_tabla;
    char **nombre_campos;
    TYPE *tipos;

    strcpy(aux2, nombre_bd);
    strcat(aux2, aux1);

    meta = fopen(aux2, "r");

    if (meta == NULL)
    {
        printf("Error al abrir el meta\n");
        return 0;
    }

    fgets(linea, 1000, meta); // Leemos los primeros 1000 caracteres

    // Funcion para obtener campos
    obtener_campo(linea, ":", 1, valor);
    Trim(valor);
    num_tablas = atoi(valor); // Lo convierte en numero

    for (i = 0; i < num_tablas; i++)
    {
        fgets(linea, 1000, meta);
        fgets(linea, 1000, meta);

        obtener_campo(linea, ":", 0, valor);

        Trim(valor); // Quita los espacios
        nombre_tabla = strdup(valor);
        obtener_campo(linea, ":", 1, valor);
        Trim(valor);
        num_campos = atoi(valor);

        nombre_campos = malloc(sizeof(char *) * num_campos);
        tipos = malloc(sizeof(TYPE *) * num_campos);

        for (j = 0; j < num_campos; j++)
        {
            fgets(linea, 1000, meta);            // cogemos la linea
            obtener_campo(linea, ":", 0, valor); // vemos el nombre del campo
            Trim(valor);
            nombre_campos[j] = strdup(valor);    // lo copiamos en la caja de nombre_campo que es un puntero doble
            obtener_campo(linea, ":", 1, valor); // cogemos el tipo
            Trim(valor);
            tipos[j] = validar_tipo(valor); // lo metemos en la caja de tipos
            // printf("--------------\n");
            // printf("%d\n", tipos[j]);
            // printf("--------------\n");
        }

        tabla_nueva = inicializar_tabla(num_campos, nombre_tabla, nombre_campos, tipos);

        if (tabla == NULL)
        {
            tabla = tabla_nueva;
        }
        else
        {
            insertar_ultima(tabla, tabla_nueva);
        }
    }

    fclose(meta);
    return tabla;
}

/*
Esta funcion se encarga de quitar los espacios en blanco
*/
void Trim(char *cad)
{
    int c, i, j, n = strlen(cad);

    if (cad == NULL || n < 1)
        return;
    n--;
    while (n >= 0 && ((c = cad[n]) == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '"'))
        n--;
    cad[n + 1] = '\0';
    i = 0;
    while (i <= n && ((c = cad[i]) == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '"'))
        i++;
    for (j = 0; j <= n - i + 1; j++)
        cad[j] = cad[j + i];
}

/*
Esta funcion se encarga de extraer el campo de la linea
*/
char *obtener_campo(char *linea, char *separador, int campo, char *valor)
{
    int i, tamaño_separador = 1;
    char *p1, *p2;

    if (linea == NULL || linea[0] == '\0')
    {
        return NULL;
    }

    p1 = linea;
    p2 = strstr(linea, separador);

    for (i = 0; i < campo; i++)
    {
        p1 = p2 + tamaño_separador;
        p2 = strstr(p1, separador);
    }

    if (p2 == NULL)
    {
        strcpy(valor, p1);
    }
    else
    {
        strncpy(valor, p1, p2 - p1);
        valor[p2 - p1] = '\0';
    }

    return valor;
}

/*
Esta funcion se encarga de cargar las lineas
*/
ROW *cargar_lineas(char *nombre_bd, char *nombre_tabla, int num_campos, TYPE *tipos)
{
    char aux1[5] = ".tab";
    char aux2[5] = "\\";
    char aux3[100] = "";
    FILE *fichero;
    char linea[1000];
    int i, comillas;
    long final;
    char *token, *linea_aux;
    char **valores;
    ROW *fila = NULL, *fila_nueva;

    strcpy(aux3, nombre_bd);
    strcat(aux3, aux2);
    strcat(aux3, nombre_tabla);
    strcat(aux3, aux1);

    if (access(aux3, F_OK) == 0)
    { // si podemos acceder a la ruta entramos

        fichero = fopen(aux3, "r"); // abrimos el fichero en modo lectura

        fseek(fichero, 0, SEEK_END); // situa el cursor en la ultima posicion

        if (ftell(fichero) == 0) // dime donde esta el cursor en el fichero, y si esta en la posicion 0 entramos
        {
            fclose(fichero);
            return fila;
        }

        final = ftell(fichero);      // dime donde esta la posicion del fichero
        fseek(fichero, 0, SEEK_SET); // mueve el cursor al principio del fichero, es decir, a la posicion 0

        while (final != ftell(fichero))
        { // mientras el cursor no llegue al final del fichero

            fgets(linea, 1000, fichero); // cogemos la linea

            valores = malloc(sizeof(char *) * num_campos); //  reservamos memoria del tamanyo de los num_campos

            for (i = 0; i < num_campos; i++)
            {

                comillas = (tipos[i] == TEXT); // si hay un campo de tipo texto comillas valdra 1

                if (i == 0)
                {
                    linea_aux = linea;
                }
                else
                {
                    linea_aux = NULL;
                }
                token = MiStrTok(linea_aux, ",\n", comillas); // dividimos la linea en tokens con las comillas activadas
                valores[i] = strdup(token);
                Trim(valores[i]);
            }

            fila_nueva = inicializar_linea(valores); // creamos la estructura de la linea

            if (fila == NULL)
            { // si es la primera linea
                fila = fila_nueva;
            }
            else
            {
                insertar_ultima_linea(fila, fila_nueva); // insertamos la linea la ultima
            }
        }
    }
    else
    {
        printf("Error, no hemos podido acceder al directorio\n");
        return 0;
    }

    fclose(fichero);
    return fila;
}

/*
Esta funcion se encarga de liberar memoria de las lineas
*/
void liberar_memoria_lineas(ROW *linea_principal, int num_campos)
{
    ROW *linea_sig;
    int i;

    if (linea_principal == NULL)
    {
        return;
    }

    while (linea_principal != NULL)
    {
        linea_sig = linea_principal->sig;

        for (i = 0; i < num_campos; i++)
        {
            free(linea_principal->valores[i]);
        }

        free(linea_principal->valores);
        free(linea_principal);

        linea_principal = linea_sig;
    }
}

/*
Esta funcion se encarga de liberar memoria de las tablas
*/
void liberar_memoria_tablas(TABLE *tabla_principal)
{
    TABLE *tabla_sig;
    int i;

    if (tabla_principal == NULL)
    {
        return;
    }

    while (tabla_principal != NULL)
    {
        tabla_sig = tabla_principal->sig;

        for (i = 0; i < tabla_principal->numCampos; i++)
        {
            free(tabla_principal->campos[i]);
        }

        free(tabla_principal->campos);
        free(tabla_principal->tipos);
        free(tabla_principal);

        tabla_principal = tabla_sig;
    }
}

/*
Esta funcion se encarga de imprimir las cabeceras
*/
void imprimir_cabeceras(TABLE *tabla_auxiliar)
{
    int i;

    for (i = 0; i < tabla_auxiliar->numCampos; i++)
    {
        printf("%s\t", tabla_auxiliar->campos[i]);
    }
    printf("\n");
}

/*
Esta funcion se encarga de imprimir todo
*/
void imprimir_todo(ROW *linea_principal, int num_campos)
{
    ROW *linea_auxiliar = linea_principal;
    int i, contador = 1;

    if (linea_auxiliar == NULL)
    {
        printf("No hay datos en la tabla\n");
    }
    else
    {
        while (linea_auxiliar != NULL)
        {
            printf("%d - ", contador);

            for (i = 0; i < num_campos - 1; i++)
            {
                printf("%s, ", linea_auxiliar->valores[i]);
            }

            printf("%s \n", linea_auxiliar->valores[num_campos - 1]);
            contador++;
            linea_auxiliar = linea_auxiliar->sig;
        }
    }
}

/*
Esta funcion se encarga de imprimir ascendentemente
*/
void imprimir_ascendete(ROW *linea_principal, int num_campos, TABLE *tabla_auxiliar)
{
    int i, contador = 1;
    ROW *linea_auxiliar = linea_principal;

    if (linea_auxiliar == NULL)
    {
        printf("No hay datos en la tabla\n");
    }
    else
    {
        // Ordenar ascendentemente
        ordenar_asc(linea_auxiliar, tabla_auxiliar, 0); // Si lo que deseamos modificar no es el primer campo, debemos modificar el 0.

        while (linea_auxiliar != NULL)
        {
            printf("%d - ", contador);

            for (i = 0; i < num_campos - 1; i++)
            {
                printf("%s, ", linea_auxiliar->valores[i]);
            }

            printf("%s \n", linea_auxiliar->valores[num_campos - 1]);
            contador++;
            linea_auxiliar = linea_auxiliar->sig;
        }
    }
}

/*
Esta funcion se encarga de imprimir descendentemente
*/
void imprimir_descendente(ROW *linea_principal, int num_campos, TABLE *tabla_auxiliar)
{
    int i, contador = 1;
    ROW *linea_auxiliar = linea_principal;

    if (linea_auxiliar == NULL)
    {
        printf("No hay datos en la tabla\n");
    }
    else
    {
        // Ordenar descendentemente
        ordenar_des(linea_auxiliar, tabla_auxiliar, 0); // Si lo que deseamos modificar no es el primer campo, debemos modificar el 0.

        while (linea_auxiliar != NULL)
        {
            printf("%d - ", contador);

            for (i = 0; i < num_campos - 1; i++)
            {
                printf("%s, ", linea_auxiliar->valores[i]);
            }

            printf("%s \n", linea_auxiliar->valores[num_campos - 1]);
            contador++;
            linea_auxiliar = linea_auxiliar->sig;
        }
    }
}

/*
Esta funcion se encarga de ordenar ascendentemente
*/
void ordenar_asc(ROW *linea_principal, TABLE *tabla_auxiliar, int orden_campo)
{
    ROW *i, *j;

    if (tabla_auxiliar->tipos[orden_campo] == NUM)
    {
        for (i = linea_principal; i != NULL; i = i->sig)
        {
            for (j = i; j != NULL; j = j->sig)
            {
                if (atof(i->valores[orden_campo]) > atof(j->valores[orden_campo]))
                {
                    intercambiar_valores(i, j);
                }
            }
        }
    }
    else if (tabla_auxiliar->tipos[orden_campo] == DATE)
    {
        for (i = linea_principal; i != NULL; i = i->sig)
        {
            for (j = i; j != NULL; j = j->sig)
            {
                if (fecha_to_number_DMA(i->valores[orden_campo]) > fecha_to_number_DMA(j->valores[orden_campo]))
                {
                    intercambiar_valores(i, j);
                }
            }
        }
    }
    else if (tabla_auxiliar->tipos[orden_campo] == EMAIL)
    {
        for (i = linea_principal; i != NULL; i = i->sig)
        {
            for (j = i; j != NULL; j = j->sig)
            {
                if (strcmp(i->valores[orden_campo], j->valores[orden_campo]) > 0)
                {

                    intercambiar_valores(i, j);
                }
            }
        }
    }
    else if (tabla_auxiliar->tipos[orden_campo] == TEXT)
    {
        for (i = linea_principal; i != NULL; i = i->sig)
        {
            for (j = i; j != NULL; j = j->sig)
            {
                if (strcmp(i->valores[orden_campo], j->valores[orden_campo]) > 0)
                {
                    intercambiar_valores(i, j);
                }
            }
        }
    }
}

/*
Esta funcion se encarga de ordenar descendentemente
*/
void ordenar_des(ROW *linea_principal, TABLE *tabla_auxiliar, int orden_campo)
{
    ROW *i, *j;

    if (tabla_auxiliar->tipos[orden_campo] == NUM)
    {
        for (i = linea_principal; i != NULL; i = i->sig)
        {
            for (j = i; j != NULL; j = j->sig)
            {
                if (atof(i->valores[orden_campo]) < atof(j->valores[orden_campo]))
                {

                    intercambiar_valores(i, j);
                }
            }
        }
    }
    else if (tabla_auxiliar->tipos[orden_campo] == DATE)
    {
        for (i = linea_principal; i != NULL; i = i->sig)
        {
            for (j = i; j != NULL; j = j->sig)
            {
                if (fecha_to_number_DMA(i->valores[orden_campo]) < fecha_to_number_DMA(j->valores[orden_campo]))
                {
                    intercambiar_valores(i, j);
                }
            }
        }
    }
    else if (tabla_auxiliar->tipos[orden_campo] == EMAIL)
    {
        for (i = linea_principal; i != NULL; i = i->sig)
        {
            for (j = i; j != NULL; j = j->sig)
            {
                if (strcmp(i->valores[orden_campo], j->valores[orden_campo]) < 0)
                {

                    intercambiar_valores(i, j);
                }
            }
        }
    }
    else if (tabla_auxiliar->tipos[orden_campo] == TEXT)
    {
        for (i = linea_principal; i != NULL; i = i->sig)
        {
            for (j = i; j != NULL; j = j->sig)
            {
                if (strcmp(i->valores[orden_campo], j->valores[orden_campo]) < 0)
                {
                    intercambiar_valores(i, j);
                }
            }
        }
    }
}

/*
Esta funcion se encarga de intercambiar los valores de la linea_principal
*/
void intercambiar_valores(ROW *i, ROW *j)
{
    char **aux;

    aux = i->valores;
    i->valores = j->valores;
    j->valores = aux;
}

/*
Esta funcion se encarga de convertir una fecha en numero
*/
int fecha_to_number_DMA(char *cad)
{
    int res, dia, mes, anyo;
    dia = (cad[0] - '0') * 10 + (cad[1] - '0');
    mes = (cad[3] - '0') * 10 + (cad[4] - '0');
    anyo = (cad[6] - '0') * 1000 + (cad[7] - '0') * 100 + (cad[8] - '0') * 10 + (cad[9] - '0');
    res = dia;

    if (mes == 1)
        res = res;
    else if (mes == 2)
        res = res + 31;
    else if (mes == 3 && (anyo % 4) != 0)
        res = res + 59;
    else if (mes == 3 && (anyo % 4) == 0)
        res = res + 60;
    else if (mes == 4 && (anyo % 4) != 0)
        res = res + 90;
    else if (mes == 4 && (anyo % 4) == 0)
        res = res + 91;
    else if (mes == 5 && (anyo % 4) != 0)
        res = res + 120;
    else if (mes == 5 && (anyo % 4) == 0)
        res = res + 121;
    else if (mes == 6 && (anyo % 4) != 0)
        res = res + 151;
    else if (mes == 6 && (anyo % 4) == 0)
        res = res + 152;
    else if (mes == 7 && (anyo % 4) != 0)
        res = res + 181;
    else if (mes == 7 && (anyo % 4) == 0)
        res = res + 182;
    else if (mes == 8 && (anyo % 4) != 0)
        res = res + 212;
    else if (mes == 8 && (anyo % 4) == 0)
        res = res + 213;
    else if (mes == 9 && (anyo % 4) != 0)
        res = res + 243;
    else if (mes == 9 && (anyo % 4) == 0)
        res = res + 244;
    else if (mes == 10 && (anyo % 4) != 0)
        res = res + 273;
    else if (mes == 10 && (anyo % 4) == 0)
        res = res + 274;
    else if (mes == 11 && (anyo % 4) != 0)
        res = res + 304;
    else if (mes == 11 && (anyo % 4) == 0)
        res = res + 305;
    else if (mes == 12 && (anyo % 4) != 0)
        res = res + 334;
    else if (mes == 12 && (anyo % 4) == 0)
        res = res + 335;

    res = (res + (anyo - 1) * 365);
    res = (res + (anyo - 1) / 4);

    return res;
}

/*
Esta funcion se encarga de imprimir el valor del campo para comprobaciones
*/
void imprimir_valor_campo(int num_campos, ROW *linea_principal, TYPE tipo, char *valor, char *simbolo, int posicion)
{
    int i, contador = 1;
    ROW *linea_auxiliar = linea_principal;

    if (linea_auxiliar == NULL)
    {
        printf("No hay datos en la tabla\n");
    }
    else
    {
        if (tipo == NUM)
        {
            if (strcmp(simbolo, "=") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (atof(valor) == atof(linea_auxiliar->valores[posicion]))
                    {
                        printf("%d - ", contador);

                        for (i = 0; i < num_campos - 1; i++)
                        {
                            printf("%s, ", linea_auxiliar->valores[i]);
                        }

                        printf("%s\n", linea_auxiliar->valores[num_campos - 1]);
                        contador++;
                    }
                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, ">") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (atof(linea_auxiliar->valores[posicion]) > atof(valor))
                    {
                        printf("%d - ", contador);

                        for (i = 0; i < num_campos - 1; i++)
                        {
                            printf("%s, ", linea_auxiliar->valores[i]);
                        }

                        printf("%s\n", linea_auxiliar->valores[num_campos - 1]);
                        contador++;
                    }
                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, "<") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (atof(linea_auxiliar->valores[posicion]) < atof(valor))
                    {
                        printf("%d - ", contador);

                        for (i = 0; i < num_campos - 1; i++)
                        {
                            printf("%s, ", linea_auxiliar->valores[i]);
                        }

                        printf("%s\n", linea_auxiliar->valores[num_campos - 1]);
                        contador++;
                    }
                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else
            {
                printf("Error, los simbolos son inadecuados\n");
                return;
            }

            if (contador == 1)
            {
                printf("No hay datos para mostrar\n");
            }
        }

        else if (tipo == DATE)
        {
            if (strcmp(simbolo, "=") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (fecha_to_number_DMA(valor) == fecha_to_number_DMA(linea_auxiliar->valores[posicion]))
                    {
                        printf("%d - ", contador);

                        for (i = 0; i < num_campos - 1; i++)
                        {
                            printf("%s, ", linea_auxiliar->valores[i]);
                        }

                        printf("%s\n", linea_auxiliar->valores[num_campos - 1]);
                        contador++;
                    }
                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, ">") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (fecha_to_number_DMA(linea_auxiliar->valores[posicion]) > fecha_to_number_DMA(valor))
                    {
                        printf("%d - ", contador);

                        for (i = 0; i < num_campos - 1; i++)
                        {
                            printf("%s, ", linea_auxiliar->valores[i]);
                        }

                        printf("%s\n", linea_auxiliar->valores[num_campos - 1]);
                        contador++;
                    }
                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, "<") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (fecha_to_number_DMA(linea_auxiliar->valores[posicion]) < fecha_to_number_DMA(valor))
                    {
                        printf("%d - ", contador);

                        for (i = 0; i < num_campos - 1; i++)
                        {
                            printf("%s, ", linea_auxiliar->valores[i]);
                        }

                        printf("%s\n", linea_auxiliar->valores[num_campos - 1]);
                        contador++;
                    }
                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else
            {
                printf("Error, los simbolos son inadecuados\n");
                return;
            }
        }

        else if (tipo == EMAIL)
        {
            if (strcmp(simbolo, "=") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (strcmp(valor, linea_auxiliar->valores[posicion]) == 0) // si en el examen nos pide hacer desigual cambiamos el simbolo de == -> != y el resto todo lo mismo
                    {
                        printf("%d - ", contador);

                        for (i = 0; i < num_campos - 1; i++)
                        {
                            printf("%s, ", linea_auxiliar->valores[i]);
                        }

                        printf("%s\n", linea_auxiliar->valores[num_campos - 1]);
                        contador++;
                    }
                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, ">") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (strcmp(linea_auxiliar->valores[posicion], valor) > 0)
                    {
                        printf("%d - ", contador);

                        for (i = 0; i < num_campos - 1; i++)
                        {
                            printf("%s, ", linea_auxiliar->valores[i]);
                        }

                        printf("%s\n", linea_auxiliar->valores[num_campos - 1]);
                        contador++;
                    }
                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, "<") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (strcmp(linea_auxiliar->valores[posicion], valor) < 0)
                    {
                        printf("%d - ", contador);

                        for (i = 0; i < num_campos - 1; i++)
                        {
                            printf("%s, ", linea_auxiliar->valores[i]);
                        }

                        printf("%s\n", linea_auxiliar->valores[num_campos - 1]);
                        contador++;
                    }
                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else
            {
                printf("Error, los simbolos son inadecuados\n");
                return;
            }
        }

        else if (tipo == TEXT)
        {
            if (strcmp(simbolo, "=") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (strcmp(valor, linea_auxiliar->valores[posicion]) == 0)
                    {
                        printf("%d - ", contador);

                        for (i = 0; i < num_campos - 1; i++)
                        {
                            printf("%s, ", linea_auxiliar->valores[i]);
                        }

                        printf("%s\n", linea_auxiliar->valores[num_campos - 1]);
                        contador++;
                    }
                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, ">") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (strcmp(linea_auxiliar->valores[posicion], valor) > 0)
                    {
                        printf("%d - ", contador);

                        for (i = 0; i < num_campos - 1; i++)
                        {
                            printf("%s, ", linea_auxiliar->valores[i]);
                        }

                        printf("%s\n", linea_auxiliar->valores[num_campos - 1]);
                        contador++;
                    }
                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, "<") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (strcmp(linea_auxiliar->valores[posicion], valor) < 0)
                    {
                        printf("%d - ", contador);

                        for (i = 0; i < num_campos - 1; i++)
                        {
                            printf("%s, ", linea_auxiliar->valores[i]);
                        }

                        printf("%s\n", linea_auxiliar->valores[num_campos - 1]);
                        contador++;
                    }
                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else
            {
                printf("Error, los simbolos son inadecuados\n");
                return;
            }
        }
    }
}

//!----------------------------DICIEMBRE-----------------------------
/*
Esta funcion se encarga de mostrar la tablas que hay dentro
*/
void mostrar_tablas(TABLE *tabla_principal)
{
    TABLE *tabla_aux = tabla_principal;

    int num_tablas = 0;

    while (tabla_aux != NULL)
    {
        num_tablas++;

        printf("%s\n", tabla_aux->nombre);

        if (tabla_aux->sig == NULL)
        {
            break;
        }
        tabla_aux = tabla_aux->sig;
    }

    if (num_tablas == 0)
    {
        printf("No hay tablas\n");
    }
}

/*
Esta funcion se encarga de mostrar las bases de datos que existen
*/
void mostrar_bd()
{
    DIR *dir;
    struct dirent *d;
    int contador = 0;

    dir = opendir("."); // Coloca en dir todo lo que esta en la carpeta que nos encontramos

    if (dir == NULL)
    {
        printf("No hay bases de datos\n");
    }
    else
    {
        while ((d = readdir(dir)) != NULL)
        {
            if (strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0)
            {
                if (comprobar_caracteres_bd(d->d_name) == 0)
                {
                    printf("%s\n", d->d_name);
                    contador++;
                }
            }
        }

        if (contador == 0)
        {
            printf("No hay bases de datos\n");
        }
    }

    closedir(dir);
}

/*
Esta funcion se encarga de comprobar los nombres de la BD
*/
int comprobar_caracteres_bd(char *nombre_bd)
{
    char copia[100] = "";
    int j = 0, errores = 0;

    strcpy(copia, nombre_bd);

    while (copia[j] != '\0')
    {
        if (copia[j] != '.')
        {
            j++;
        }
        else
        {
            errores++;
            break;
        }
    }

    return errores;
}

/*
Esta funcion se encarga de comprobar el email, 1 si es error y 0 si OK
*/
int validar_email(char *email)
{
    char copia[100] = "";
    int i = 0, errores = 0, punto = 0, arroba = 0;

    strcpy(copia, email);

    int g = strlen(copia);

    if (g < 5)
    {

        return 1;
    }

    if (copia[g - 1] == '.')
    {

        return 1;
    }

    if (copia[0] == '.' || copia[0] == '@')
    {

        errores++;
        return errores;
    }

    while (copia[i] != '\0')
    {

        if (copia[i] >= 'a' && copia[i] <= 'z')
        {

            i++;
        }
        else if (copia[i] >= 'A' && copia[i] <= 'Z')
        {

            i++;
        }
        else if (copia[i] >= '0' && copia[i] <= '9')
        {

            i++;
        }
        else if (copia[i] == '.')
        {
            if (arroba == 0)
            {
                errores++;
                return errores;
            }

            i++;
            punto++;
        }
        else if (copia[i] == '@')
        {

            if (punto != 0)
            {

                errores++;
                return errores;
            }

            i++;
            arroba++;
        }
        else
        {

            errores++;
            return errores;
        }
    }

    if (errores != 0 || arroba != 1 || punto != 1)
    {

        return 1;
    }

    return errores;
}

/*
Esta funcion se encarga de eliminar los campos deseados
*/
ROW *borrar_valor(int num_campos, ROW *linea, TYPE tipo, char *valor, char *simbolo, int posicion)
{
    int i, k;
    ROW *linea_aux = linea, *linea_sig, *linea_ant;

    if (linea == NULL)
    {
        printf("No hay valores a borrar\n");
    }
    else
    {

        if (tipo == NUM)
        {

            if (strcmp(simbolo, "=") == 0)
            {

                while (linea_aux != NULL)
                {

                    linea_sig = linea_aux->sig;
                    linea_ant = linea_aux->ant;

                    if (atof(valor) == atof(linea_aux->valores[posicion]))
                    {

                        if (linea_aux == linea && linea_aux->sig == NULL)
                        { // SER EL UNICO VALOR
                            linea = NULL;
                        }
                        else if (linea_aux == linea && linea_aux->ant == NULL)
                        { // SER EL PRIMER VALOR
                            linea = linea_aux->sig;
                            linea_sig->ant = NULL;
                            linea_aux->sig = NULL;
                        }
                        else if (linea_aux->sig == NULL)
                        { // SER EL ULTIMO VALOR
                            linea_ant->sig = NULL;
                            linea_aux->ant = NULL;
                        }
                        else
                        { // SER UN VALOR DE EN MEDIO

                            linea_ant->sig = linea_aux->sig; // linea_sig
                            linea_sig->ant = linea_aux->ant; // lineanterior
                        }

                        // FREE

                        for (i = 0; i < num_campos; i++)
                        {
                            free(linea_aux->valores[i]);
                        }

                        free(linea_aux->valores);
                        free(linea_aux);
                    }

                    linea_aux = linea_sig;
                }

                return linea;
            }
            else if (strcmp(simbolo, ">") == 0)
            {

                while (linea_aux != NULL)
                {

                    linea_sig = linea_aux->sig;
                    linea_ant = linea_aux->ant;

                    if (atof(linea_aux->valores[posicion]) > atof(valor))
                    {

                        if (linea_aux == linea && linea_aux->sig == NULL)
                        {
                            linea = NULL;
                        }
                        else if (linea_aux == linea && linea_aux->ant == NULL)
                        {
                            linea = linea_aux->sig;
                            linea_sig->ant = NULL;
                            linea_aux->sig = NULL;
                        }
                        else if (linea_aux->sig == NULL)
                        {
                            linea_ant->sig = NULL;
                            linea_aux->ant = NULL;
                        }
                        else
                        {
                            linea_ant->sig = linea_aux->sig;
                            linea_sig->ant = linea_aux->ant;
                        }

                        for (i = 0; i < num_campos; i++)
                        {
                            free(linea_aux->valores[i]);
                        }

                        free(linea_aux->valores);
                        free(linea_aux);
                    }

                    linea_aux = linea_sig;
                }

                return linea;
            }
            else if (strcmp(simbolo, "<") == 0)
            {

                while (linea_aux != NULL)
                {

                    linea_sig = linea_aux->sig;
                    linea_ant = linea_aux->ant;

                    if (atof(linea_aux->valores[posicion]) < atof(valor))
                    {

                        if (linea_aux == linea && linea_aux->sig == NULL)
                        {
                            linea = NULL;
                        }
                        else if (linea_aux == linea && linea_aux->ant == NULL)
                        {
                            linea = linea_aux->sig;
                            linea_sig->ant = NULL;
                            linea_aux->sig = NULL;
                        }
                        else if (linea_aux->sig == NULL)
                        {
                            linea_ant->sig = NULL;
                            linea_aux->ant = NULL;
                        }
                        else
                        {
                            linea_ant->sig = linea_aux->sig;
                            linea_sig->ant = linea_aux->ant;
                        }

                        for (i = 0; i < num_campos; i++)
                        {
                            free(linea_aux->valores[i]);
                        }

                        free(linea_aux->valores);
                        free(linea_aux);
                    }

                    linea_aux = linea_sig;
                }

                return linea;
            }
            else
            {
                printf("Error de simbolo\n");
            }
        }
        else if (tipo == DATE)
        {

            if (strcmp(simbolo, "=") == 0)
            {

                while (linea_aux != NULL)
                {

                    linea_sig = linea_aux->sig;
                    linea_ant = linea_aux->ant;

                    if (fecha_to_number_DMA(valor) == fecha_to_number_DMA(linea_aux->valores[posicion]))
                    {

                        if (linea_aux == linea && linea_aux->sig == NULL)
                        { // SER EL UNICO VALOR
                            linea = NULL;
                        }
                        else if (linea_aux == linea && linea_aux->ant == NULL)
                        { // SER EL PRIMER VALOR
                            linea = linea_aux->sig;
                            linea_sig->ant = NULL;
                            linea_aux->sig = NULL;
                        }
                        else if (linea_aux->sig == NULL)
                        { // SER EL ULTIMO VALOR
                            linea_ant->sig = NULL;
                            linea_aux->ant = NULL;
                        }
                        else
                        { // SER UN VALOR DE EN MEDIO

                            linea_ant->sig = linea_aux->sig; // linea_sig
                            linea_sig->ant = linea_aux->ant; // lineanterior
                        }

                        // FREE

                        for (i = 0; i < num_campos; i++)
                        {
                            free(linea_aux->valores[i]);
                        }

                        free(linea_aux->valores);
                        free(linea_aux);
                    }

                    linea_aux = linea_sig;
                }

                return linea;
            }
            else if (strcmp(simbolo, ">") == 0)
            {

                while (linea_aux != NULL)
                {

                    linea_sig = linea_aux->sig;
                    linea_ant = linea_aux->ant;

                    if (fecha_to_number_DMA(linea_aux->valores[posicion]) > fecha_to_number_DMA(valor))
                    {

                        if (linea_aux == linea && linea_aux->sig == NULL)
                        { // SER EL UNICO VALOR
                            linea = NULL;
                        }
                        else if (linea_aux == linea && linea_aux->ant == NULL)
                        { // SER EL PRIMER VALOR
                            linea = linea_aux->sig;
                            linea_sig->ant = NULL;
                            linea_aux->sig = NULL;
                        }
                        else if (linea_aux->sig == NULL)
                        { // SER EL ULTIMO VALOR
                            linea_ant->sig = NULL;
                            linea_aux->ant = NULL;
                        }
                        else
                        { // SER UN VALOR DE EN MEDIO

                            linea_ant->sig = linea_aux->sig; // linea_sig
                            linea_sig->ant = linea_aux->ant; // lineanterior
                        }

                        // FREE

                        for (i = 0; i < num_campos; i++)
                        {
                            free(linea_aux->valores[i]);
                        }

                        free(linea_aux->valores);
                        free(linea_aux);
                    }

                    linea_aux = linea_sig;
                }

                return linea;
            }
            else if (strcmp(simbolo, "<") == 0)
            {

                while (linea_aux != NULL)
                {

                    linea_sig = linea_aux->sig;
                    linea_ant = linea_aux->ant;

                    if (fecha_to_number_DMA(linea_aux->valores[posicion]) < fecha_to_number_DMA(valor))
                    {

                        if (linea_aux == linea && linea_aux->sig == NULL)
                        { // SER EL UNICO VALOR
                            linea = NULL;
                        }
                        else if (linea_aux == linea && linea_aux->ant == NULL)
                        { // SER EL PRIMER VALOR
                            linea = linea_aux->sig;
                            linea_sig->ant = NULL;
                            linea_aux->sig = NULL;
                        }
                        else if (linea_aux->sig == NULL)
                        { // SER EL ULTIMO VALOR
                            linea_ant->sig = NULL;
                            linea_aux->ant = NULL;
                        }
                        else
                        { // SER UN VALOR DE EN MEDIO

                            linea_ant->sig = linea_aux->sig; // linea_sig
                            linea_sig->ant = linea_aux->ant; // lineanterior
                        }

                        // FREE

                        for (i = 0; i < num_campos; i++)
                        {
                            free(linea_aux->valores[i]);
                        }

                        free(linea_aux->valores);
                        free(linea_aux);
                    }

                    linea_aux = linea_sig;
                }

                return linea;
            }
            else
            {
                printf("Error de simbolo\n");
            }
        }
        else if (tipo == EMAIL)
        {

            if (strcmp(simbolo, "=") == 0)
            {

                while (linea_aux != NULL)
                {

                    linea_sig = linea_aux->sig;
                    linea_ant = linea_aux->ant;

                    if (strcmp(valor, linea_aux->valores[posicion]) == 0)
                    {

                        if (linea_aux == linea && linea_aux->sig == NULL)
                        { // SER EL UNICO VALOR
                            linea = NULL;
                        }
                        else if (linea_aux == linea && linea_aux->ant == NULL)
                        { // SER EL PRIMER VALOR
                            linea = linea_aux->sig;
                            linea_sig->ant = NULL;
                            linea_aux->sig = NULL;
                        }
                        else if (linea_aux->sig == NULL)
                        { // SER EL ULTIMO VALOR
                            linea_ant->sig = NULL;
                            linea_aux->ant = NULL;
                        }
                        else
                        { // SER UN VALOR DE EN MEDIO

                            linea_ant->sig = linea_aux->sig; // linea_sig
                            linea_sig->ant = linea_aux->ant; // lineanterior
                        }

                        // FREE

                        for (i = 0; i < num_campos; i++)
                        {
                            free(linea_aux->valores[i]);
                        }

                        free(linea_aux->valores);
                        free(linea_aux);
                    }

                    linea_aux = linea_sig;
                }

                return linea;
            }
            else if (strcmp(simbolo, ">") == 0)
            {

                while (linea_aux != NULL)
                {

                    linea_sig = linea_aux->sig;
                    linea_ant = linea_aux->ant;

                    if (strcmp(linea_aux->valores[posicion], valor) > 0)
                    {

                        if (linea_aux == linea && linea_aux->sig == NULL)
                        { // SER EL UNICO VALOR
                            linea = NULL;
                        }
                        else if (linea_aux == linea && linea_aux->ant == NULL)
                        { // SER EL PRIMER VALOR
                            linea = linea_aux->sig;
                            linea_sig->ant = NULL;
                            linea_aux->sig = NULL;
                        }
                        else if (linea_aux->sig == NULL)
                        { // SER EL ULTIMO VALOR
                            linea_ant->sig = NULL;
                            linea_aux->ant = NULL;
                        }
                        else
                        { // SER UN VALOR DE EN MEDIO

                            linea_ant->sig = linea_aux->sig; // linea_sig
                            linea_sig->ant = linea_aux->ant; // lineanterior
                        }

                        // FREE

                        for (i = 0; i < num_campos; i++)
                        {
                            free(linea_aux->valores[i]);
                        }

                        free(linea_aux->valores);
                        free(linea_aux);
                    }

                    linea_aux = linea_sig;
                }

                return linea;
            }
            else if (strcmp(simbolo, "<") == 0)
            {

                while (linea_aux != NULL)
                {

                    linea_sig = linea_aux->sig;
                    linea_ant = linea_aux->ant;

                    if (strcmp(linea_aux->valores[posicion], valor) < 0)
                    {

                        if (linea_aux == linea && linea_aux->sig == NULL)
                        { // SER EL UNICO VALOR
                            linea = NULL;
                        }
                        else if (linea_aux == linea && linea_aux->ant == NULL)
                        { // SER EL PRIMER VALOR
                            linea = linea_aux->sig;
                            linea_sig->ant = NULL;
                            linea_aux->sig = NULL;
                        }
                        else if (linea_aux->sig == NULL)
                        { // SER EL ULTIMO VALOR
                            linea_ant->sig = NULL;
                            linea_aux->ant = NULL;
                        }
                        else
                        { // SER UN VALOR DE EN MEDIO

                            linea_ant->sig = linea_aux->sig; // linea_sig
                            linea_sig->ant = linea_aux->ant; // lineanterior
                        }

                        // FREE

                        for (i = 0; i < num_campos; i++)
                        {
                            free(linea_aux->valores[i]);
                        }

                        free(linea_aux->valores);
                        free(linea_aux);
                    }

                    linea_aux = linea_sig;
                }

                return linea;
            }
            else
            {
                printf("Error de simbolo\n");
            }
        }
        else if (tipo == TEXT)
        {

            if (strcmp(simbolo, "=") == 0)
            {

                while (linea_aux != NULL)
                {

                    linea_sig = linea_aux->sig;
                    linea_ant = linea_aux->ant;

                    if (strcmp(valor, linea_aux->valores[posicion]) == 0)
                    {

                        if (linea_aux == linea && linea_aux->sig == NULL)
                        { // SER EL UNICO VALOR
                            linea = NULL;
                        }
                        else if (linea_aux == linea && linea_aux->ant == NULL)
                        { // SER EL PRIMER VALOR
                            linea = linea_aux->sig;
                            linea_sig->ant = NULL;
                            linea_aux->sig = NULL;
                        }
                        else if (linea_aux->sig == NULL)
                        { // SER EL ULTIMO VALOR
                            linea_ant->sig = NULL;
                            linea_aux->ant = NULL;
                        }
                        else
                        { // SER UN VALOR DE EN MEDIO

                            linea_ant->sig = linea_aux->sig; // linea_sig
                            linea_sig->ant = linea_aux->ant; // lineanterior
                        }

                        // FREE

                        for (i = 0; i < num_campos; i++)
                        {
                            free(linea_aux->valores[i]);
                        }

                        free(linea_aux->valores);
                        free(linea_aux);
                    }

                    linea_aux = linea_sig;
                }

                return linea;
            }
            else if (strcmp(simbolo, ">") == 0)
            {

                while (linea_aux != NULL)
                {

                    linea_sig = linea_aux->sig;
                    linea_ant = linea_aux->ant;

                    if (strcmp(linea_aux->valores[posicion], valor) > 0)
                    {

                        if (linea_aux == linea && linea_aux->sig == NULL)
                        { // SER EL UNICO VALOR
                            linea = NULL;
                        }
                        else if (linea_aux == linea && linea_aux->ant == NULL)
                        { // SER EL PRIMER VALOR
                            linea = linea_aux->sig;
                            linea_sig->ant = NULL;
                            linea_aux->sig = NULL;
                        }
                        else if (linea_aux->sig == NULL)
                        { // SER EL ULTIMO VALOR
                            linea_ant->sig = NULL;
                            linea_aux->ant = NULL;
                        }
                        else
                        { // SER UN VALOR DE EN MEDIO

                            linea_ant->sig = linea_aux->sig; // linea_sig
                            linea_sig->ant = linea_aux->ant; // lineanterior
                        }

                        // FREE

                        for (i = 0; i < num_campos; i++)
                        {
                            free(linea_aux->valores[i]);
                        }

                        free(linea_aux->valores);
                        free(linea_aux);
                    }

                    linea_aux = linea_sig;
                }

                return linea;
            }
            else if (strcmp(simbolo, "<") == 0)
            {

                while (linea_aux != NULL)
                {

                    linea_sig = linea_aux->sig;
                    linea_ant = linea_aux->ant;

                    if (strcmp(linea_aux->valores[posicion], valor) < 0)
                    {

                        if (linea_aux == linea && linea_aux->sig == NULL)
                        { // SER EL UNICO VALOR
                            linea = NULL;
                        }
                        else if (linea_aux == linea && linea_aux->ant == NULL)
                        { // SER EL PRIMER VALOR
                            linea = linea_aux->sig;
                            linea_sig->ant = NULL;
                            linea_aux->sig = NULL;
                        }
                        else if (linea_aux->sig == NULL)
                        { // SER EL ULTIMO VALOR
                            linea_ant->sig = NULL;
                            linea_aux->ant = NULL;
                        }
                        else
                        { // SER UN VALOR DE EN MEDIO

                            linea_ant->sig = linea_aux->sig; // linea_sig
                            linea_sig->ant = linea_aux->ant; // lineanterior
                        }

                        // FREE

                        for (i = 0; i < num_campos; i++)
                        {
                            free(linea_aux->valores[i]);
                        }

                        free(linea_aux->valores);
                        free(linea_aux);
                    }

                    linea_aux = linea_sig;
                }

                return linea;
            }
            else
            {
                printf("Error de simbolo\n");
            }
        }
        else
        {
            printf("Error de tipos\n");
        }
    }
}

//!--------------------------------ENERO--------------------------------

/*
Esta funcion se encarga de borrar las tablas
*/
TABLE *borrar_tabla(TABLE *tabla_principal, char *nombre_tabla)
{
    int i;
    TABLE *tabla_auxiliar = NULL, *tabla_sig = NULL, *tabla_ant = NULL;
    tabla_auxiliar = buscar_tabla(nombre_tabla, tabla_principal);

    tabla_sig = tabla_auxiliar->sig;
    tabla_ant = tabla_auxiliar->ant;

    if (tabla_auxiliar == tabla_principal && tabla_auxiliar->sig == NULL) // unica tabla
    {
        tabla_principal = NULL;
    }
    else if (tabla_auxiliar == tabla_principal && tabla_auxiliar->ant == NULL) // primera tabla
    {
        tabla_principal = tabla_principal->sig;
        tabla_sig = NULL;
        tabla_auxiliar->sig = NULL;
    }
    else if (tabla_auxiliar->sig == NULL) // ultima tabla
    {
        tabla_ant->sig = NULL;
        tabla_auxiliar->ant = NULL;
    }
    else
    {
        tabla_ant->sig = tabla_auxiliar->sig;
        tabla_sig->ant = tabla_auxiliar->ant;
    }

    // FREE
    for (i = 0; i < tabla_auxiliar->numCampos; i++)
    {
        free(tabla_auxiliar->campos[i]);
    }

    free(tabla_auxiliar->campos);
    free(tabla_auxiliar->tipos);
    free(tabla_auxiliar);

    return tabla_principal;
}

/*
Esta funcion se encarga de importar una tabla a otra base de datos
*/
TABLE *seleccionar_tabla_otra_BD(char *nombre_tabla, char *nombre_bd_origen, char *nombre_bd_destino, TABLE *tabla_principal)
{
    TABLE *tabla_principal_aux = NULL, *tabla_aux = NULL, *tabla_aux2 = NULL;
    ROW *linea_principal = NULL;

    tabla_principal_aux = cargar_tablas(nombre_bd_origen);
    // mostrar_tablas(tabla_principal_aux);

    tabla_aux = buscar_tabla(nombre_tabla, tabla_principal_aux);
    tabla_aux2 = inicializar_tabla(tabla_aux->numCampos, tabla_aux->nombre, tabla_aux->campos, tabla_aux->tipos);

    // printf("%s\n", tabla_aux2->nombre);
    //  cargamos las lineas correspondientes

    linea_principal = cargar_lineas(nombre_bd_origen, nombre_tabla, tabla_aux->numCampos, tabla_aux->tipos);

    // imprimir_todo(linea_principal, tabla_aux->num_campos);

    if (tabla_principal == NULL)
    {
        tabla_principal = tabla_aux2;
    }
    else
    {
        insertar_ultima(tabla_principal, tabla_aux2);
    }

    crear_tabla(tabla_principal, nombre_bd_destino, nombre_tabla);

    actualizar_lineas(nombre_bd_destino, linea_principal, tabla_aux);

    return tabla_principal;
}

//!-------------------------------EXTRA---------------------------------

int contar_bd()
{

    DIR *d;
    struct dirent *f;
    int contador = 0;

    d = opendir(".");

    if (d == NULL)
    {
        printf("No hay bases de datos disponibles\n");
    }
    else
    {

        while ((f = readdir(d)) != NULL)
        {

            if (strcmp(f->d_name, ".") != 0 && strcmp(f->d_name, "..") != 0)
            {

                if (comprobar_caracteres_bd(f->d_name) == 0)
                {

                    contador++;
                }
            }
        }

        if (contador == 0)
        {
            printf("No hay bases de datos disponibles\n");
        }
    }

    closedir(d);
    return contador;
}

char **mostrar_bd_DES(int contador)
{

    DIR *d;
    struct dirent *f;
    char **nombres;
    nombres = malloc(sizeof(char *) * (contador));

    int i = 100, j = 0;
    d = opendir(".");

    if (d == NULL)
    {
        printf("No hay bases de datos disponibles\n");
    }
    else
    {

        while ((f = readdir(d)) != NULL)
        {

            if (strcmp(f->d_name, ".") != 0 && strcmp(f->d_name, "..") != 0)
            {

                if (comprobar_caracteres_bd(f->d_name) == 0)
                {

                    nombres[j] = strdup(f->d_name);

                    j++;
                }
            }
        }

        if (j == 0)
        {
            printf("No hay bases de datos disponibles\n");
        }
    }

    closedir(d);
    return nombres;
}

void ordenar_DES_tablas(TABLE *tabla)
{

    TABLE *tabla_aux, *i = NULL, *j = NULL;

    tabla_aux = tabla;

    for (i = tabla; i != NULL; i = i->sig)
    {

        for (j = i; j != NULL; j = j->sig)
        {

            if (strcmp(i->nombre, j->nombre) < 0)
            {
                char aux2[100];

                strcpy(aux2, i->nombre);
                strcpy(i->nombre, j->nombre);
                strcpy(j->nombre, aux2);
            }
        }
    }
}

void ordenar_ASC_tablas(TABLE *tabla)
{

    TABLE *tabla_aux, *i = NULL, *j = NULL;

    tabla_aux = tabla;

    for (i = tabla; i != NULL; i = i->sig)
    {

        for (j = i; j != NULL; j = j->sig)
        {

            if (strcmp(i->nombre, j->nombre) > 0)
            {
                char aux2[100];

                strcpy(aux2, i->nombre);
                strcpy(i->nombre, j->nombre);
                strcpy(j->nombre, aux2);
            }
        }
    }
}

int renombrar_db(char *dbactual, char *dbnuevo)
{

    int exito = 0;

    exito = rename(dbactual, dbnuevo);

    return exito;
}

ROW *intercambiar_valor_campo(int numCampos, ROW *Big_Row, TYPE tipo, char *valor, char *valor_nuevo, char *simbolo, int posicion)
{
    int i, j = 1;

    ROW *linea_auxiliar = Big_Row;
    ROW *linea_siguiente = NULL;
    ROW *linea_anterior = NULL;

    if (Big_Row == NULL)
    {
        printf("No se han encontrado datos\n");
    }
    else
    {
        if (tipo == NUM)
        {
            if (strcmp(simbolo, "=") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    linea_anterior = linea_auxiliar->ant;
                    linea_siguiente = linea_auxiliar->sig;

                    if (atof(valor) == atof(linea_auxiliar->valores[posicion]))
                    {
                        if (validar_decimal(valor_nuevo) == 1)
                        {
                            linea_auxiliar->valores[posicion] = valor_nuevo;
                        }
                        else
                        {
                            printf("El nuevo valor no es un numero\n");
                        }
                    }

                    linea_auxiliar = linea_siguiente;
                }

                return Big_Row;
            }
            else if (strcmp(simbolo, "<") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    linea_anterior = linea_auxiliar->ant;
                    linea_siguiente = linea_auxiliar->sig;

                    if (atof(valor) > atof(linea_auxiliar->valores[posicion]))
                    {
                        if (validar_decimal(valor_nuevo) == 1)
                        {
                            linea_auxiliar->valores[posicion] = valor_nuevo;
                        }
                        else
                        {
                            printf("El nuevo valor no es un numero\n");
                        }
                    }

                    linea_auxiliar = linea_siguiente;
                }

                return Big_Row;
            }
            else if (strcmp(simbolo, ">") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    linea_anterior = linea_auxiliar->ant;
                    linea_siguiente = linea_auxiliar->sig;

                    if (atof(valor) < atof(linea_auxiliar->valores[posicion]))
                    {
                        if (validar_decimal(valor_nuevo) == 1)
                        {
                            linea_auxiliar->valores[posicion] = valor_nuevo;
                        }
                        else
                        {
                            printf("El nuevo valor no es un numero\n");
                        }
                    }

                    linea_auxiliar = linea_siguiente;
                }

                return Big_Row;
            }

            else
            {
                printf("ERROR de simbolos.\n");
                return Big_Row;
            }
        }
        else if (tipo == DATE)
        {
            if (strcmp(simbolo, "=") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    linea_anterior = linea_auxiliar->ant;
                    linea_siguiente = linea_auxiliar->sig;

                    if (fecha_to_number_DMA(valor) == fecha_to_number_DMA(linea_auxiliar->valores[posicion]))
                    {
                        if (validar_fecha_DMA(valor_nuevo) == 1)
                        {
                            linea_auxiliar->valores[posicion] = valor_nuevo;
                        }
                        else
                        {
                            printf("El nuevo valor no es un numero\n");
                        }
                    }

                    linea_auxiliar = linea_siguiente;
                }

                return Big_Row;
            }
            else if (strcmp(simbolo, "<") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    linea_anterior = linea_auxiliar->ant;
                    linea_siguiente = linea_auxiliar->sig;

                    if (fecha_to_number_DMA(valor) > fecha_to_number_DMA(linea_auxiliar->valores[posicion]))
                    {
                        if (validar_fecha_DMA(valor_nuevo) == 1)
                        {
                            linea_auxiliar->valores[posicion] = valor_nuevo;
                        }
                        else
                        {
                            printf("El nuevo valor no es un numero\n");
                        }
                    }

                    linea_auxiliar = linea_siguiente;
                }

                return Big_Row;
            }
            else if (strcmp(simbolo, ">") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    linea_anterior = linea_auxiliar->ant;
                    linea_siguiente = linea_auxiliar->sig;

                    if (fecha_to_number_DMA(valor) < fecha_to_number_DMA(linea_auxiliar->valores[posicion]))
                    {
                        if (validar_fecha_DMA(valor_nuevo) == 1)
                        {
                            linea_auxiliar->valores[posicion] = valor_nuevo;
                        }
                        else
                        {
                            printf("El nuevo valor no es un numero\n");
                        }
                    }

                    linea_auxiliar = linea_siguiente;
                }

                return Big_Row;
            }

            else
            {
                printf("ERROR de simbolos.\n");
                return Big_Row;
            }
        }
        else if (tipo == EMAIL)
        {
            if (strcmp(simbolo, "=") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    linea_anterior = linea_auxiliar->ant;
                    linea_siguiente = linea_auxiliar->sig;

                    if (strcmp(linea_auxiliar->valores[posicion], valor) == 0)
                    {
                        if (validar_email(valor_nuevo) == 0)
                        {
                            linea_auxiliar->valores[posicion] = valor_nuevo;
                        }
                        else
                        {
                            printf("El nuevo valor no es un numero\n");
                        }
                    }

                    linea_auxiliar = linea_siguiente;
                }

                return Big_Row;
            }
            else if (strcmp(simbolo, "<") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    linea_anterior = linea_auxiliar->ant;
                    linea_siguiente = linea_auxiliar->sig;

                    if (strcmp(linea_auxiliar->valores[posicion], valor) > 0)
                    {
                        if (validar_email(valor_nuevo) == 0)
                        {
                            linea_auxiliar->valores[posicion] = valor_nuevo;
                        }
                        else
                        {
                            printf("El nuevo valor no es un numero\n");
                        }
                    }

                    linea_auxiliar = linea_siguiente;
                }

                return Big_Row;
            }
            else if (strcmp(simbolo, ">") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    linea_anterior = linea_auxiliar->ant;
                    linea_siguiente = linea_auxiliar->sig;

                    if (strcmp(linea_auxiliar->valores[posicion], valor) < 0)
                    {
                        if (validar_decimal(valor_nuevo) == 0)
                        {
                            linea_auxiliar->valores[posicion] = valor_nuevo;
                        }
                        else
                        {
                            printf("El nuevo valor no es un numero\n");
                        }
                    }

                    linea_auxiliar = linea_siguiente;
                }

                return Big_Row;
            }

            else
            {
                printf("ERROR de simbolos.\n");
                return Big_Row;
            }
        }
        else if (tipo == TEXT)
        {
            if (strcmp(simbolo, "=") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    linea_anterior = linea_auxiliar->ant;
                    linea_siguiente = linea_auxiliar->sig;

                    if (strcmp(linea_auxiliar->valores[posicion], valor) == 0)
                    {
                        linea_auxiliar->valores[posicion] = valor_nuevo;
                    }

                    linea_auxiliar = linea_siguiente;
                }

                return Big_Row;
            }
            else if (strcmp(simbolo, "<") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    linea_anterior = linea_auxiliar->ant;
                    linea_siguiente = linea_auxiliar->sig;

                    if (strcmp(linea_auxiliar->valores[posicion], valor) > 0)
                    {

                        linea_auxiliar->valores[posicion] = valor_nuevo;
                    }

                    linea_auxiliar = linea_siguiente;
                }

                return Big_Row;
            }
            else if (strcmp(simbolo, ">") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    linea_anterior = linea_auxiliar->ant;
                    linea_siguiente = linea_auxiliar->sig;

                    if (strcmp(linea_auxiliar->valores[posicion], valor) < 0)
                    {
                        linea_auxiliar->valores[posicion] = valor_nuevo;
                    }

                    linea_auxiliar = linea_siguiente;
                }

                return Big_Row;
            }

            else
            {
                printf("ERROR de simbolos.\n");
                return Big_Row;
            }
        }
    }
    return Big_Row;
}

void borrar_meta(char *db)
{

    char aux_ruta[100] = "";

    strcpy(aux_ruta, db);
    strcat(aux_ruta, "\\");
    strcat(aux_ruta, "meta");
    strcat(aux_ruta, ".db");
    remove(aux_ruta);
}

void borrar_directorio(char *db)
{
    char path[200];
    char ruta[100] = "";
    char bar[10] = "\\";

    if (getcwd(path, 200) == NULL)
    {
        printf("Error\n");
    }

    strcat(path, bar);
    strcat(path, db);

    if (rmdir(path) != 0)
    {

        printf("Error al borrar\n");
    }
}

/*void eliminarDB(char nombreDB){
    chdir(nombreDB);
     DIRdir;
     struct dirent *ent;
         dir = opendir(".");
     while ((ent = readdir(dir)) != NULL) {
            if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                remove(ent->d_name);

            }
        }

        closedir(dir);
        chdir("..");
        rmdir(nombreDB);
}*/

void borrar_ficheros(char *db, TABLE *tabla_principal)
{
    TABLE *table_aux = NULL;
    tabla_principal = cargar_tablas(db);
    table_aux = cargar_tablas(db);

    char aux_ruta2[100] = "";
    while (table_aux != NULL)
    {
        strcpy(aux_ruta2, db);
        strcat(aux_ruta2, "\\");
        strcat(aux_ruta2, table_aux->nombre);
        strcat(aux_ruta2, ".tab");

        remove(aux_ruta2);

        // Borrar la tabla
        tabla_principal = borrar_tabla(tabla_principal, table_aux->nombre);
        table_aux = table_aux->sig;
    }
}

ROW *imprimir_valor_MAX_MIN(ROW *linea_principal, TYPE tipo, char *simbolo, int posicion)
{

    int i, contador = 1, aux_int;
    ROW *linea_auxiliar = linea_principal;
    char aux[100];

    if (linea_auxiliar == NULL)
    {
        printf("No hay datos en la tabla\n");
    }
    else
    {

        if (tipo == NUM)
        {
            aux_int = atof(linea_auxiliar->valores[posicion]);
            if (strcmp(simbolo, "MAX") == 0)
            {

                while (linea_auxiliar != NULL)
                {

                    if (atof(linea_auxiliar->valores[posicion]) > aux_int)
                    {
                        aux_int = atof(linea_auxiliar->valores[posicion]);
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
                printf("El valor maximo es: %d\n", aux_int);
            }
            else if (strcmp(simbolo, "MIN") == 0)
            {

                while (linea_auxiliar != NULL)
                {

                    if (atof(linea_auxiliar->valores[posicion]) < aux_int)
                    {
                        aux_int = atof(linea_auxiliar->valores[posicion]);
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
                printf("El valor minimo es: %d\n", aux_int);
            }
        }
        else if (tipo == DATE)
        {
            aux_int = fecha_to_number_DMA(linea_auxiliar->valores[posicion]);
            if (strcmp(simbolo, "MAX") == 0)
            {

                while (linea_auxiliar != NULL)
                {

                    if (fecha_to_number_DMA(linea_auxiliar->valores[posicion]) > aux_int)
                    {
                        aux_int = fecha_to_number_DMA(linea_auxiliar->valores[posicion]);
                        strcpy(aux, linea_auxiliar->valores[posicion]);
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
                printf("El valor maximo es: %s\n", aux);
            }
            else if (strcmp(simbolo, "MIN") == 0)
            {

                while (linea_auxiliar != NULL)
                {

                    if (fecha_to_number_DMA(linea_auxiliar->valores[posicion]) < aux_int)
                    {
                        aux_int = fecha_to_number_DMA(linea_auxiliar->valores[posicion]);
                        strcpy(aux, linea_auxiliar->valores[posicion]);
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
                printf("El valor minimo es: %s\n", aux);
            }
        }
        else if (tipo == EMAIL)
        {
            strcpy(aux, linea_auxiliar->valores[posicion]);
            if (strcmp(simbolo, "MAX") == 0)
            {

                while (linea_auxiliar != NULL)
                {

                    if (strcmp(linea_auxiliar->valores[posicion], aux) == 1)
                    {
                        strcpy(aux, linea_auxiliar->valores[posicion]);
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
                printf("El valor maximo es: %s\n", aux);
            }
            else if (strcmp(simbolo, "MIN") == 0)
            {

                while (linea_auxiliar != NULL)
                {

                    if (strcmp(linea_auxiliar->valores[posicion], aux) == -1)
                    {
                        strcpy(aux, linea_auxiliar->valores[posicion]);
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
                printf("El valor maximo es: %s\n", aux);
            }
        }
        else if (tipo == TEXT)
        {
            strcpy(aux, linea_auxiliar->valores[posicion]);
            if (strcmp(simbolo, "MAX") == 0)
            {

                while (linea_auxiliar != NULL)
                {

                    if (strcmp(linea_auxiliar->valores[posicion], aux) == 1)
                    {
                        strcpy(aux, linea_auxiliar->valores[posicion]);
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
                printf("El valor maximo es: %s\n", aux);
            }
            else if (strcmp(simbolo, "MIN") == 0)
            {

                while (linea_auxiliar != NULL)
                {

                    if (strcmp(linea_auxiliar->valores[posicion], aux) == -1)
                    {
                        strcpy(aux, linea_auxiliar->valores[posicion]);
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }

                printf("El valor minimo es: %s\n", aux);
            }
        }
    }
}

void join_tablas(char *nombre_bd, char *nombretablaA, char *nombretablaB, TABLE *tabla_principal)
{

    TABLE *tablaA = NULL, *tablaB = NULL;
    ROW *lineaA = NULL, *lineaB = NULL;

    int i = 0, j = 0, existe = 0;
    int auxi = 0, auxj = 0;

    tablaA = buscar_tabla(nombretablaA, tabla_principal);
    tablaB = buscar_tabla(nombretablaB, tabla_principal);

    lineaA = cargar_lineas(nombre_bd, nombretablaA, tablaA->numCampos, tablaA->tipos);
    lineaB = cargar_lineas(nombre_bd, nombretablaB, tablaB->numCampos, tablaB->tipos);
    ROW *copiaLineaB = lineaB;

    for (i = 0; i < tablaA->numCampos; i++)
    {
        for (j = 0; j < tablaB->numCampos; j++)
        {

            if (strcmp(tablaA->campos[i], tablaB->campos[j]) == 0)
            {
                auxi = i;
                auxj = j;
                existe++;
                break;
            }
        }
    }
    if (existe == 0)
    {
        printf("No se han encontrado dos campos coincidentes\n");
        return;
    }
    int contador = 0;
    if (tablaA->tipos[auxi] == tablaB->tipos[auxj])
    {
        while (lineaA != NULL)
        {
            while (lineaB != NULL)
            {

                if (strcmp(lineaA->valores[auxi], lineaB->valores[auxj]) == 0)
                {

                    for (int u = 0; u < tablaA->numCampos; u++)
                    {
                        printf("%s ", lineaA->valores[u]);
                    }
                    for (int o = 0; o < tablaB->numCampos; o++)
                    {
                        if (strcmp(tablaB->campos[o], tablaB->campos[auxj]) != 0)
                            printf("%s ", lineaB->valores[o]);
                    }

                    printf("\n");
                }
                lineaB = lineaB->sig;
            }
            lineaA = lineaA->sig;
            lineaB = copiaLineaB;
        }
    }
}

void join_tablas_fichero(char *nombredb, char *nombretablaA, char *nombretablaB, TABLE *tabla_principal)
{

    TABLE *tablaA = NULL, *tablaB = NULL;
    ROW *lineaA = NULL, *lineaB = NULL;
    tablaA = buscar_tabla(nombretablaA, tabla_principal);
    tablaB = buscar_tabla(nombretablaB, tabla_principal);

    int ncampos = tablaA->numCampos + (tablaB->numCampos - 1);

    char **valores = malloc(sizeof(char *) * (ncampos));

    char **nombres = malloc(sizeof(char *) * (ncampos));
    TYPE *tipos = malloc(sizeof(TYPE *) * (ncampos));

    int i = 0, j = 0, existe = 0;
    int auxi = 0, auxj = 0;
    int entra = 0;

    char ruta[100] = "";

    lineaA = cargar_lineas(nombredb, nombretablaA, tablaA->numCampos, tablaA->tipos);
    lineaB = cargar_lineas(nombredb, nombretablaB, tablaB->numCampos, tablaB->tipos);

    ROW *copiaLineaB = lineaB;

    for (i = 0; i < tablaA->numCampos; i++)
    {
        for (j = 0; j < tablaB->numCampos; j++)
        {

            if (strcmp(tablaA->campos[i], tablaB->campos[j]) == 0)
            {
                auxi = i;
                auxj = j;
                entra++;
                break;
            }
        }
    }
    if (entra == 0)
    {
        printf("No se han encontrado dos campos coincidentes\n");
        return;
    }

    int ncamposA = tablaA->numCampos;

    for (int i = 0; i < tablaA->numCampos; i++)
    {
        nombres[i] = strdup(tablaA->campos[i]);
    }
    for (int i = 0; i < tablaB->numCampos; i++)
    {
        if (strcmp(tablaB->campos[i], tablaB->campos[auxj]) != 0)
        {
            nombres[ncamposA] = strdup(tablaB->campos[i]);
            ncamposA++;
        }
    }
    ncamposA = tablaA->numCampos;
    for (int i = 0; i < tablaA->numCampos; i++)
    {
        tipos[i] = tablaA->tipos[i];
    }
    for (int i = 0; i < tablaB->numCampos; i++)
    {
        if (strcmp(tablaB->campos[i], tablaB->campos[auxj]) != 0)
        {
            tipos[ncamposA] = tablaB->tipos[i];
            ncamposA++;
        }
    }

    TABLE *tablejoin = NULL;
    ROW *lineajoin = NULL;
    ROW *LalineaJoin = NULL;
    strcat(ruta, tablaA->nombre);
    strcat(ruta, tablaB->nombre);

    tablejoin = inicializar_tabla(ncampos, ruta, nombres, tipos);

    if (tabla_principal == NULL)
    {
        tabla_principal = tablejoin;
        crear_tabla(tabla_principal, nombredb, ruta);
    }
    else
    {
        insertar_ultima(tabla_principal, tablejoin);
        crear_tabla(tabla_principal, nombredb, ruta);
    }
    ncamposA = tablaA->numCampos;
    if (tablaA->tipos[auxi] == tablaB->tipos[auxj])
    {
        while (lineaA != NULL)
        {
            while (lineaB != NULL)
            {
                if (strcmp(lineaA->valores[auxi], lineaB->valores[auxj]) == 0)
                {
                    for (int u = 0; u < tablaA->numCampos; u++)
                    {
                        valores[u] = strdup(lineaA->valores[u]);
                    }
                    for (int o = 0; o < tablaB->numCampos; o++)
                    {
                        if (strcmp(tablaB->campos[o], tablaB->campos[auxj]) != 0)
                        {
                            valores[ncamposA] = strdup(lineaB->valores[o]);
                            ncamposA++;
                        }
                    }
                    LalineaJoin = cargar_lineas(nombredb, ruta, ncampos, tablejoin->tipos);
                    lineajoin = inicializar_linea(valores);

                    if (LalineaJoin == NULL)
                    {
                        LalineaJoin = lineajoin;
                        actualizar_lineas(nombredb, LalineaJoin, tablejoin);
                    }
                    else
                    {
                        insertar_ultima_linea(LalineaJoin, lineajoin);
                        actualizar_lineas(nombredb, LalineaJoin, tablejoin);
                    }
                    // printf("\n");
                }
                lineaB = lineaB->sig;
            }
            ncamposA = tablaA->numCampos;
            lineaA = lineaA->sig;
            lineaB = copiaLineaB;
        }
    }
}

TABLE *eliminar_campo(TABLE *LaTabla, char *campo, char *nombretabla, int *uncampo, char *nombredb)
{
    TABLE *aux = NULL;

    ROW *linea_principal = NULL;

    int i = 0;
    int j = 0;
    int error = 0;
    *uncampo = 0;
    int numcampos = 0;
    int prueba = 0;
    int contador = 0;
    aux = LaTabla;

    TABLE *tabla_aux = buscar_tabla(nombretabla, LaTabla);
    linea_principal = cargar_lineas(nombredb, nombretabla, tabla_aux->numCampos, tabla_aux->tipos);

    while (aux != NULL)
    {
        numcampos = aux->numCampos;
        if (strcmp(aux->nombre, nombretabla) == 0)
        {
            for (i = 0; i < aux->numCampos; i++)
            {
                if (strcmp(aux->campos[i], campo) == 0)
                {
                    contador++;
                    if (aux->numCampos == 1)
                    {
                        *uncampo = 1;
                        return LaTabla;
                    }
                    if (i == aux->numCampos - 1)
                    {
                        free(aux->campos[i]);
                    }
                    else
                    {
                        for (j = i; j < aux->numCampos - 1; j++)
                        {
                            aux->campos[j] = aux->campos[j + 1];
                            aux->tipos[j] = aux->tipos[j + 1];
                        }
                        free(aux->campos[aux->numCampos]);
                    }
                    aux->numCampos = aux->numCampos - 1;

                    break;
                }
            }
        }
        aux = aux->sig;
    }
    if (contador == 0)
    {
        printf("Error, no se ha encontrado el campo \n");
    }

    ROW *auxlinea_principal = linea_principal;
    tabla_aux = buscar_tabla(nombretabla, LaTabla);
    while (auxlinea_principal != NULL)
    {
        for (int k = i; k < numcampos - 1; k++)
        {
            if (k == numcampos - 1)
            {
                free(auxlinea_principal->valores[k]);
            }
            else
            {
                auxlinea_principal->valores[k] = auxlinea_principal->valores[k + 1];
            }
        }
        actualizar_lineas(nombredb, linea_principal, tabla_aux);
        if (auxlinea_principal->sig == NULL)
        {
            break;
        }
        auxlinea_principal = auxlinea_principal->sig;
    }
    return LaTabla;
}

int BuscarValores(int numCampos, ROW *Big_Row, TABLE *t, char *valor)
{
    int i;
    int contador = 0;

    ROW *linea_auxiliar = Big_Row;

    if (Big_Row == NULL)
    {
        printf("No se han encontrado datos\n");
    }
    else
    {
        while (linea_auxiliar != NULL)
        {
            for (i = 0; i < numCampos; i++)
            {
                if (strcmp(linea_auxiliar->valores[i], valor) == 0) // si encuentra el campo deseado
                {
                    contador++;
                }
            }

            linea_auxiliar = linea_auxiliar->sig;
        }
    }

    return contador;
}

void ObtenerValores(char *ValoresABuscar[], char *argum[], int numArg) // Array para almacenar los valores
{
    int numValores = numArg - 1;
    int i = 0;

    for (i = 0; i < numValores; i++)
    {

        ValoresABuscar[i] = argum[i + 1];
    }
}

void ContarValores(int numArg, char *valores[], ROW *Big_Row, TABLE *t)

{
    ROW *linea_auxiliar = Big_Row;

    if (Big_Row == NULL)
    {
        printf("No se han encontrado datos\n");
    }
    else
    {
        for (int i = 0; i < numArg; i++)
        {
            printf("- El valor '%s' aparece %d veces en la tabla.\n", valores[i], BuscarValores(t->numCampos, linea_auxiliar, t, valores[i]));
        }
    }
}

TABLE *alter_campos(TABLE *tabla_principal, char *campo1, char *campo2, TYPE tipo1, TYPE tipo2, char *nombre_tabla, char *nombre_bd)
{

    TABLE *tabla_aux = NULL;
    int i = 0;
    tabla_aux = buscar_tabla(nombre_tabla, tabla_principal);
    ROW *linea = NULL;
    linea = cargar_lineas(nombre_bd, nombre_tabla, tabla_aux->numCampos, tabla_aux->tipos);

    for (i; i < tabla_aux->numCampos; i++)
    {

        if (strcmp(campo1, tabla_aux->campos[i]) == 0)
        {

            tabla_aux->campos[i] = strdup(campo2);
            tabla_aux->tipos[i] = tipo2;
        }
    }

    liberar_memoria_lineas(linea, tabla_aux->numCampos);
    linea = NULL;
    actualizar_lineas(nombre_bd, linea, tabla_aux);

    return tabla_principal;
}

int contar_lineas(ROW *linea_principal)
{
    ROW *aux = NULL;
    aux = linea_principal;
    int contador = 0;

    while (aux != NULL)
    {
        contador++;
        aux = aux->sig;
    }

    return contador;
}

// Sin contador
void group_by(ROW *linea_principal, TABLE *tabla_aux, int posicion)
{

    int nlineas = contar_lineas(linea_principal);
    char **valores = malloc(sizeof(char *) * (nlineas));

    ROW *aux1 = NULL;
    aux1 = linea_principal;

    int primera = 0;
    int narray = 0;
    int entra = 0;

    while (aux1 != NULL)
    {
        if (primera == 0)
        {
            valores[primera] = strdup(aux1->valores[posicion]);
            printf("%s\n", valores[primera]);
            primera++;
            narray = primera;
        }
        else
        {
            for (int i = 0; i < narray; i++)
            {
                if (strcmp(valores[i], aux1->valores[posicion]) == 0)
                {
                    entra++;
                }
            }
            if (entra == 0)
            {
                valores[primera] = strdup(aux1->valores[posicion]);
                printf("%s\n", valores[primera]);
                primera++;
            }
            entra = 0;
            narray = primera;
        }
        aux1 = aux1->sig;
    }
}

// Con contador
void group_by_cont(ROW *linea_principal, TABLE *tabla_aux, int posicion)
{

    int nlineas = contar_lineas(linea_principal);
    char **valores = malloc(sizeof(char *) * (nlineas));
    int *numero = (int *)malloc(sizeof(int) * (nlineas));

    ROW *aux1 = NULL;
    aux1 = linea_principal;

    int primera = 0;
    int narray = 0;
    int entra = 0;

    int contador = 0;

    while (aux1 != NULL)
    {
        if (primera == 0)
        {
            valores[primera] = strdup(aux1->valores[posicion]);
            contador++;
            numero[primera] = contador;
            // printf("%s\n", valores[primera]);
            primera++;
            narray = primera;
        }
        else
        {
            for (int i = 0; i < narray; i++)
            {
                if (strcmp(valores[i], aux1->valores[posicion]) == 0)
                {
                    contador = numero[i];
                    contador++;
                    numero[i] = contador;

                    entra++;
                }
            }
            if (entra == 0)
            {
                contador = 1;
                valores[primera] = strdup(aux1->valores[posicion]);
                numero[primera] = contador;
                // printf("%s\n", valores[primera]);
                primera++;
            }
            entra = 0;
            narray = primera;
        }
        aux1 = aux1->sig;
    }

    for (int j = 0; j < narray; j++)
    {
        printf("%s - %d\n", valores[j], numero[j]);
    }
}

TABLE *add_campos(TABLE *tabla, char *campo, TYPE tipo, char *nombre_tabla, char *nombre_db)
{

    TABLE *tabla_aux = NULL, *tabla_aux2, *tabla_aux3 = NULL;
    tabla_aux3 = tabla;
    ROW *linea = NULL, *linea_aux;

    int i = 0, j, num_camp;
    char **nombre;
    TYPE *tipos;

    tabla_aux = buscar_tabla(nombre_tabla, tabla_aux3);

    linea = cargar_lineas(nombre_db, nombre_tabla, tabla_aux->numCampos, tabla_aux->tipos);

    linea_aux = linea;

    if (linea != NULL)
    {
        printf("Error, la tabla tiene que estar vacia\n");
        return tabla;
    }

    if (campo_duplicado(tabla_aux->campos, tabla_aux->numCampos, campo) == 1)
    {
        printf("Error, el campo ya existe\n");
        return tabla;
    }

    tabla_aux->numCampos = tabla_aux->numCampos + 1;

    num_camp = tabla_aux->numCampos;

    nombre = malloc(sizeof(char *) * tabla_aux->numCampos);
    tipos = malloc(sizeof(TYPE *) * tabla_aux->numCampos);

    for (i = 0; i < tabla_aux->numCampos; i++)
    {

        if (i != tabla_aux->numCampos - 1)
        {
            tipos[i] = tabla_aux->tipos[i];

            nombre[i] = strdup(tabla_aux->campos[i]);
        }
        else
        {
            tipos[i] = tipo;
            nombre[i] = campo;
        }
    }

    // for (j = 0; j < tabla_aux->numCampos; j++)
    // {
    //     printf("%d-%s\n", j, nombre[j]);
    //     printf("%d-%d\n", j, tipos[j]);
    // }

    if (ver_duplicados(nombre, num_camp) != 0)
    {

        printf("Error el campo ya existe\n");
        return tabla;
    }
    else
    {

        tabla_aux3 = cargar_tablas(nombre_db);

        tabla_aux3 = borrar_tabla(tabla_aux3, nombre_tabla);

        char db[50];
        char bar[5] = "/";
        char tab[10] = ".tab";
        char copia_nom_tabla[50];

        strcpy(db, nombre_db);
        strcpy(copia_nom_tabla, nombre_tabla);
        strcat(db, bar);
        strcat(db, copia_nom_tabla);
        strcat(db, tab);

        remove(db);

        editar_meta(nombre_db, tabla_aux3);

        tabla_aux2 = inicializar_tabla(num_camp, nombre_tabla, nombre, tipos);

        if (tabla_aux2 != NULL)
        {
            if (tabla_aux3 == NULL)
            {

                tabla_aux3 = tabla_aux2;

                crear_tabla(tabla_aux3, nombre_db, nombre_tabla);
            }
            else
            {

                insertar_ultima(tabla_aux3, tabla_aux2);

                crear_tabla(tabla_aux3, nombre_db, nombre_tabla);
            }
            // mostrar_tablas(tabla_aux3);
        }

        // while (linea != NULL)
        // {

        //     linea->valores[num_camp - 1] = (char *)malloc(sizeof(char *));
        //     strcpy(linea->valores[num_camp - 1], "NULL");
        //     linea = linea->sig;
        // }

        // actualizar_lineas(nombre_db, linea_aux, tabla_aux3);

        return tabla_aux3;
    }
}

int validar_fecha_AMD(char *cadena)
{
    int i, d, m, a;

    if (strlen(cadena) != 10)
    {
        return 0;
    }

    for (i = 0; i < 10; i++)
    {

        if (i != 4 && i != 7)
        {

            if (cadena[i] < '0' || cadena[i] > '9')
            {
                return 0;
            }
        }
    }

    if ((cadena[4] != '/' && cadena[7] != '/') && (cadena[4] != '-' && cadena[7] != '-'))
    {

        return 0;
    }

    a = (cadena[0] - '0') * 1000 + (cadena[1] - '0') * 100 + (cadena[2] - '0') * 10 + (cadena[3] - '0');
    m = (cadena[5] - '0') * 10 + (cadena[6] - '0');
    d = (cadena[8] - '0') * 10 + (cadena[9] - '0');

    if (d < 1 || m < 1 || a < 1000 || d > 31 || m > 12 || a > 9999)
    {

        return 0;
    }
    else if ((m == 4 || m == 6 || m == 9 || m == 11) && d > 30)
    {

        return 0;
    }
    else if (m == 2 && a % 4 == 0 && d > 29)
    { // año bisiesto
        return 0;
    }
    else if (m == 2 && a % 4 != 0 && d > 28)
    { // año no bisiesto
        return 0;
    }

    return 1;
}

int fecha_to_number_AMD(char *cad)
{
    int res, dia, mes, anyo;
    dia = (cad[8] - '0') * 10 + (cad[9] - '0');
    mes = (cad[5] - '0') * 10 + (cad[6] - '0');
    anyo = (cad[0] - '0') * 1000 + (cad[1] - '0') * 100 + (cad[2] - '0') * 10 + (cad[3] - '0');

    res = dia;

    if (mes == 1)
        res = res;
    else if (mes == 2)
        res = res + 31;
    else if (mes == 3 && (anyo % 4) != 0)
        res = res + 59;
    else if (mes == 3 && (anyo % 4) == 0)
        res = res + 60;
    else if (mes == 4 && (anyo % 4) != 0)
        res = res + 90;
    else if (mes == 4 && (anyo % 4) == 0)
        res = res + 91;
    else if (mes == 5 && (anyo % 4) != 0)
        res = res + 120;
    else if (mes == 5 && (anyo % 4) == 0)
        res = res + 121;
    else if (mes == 6 && (anyo % 4) != 0)
        res = res + 151;
    else if (mes == 6 && (anyo % 4) == 0)
        res = res + 152;
    else if (mes == 7 && (anyo % 4) != 0)
        res = res + 181;
    else if (mes == 7 && (anyo % 4) == 0)
        res = res + 182;
    else if (mes == 8 && (anyo % 4) != 0)
        res = res + 212;
    else if (mes == 8 && (anyo % 4) == 0)
        res = res + 213;
    else if (mes == 9 && (anyo % 4) != 0)
        res = res + 243;
    else if (mes == 9 && (anyo % 4) == 0)
        res = res + 244;
    else if (mes == 10 && (anyo % 4) != 0)
        res = res + 273;
    else if (mes == 10 && (anyo % 4) == 0)
        res = res + 274;
    else if (mes == 11 && (anyo % 4) != 0)
        res = res + 304;
    else if (mes == 11 && (anyo % 4) == 0)
        res = res + 305;
    else if (mes == 12 && (anyo % 4) != 0)
        res = res + 334;
    else if (mes == 12 && (anyo % 4) == 0)
        res = res + 335;

    res = (res + (anyo - 1) * 365);
    res = (res + (anyo - 1) / 4);

    return res;
}

char *numero_to_fecha(int res)
{
    int anyo, mes = 0, dia, bi, i;
    char *fecha;
    float a;

    fecha = malloc(sizeof(char) * 10);

    a = 0.00273785463;
    anyo = ((res)*a);
    bi = ((anyo) / 4);
    anyo = anyo + (bi / 365);
    if (anyo % 4 != 0)
        res = (res - bi - ((anyo - 1) * 365));
    else if (anyo % 4 == 0)
        res = (res - bi - ((anyo - 1) * 365));

    if (res <= 31)
        mes = 1;
    else if (res <= 59 && (anyo % 4) != 0)
    {
        mes = 2;
        res = res - 31;
    }
    else if (res <= 60 && (anyo % 4) == 0)
    {
        mes = 2;
        res = res - 31;
    }
    else if (res <= 90 && (anyo % 4) != 0)
    {
        mes = 3;
        res = res - 59;
    }
    else if (res <= 91 && (anyo % 4) == 0)
    {
        mes = 3;
        res = res - 60;
    }
    else if (res <= 120 && (anyo % 4) != 0)
    {
        mes = 4;
        res = res - 90;
    }
    else if (res <= 121 && (anyo % 4) == 0)
    {
        mes = 4;
        res = res - 91;
    }
    else if (res <= 151 && (anyo % 4) != 0)
    {
        mes = 5;
        res = res - 120;
    }
    else if (res <= 152 && (anyo % 4) == 0)
    {
        mes = 5;
        res = res - 121;
    }
    else if (res <= 181 && (anyo % 4) != 0)
    {
        mes = 6;
        res = res - 151;
    }
    else if (res <= 182 && (anyo % 4) == 0)
    {
        mes = 6;
        res = res - 152;
    }
    else if (res <= 212 && (anyo % 4) != 0)
    {
        mes = 7;
        res = res - 181;
    }
    else if (res <= 213 && (anyo % 4) == 0)
    {
        mes = 7;
        res = res - 182;
    }
    else if (res <= 243 && (anyo % 4) != 0)
    {
        mes = 8;
        res = res - 212;
    }
    else if (res <= 244 && (anyo % 4) == 0)
    {
        mes = 8;
        res = res - 213;
    }
    else if (res <= 273 && (anyo % 4) != 0)
    {
        mes = 9;
        res = res - 243;
    }
    else if (res <= 274 && (anyo % 4) == 0)
    {
        mes = 9;
        res = res - 244;
    }
    else if (res <= 304 && (anyo % 4) != 0)
    {
        mes = 10;
        res = res - 273;
    }
    else if (res <= 305 && (anyo % 4) == 0)
    {
        mes = 10;
        res = res - 274;
    }
    else if (res <= 334 && (anyo % 4) != 0)
    {
        mes = 11;
        res = res - 304;
    }
    else if (res <= 335 && (anyo % 4) == 0)
    {
        mes = 11;
        res = res - 305;
    }
    else if (res <= 365 && (anyo % 4) != 0)
    {
        mes = 12;
        res = res - 334;
    }
    else if (res <= 366 && (anyo % 4) == 0)
    {
        mes = 12;
        res = res - 335;
    }
    dia = res;
    if (dia == 0)
    {
        dia = 31;
        mes = 12;
        anyo = anyo - 1;
    }
    if (dia == -1)
    {
        dia = 30;
        mes = 12;
        anyo = anyo - 1;
    }

    if (dia < 10 && mes < 10)
        sprintf(fecha, "%i/0%i/0%i", anyo, mes, dia);
    else if (dia < 10 && mes >= 10)
        sprintf(fecha, "%i/%i/0%i", anyo, mes, dia);
    else if (dia >= 10 && mes < 10)
        sprintf(fecha, "%i/0%i/%i", anyo, mes, dia);
    else
        sprintf(fecha, "%i/%i/%i", anyo, mes, dia);

    return fecha;
}

char *NumerotoFechaDMA(int res) // de numero pasamos a fecha en el formato DMA
{
    int anyo, mes = 0, dia, bi, i;
    char *fecha;
    float a;

    fecha = malloc(sizeof(char) * 10);

    a = 0.00273785463;
    anyo = ((res)*a);
    bi = ((anyo) / 4);
    anyo = anyo + (bi / 365);
    if (anyo % 4 != 0)
        res = (res - bi - ((anyo - 1) * 365));
    else if (anyo % 4 == 0)
        res = (res - bi - ((anyo - 1) * 365));

    if (res <= 31)
        mes = 1;
    else if (res <= 59 && (anyo % 4) != 0)
    {
        mes = 2;
        res = res - 31;
    }
    else if (res <= 60 && (anyo % 4) == 0)
    {
        mes = 2;
        res = res - 31;
    }
    else if (res <= 90 && (anyo % 4) != 0)
    {
        mes = 3;
        res = res - 59;
    }
    else if (res <= 91 && (anyo % 4) == 0)
    {
        mes = 3;
        res = res - 60;
    }
    else if (res <= 120 && (anyo % 4) != 0)
    {
        mes = 4;
        res = res - 90;
    }
    else if (res <= 121 && (anyo % 4) == 0)
    {
        mes = 4;
        res = res - 91;
    }
    else if (res <= 151 && (anyo % 4) != 0)
    {
        mes = 5;
        res = res - 120;
    }
    else if (res <= 152 && (anyo % 4) == 0)
    {
        mes = 5;
        res = res - 121;
    }
    else if (res <= 181 && (anyo % 4) != 0)
    {
        mes = 6;
        res = res - 151;
    }
    else if (res <= 182 && (anyo % 4) == 0)
    {
        mes = 6;
        res = res - 152;
    }
    else if (res <= 212 && (anyo % 4) != 0)
    {
        mes = 7;
        res = res - 181;
    }
    else if (res <= 213 && (anyo % 4) == 0)
    {
        mes = 7;
        res = res - 182;
    }
    else if (res <= 243 && (anyo % 4) != 0)
    {
        mes = 8;
        res = res - 212;
    }
    else if (res <= 244 && (anyo % 4) == 0)
    {
        mes = 8;
        res = res - 213;
    }
    else if (res <= 273 && (anyo % 4) != 0)
    {
        mes = 9;
        res = res - 243;
    }
    else if (res <= 274 && (anyo % 4) == 0)
    {
        mes = 9;
        res = res - 244;
    }
    else if (res <= 304 && (anyo % 4) != 0)
    {
        mes = 10;
        res = res - 273;
    }
    else if (res <= 305 && (anyo % 4) == 0)
    {
        mes = 10;
        res = res - 274;
    }
    else if (res <= 334 && (anyo % 4) != 0)
    {
        mes = 11;
        res = res - 304;
    }
    else if (res <= 335 && (anyo % 4) == 0)
    {
        mes = 11;
        res = res - 305;
    }
    else if (res <= 365 && (anyo % 4) != 0)
    {
        mes = 12;
        res = res - 334;
    }
    else if (res <= 366 && (anyo % 4) == 0)
    {
        mes = 12;
        res = res - 335;
    }
    dia = res;
    if (dia == 0)
    {
        dia = 31;
        mes = 12;
        anyo = anyo - 1;
    }
    if (dia == -1)
    {
        dia = 30;
        mes = 12;
        anyo = anyo - 1;
    }
    if (dia < 10 && mes < 10)
        sprintf(fecha, "0%i/0%i/%i", dia, mes, anyo);
    else if (dia < 10 && mes >= 10)
        sprintf(fecha, "0%i/%i/%i", dia, mes, anyo);
    else if (dia >= 10 && mes < 10)
        sprintf(fecha, "%i/0%i/%i", dia, mes, anyo);
    else
        sprintf(fecha, "%i/%i/%i", dia, mes, anyo);

    return fecha;
}

int campo_duplicado(char **nombre, int num_camp, char *campo)
{

    for (int i = 0; i < num_camp; i++)
    {
        if (strcmp(nombre[i], campo) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void contar_valores(TABLE *tabla_principal, char *nombredb, char *nombretab, char **valor, int numarg)
{
    TABLE *aux = NULL;

    aux = buscar_tabla(nombretab, tabla_principal);

    ROW *Lalinea = cargar_lineas(nombredb, nombretab, aux->numCampos, aux->tipos);
    ROW *auxLalinea = NULL;
    int contador = 0;
    for (int i = 0; i < numarg; i++)
    {
        auxLalinea = Lalinea;
        while (auxLalinea != NULL)
        {
            for (int j = 0; j < aux->numCampos; j++)
            {
                if (strcmp(auxLalinea->valores[j], valor[i]) == 0)
                {
                    contador++;
                }
            }
            auxLalinea = auxLalinea->sig;
        }
        if (contador == 1)
        {
            printf("El valor %s se repite %d vez\n", valor[i], contador);
        }
        else
        {
            printf("El valor %s se repite %d veces\n", valor[i], contador);
        }
        contador = 0;
    }
}

void count(int numcampos, ROW *linea_principal, TYPE tipo, char *valor, char *simbolo, int posicion)
{
    int i, contador = 0;
    ROW *linea_auxiliar = linea_principal;

    if (linea_auxiliar == NULL)
    {
        printf("No hay datos en la tabla\n");
    }
    else
    {
        if (tipo == NUM)
        {
            if (strcmp(simbolo, "=") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (atof(valor) == atof(linea_auxiliar->valores[posicion]))
                    {

                        contador++;
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, ">") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (atof(valor) < atof(linea_auxiliar->valores[posicion]))
                    {

                        contador++;
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, "<") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (atof(valor) > atof(linea_auxiliar->valores[posicion]))
                    {

                        contador++;
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, "<=") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (atof(valor) > atof(linea_auxiliar->valores[posicion]))
                    {

                        contador++;
                    }
                    else if (atof(valor) == atof(linea_auxiliar->valores[posicion]))
                    {

                        contador++;
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else
            {
                printf("Error con los simbolos\n");
                return;
            }
        }
        else if (tipo == DATE)
        {
            if (strcmp(simbolo, "=") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (fecha_to_number_DMA(valor) == fecha_to_number_DMA(linea_auxiliar->valores[posicion]))
                    {
                        contador++;
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, ">") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (fecha_to_number_DMA(valor) < fecha_to_number_DMA(linea_auxiliar->valores[posicion]))
                    {

                        contador++;
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, "<") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (fecha_to_number_DMA(valor) > fecha_to_number_DMA(linea_auxiliar->valores[posicion]))
                    {

                        contador++;
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else
            {
                printf("Error con los simbolos\n");
                return;
            }
        }
        else if (tipo == EMAIL)
        {
            if (strcmp(simbolo, "=") == 0)
            {

                while (linea_auxiliar != NULL)
                {
                    if (strcmp(valor, linea_auxiliar->valores[posicion]) == 0)
                    {

                        contador++;
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, ">") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (strcmp(valor, linea_auxiliar->valores[posicion]) < 0)
                    {

                        contador++;
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, "<") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (strcmp(valor, linea_auxiliar->valores[posicion]) > 0)
                    {

                        contador++;
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else
            {
                printf("Error con los simbolos\n");
                return;
            }
        }
        else if (tipo == TEXT)
        {
            if (strcmp(simbolo, "=") == 0)
            {

                while (linea_auxiliar != NULL)
                {
                    if (strcmp(valor, linea_auxiliar->valores[posicion]) == 0)
                    {

                        contador++;
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, ">") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (strcmp(valor, linea_auxiliar->valores[posicion]) < 0)
                    {

                        contador++;
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else if (strcmp(simbolo, "<") == 0)
            {
                while (linea_auxiliar != NULL)
                {
                    if (strcmp(valor, linea_auxiliar->valores[posicion]) > 0)
                    {

                        contador++;
                    }

                    linea_auxiliar = linea_auxiliar->sig;
                }
            }
            else
            {
                printf("Error con los simbolos\n");
                return;
            }
        }
    }

    printf("Hay %d veces %s que %s\n", contador, simbolo, valor);
}

void borrar_lineas(ROW *lineas, int num_campos, char *prompt, char *nombre_bd)
{ // BORRAR TODAS LAS LINEAS DE UN FICHERO

    ROW *lineasiguiente;
    DIR *dir;
    char aux1[10] = ".tab";
    char aux2[110] = "";
    int i;

    printf("%s\n", prompt);
    strcpy(aux2, prompt);
    strcat(aux2, aux1);

    if (dir = opendir(nombre_bd))
    {

        FILE *fichero;
        fichero = fopen(aux2, "w");
        if (fichero != NULL)
        {

            if (lineas != NULL)
            {

                liberar_memoria_lineas(lineas, num_campos);

                fprintf(fichero, "");
                fclose(fichero);
            }
        }
    }
    else
    {
        printf("ERROR al abrir el directorio");
    }
}
