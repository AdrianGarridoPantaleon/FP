#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <dirent.h>
#include "lib.h"

// gcc lib.c main.c -o main.exe

int main()
{

    char prompt[100] = "*";
    char com[100];
    char com_aux[100];
    char nombre_bd[100] = "*";
    char nombre_tabla[100] = "*";
    char *comtok[100];
    int num_arg, aux, i, j, aux2, comprobacion, num_arg_aux;
    char limitador[4] = " \n\t";
    char *comillas, *comillas_aux;
    char **nombre, **valores;
    TYPE *tipos;
    TABLE *tabla_principal = NULL, *tabla_auxiliar = NULL, *tabla_auxiliar2 = NULL;
    ROW *linea_principal = NULL, *linea_auxiliar = NULL;

    menu();

    while (1)
    {
        printf("[%s]:>", prompt);
        gets(com);
        strcpy(com_aux, com);

        num_arg = 0;

        comtok[num_arg] = strtok(com, limitador);
        num_arg++;

        while ((comtok[num_arg] = strtok(NULL, limitador)))
        {
            num_arg++;
        }

        if (comtok[0] == NULL) // si el prompt no ponen nada vuelve a ejecutarse
        {
            continue;
        }

        if (num_arg == 1 && strcmp(comtok[0], "exit") != 0 && strcmp(comtok[0], "select") != 0)
        {
            printf("Error, faltan argumentos\n");
            continue;
        }

        if (strcmp(comtok[0], "exit") == 0)
        {
            if (num_arg != 1)
            {
                printf("Error, numero de parametros incorrectos\n");
                continue;
            }

            if (tabla_principal != NULL)
            {
                if (linea_principal != NULL)
                {
                    // Liberar lineas
                    liberar_memoria_lineas(linea_principal, tabla_principal->numCampos);

                    linea_principal = NULL;
                }
                // Liberar memoria de las BD
                liberar_memoria_tablas(tabla_principal);

                tabla_principal = NULL;
                return 0;
            }
            else if (tabla_principal == NULL)
            {
                return 0;
            }
            else
            {
                printf("Error, algo ha salido mal\n");
            }
        }

        if (strcmp(comtok[0], "new") == 0 && strcmp(comtok[1], "db") == 0)
        {
            if (num_arg != 3)
            {
                printf("Error, numero de parametros incorrectos\n");
                continue;
            }

            aux = comprobar_caracteres(comtok[2]);

            if (aux == 0)
            {
                crear_bd(comtok[2]);
            }
        }

        else if (strcmp(comtok[0], "use") == 0 && strcmp(comtok[1], "db") == 0)
        {
            if (num_arg != 3)
            {
                printf("Error, numero de parametros incorrectos\n");
                continue;
            }

            if (strcmp(comtok[2], ".") == 0 || strcmp(comtok[2], "..") == 0)
            {
                printf("Estas carpetas no se tocan\n");
            }

            aux = entrar_dir(comtok[2]); // comprobamos si la ruta existe

            if (aux == 0)
            {
                strcpy(prompt, comtok[2]);
                strcpy(nombre_bd, comtok[2]);
                strcpy(nombre_tabla, "*"); // por si es la segunda vez que la llamamos y ya tiene una lista activa

                if (tabla_principal != NULL)
                {
                    if (linea_principal != NULL)
                    {
                        liberar_memoria_lineas(linea_principal, tabla_auxiliar->numCampos);
                        linea_principal = NULL;
                    }

                    liberar_memoria_tablas(tabla_principal);
                    tabla_principal = NULL;
                }
            }
            else
            {
                continue;
            }

            if (tabla_principal != NULL)
            {
                if (tabla_principal != NULL)
                {
                    liberar_memoria_lineas(linea_principal, tabla_auxiliar->numCampos);
                    tabla_principal = NULL;
                }
                liberar_memoria_tablas(tabla_principal);
                tabla_principal = NULL;
            }

            tabla_principal = cargar_tablas(nombre_bd);
            mostrar_tablas(tabla_principal);
        }

        else if (strcmp(comtok[0], "new") == 0 && strcmp(comtok[1], "table") == 0)
        {
            if (num_arg % 2 == 0 || num_arg < 5)
            {
                printf("Error, numero de parametros incorrecto\n");
                continue;
            }
            else if (strcmp(prompt, "*") == 0)
            {
                printf("Error, la BD esta inactiva\n");
                continue;
            }

            for (i = 2; i < num_arg; i++)
            {
                aux = comprobar_caracteres(comtok[i]);

                if (aux > 0)
                {
                    printf("Hay algo mal escrito\n");
                    break;
                }
            }

            if (aux > 0)
            {
                continue;
            }

            // Numero de campos que posee la tabla
            int num_campos = (num_arg - 3) / 2;

            // Reserva de memoria malloc(tamaño char 9)
            nombre = malloc(sizeof(char *) * (num_campos)); // reservamos memoria para el array de columnas de la tabla
            tipos = malloc(sizeof(TYPE *) * (num_campos));  // reservamos memoria para los tipos de la tabla
            aux2 = 0;
            comprobacion = 0;

            for (i = 3; i < num_arg; i++)
            {
                if (i % 2 == 1)
                {
                    nombre[aux2] = strdup(comtok[i]);
                }
                else
                {
                    aux = validar_tipo(comtok[i]);

                    if (aux == 4)
                    {
                        printf("Error de tipos\n");
                        comprobacion = 1;
                        break;
                    }

                    tipos[aux2] = validar_tipo(comtok[i]);
                    aux2++;
                }
            }

            i = ver_duplicados(nombre, aux2);

            j = entrar_tabla(nombre_bd, comtok[2]);

            if (comprobacion == 0 && i == 0 && j == 0)
            {
                tabla_auxiliar2 = inicializar_tabla(num_campos, comtok[2], nombre, tipos);

                if (tabla_auxiliar2 != NULL)
                {
                    if (tabla_principal == NULL)
                    {
                        tabla_principal = tabla_auxiliar2;

                        // Crear la tabla en .tab
                        crear_tabla(tabla_principal, nombre_bd, comtok[2]);
                    }
                    else
                    {
                        // Recorrer la tabla principal hasta llegar al ultimo e insertarlo
                        insertar_ultima(tabla_principal, tabla_auxiliar2);

                        // Crear la tabla en .tab
                        crear_tabla(tabla_principal, nombre_bd, comtok[2]);
                    }
                }
            }
            else
            {
                printf("Error, la tabla %s ya existe\n", comtok[2]);
            }
        }

        else if (strcmp(comtok[0], "use") == 0 && strcmp(comtok[1], "table") == 0)
        {
            if (num_arg != 3)
            {
                printf("Error, numero de parametros incorrectos\n");
                continue;
            }

            if (strcmp(prompt, "*") == 0)
            {
                printf("No hay una base de datos activa\n");
                continue;
            }

            comprobacion = entrar_tabla(nombre_bd, comtok[2]);

            if (comprobacion == 1)
            {
                strcpy(prompt, nombre_bd);
                strcpy(nombre_tabla, comtok[2]);
                strcat(prompt, "/");
                strcat(prompt, nombre_tabla);

                if (linea_principal != NULL)
                {
                    liberar_memoria_lineas(linea_principal, tabla_auxiliar->numCampos);
                }

                linea_principal = NULL;
            }
            else
            {
                printf("Error, la tabla %s no existe\n", comtok[2]);
                continue;
            }

            // Buscamos la tabla con la que vamos a trabajar

            tabla_auxiliar = buscar_tabla(nombre_tabla, tabla_principal);

            linea_principal = cargar_lineas(nombre_bd, nombre_tabla, tabla_auxiliar->numCampos, tabla_auxiliar->tipos);

            // linea_auxiliar = linea_principal;

            // while (linea_auxiliar != NULL)
            // {
            //     for (i = 0; i < tabla_auxiliar->numCampos; i++)
            //     {
            //         printf("[%s]", linea_auxiliar->valores[i]);
            //     }
            //     printf("\n");
            //     linea_auxiliar = linea_auxiliar->sig;
            // }
        }

        else if (strcmp(comtok[0], "insert") == 0)
        {

            if (strcmp(prompt, nombre_bd) == 0)
            {
                printf("Error, no hay ninguan tabla activa\n");
                continue;
            }

            if (strcmp(prompt, "*") == 0)
            {
                printf("Error, no hay ninguan BD activa\n");
                continue;
            }

            comprobacion = 0;

            comillas = strchr(com_aux, '"');

            if (comillas != NULL)
            {
                comillas_aux = strchr(comillas + 1, '"');

                if (comillas_aux == NULL)
                {
                    printf("Error de comillas\n");
                    comprobacion++;
                    continue;
                }
                else
                {
                    // Llamamos a la funcion Mistrtok con las comillas activadas
                    num_arg_aux = 0;
                    comtok[num_arg_aux] = MiStrTok(com_aux, limitador, 0);
                    num_arg_aux++;

                    while ((comtok[num_arg_aux] = MiStrTok(NULL, limitador, 1)) != NULL)
                    {
                        num_arg_aux++;
                    }
                }
            }
            else
            {
                // Llamamos a la funcion Mistrtok con las comillas desactivadas
                num_arg_aux = 0;
                comtok[num_arg_aux] = MiStrTok(com_aux, limitador, 0);
                num_arg_aux++;

                while ((comtok[num_arg_aux] = MiStrTok(NULL, limitador, 0)) != NULL)
                {
                    num_arg_aux++;
                }
            }

            // Comprobaccion que el numero de campos es correcto
            if (num_arg_aux != (tabla_auxiliar->numCampos + 1))
            {
                printf("Error, numero de parametros incorrecto\n");
                continue;
            }

            for (i = 0; i < tabla_auxiliar->numCampos; i++)
            {
                if (tabla_auxiliar->tipos[i] == NUM)
                {
                    if (validar_decimal(comtok[i + 1]) == 0)
                    {
                        printf("Error al insertar un numero\n");
                        comprobacion++;
                        break;
                    }
                }
                else if (tabla_auxiliar->tipos[i] == DATE)
                {
                    if ((validar_fecha_DMA(comtok[i + 1]) == 0) && (validar_fecha_AMD(comtok[i + 1]) == 0))
                    {
                        printf("Error al insertar una fecha\n");
                        comprobacion++;
                        break;
                    }
                }
                else if (tabla_auxiliar->tipos[i] == EMAIL)
                {
                    if (validar_email(comtok[i + 1]) != 0)
                    {
                        printf("Este correo esta mal escrito\n");
                        comprobacion++;
                        break;
                    }
                }
                else if (tabla_auxiliar->tipos[i] == TEXT)
                {
                    if (validar_email(comtok[i + 1]) == 0 || validar_fecha_DMA(comtok[i + 1]) == 1 || validar_decimal(comtok[i + 1]) == 1)
                    {

                        printf("ERROR esto no se puede insertar en un campo tipo texto\n");
                        comprobacion++;
                        break;
                    }
                }
            }

            if (comprobacion != 0)
            {
                continue;
            }

            valores = malloc(sizeof(char *) * tabla_auxiliar->numCampos);

            for (i = 0; i < tabla_auxiliar->numCampos; i++)
            {
                valores[i] = strdup(comtok[i + 1]);
            }

            // for (i = 0; i < tabla_auxiliar->numCampos; i++)
            // {
            //     printf("%s\n", valores[i]);
            // }

            // Inicializamos la linea
            linea_auxiliar = inicializar_linea(valores);

            // vemos si hay mas lineas
            if (linea_auxiliar != NULL)
            {
                // no, pues la ponemos la 1º
                if (linea_principal == NULL)
                {
                    linea_principal = linea_auxiliar;

                    actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                }
                else
                {
                    // si, pues recorremos la linea y la insertamos la última
                    insertar_ultima_linea(linea_principal, linea_auxiliar);

                    // Actualizar el .tab
                    actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                }
            }
        }

        else if (strcmp(comtok[0], "select") == 0)
        {
            if (tabla_principal == NULL)
            {
                printf("No hay una BD cargada en memoria\n");
                continue;
            }
            if (strcmp(prompt, nombre_bd) == 0)
            {
                printf("No hay una tabla activa\n");
                continue;
            }

            comillas = strchr(com_aux, '"');

            if (comillas != NULL)
            {
                comillas_aux = strchr(comillas + 1, '"');

                if (comillas == NULL)
                {
                    printf("Error, faltan comillas\n");
                    break;
                }
                else
                {
                    // LLamamos a MiStrTok con las comillas activadas
                    num_arg_aux = 0;
                    comtok[num_arg_aux] = MiStrTok(com_aux, limitador, 0);
                    num_arg_aux++;

                    while ((comtok[num_arg_aux] = MiStrTok(NULL, limitador, 1)) != NULL)
                    {
                        // printf("%d, %s\n", num_arg_aux, comtok[num_arg_aux]);
                        num_arg_aux++;
                    }
                }
            }
            else
            {
                num_arg_aux = 0;
                comtok[num_arg_aux] = MiStrTok(com_aux, limitador, 0);
                num_arg_aux++;

                while ((comtok[num_arg_aux] = MiStrTok(NULL, limitador, 0)) != NULL)
                {
                    // printf("%d, %s\n", num_arg_aux, comtok[num_arg_aux]);
                    num_arg_aux++;
                }
            }

            if (num_arg_aux == 1) // select
            {
                // if (linea_principal == NULL)
                // {
                //     printf("No hay datos en la tabla\n");
                //     continue;
                // }

                imprimir_cabeceras(tabla_auxiliar);
                imprimir_todo(linea_principal, tabla_auxiliar->numCampos);
            }
            else if (num_arg_aux == 2) // select ASC o DESC
            {
                if (strcmp(comtok[1], "ASC") == 0)
                {
                    imprimir_cabeceras(tabla_auxiliar);
                    imprimir_ascendete(linea_principal, tabla_auxiliar->numCampos, tabla_auxiliar);
                }
                else if (strcmp(comtok[1], "DES") == 0)
                {
                    imprimir_cabeceras(tabla_auxiliar);
                    imprimir_descendente(linea_principal, tabla_auxiliar->numCampos, tabla_auxiliar);
                }
                else
                {
                    printf("Error, segundo parametro no reconocido\n");
                }
            }
            else if (num_arg_aux == 3) // select campo MAX/MIN select campo sum/prom
            {
                int k = 0;
                ROW *auxLaLinea = linea_principal;
                float suma = 0, prom = 0;
                int contadorprom = 0;
                int errorestipos = 0;

                if (strcmp(comtok[2], "MAX") == 0 || strcmp(comtok[2], "MIN") == 0)
                {

                    for (i = 0; i < tabla_auxiliar->numCampos; i++)
                    {

                        if (strcmp(tabla_auxiliar->campos[i], comtok[1]) == 0)
                        {

                            // imprimir_cabeceras(tabla_auxiliar);

                            imprimir_valor_MAX_MIN(linea_principal, tabla_auxiliar->tipos[i], comtok[2], i);

                            break;
                        }
                    }
                    if (i == tabla_auxiliar->numCampos)
                    {
                        printf("Error campo no encontrado\n");
                    }
                }

                else if (strcmp(comtok[1], "groupby") == 0)
                {
                    for (int i = 0; i < tabla_auxiliar->numCampos; i++)
                    {
                        if (strcmp(tabla_auxiliar->campos[i], comtok[2]) == 0)
                        {
                            if (linea_principal == NULL)
                            {
                                printf("No hay valores en la tabla\n");
                                break;
                            }
                            group_by_cont(linea_principal, tabla_auxiliar, i);
                            // group_by(linea_principal, tabla_auxiliar, i);
                        }
                    }
                }

                else if (strcmp(comtok[2], "sum") == 0)
                {
                    for (k = 0; k < tabla_auxiliar->numCampos; k++)
                    {
                        if (strcmp(tabla_auxiliar->campos[k], comtok[1]) == 0)
                        {
                            if (tabla_auxiliar->tipos[k] == 0)
                            {
                                while (auxLaLinea != NULL)
                                {
                                    suma = suma + atof(auxLaLinea->valores[k]);
                                    auxLaLinea = auxLaLinea->sig;
                                }
                            }
                            else
                            {

                                errorestipos++;
                                break;
                            }
                        }
                    }

                    if (k + 1 == tabla_auxiliar->numCampos)
                    {
                        printf("Error de campo\n");
                        break;
                    }
                    else if (errorestipos != 0)
                    {
                        printf("Error, campo no encontrado\n");
                    }
                    else
                    {
                        printf("La suma de valores del campo %s es %f\n", comtok[1], suma);
                    }
                }
                else if (strcmp(comtok[2], "prom") == 0)
                {
                    for (k = 0; k < tabla_auxiliar->numCampos; k++)
                    {
                        if (strcmp(tabla_auxiliar->campos[k], comtok[1]) == 0)
                        {
                            if (tabla_auxiliar->tipos[k] == 0)
                            {
                                while (auxLaLinea != NULL)
                                {
                                    suma = suma + atof(auxLaLinea->valores[k]);
                                    auxLaLinea = auxLaLinea->sig;
                                    contadorprom++;
                                }
                            }
                            else
                            {
                                errorestipos++;
                                break;
                            }
                        }
                    }

                    if (k + 1 == tabla_auxiliar->numCampos)
                    {
                        printf("Error de campo\n");
                        break;
                    }
                    else if (errorestipos != 0)
                    {
                        printf("Error, campo no encontrado\n");
                    }
                    else
                    {
                        prom = suma / contadorprom;
                        printf("El promedio de valores del campo %s es %f\n", comtok[1], prom);
                    }
                }
                else
                {
                    printf("Error, el comando es incorrecto\n");

                    continue;
                }
            }
            else if (num_arg_aux == 4) // select nombres = Adrian
            {

                for (int i = 0; i < tabla_auxiliar->numCampos; i++)
                {
                    if (strcmp(tabla_auxiliar->campos[i], comtok[1]) == 0)
                    {
                        if (tabla_auxiliar->tipos[i] == NUM)
                        {
                            if (validar_decimal(comtok[3]) == 1)
                            {
                                imprimir_cabeceras(tabla_auxiliar);

                                // Mostrar los datos de la linea principal
                                imprimir_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);

                                break;
                            }
                            else
                            {
                                printf("Error de tipos con las columnas\n");
                                break;
                            }
                        }
                        else if (tabla_auxiliar->tipos[i] == DATE)
                        {
                            if (validar_fecha_DMA(comtok[3]) == 1)
                            {
                                imprimir_cabeceras(tabla_auxiliar);

                                // Mostrar los datos de la linea principal
                                imprimir_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);

                                break;
                            }
                            else
                            {
                                printf("Error de tipos con las columnas\n");
                                break;
                            }
                        }
                        else if (tabla_auxiliar->tipos[i] == EMAIL)
                        {

                            if (validar_email(comtok[3]) == 0)
                            {

                                imprimir_cabeceras(tabla_auxiliar);

                                // Mostrar los datos de la linea principal
                                imprimir_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);

                                break;
                            }
                            else
                            {
                                printf("Error de tipos con las columnas\n");
                                break;
                            }
                        }
                        else if (tabla_auxiliar->tipos[i] == TEXT)
                        {

                            imprimir_cabeceras(tabla_auxiliar);

                            // Mostrar los datos de la linea principal
                            imprimir_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);

                            break;
                        }
                        else
                        {
                            printf("Error de tipos\n");
                            i++;
                            continue;
                        }
                    }

                    if (i + 1 == tabla_auxiliar->numCampos)
                    {
                        printf("Campo no encontrado\n");
                    }
                }
            }
            else if (num_arg_aux == 5) // SELECT NOMBRES = Maria ASC
            {

                if (strcmp(comtok[4], "ASC") == 0)
                {

                    i = 0;
                    for (i; i < tabla_auxiliar->numCampos; i++)
                    {

                        if (strcmp(tabla_auxiliar->campos[i], comtok[1]) == 0)
                        {

                            if (tabla_auxiliar->tipos[i] == NUM)
                            {

                                if (validar_decimal(comtok[3]) == 1)
                                {

                                    imprimir_cabeceras(tabla_auxiliar);

                                    // ordenamos de forma ascendente
                                    ordenar_asc(linea_principal, tabla_auxiliar, 0);

                                    // Mostrar los datos de linea_principal
                                    imprimir_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);
                                    break;
                                }
                                else
                                {
                                    printf("error de tipos con las columnas\n");
                                    break;
                                }
                            }
                            else if (tabla_auxiliar->tipos[i] == DATE)
                            {

                                if (validar_fecha_DMA(comtok[3]) == 1)
                                {

                                    imprimir_cabeceras(tabla_auxiliar);

                                    // ordenamos de forma ascendente
                                    ordenar_asc(linea_principal, tabla_auxiliar, 0);

                                    // Mostrar los datos de linea_principal
                                    imprimir_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);
                                    break;
                                }
                                else
                                {
                                    printf("error de tipos con las columnas\n");
                                    break;
                                }
                            }
                            else if (tabla_auxiliar->tipos[i] == EMAIL)
                            {

                                if (validar_email(comtok[3]) == 0)
                                {

                                    imprimir_cabeceras(tabla_auxiliar);

                                    // ordenamos de forma ascendente
                                    ordenar_asc(linea_principal, tabla_auxiliar, 0);

                                    // Mostrar los datos de linea_principal
                                    imprimir_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);
                                    break;
                                }
                                else
                                {
                                    printf("error de tipos con las columnas\n");
                                    break;
                                }
                            }
                            else if (tabla_auxiliar->tipos[i] == TEXT)
                            {

                                imprimir_cabeceras(tabla_auxiliar);

                                // ordenamos de forma ascendente
                                ordenar_asc(linea_principal, tabla_auxiliar, 0);

                                // Mostrar los datos de linea_principal
                                imprimir_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);
                                break;
                            }
                            else
                            {
                                printf("ERROR de tipos\n");
                                i++;
                                continue;
                            }
                        }

                        if (i + 1 == tabla_auxiliar->numCampos)
                        {
                            printf("Campo no encontrado");
                        }
                    }
                }
                if (strcmp(comtok[4], "DES") == 0)
                {

                    i = 0;
                    for (i; i < tabla_auxiliar->numCampos; i++)
                    {

                        if (strcmp(tabla_auxiliar->campos[i], comtok[1]) == 0)
                        {

                            if (tabla_auxiliar->tipos[i] == NUM)
                            {

                                if (validar_decimal(comtok[3]) == 1)
                                {

                                    imprimir_cabeceras(tabla_auxiliar);

                                    // ordenamos de forma ascendente
                                    ordenar_des(linea_principal, tabla_auxiliar, 0);

                                    // Mostrar los datos de linea_principal
                                    imprimir_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);
                                    break;
                                }
                                else
                                {
                                    printf("error de tipos con las columnas\n");
                                    break;
                                }
                            }
                            else if (tabla_auxiliar->tipos[i] == DATE)
                            {

                                if (validar_fecha_DMA(comtok[3]) == 1)
                                {

                                    imprimir_cabeceras(tabla_auxiliar);

                                    // ordenamos de forma ascendente
                                    ordenar_des(linea_principal, tabla_auxiliar, 0);

                                    // Mostrar los datos de linea_principal
                                    imprimir_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);
                                    break;
                                }
                                else
                                {
                                    printf("error de tipos con las columnas\n");
                                    break;
                                }
                            }
                            else if (tabla_auxiliar->tipos[i] == EMAIL)
                            {

                                if (validar_email(comtok[3]) == 0)
                                {

                                    imprimir_cabeceras(tabla_auxiliar);

                                    // ordenamos de forma ascendente
                                    ordenar_des(linea_principal, tabla_auxiliar, 0);

                                    // Mostrar los datos de linea_principal
                                    imprimir_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);
                                    break;
                                }
                                else
                                {
                                    printf("error de tipos con las columnas\n");
                                    break;
                                }
                            }
                            else if (tabla_auxiliar->tipos[i] == TEXT)
                            {

                                imprimir_cabeceras(tabla_auxiliar);

                                // ordenamos de forma ascendente
                                ordenar_des(linea_principal, tabla_auxiliar, 0);

                                // Mostrar los datos de linea_principal
                                imprimir_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);
                                break;
                            }
                            else
                            {
                                printf("ERROR de tipos\n");
                                i++;
                                continue;
                            }
                        }

                        if (i + 1 == tabla_auxiliar->numCampos)
                        {
                            printf("Campo no encontrado");
                        }
                    }
                }
            }
            else
            {
                printf("Estamos en un caso de numero de parametros incorrectos\n");
            }
        }

        else if (strcmp(comtok[0], "list") == 0)
        {
            if (num_arg != 3)
            {
                printf("Error, numero de parametros incorrectos\n");
                continue;
            }

            if (strcmp(comtok[1], "db") == 0)
            {
                if (strcmp(comtok[2], "ASC") == 0)
                {
                    mostrar_bd();
                }
                else if (strcmp(comtok[2], "DES") == 0)
                {

                    int num_db, num2;
                    char **nombres;
                    num_db = contar_bd();
                    num2 = num_db;

                    nombres = mostrar_bd_DES(num_db);

                    for (i = 0; i < num_db; i++)
                    {

                        printf("%s\n", nombres[num2 - i - 1]);
                        num_arg_aux--;
                    }
                }
                else
                {
                    printf("Orden indicado errorneo\n");
                    continue;
                }

                // mostrar_bd();
            }
            else if (strcmp(comtok[1], "table") == 0)
            {
                if (strcmp(prompt, "*") == 0)
                {
                    printf("Error, no hay ninguna BD activa\n");
                    continue;
                }
                if (strcmp(comtok[2], "ASC") == 0)
                {
                    ordenar_ASC_tablas(tabla_principal);
                }
                else if (strcmp(comtok[2], "DES") == 0)
                {

                    ordenar_DES_tablas(tabla_principal);
                }
                else
                {
                    printf("Orden indicado errorneo\n");
                    continue;
                }

                mostrar_tablas(tabla_principal);
            }
            else
            {
                printf("Los parametros son incorrectos\n");
            }
        }

        else if (strcmp(comtok[0], "delete") == 0)
        {
            if (tabla_principal == NULL)
            {
                printf("Error, no hay ninguna BD activa\n");
                continue;
            }

            if (strcmp(nombre_bd, prompt) == 0)
            {
                printf("Error, no hay ninguna tabla activa\n");
            }

            comillas = strchr(com_aux, '"');

            if (comillas != NULL)
            {
                comillas_aux = strchr(comillas + 1, '"');

                if (comillas == NULL)
                {
                    printf("Error, faltan comillas\n");
                    break;
                }
                else
                {
                    // LLamamos a MiStrTok con las comillas activadas
                    num_arg_aux = 0;
                    comtok[num_arg_aux] = MiStrTok(com_aux, limitador, 0);
                    num_arg_aux++;

                    while ((comtok[num_arg_aux] = MiStrTok(NULL, limitador, 1)) != NULL)
                    {
                        // printf("%d, %s\n", num_arg_aux, comtok[num_arg_aux]);
                        num_arg_aux++;
                    }
                }
            }
            else
            {
                // LLamamos a MiStrTok con las comillas desactivadas
                num_arg_aux = 0;
                comtok[num_arg_aux] = MiStrTok(com_aux, limitador, 0);
                num_arg_aux++;

                while ((comtok[num_arg_aux] = MiStrTok(NULL, limitador, 0)) != NULL)
                {
                    // printf("%d, %s\n", num_arg_aux, comtok[num_arg_aux]);
                    num_arg_aux++;
                }
            }

            if (num_arg_aux != 4) // delete edad = 2
            {
                printf("Error, numero de parametros incorrecto\n");
                continue;
            }

            for (i = 0; i < tabla_auxiliar->numCampos; i++)
            {
                if (strcmp(tabla_auxiliar->campos[i], comtok[1]) == 0)
                {
                    if (tabla_auxiliar->tipos[i] == NUM && validar_decimal(comtok[3]) == 1)
                    {
                        linea_principal = borrar_valor(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);

                        actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);

                        break;
                    }
                    else if (tabla_auxiliar->tipos[i] == DATE && validar_fecha_DMA(comtok[3]) == 1)
                    {
                        linea_principal = borrar_valor(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);

                        actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);

                        break;
                    }
                    else if (tabla_auxiliar->tipos[i] == EMAIL && validar_email(comtok[3]) == 0)
                    {
                        linea_principal = borrar_valor(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);

                        actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);

                        break;
                    }
                    else if (tabla_auxiliar->tipos[i] == TEXT)
                    {
                        linea_principal = borrar_valor(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);

                        actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);

                        break;
                    }
                    else
                    {
                        printf("Error de parametros\n");
                        continue;
                    }
                }
            }
        }

        else if (strcmp(comtok[0], "remove") == 0)
        {

            if (num_arg != 3)
            {
                printf("ERROR numero de argumentos incorrecto\n");
                continue;
            }

            if (strcmp(comtok[1], "table") == 0)
            {

                if (strcmp(prompt, "*") == 0)
                {
                    printf("ERROR no hay una bd activa\n");
                    continue;
                }
                comprobacion = entrar_tabla(nombre_bd, comtok[2]);

                if (comprobacion == 0)
                {
                    printf("No hemos encontrado la tabla indicada\n");
                    continue;
                }

                if (strcmp(nombre_tabla, comtok[2]) == 0)
                {
                    strcpy(prompt, nombre_bd);
                    strcpy(nombre_tabla, "*");
                }

                // borramos la tabla

                tabla_principal = borrar_tabla(tabla_principal, comtok[2]);

                // Borrar fisicamente el fichero

                char aux_ruta[100] = "";

                strcpy(aux_ruta, nombre_bd);
                strcat(aux_ruta, "\\");
                strcat(aux_ruta, comtok[2]);
                strcat(aux_ruta, ".tab");

                remove(aux_ruta);

                // actualizamos el meta

                editar_meta(nombre_bd, tabla_principal);
            }

            else if (strcmp(comtok[1], "db") == 0)
            {
                if (entrar_dir(comtok[2]) == 0)
                {

                    DIR *d, *a;
                    struct dirent *f;
                    struct dirent *t;

                    d = opendir(".");

                    if (d == NULL)
                    {
                        printf("No hay bases de datos disponibles\n");
                    }
                    else
                    {

                        while ((f = readdir(d)) != NULL)
                        {

                            if (strcmp(f->d_name, comtok[2]) == 0)
                            {
                                borrar_ficheros(comtok[2], tabla_principal);
                                borrar_meta(comtok[2]);
                                borrar_directorio(comtok[2]);
                            }
                        }

                        closedir(d);

                        if (strcmp(prompt, comtok[2]) == 0)
                        {
                            strcpy(prompt, "*");
                        }
                    }
                }
                else
                {
                    printf("DB no encontrada\n");
                    continue;
                }
            }
        }

        else if (strcmp(comtok[0], "order") == 0)
        {
            if (num_arg != 3)
            {
                printf("Error, numero de parametros incorrectos\n");
                continue;
            }

            if (strcmp(prompt, "*") == 0)
            {
                printf("Error, no hay ninguna BD activa\n");
                continue;
            }

            if (strcmp(prompt, nombre_bd) == 0)
            {
                printf("Error, no hay ninguna tabla activa\n");
                continue;
            }

            if (linea_principal == NULL)
            {
                printf("Error, no hay datos\n");
            }

            if (strcmp(comtok[2], "ASC") == 0)
            {
                for (i = 0; i < tabla_auxiliar->numCampos; i++)
                {
                    if (strcmp(tabla_auxiliar->campos[i], comtok[1]) == 0)
                    {
                        if (tabla_auxiliar->tipos[i] == NUM)
                        {
                            ordenar_asc(linea_principal, tabla_auxiliar, i);
                            actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                            break;
                        }
                        else if (tabla_auxiliar->tipos[i] == DATE)
                        {
                            ordenar_asc(linea_principal, tabla_auxiliar, i);
                            actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                            break;
                        }
                        else if (tabla_auxiliar->tipos[i] == EMAIL)
                        {
                            ordenar_asc(linea_principal, tabla_auxiliar, i);
                            actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                            break;
                        }
                        else if (tabla_auxiliar->tipos[i] == TEXT)
                        {
                            ordenar_asc(linea_principal, tabla_auxiliar, i);
                            actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                            break;
                        }
                        else
                        {
                            printf("Error de tipos\n");
                        }
                    }
                    if (i + 1 == tabla_auxiliar->numCampos)
                    {
                        printf("Campo no encontrado\n");
                    }
                }
            }
            else if (strcmp(comtok[2], "DES") == 0)
            {
                for (i = 0; i < tabla_auxiliar->numCampos; i++)
                {
                    if (strcmp(tabla_auxiliar->campos[i], comtok[1]) == 0)
                    {
                        if (tabla_auxiliar->tipos[i] == NUM)
                        {
                            ordenar_des(linea_principal, tabla_auxiliar, i);
                            actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                            break;
                        }
                        else if (tabla_auxiliar->tipos[i] == DATE)
                        {
                            ordenar_des(linea_principal, tabla_auxiliar, i);
                            actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                            break;
                        }
                        else if (tabla_auxiliar->tipos[i] == EMAIL)
                        {
                            ordenar_des(linea_principal, tabla_auxiliar, i);
                            actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                            break;
                        }
                        else if (tabla_auxiliar->tipos[i] == TEXT)
                        {
                            ordenar_des(linea_principal, tabla_auxiliar, i);
                            actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                            break;
                        }
                        else
                        {
                            printf("Error de tipos\n");
                        }
                    }
                    if (i + 1 == tabla_auxiliar->numCampos)
                    {
                        printf("Campo no encontrado\n");
                    }
                }
            }
            else
            { // Has metido un comando erroneo
                printf("Error con el tercer argumento\n");
                continue;
            }
        }

        else if (strcmp(comtok[0], "import") == 0 && strcmp(comtok[1], "table") == 0 && strcmp(comtok[3], "from") == 0)
        {
            if (num_arg != 5)
            {
                printf("Error numero de argumentos\n");
                continue;
            }

            if (strcmp(prompt, "*") == 0)
            { // si no hay una base de datos activa
                printf("error no hay yna base de datos activa\n");
                continue;
            }

            if (strcmp(nombre_bd, comtok[4]) == 0)
            { // Si la base de datos origen se llama igual que la activa
                printf("error, no se puede usar la misma base de datos\n");
                continue;
            }

            if (entrar_dir(comtok[4]) == 1)
            { // si la bd origen no existe
                continue;
            }

            if (entrar_tabla(comtok[4], comtok[2]) == 0)
            { // si la tabla origen no existe
                printf("La tabla a copiar no se ha podido encontrar\n");
                continue;
            }

            if (entrar_tabla(nombre_bd, comtok[2]) == 1)
            { // si la tabla ya existe en el destino
                printf("La tabla a copiar ya existe con el mismo nombre\n");
                continue;
            }

            tabla_principal = seleccionar_tabla_otra_BD(comtok[2], comtok[4], nombre_bd, tabla_principal);

            // mostrar_tablas(tabla_principal);
        }

        else if (strcmp(comtok[0], "rename") == 0)
        {
            if (num_arg != 4)
            {
                printf("Error numero de argumentos\n");
                continue;
            }

            if (strcmp(comtok[1], "db") == 0)
            {
                if (entrar_dir(comtok[2]) == 1)
                {
                    printf("ERROR no se encuentra la base de datos actual\n");
                    continue;
                }
                if (comprobar_caracteres(comtok[3]) == 1)
                {
                    printf("Nombre de la capeta mal escrito\n");
                    continue;
                }

                if (renombrar_db(comtok[2], comtok[3]) != 0)
                {
                    printf("ERROR no se ha podido renombrar la carpeta %s\n", comtok[2]);
                    continue;
                }
                else
                {
                    if (strcmp(prompt, "*") != 0)
                    {
                        strcpy(prompt, comtok[3]);
                        strcpy(nombre_bd, comtok[3]);
                    }
                }
            }

            if (strcmp(comtok[1], "table") == 0)
            {
                if (strcmp(prompt, "*") == 0)
                { // si no hay una base de datos activa
                    printf("Error no hay yna base de datos activa\n");
                    continue;
                }

                if (comprobar_caracteres(comtok[2]) != 0)
                {
                    printf("error nombre no valido\n");
                    continue;
                }

                if (strcmp(comtok[2], comtok[3]) == 0)
                {
                    printf("error nombre igual\n");
                    continue;
                }

                if (entrar_tabla(nombre_bd, comtok[2]) == 0)
                {
                    printf("Tabla no encontrada\n");
                    continue;
                }

                if (entrar_tabla(nombre_bd, comtok[3]) == 1)
                {
                    printf("Nombre de tabla no disponible\n");
                    continue;
                }

                tabla_auxiliar = buscar_tabla(comtok[2], tabla_principal);
                strcpy(tabla_auxiliar->nombre, comtok[3]);

                if (strcmp(nombre_tabla, comtok[2]) == 0)
                {

                    strcpy(prompt, nombre_bd);
                    strcpy(nombre_tabla, comtok[3]);
                    strcat(prompt, "/");
                    strcat(prompt, nombre_tabla);
                }

                char aux1[10] = ".tab";
                char aux2[5] = "\\";
                char aux3[100] = "";
                char aux4[100] = "";

                strcpy(aux3, nombre_bd);
                strcat(aux3, aux2);

                strcpy(aux4, aux3); // nombre_bd/

                strcat(aux3, comtok[2]);
                strcat(aux3, aux1);

                strcat(aux4, comtok[3]);
                strcat(aux4, aux1);

                if (rename(aux3, aux4) == 1)
                {
                    printf("Error al cambiar el nombre de la tabla\n");
                    continue;
                }

                editar_meta(nombre_bd, tabla_principal);
            }
        }

        else if (strcmp(comtok[0], "update") == 0) // update nombre_campo <>= valor_actual valor nuevo
        {

            int c;

            comillas = strchr(com_aux, '"');

            if (comillas != NULL)
            {
                comillas_aux = strchr(comillas + 1, '"');

                if (comillas_aux == NULL)
                {
                    printf("Error de comillas\n");
                    break;
                }
                else
                {

                    num_arg_aux = 0;
                    comtok[num_arg_aux] = MiStrTok(com_aux, limitador, 0);
                    num_arg_aux++;

                    while ((comtok[num_arg_aux] = MiStrTok(NULL, limitador, 1)) != NULL)
                    {
                        num_arg_aux++;
                    }
                }
            }
            else
            {

                num_arg_aux = 0;
                comtok[num_arg_aux] = MiStrTok(com_aux, limitador, 0);
                num_arg_aux++;

                while ((comtok[num_arg_aux] = MiStrTok(NULL, limitador, 0)) != NULL)
                {
                    num_arg_aux++;
                }
            }

            if (num_arg_aux != 5)
            {
                printf("Error numero de argumentos\n");
                continue;
            }

            if (strcmp(prompt, "*") == 0)
            { // si no hay una base de datos activa
                printf("Error no hay yna base de datos activa\n");
                continue;
            }

            for (int i = 0; i < tabla_auxiliar->numCampos; i++)
            {

                if (strcmp(tabla_auxiliar->campos[i], comtok[1]) == 0)
                {
                    if (tabla_auxiliar->tipos[i] == NUM)
                    {
                        if (validar_decimal(comtok[3]) == 1)
                        {

                            c++;
                            // imprimir_cabeceras(tabla_auxiliar);
                            linea_principal = intercambiar_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[4], comtok[2], i);
                            actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                            linea_principal = cargar_lineas(nombre_bd, tabla_auxiliar->nombre, tabla_auxiliar->numCampos, tabla_auxiliar->tipos);
                            break;
                        }
                        else
                        {
                            printf("Error decimal no valido\n");
                            continue;
                        }
                    }
                    else if (tabla_auxiliar->tipos[i] == DATE)
                    {

                        if (validar_fecha_DMA(comtok[3]) == 1)
                        {
                            c++;
                            // imprimir_cabeceras(tabla_auxiliar);
                            linea_principal = intercambiar_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[4], comtok[2], i);
                            actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                            linea_principal = cargar_lineas(nombre_bd, tabla_auxiliar->nombre, tabla_auxiliar->numCampos, tabla_auxiliar->tipos);
                            break;
                        }
                        else
                        {
                            printf("Error fecha no valida\n");
                            continue;
                        }
                    }
                    else if (tabla_auxiliar->tipos[i] == EMAIL)
                    {

                        if (validar_email(comtok[3]) == 0)
                        {
                            c++;
                            // imprimir_cabeceras(tabla_auxiliar);
                            linea_principal = intercambiar_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[4], comtok[2], i);
                            actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                            linea_principal = cargar_lineas(nombre_bd, tabla_auxiliar->nombre, tabla_auxiliar->numCampos, tabla_auxiliar->tipos);
                            break;
                        }
                        else
                        {
                            printf("Error email no valido\n");
                            continue;
                        }
                    }
                    else if (tabla_auxiliar->tipos[i] == TEXT)
                    {
                        c++;
                        // imprimir_cabeceras(tabla_auxiliar);
                        linea_principal = intercambiar_valor_campo(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[4], comtok[2], i);
                        actualizar_lineas(nombre_bd, linea_principal, tabla_auxiliar);
                        linea_principal = cargar_lineas(nombre_bd, tabla_auxiliar->nombre, tabla_auxiliar->numCampos, tabla_auxiliar->tipos);
                        break;
                    }
                    else
                    {
                        printf("ERROR de tipos\n");
                        i++;
                        continue;
                    }
                }
                else if (i + 1 == tabla_auxiliar->numCampos)
                {
                    printf("Campo no encontrado\n");
                    continue;
                }
            }
        }

        else if (strcmp(comtok[0], "join") == 0)
        {

            if (num_arg != 3)
            {
                printf("Numero de argumentos incorrecto\n");
                continue;
            }
            if (strcmp(prompt, "*") == 0)
            {
                printf("Error, no hay ninguna bd activa\n");
                continue;
            }

            if (entrar_tabla(nombre_bd, comtok[1]) == 0)
            {
                printf("No se ha encontrado la tabla");
                continue;
            }
            if (entrar_tabla(nombre_bd, comtok[2]) == 0)
            {
                printf("No se ha encontrado la tabla %s", comtok[2]);
                continue;
            }
            join_tablas_fichero(nombre_bd, comtok[1], comtok[2], tabla_principal);

            join_tablas(nombre_bd, comtok[1], comtok[2], tabla_principal);
        }

        else if (strcmp(comtok[0], "borrar") == 0 && strcmp(comtok[2], "from") == 0)
        { // borrar campo from nom_table

            if (num_arg != 4)
            {
                printf("Numero de argumentos incorrecto\n");
                continue;
            }
            if (strcmp(prompt, "*") == 0)
            {
                printf("Error, no hay ninguna bd activa\n");
                continue;
            }
            if (strcmp(prompt, nombre_bd) == 0)
            {
                printf("Error, no hay ninguan tabla activa\n");
                continue;
            }
            TABLE *auxtabla_principal = tabla_principal;
            int uncampo = 0;
            // tabla_principal = cargar_tablas(nombre_bd);
            tabla_principal = eliminar_campo(tabla_principal, comtok[1], nombre_tabla, &uncampo, nombre_bd);

            if (uncampo == 1)
            {
                // BORRAMOS LA TABLA
                char auxruta[100] = "";
                strcpy(auxruta, nombre_bd);
                strcat(auxruta, "\\");
                strcat(auxruta, nombre_tabla);
                strcat(auxruta, ".tab");
                remove(auxruta);

                tabla_principal = auxtabla_principal;
                tabla_principal = borrar_tabla(tabla_principal, nombre_tabla);
                editar_meta(nombre_bd, tabla_principal);

                strcpy(prompt, nombre_bd);
                strcpy(nombre_tabla, "*");
            }
            else
            {
                editar_meta(nombre_bd, tabla_principal);
            }
        }

        else if (strcmp(comtok[0], "cut") == 0 && strcmp(comtok[1], "table") == 0 && strcmp(comtok[3], "from") == 0) // cortar y pegar
        {
            if (num_arg != 5)
            {
                printf("Error numero de argumentos\n");
                continue;
            }

            if (strcmp(prompt, "*") == 0)
            {
                printf("Error, no hay ninguna bd activa\n");
                continue;
            }

            if (entrar_dir(comtok[4]) == 1) // si no existe la bd de origen //
            {
                printf("ERROR: La bd de origen no existe.\n");
                continue;
            }

            if (strcmp(nombre_bd, comtok[4]) == 0) // Si la base de datos origen se llama igual que la activa //
            {
                printf("ERROR: La bd de origen no se puede llamar igual que la activa.\n");
                continue;
            }

            if (entrar_tabla(comtok[4], comtok[2]) == 0) // Si la tabla de origen no existe //
            {
                printf("ERROR: La tabla de origen no existe.\n");
                continue;
            }

            if (entrar_tabla(nombre_bd, comtok[2]) == 1) // Si la tabla ya existe en el destino
            {
                printf("ERROR: La tabla a copiar ya existe en la base de datos de destino.\n");
                continue;
            }

            TABLE *tabla_principal2 = NULL;
            tabla_principal2 = cargar_tablas(comtok[4]);

            tabla_principal = seleccionar_tabla_otra_BD(comtok[2], comtok[4], nombre_bd, tabla_principal);

            // borramos la tabla
            tabla_principal2 = borrar_tabla(tabla_principal2, comtok[2]);

            // borrar el fichero
            char ruta[100] = "";

            strcpy(ruta, comtok[4]);
            strcat(ruta, "\\");
            strcat(ruta, comtok[2]);
            strcat(ruta, ".tab");

            remove(ruta);

            // actualizamos el meta
            editar_meta(comtok[4], tabla_principal2);
        }

        else if (strcmp(comtok[0], "alter") == 0)
        {

            int campo1 = 0;
            int campo2 = 0;

            if (num_arg != 6)
            {
                printf("Numero de argumentos incorrecto\n");
                continue;
            }

            if (strcmp(prompt, "*") == 0)
            {

                printf("No hay una base de datos activa\n");
                continue;
            }

            if (strcmp(prompt, nombre_bd) == 0)
            {
                printf("ERROR no hay ninguna tabla activa\n");
                continue;
            }

            if (validar_tipo(comtok[5]) == 4 || validar_tipo(comtok[3]) == 4)
            {
                printf("No se reconoce el tipo\n");
                continue;
            }

            for (i = 0; i < tabla_auxiliar->numCampos; i++)
            {

                if (strcmp(comtok[2], tabla_auxiliar->campos[i]) == 0)
                {

                    campo1++;
                }
                else if (strcmp(comtok[4], tabla_auxiliar->campos[i]) == 0)
                {

                    campo2++;
                }
            }

            if (campo1 != 1 || campo2 != 0)
            {

                printf("Error\n");
                continue;
            }

            for (i = 2; i < num_arg; i++)
            {
                aux = comprobar_caracteres(comtok[i]);

                if (aux > 0)
                {
                    printf("Hay algo mal escrito\n");
                    break;
                }
            }

            if (aux > 0)
            {
                continue;
            }

            tabla_principal = alter_campos(tabla_principal, comtok[2], comtok[4], validar_tipo(comtok[3]), validar_tipo(comtok[5]), nombre_tabla, nombre_bd);

            editar_meta(nombre_bd, tabla_principal);

            linea_principal = NULL;
        }

        else if (strcmp(comtok[0], "add") == 0 && strcmp(comtok[1], "campo") == 0)
        {
            TYPE tipo_aux;

            tipo_aux = validar_tipo(comtok[3]);

            if (num_arg != 4)
            {
                printf("Numero de argumentos incorrecto\n");
                continue;
            }
            if (strcmp(prompt, "*") == 0)
            {
                printf("Error, no hay ninguna bd activa\n");
                continue;
            }
            if (strcmp(prompt, nombre_bd) == 0)
            {
                printf("Error, no hay ninguan tabla activa\n");
                continue;
            }

            if (tipo_aux >= 4)
            {
                printf("No se reconoce el tipo\n");
                continue;
            }

            tabla_principal = add_campos(tabla_principal, comtok[2], tipo_aux, nombre_tabla, nombre_bd);

            editar_meta(nombre_bd, tabla_principal);

            tabla_auxiliar = cargar_tablas(nombre_bd);
            tabla_principal = cargar_tablas(nombre_bd);
        }

        else if (strcmp(comtok[0], "count") == 0) // count <valor> <valor> ...
        {
            if (strcmp(prompt, "*") == 0)
            {
                printf("ERROR: No hay una base de datos activa.\n");
                continue;
            }
            if (strcmp(prompt, nombre_bd) == 0)
            {
                printf("Todavia no hay una tabla activa\n");
                continue;
            }

            comillas = strchr(com_aux, '"');

            if (comillas != NULL)
            {
                comillas_aux = strchr(comillas + 1, '"');

                if (comillas_aux == NULL)
                {
                    // ERROR
                    printf("ERROR DE COMILLAS\n");
                    continue;
                }
                else
                {
                    // Llamamos a la funcion strtok2 con las comillas activadas
                    num_arg_aux = 0;
                    comtok[num_arg_aux] = MiStrTok(com_aux, limitador, 0);
                    num_arg_aux++;

                    while ((comtok[num_arg_aux] = MiStrTok(NULL, limitador, 1)) != NULL) // strtok2 con comillas activas
                    {
                        num_arg_aux++;
                    }
                }
            }
            else
            {
                // Llamamos a la funcion strtok2 con las comillas desactivadas

                num_arg_aux = 0;
                comtok[num_arg_aux] = MiStrTok(com_aux, limitador, 0);
                num_arg_aux++;

                while ((comtok[num_arg_aux] = MiStrTok(NULL, limitador, 0)) != NULL) // strtok2 con las comillas desactivadas
                {
                    num_arg_aux++;
                }
            }

            if (num_arg_aux < 2)
            {
                printf("Error, numero de parametros incorrecto.\n");
                continue;
            }

            char **values;

            values = malloc(sizeof(char *) * (num_arg_aux - 1));

            for (int i = 0; i < num_arg_aux - 1; i++)
            {
                values[i] = strdup(comtok[i + 1]);
            }

            if (ver_duplicados(values, num_arg_aux - 1) == 1)
            {
                continue;
            }

            contar_valores(tabla_principal, nombre_bd, nombre_tabla, values, num_arg_aux - 1);
        }

        else if (strcmp(comtok[0], "count1") == 0) // count campo simbolo valor
        {
            for (i = 0; i < tabla_auxiliar->numCampos; i++)
            {
                if (strcmp(tabla_auxiliar->campos[i], comtok[1]) == 0)
                {

                    if (tabla_auxiliar->tipos[i] == NUM)
                    {

                        if (validar_decimal(comtok[3]) == 1)
                        {
                            count(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);
                            break;
                        }
                        else
                        {
                            printf("ERROR: tipos con las columnas\n");
                            break;
                        }
                    }
                    else if (tabla_auxiliar->tipos[i] == DATE)
                    {
                        if (validar_fecha_DMA(comtok[3]) == 1)
                        {
                            count(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);
                            break;
                        }
                        else
                        {
                            printf("ERROR: tipos con las columnas\n");
                            break;
                        }
                    }
                    else if (tabla_auxiliar->tipos[i] == EMAIL)
                    {
                        if (validar_email(comtok[3]) == 1)
                        {
                            count(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);
                            break;
                        }
                        else
                        {
                            printf("ERROR: tipos con las columnas\n");
                            break;
                        }
                    }
                    else if (tabla_auxiliar->tipos[i] == TEXT)
                    {
                        count(tabla_auxiliar->numCampos, linea_principal, tabla_auxiliar->tipos[i], comtok[3], comtok[2], i);
                        break;
                    }
                    else
                    {
                        printf("ERROR de tipos\n");
                        i++;
                        continue;
                    }
                }

                if (i + 1 == tabla_auxiliar->numCampos)
                {
                    printf("ERROR: campo no encontrado");
                }
            }
        }

        else if (strcmp(comtok[0], "erase") == 0 && strcmp(comtok[1], "lineas") == 0)
        {

            if (num_arg != 2)
            {
                printf("Error, numero de parametros incorrecto\n");
                continue;
            }

            if (strcmp(prompt, "*") == 0)
            {
                printf("Error, no hay una base de datos activa.\n");
                continue;
            }
            if (strcmp(prompt, nombre_bd) == 0)
            {
                printf("Todavia no hay una tabla activa\n");
                continue;
            }

            borrar_lineas(linea_principal, tabla_principal->numCampos, prompt, nombre_bd);
            linea_principal = NULL;
        }

        else
        {
            printf("No reconozco esta orden\n");
            continue;
        }
    }
}

//! FUNCIONES EXTRA:

//? LIST DB ASC/DES -- LIST TABLE ASC/DES

//? RENAME DB/TABLE

//? UPDATE edad = 18 25 -> pasa de 18 a 25 años

//? REMOVE TABLE/DB

//? SELECT CAMPO MAX/MIN

//? CUT TABLE nombre_tabla FROM nombreBD

//? SELECT GROUPBY nombre_campo

//? ALTER clientes EDADES NUM DIRECCION TEXT

//? JOIN tabla1 tabla2 y JOIN tabla1 tabla2 con fichero

//? BORRAR CAMPO FROM TABLA

//? SELECT COUNT VALOR VALOR ... // SELECT COUNT CAMPO <=> VALOR

//? SELECT campo SUM/PROM

//? ADD CAMPO nombre TEXT

//? BORRAR LINEAS
