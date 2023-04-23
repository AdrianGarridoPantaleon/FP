#ifndef _lib_H_
#define _lib_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    NUM,
    DATE,
    EMAIL,
    TEXT,
    ERROR

} TYPE;

typedef struct tab
{
    char nombre[100];
    int numCampos;
    char **campos;
    TYPE *tipos;
    struct tab *sig, *ant;

} TABLE;

typedef struct line
{
    char **valores;
    int nuevo;
    struct line *sig, *ant;
} ROW;

void menu();
int comprobar_caracteres(char *nombre_bd);
void crear_bd(char *nombre_bd);
void crear_meta(char *nombre_bd, char *aux2);
int entrar_dir(char *nombre_bd);
TYPE validar_tipo(char *valor);
TABLE *inicializar_tabla(int num_campos, char *nombre_tabla, char **campos, TYPE *tipos);
void crear_tabla(TABLE *tabla, char *nombre_bd, char *nombre_tabla);
int contar_tablas(char *nombre_bd);
void editar_meta(char *nombre_bd, TABLE *tabla);
void insertar_ultima(TABLE *tabla_principal, TABLE *tabla_auxiliar);
int ver_duplicados(char **cadena, int num_arg);
int entrar_tabla(char *nombre_bd, char *nombre_tabla);
char *MiStrTok(char *chorizo, char *sep, int comillas);
TABLE *buscar_tabla(char *nombre_tabla, TABLE *tabla);
int validar_decimal(char *cadena);
int validar_fecha_DMA(char *cadena);
ROW *inicializar_linea(char **valores);
void insertar_ultima_linea(ROW *linea_principal, ROW *linea_auxiliar);
void actualizar_lineas(char *nombre_bd, ROW *linea, TABLE *tabla);
TABLE *cargar_tablas(char *nombre_bd);
void Trim(char *cad);
char *obtener_campo(char *linea, char *separador, int campo, char *valor);
ROW *cargar_lineas(char *nombre_bd, char *nombre_tabla, int num_campos, TYPE *tipos);
void liberar_memoria_lineas(ROW *linea_principal, int num_campos);
void liberar_memoria_tablas(TABLE *tabla_principal);
void imprimir_cabeceras(TABLE *tabla_auxiliar);
void imprimir_todo(ROW *linea_principal, int num_campos);
void imprimir_ascendete(ROW *linea_principal, int num_campos, TABLE *tabla_auxiliar);
void imprimir_descendente(ROW *linea_principal, int num_campos, TABLE *tabla_auxiliar);
void ordenar_asc(ROW *linea_principal, TABLE *tabla_auxiliar, int orden_campo);
void ordenar_des(ROW *linea_principal, TABLE *tabla_auxiliar, int orden_campo);
void intercambiar_valores(ROW *i, ROW *j);
int fecha_to_number_DMA(char *cad);
void imprimir_valor_campo(int num_campos, ROW *linea_principal, TYPE tipo, char *valor, char *simbolo, int posicion);
void mostrar_tablas(TABLE *tabla_principal);
void mostrar_bd();
int comprobar_caracteres_bd(char *nombre_bd);
int validar_email(char *email);
ROW *borrar_valor(int num_campos, ROW *linea, TYPE tipo, char *valor, char *simbolo, int posicion);
TABLE *borrar_tabla(TABLE *tabla_principal, char *nombre_tabla);
TABLE *seleccionar_tabla_otra_BD(char *nombre_tabla, char *nombre_bd_origen, char *nombre_bd_destino, TABLE *tabla_principal);

//--------------------EXTRA----------------------

int contar_bd();
char **mostrar_bd_DES(int contador);
void ordenar_DES_tablas(TABLE *tabla);
void ordenar_ASC_tablas(TABLE *tabla);
int renombrar_db(char *dbactual, char *dbnuevo);
ROW *intercambiar_valor_campo(int numCampos, ROW *Big_Row, TYPE tipo, char *valor, char *valor_nuevo, char *simbolo, int posicion);
void borrar_meta(char *db);
void borrar_directorio(char *db);
void borrar_ficheros(char *db, TABLE *tabla_principal);
ROW *imprimir_valor_MAX_MIN(ROW *linea_principal, TYPE tipo, char *simbolo, int posicion);
void join_tablas(char *nombre_bd, char *nombretablaA, char *nombretablaB, TABLE *tabla_principal);
void join_tablas_fichero(char *nombredb, char *nombretablaA, char *nombretablaB, TABLE *tabla_principal);
TABLE *eliminar_campo(TABLE *LaTabla, char *campo, char *nombretabla, int *uncampo, char *nombredb);
int BuscarValores(int numCampos, ROW *Big_Row, TABLE *t, char *valor);
void ObtenerValores(char *ValoresABuscar[], char *argum[], int numArg);
void ContarValores(int numArg, char *valores[], ROW *Big_Row, TABLE *t);
TABLE *alter_campos(TABLE *tabla_principal, char *campo1, char *campo2, TYPE tipo1, TYPE tipo2, char *nombre_tabla, char *nombre_bd);
int contar_lineas(ROW *linea_principal);
void group_by(ROW *linea_principal, TABLE *tabla_aux, int posicion);
void group_by_cont(ROW *linea_principal, TABLE *tabla_aux, int posicion);
TABLE *add_campos(TABLE *tabla, char *campo, TYPE tipo, char *nombre_tabla, char *nombre_db);
int validar_fecha_AMD(char *cadena);
int fecha_to_number_AMD(char *cad);
char *numero_to_fecha_AMD(int res);
char *numero_to_fecha_DMA(int res);
void borrar_lineas(ROW *lineas, int num_campos, char *prompt, char *nombre_bd);
int campo_duplicado(char **nombre, int num_camp, char *campo);
void contar_valores(TABLE *tabla_principal, char *nombredb, char *nombretab, char **valor, int numarg);
void count(int numcampos, ROW *linea_principal, TYPE tipo, char *valor, char *simbolo, int posicion);

#endif