#ifndef __SVG__
#define __SVG__

#include "Formula.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct SVG_data SVG_data;
typedef struct SVG SVG;
typedef struct NodoRep *Nodo;
typedef struct PuntoRep *Punto;

#define ALTURA_CARACTER 15
#define MAX_NIVELES 50
#define INCY 140 //Incremento y
#define INCX 100 //Incremento x
#define YI 50 //y inicial
#define XI 50 //x inicial
static const char* NOMBRE_ARCHIVO = "sol.svg";

static const char* SIMBOLO_ABIERTO = "&#10004;";
static const char* SIMBOLO_CERRADO = "&#10008;";
static const char* SIMBOLO_AND = " &#8743; ";
static const char* SIMBOLO_OR = " &#8744; ";
static const char* SIMBOLO_IMP = " &#8594; ";
static const char* SIMBOLO_DIMP = " &#8596; ";
static const char* SIMBOLO_NOT = "&#172;";
static const float PIXELES_POR_CARACTER = 7.2025;

void showTableauxSVG(Tableaux t, FILE *fich);

#endif
