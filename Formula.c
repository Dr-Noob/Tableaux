#include "Formula.h"
#include "Tree.h"
#include "SVG.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct AtomoRep {
  char id; //Identificador
  int not; //NEGADO-> el atomo va negado, SIN_NEGAR-> no va negado
};

struct FormulaRep {
  Atomo a1;	//Atomo1
  Atomo a2;	//Atomo2
  Formula f1; //Formula anidada
  Formula f2; //Formula anidada
  int COD_OP;	//Codigo operador
  int not; //NEGADO-> el atomo va negado, SIN_NEGAR-> no va negado
  struct FormulaRep *sig; //Siguiente formula
};

struct TableauxRep {
  Formula f;
  Tableaux ti; //Tableaux izquierdo
  Tableaux td; //Tableaux derecho
  int etiqueta; //VACIO, CERRADO, ABIERTO
};

Formula CrearFormula(Atomo a) {
	Formula f = malloc(sizeof(struct FormulaRep));
	f->a1 = a;
  f->a2 = NULL;
  f->f1 = NULL;
  f->f2 = NULL;
	f->COD_OP = COD_INVALIDO;
  f->not = SIN_NEGAR;
  f->sig = NULL;
	return f;
}

Atomo CrearAtomo(char _id,int _not) {
  Atomo a = malloc(sizeof(struct AtomoRep));
  a->id = _id;
  a->not = _not;
  return a;
}

Atomo CopiarAtomo(Atomo original) {
    return CrearAtomo(original->id, original->not);
}

//Devuelve el unico atomo que  contiene(si tiene mas de uno, devuelve NULL)
Atomo ExtraerAtomo(Formula f) {
  if(f->f1 == NULL && f->f2 == NULL) {
    if(f->a1 == NULL)return f->a2;
    else if(f->a2 == NULL)return f->a1;
    else return NULL;
  }
  return NULL;
}

int SoloTieneUnAtomo(Formula f) {
  return(f->f1 == NULL && (f->a1 != NULL ^ f->a2 != NULL) && f->f2 == NULL);
}

int EsAlfaFormula(Formula f) {
  return ((f->COD_OP == COD_AND) || (f->COD_OP == COD_OR && f->not == NEGADO) || (f->COD_OP == COD_IMP && f->not == NEGADO) || (f->COD_OP == COD_DIMP));
}

Formula NegarFormula(Formula f) {
  if(SoloTieneUnAtomo(f)){
    if(f->a1 == NULL) { //Negar a2
      if (f->a2->not == NEGADO)f->a2->not = SIN_NEGAR;
      else f->a2->not = NEGADO;
    }
    else { //Negar a1
      if(f->a1->not == NEGADO)f->a1->not = SIN_NEGAR;
      else f->a1->not = NEGADO;
    }
  }
  else { //Negar la formula
    if(f->not == NEGADO)f->not = SIN_NEGAR;
    else f->not = NEGADO;
  }
  return f;
}

Formula CopiarFormula(Formula original) {
  Formula copia=NULL;
  Formula anterior=NULL;
  Atomo a1;
  Atomo a2;

  while(original != NULL) {
    //Copiar
    Formula temp = malloc(sizeof(struct FormulaRep));
    if(original->a1 != NULL) {
      a1 = CrearAtomo(original->a1->id,original->a1->not);
      temp->a1=a1;
    }
    if(original->a2 != NULL) {
      a2 = CrearAtomo(original->a2->id,original->a2->not);
      temp->a2=a2;
    }
    if(original->f1 != NULL)temp->f1 = CopiarFormula(original->f1);
    if(original->f2 != NULL)temp->f2 = CopiarFormula(original->f2);
    temp->COD_OP=original->COD_OP;
    temp->not = original->not;

    //Enlazar
    if(copia==NULL){
        copia=temp;
        anterior=temp;
    }
    else {
        anterior->sig=temp;
        anterior=temp;
    }
    original=original->sig;
  }

  return copia;
}

Formula Unir(Formula formula1,int operador,Formula formula2) {
  Formula f = malloc(sizeof(struct FormulaRep));

  if(formula1 == NULL || formula2 == NULL) {
    printf("ERROR: Formula nula\n");
    return NULL;
  }

  if(formula1->f1 == NULL) { //formula1 no tiene la primera formula
    if(formula1->f2 == NULL) { //formula1 no tiene ni la primera ni la segunda formula
      if(formula1->a2 == NULL) { //formula1 solo tiene un atomo
        f->a1 = CopiarAtomo(formula1->a1);
        if(formula1->not == NEGADO)f->a1->not = NEGADO;
      }
      else { //formula1 tiene los dos atomos
        Formula aux = malloc(sizeof(struct FormulaRep));
        aux->a1 = CopiarAtomo(formula1->a1);
        aux->a2 = CopiarAtomo(formula1->a2);
        aux->not = NEGADO;
        aux->COD_OP = formula1->COD_OP;
        aux->not = formula1->not;
        f->f1 = aux;
      }
    }
    else { //formula1 si tiene la segunda formula
      if(formula1->a1 == NULL) { //formula1 no tiene atomos
        f->f1 = CopiarFormula(formula1);
      }
      else { //formula1 tiene un atomo(el a1)
        Formula aux = malloc(sizeof(struct FormulaRep));
        aux->f2 = CopiarFormula(formula1->f2);
        aux->a1 = CopiarAtomo(formula1->a1);
        if(formula1->not == NEGADO)aux->a1->not = NEGADO;
        aux->COD_OP = formula1->COD_OP;
        aux->not = formula1->not;
        f->f1 = aux;
      }
    }
  }
  else { //formula1 tiene formulas
    if(formula1->f2 == NULL) { //formula1 solo tiene una formula
      if(formula1->a2 == NULL) { //formula1 no tiene atomos
        f->f1 = CopiarFormula(formula1);
      }
      else { //formula1 tiene un atomo(el a2)
        Formula aux = malloc(sizeof(struct FormulaRep));
        aux->f1 = CopiarFormula(formula1->f1);
        aux->a2 = CopiarAtomo(formula1->a2);
        if(formula1->not == NEGADO)aux->a2->not = NEGADO;
        aux->COD_OP = formula1->COD_OP;
        aux->not = formula1->not;
        f->f1 = aux;
      }
    }
    else { //formula1 tiene las dos formulas
      Formula aux = malloc(sizeof(struct FormulaRep));
      aux->f1 = CopiarFormula(formula1->f1);
      aux->f2 = CopiarFormula(formula1->f2);
      aux->COD_OP = formula1->COD_OP;
      aux->not = formula1->not;
      f->f1 = aux;
    }
  }

  //FORMULA 2//
  if(formula2->f1 == NULL) { //formula2 no la primera formula
    if(formula2->f2 == NULL) { //formula2 no tiene ni la primera ni la segunda formula
      if(formula2->a2 == NULL) { //formula2 solo tiene un atomo
        f->a2 = CopiarAtomo(formula2->a1);
        if(formula2->not == NEGADO)f->a2->not = NEGADO;
      }
      else { //formula2 tiene los dos atomos
        Formula aux = malloc(sizeof(struct FormulaRep));
        aux->a1 = CopiarAtomo(formula2->a1);
        aux->a2 = CopiarAtomo(formula2->a2);
        aux->COD_OP = formula2->COD_OP;
        aux->not = formula2->not;
        f->f2 = aux;
      }
    }
    else { //formula2 si tiene la segunda formula
      if(formula2->a1 == NULL) { //formula2 no tiene atomos
        f->f2 = CopiarFormula(formula2);
      }
      else { //formula2 tiene un atomo(el a1)
        Formula aux = malloc(sizeof(struct FormulaRep));
        aux->f2 = CopiarFormula(formula2->f2);
        aux->a1 = CopiarAtomo(formula2->a1);
        if(formula2->not == NEGADO)aux->a1->not = NEGADO;
        aux->COD_OP = formula2->COD_OP;
        aux->not = formula2->not;
        f->f2 = aux;
      }
    }
  }
  else { //formula2 tiene formulas
    if(formula2->f2 == NULL) { //formula2 solo tiene una formula
      if(formula2->a2 == NULL) { //formula2 no tiene atomos
        f->f2 = CopiarFormula(formula2);
      }
      else { //formula2 tiene un atomo
        Formula aux = malloc(sizeof(struct FormulaRep));
        aux->f1 = CopiarFormula(formula2->f1);
        aux->a2 = CopiarAtomo(formula2->a2);
        if(formula2->not == NEGADO)aux->a2->not = NEGADO;
        aux->COD_OP = formula2->COD_OP;
        aux->not = formula2->not;
        f->f2 = aux;
      }
    }
    else { //formula2 tiene las dos formulas
      f->f2 = CopiarFormula(formula2);
    }
  }
  f->COD_OP = operador;
	return f;
}

//Concatena las dos formulas
Formula Concatenar(Formula f1,Formula f2) {
  Formula aux = f1;
  while(aux->sig != NULL)aux = aux->sig;
	aux->sig = f2;
	return f1;
}

Tableaux CrearTableaux(Formula inicial) {
  Tableaux t = malloc(sizeof(struct TableauxRep));
  t->f = inicial;
  t->ti = NULL;
  t->td = NULL;
  t->etiqueta = VACIO;
  return t;
}

//Funciones IMPRIMIR

void printCOD(int COD_OP) {
  switch(COD_OP) {
    case COD_DIMP:
      printf(" <-> ");
      break;
    case COD_IMP:
      printf(" -> ");
      break;
    case COD_AND:
      printf(" ^ ");
      break;
    case COD_OR:
      printf(" v ");
      break;
  }
}

void printAtomo(Atomo a) {
  if(a->not == NEGADO)printf("~");
  printf("%c",a->id);
}

void show(Formula f) {
	Formula aux = f;

  if(aux->not == NEGADO)printf("~");
  printf("[");
  if(aux->f1 != NULL) { //Existe formula izquierda
    show(aux->f1);
    printCOD(aux->COD_OP);
    if(aux->f2 != NULL)show(aux->f2); //Existe formula derecha
    else if(aux->a2 != NULL)printAtomo(aux->a2); //No existe formula derecha
  }
  else { //No existe formula izquierda
    if(aux->a1 == NULL)printf("ERROR: a1 es nulo\n");
    printAtomo(aux->a1);
    printCOD(aux->COD_OP);
    if(aux->f2 != NULL)show(aux->f2); //Existe formula derecha
    else if(aux->a2 != NULL)printAtomo(aux->a2); //No existe formula derecha
  }
  printf("]");


	while(aux->sig != NULL){
		aux = aux->sig;

    if(aux->not == NEGADO)printf("~");
    printf("[");
    if(aux->f1 != NULL) { //Existe formula izquierda
      show(aux->f1);
      printCOD(aux->COD_OP);
      if(aux->f2 != NULL)show(aux->f2); //Existe formula derecha
      else if(aux->a2 != NULL)printAtomo(aux->a2); //No existe formula derecha
    }
    else { //No existe formula izquierda
      if(aux->a1 == NULL)printf("ERROR: a1 es nulo\n");
      printAtomo(aux->a1);
      printCOD(aux->COD_OP);
      if(aux->f2 != NULL)show(aux->f2); //Existe formula derecha
      else if(aux->a2 != NULL)printAtomo(aux->a2); // No existe formula derecha
    }
    printf("]");
  }

}

//Funciones ascii para devolver un char* en vez de imprimir directamente
//Usadas por el ascii_tree

char* showAtomo_ascii(char* buf, Atomo a) {
  if(a->not == NEGADO)sprintf(buf+strlen(buf),"&#172;");
  sprintf(buf+strlen(buf),"%c",a->id);
  return buf;
}

//https://www.w3schools.com/charsets/ref_utf_dingbats.asp
//https://www.w3schools.com/charsets/ref_html_entities_r.asp
char* showCOD_ascii(char* buf, int COD_OP) {
  switch(COD_OP) {
    case COD_DIMP:
      sprintf(buf+strlen(buf)," &#8596; ");
      break;
    case COD_IMP:
      sprintf(buf+strlen(buf)," &#8594; ");
      break;
    case COD_AND:
      sprintf(buf+strlen(buf)," &#8743; ");
      break;
    case COD_OR:
      sprintf(buf+strlen(buf)," &#8744; ");
      break;
  }
  return buf;
}

char* show_ascii(char* buf, Formula f) {
  Formula aux = f;

  if(aux->not == NEGADO)sprintf(buf+strlen(buf),"&#172;");
  sprintf(buf+strlen(buf),"(");
  if(aux->f1 != NULL) { //Existe formula izquierda
    show_ascii(buf,aux->f1);
    showCOD_ascii(buf,aux->COD_OP);
    if(aux->f2 != NULL)show_ascii(buf,aux->f2); //Existe formula derecha
    else if(aux->a2 != NULL)showAtomo_ascii(buf,aux->a2); //No existe formula derecha
  }
  else { //No existe formula izquierda
    if(aux->a1 == NULL)printf("ERROR: a1 es nulo\n");
    showAtomo_ascii(buf,aux->a1);
    showCOD_ascii(buf,aux->COD_OP);
    if(aux->f2 != NULL)show_ascii(buf,aux->f2); //Existe formula derecha
    else if(aux->a2 != NULL)showAtomo_ascii(buf,aux->a2); //No existe formula derecha
  }
  sprintf(buf+strlen(buf),")");

  while(aux->sig != NULL){
		aux = aux->sig;

    if(aux->not == NEGADO)sprintf(buf+strlen(buf),"&#172;");
    sprintf(buf+strlen(buf),"(");
    if(aux->f1 != NULL) { //Existe formula izquierda
      show_ascii(buf,aux->f1);
      showCOD_ascii(buf,aux->COD_OP);
      if(aux->f2 != NULL)show_ascii(buf,aux->f2); //Existe formula derecha
      else if(aux->a2 != NULL)showAtomo_ascii(buf,aux->a2); //No existe formula derecha
    }
    else { //No existe formula izquierda
      if(aux->a1 == NULL)printf("ERROR: a1 es nulo\n");
      showAtomo_ascii(buf,aux->a1);
      showCOD_ascii(buf,aux->COD_OP);
      if(aux->f2 != NULL)show_ascii(buf,aux->f2); //Existe formula derecha
      else if(aux->a2 != NULL)showAtomo_ascii(buf,aux->a2); //No existe formula derecha
    }
    sprintf(buf+strlen(buf),")");
  }

  return buf;
}

//Imprime un tableaux
void showTableaux(Tableaux t) {
  show(t->f);
  printf("\n");
  if(t->ti != NULL)showTableaux(t->ti);
  if(t->td != NULL)showTableaux(t->td);
}

//FIN Funciones IMPRIMIR

//Funcion recursiva que devuelve el 'tree' correspondiende al tableaux 't'
Tree *CrearArbolDesdeTableaux(Tree *tree, Tableaux t) {
  char *buffer = malloc(sizeof(char)*MAX_CHAR);
  tree = malloc (sizeof (Tree));
  tree->element = show_ascii(buffer,t->f);
  if(t->etiqueta != VACIO) {
    if(t->etiqueta == CERRADO)tree->color = COLOR_RED;
    else tree->color = COLOR_GREEN;
  }
  if(t->ti != NULL)tree->left = CrearArbolDesdeTableaux(tree->left,t->ti);
  if(t->td != NULL)tree->right = CrearArbolDesdeTableaux(tree->right,t->td);
  return tree;
}

int nCaracteres(char* cadena) {
  int count = 0;
  for(int m=0; cadena[m]; m++) {
    if(cadena[m] != ' ')count ++;
  }
  return count;
}

int offset = 100;

SVG_data *CrearSVGDesdeTableauxRecursivo(SVG_data *s,Tableaux t,int incX,int incY,int nivel) {
	s = malloc(sizeof(SVG_data));
	if(t->ti != NULL)s->hi = CrearSVGDesdeTableauxRecursivo(s->hi,t->ti,incX,incY,nivel+1);
	if(t->td != NULL)s->hd = CrearSVGDesdeTableauxRecursivo(s->hd,t->td,incX,incY,nivel+1);
	char *buffer = malloc(sizeof(char)*MAX_CHAR);
  s->formula = show_ascii(buffer,t->f);
  s->centro = (nCaracteres(buffer)/2)*6;
  s->y = nivel*incY+20; 
  
	if(t->ti == NULL) {
		s->x = offset;
		offset += incX;
	}
	else if (t->td != NULL) s->x = s->hi->x + (s->hd->x - s->hi->x)/2;
	else s->x = s->hi->x;
	
	return s;
}


//Minimo y maximo

int *MinXSVGRecursivo(SVG_data *s,int *min) {
    if(*min > s->x)*min = s->x;
    if(s->hi != NULL)MinXSVGRecursivo(s->hi,min);
    if(s->hd != NULL)MinXSVGRecursivo(s->hd,min);
    return min;
}

int *MinXSVG(SVG_data *s) {
  int *min = malloc(sizeof(int));
  *min = 2000;
  return MinXSVGRecursivo(s,min);
}

int *MaxXSVGRecursivo(SVG_data *s,int *max) {
    if(*max < s->x)*max = s->x;
    if(s->hi != NULL)MaxXSVGRecursivo(s->hi,max);
    if(s->hd != NULL)MaxXSVGRecursivo(s->hd,max);
    return max;
}

int *MaxXSVG(SVG_data *s) {
  int *max = malloc(sizeof(int));
  *max = 0;
  return MaxXSVGRecursivo(s,max);
}

int *MaxYSVGRecursivo(SVG_data *s,int *max) {
    if(*max < s->y)*max = s->y;
    if(s->hi != NULL)MaxYSVGRecursivo(s->hi,max);
    if(s->hd != NULL)MaxYSVGRecursivo(s->hd,max);
    return max;
}

int *MaxYSVG(SVG_data *s) {
  int *max = malloc(sizeof(int));
  *max = 0;
  return MaxYSVGRecursivo(s,max);
}

//Minimo y maximo

SVG_data *AjustarSVGMargenRecursivo(SVG_data *s,int *min) {
  s->x -= *min;
  if(s->hi != NULL)AjustarSVGMargenRecursivo(s->hi,min);
  if(s->hd != NULL)AjustarSVGMargenRecursivo(s->hd,min);
  return s;
}

SVG *AjustarSVGMargen(SVG *s) {
  int* xmin = MinXSVG(s->data);
  int* xmax = MaxXSVG(s->data);
  *xmax -= *xmin - 100;
  int* ymax = MaxYSVG(s->data);
  *ymax += 100;
  s->data = AjustarSVGMargenRecursivo(s->data,xmin);
  s->xmax = *xmax;
  s->ymax = *ymax;
  free(xmin);
  free(xmax);
  free(ymax);
  return s;
}

SVG *CrearSVGDesdeTableaux(SVG *s, Tableaux t) {
	int incY = 140;
	int incX = 100;
	int nivel = 0;
	s = malloc(sizeof(struct SVG));
	s->xmax = 2000;
	s->ymax = 2000;
	s->data = CrearSVGDesdeTableauxRecursivo(s->data,t,incX,incY,nivel);
  //s = AjustarSVGMargen(s);
  return s;
}

int TableauxCerrado(Tableaux t) {
  int tmp;
  if(t->etiqueta == ABIERTO)return BOOLEAN_FALSE;

  if(t->ti != NULL) {
    tmp = TableauxCerrado(t->ti);
    if(tmp == BOOLEAN_FALSE)return BOOLEAN_FALSE;
  }
  if(t->td != NULL) {
    tmp = TableauxCerrado(t->td);
    if(tmp == BOOLEAN_FALSE)return BOOLEAN_FALSE;
  }
  return BOOLEAN_TRUE;
}

//Imprime el Tableaux en forma de arbol
void showTableauxTree(Tableaux t) {
  Tree *arbol = CrearArbolDesdeTableaux(arbol,t);
  print_ascii_tree(arbol);
}

void showTableauxSVG(Tableaux t) {
	SVG *s = CrearSVGDesdeTableaux(s,t);
	print_svg(s);
}

//Devuelve la formula o atomo que haya en la izquierda de 'f'
Formula ExtraerIzquierda(Formula f) {
  if(f->f1 == NULL)return CrearFormula(f->a1);
  return CopiarFormula(f->f1);
}

//Devuelve la formula o atomo que haya en la derecha de 'f'
Formula ExtraerDerecha(Formula f) {
  if(f->f2 == NULL)return CrearFormula(f->a2);
  return CopiarFormula(f->f2);
}

//Busca la oracion 'aux' en el tableaux 't' que esta a 'busqueda' iteraciones de distancia
Formula BuscarOracion(int busqueda, Formula aux, Tableaux t) {
  aux = t->f;
  for(int i=0;i<busqueda-1;i++)aux = aux->sig;
  return aux;
}

//True si en 'f' existe el negado de 'a', false si no existe
int ContieneSuNegado(Atomo a, Formula f) {
  Formula aux = f;
  Atomo tmp;
  while(aux->sig != NULL) {
    tmp = ExtraerAtomo(aux);
    if(tmp != NULL && tmp->id == a->id && tmp->not != a->not)return BOOLEAN_TRUE;
    aux = aux->sig;
  }
  tmp = ExtraerAtomo(aux);
  if(tmp != NULL && tmp->id == a->id && tmp->not != a->not)return BOOLEAN_TRUE;
  else return BOOLEAN_FALSE;
}

//True si existe un atomo y su negado, False si no
int ContieneContradiccion(Formula f) {
  Formula aux = f;
  Atomo tmp;
  while(aux->sig != NULL) {
    tmp = ExtraerAtomo(aux);
    //Si no es nulo, extraer devolvio el unico atomo
    if(tmp != NULL && ContieneSuNegado(tmp,f))return BOOLEAN_TRUE;
    aux = aux->sig;
  }
  tmp = ExtraerAtomo(aux);
  if(tmp != NULL && ContieneSuNegado(tmp,f))return BOOLEAN_TRUE;
  else return BOOLEAN_FALSE;
}

//Funciones para resolver alfa formulas
//(P ^ Q)
void and(Tableaux t,int busqueda);
//~(P V Q)
void orNegado(Tableaux t,int busqueda);
//~(P -> Q)
void impNegado(Tableaux t,int busqueda);
//(P <-> Q)
void dobleImp(Tableaux t,int busqueda);
//FIN_Funciones para resolver alfa formulas

//Funciones para resolver beta formulas
//~(P ^ Q)
void andNegado(Tableaux t,int busqueda);
//(P V Q)
void or(Tableaux t,int busqueda);
//(P -> Q)
void imp(Tableaux t,int busqueda);
//~(P <-> Q)
void dobleImpNegado(Tableaux t,int busqueda);
//FIN_Funciones para resolver beta formulas

//Resuelve el tableaux 't'
void Resolver(Tableaux t) {
  int busqueda = 0;
  Formula oracion = t->f;

  //Busqueda de lo siguiente que sea necesario resolver, intentado que sea alfa formula
  while(oracion->sig != NULL && (SoloTieneUnAtomo(oracion) || !EsAlfaFormula(oracion))) {
    busqueda++;
    oracion = oracion->sig;
  }

  //No hemos encontrado una alfa formula para resolver; buscar una beta formula
  if(!EsAlfaFormula(oracion)) {
    oracion = t->f;
    busqueda = 0;
    while(oracion->sig != NULL && SoloTieneUnAtomo(oracion)) {
      busqueda++;
      oracion = oracion->sig;
    }
  }

  //Si ya esta todo resuelto en esta rama o hay una contradiccion, terminar
  //y marcar el tableaux como corresponda(ABIERTO o CERRADO)
  if(oracion->sig == NULL && SoloTieneUnAtomo(oracion)) {
    if (ContieneContradiccion(t->f))t->etiqueta = CERRADO;
    else t->etiqueta = ABIERTO;
    return;
  }
  if (ContieneContradiccion(t->f)) {
    t->etiqueta = CERRADO;
    return;
  }
  //Si no lo esta, ramificar
  else {
    switch(oracion->COD_OP) {
      case COD_DIMP:
        if(oracion->not == NEGADO) dobleImpNegado(t,busqueda);
        else dobleImp(t,busqueda);
        break;

      case COD_AND:
        if(oracion->not == NEGADO) andNegado(t,busqueda);
        else and(t,busqueda);
        break;

      case COD_IMP:
        if(oracion->not == NEGADO) impNegado(t,busqueda);
        else imp(t,busqueda);
        break;

      case COD_OR:
        if(oracion->not == NEGADO) orNegado(t,busqueda);
        else or(t,busqueda);
        break;
    }
  }
}

//Funciones para resolver alfa formulas
void and(Tableaux t,int busqueda) {
  Formula aux;
  Formula izquierda;
  Formula derecha;

  t->ti = CrearTableaux(CopiarFormula(t->f));
  if(busqueda == 0) {
    izquierda = ExtraerIzquierda(t->ti->f);
    derecha = ExtraerDerecha(t->ti->f);
    derecha->sig = t->ti->f->sig;
    izquierda->sig = derecha;
    t->ti->f = izquierda;
  }
  else {
    aux = BuscarOracion(busqueda,aux,t->ti);
    izquierda = ExtraerIzquierda(aux->sig);
    derecha = ExtraerDerecha(aux->sig);
    derecha->sig = aux->sig->sig;
    izquierda->sig = derecha;
    aux->sig = izquierda;
  }
  Resolver(t->ti);
}

void orNegado(Tableaux t,int busqueda) {
  Formula aux;
  Formula izquierda;
  Formula derecha;

  t->ti = CrearTableaux(CopiarFormula(t->f));
  if(busqueda == 0) {
    izquierda = NegarFormula(ExtraerIzquierda(t->ti->f)); //La unica diferencia respecto al AND
    derecha = NegarFormula(ExtraerDerecha(t->ti->f)); //La unica diferencia respecto al AND
    derecha->sig = t->ti->f->sig;
    izquierda->sig = derecha;
    t->ti->f = izquierda;
  }
  else {
    aux = BuscarOracion(busqueda,aux,t->ti);
    izquierda = NegarFormula(ExtraerIzquierda(aux->sig)); //La unica diferencia respecto al AND
    derecha = NegarFormula(ExtraerDerecha(aux->sig)); //La unica diferencia respecto al AND
    derecha->sig = aux->sig->sig;
    izquierda->sig = derecha;
    aux->sig = izquierda;
  }
  Resolver(t->ti);
}

void impNegado(Tableaux t,int busqueda) {
  Formula aux;
  Formula izquierda;
  Formula derecha;

  t->ti = CrearTableaux(CopiarFormula(t->f));
  if(busqueda == 0) {
    izquierda = ExtraerIzquierda(t->ti->f);
    derecha = NegarFormula(ExtraerDerecha(t->ti->f)); //La unica diferencia respecto al AND
    derecha->sig = t->ti->f->sig;
    izquierda->sig = derecha;
    t->ti->f = izquierda;
  }
  else {
    aux = BuscarOracion(busqueda,aux,t->ti);
    izquierda = ExtraerIzquierda(aux->sig);
    derecha = NegarFormula(ExtraerDerecha(aux->sig)); //La unica diferencia respecto al AND
    derecha->sig = aux->sig->sig;
    izquierda->sig = derecha;
    aux->sig = izquierda;
  }
  Resolver(t->ti);
}

void dobleImp(Tableaux t,int busqueda) {
  Formula aux;
  Formula izquierda;
  Formula derecha;

  t->ti = CrearTableaux(CopiarFormula(t->f));
  if(busqueda == 0) {
    izquierda = Unir(ExtraerIzquierda(t->ti->f), COD_IMP, ExtraerDerecha(t->ti->f));
    derecha = Unir(ExtraerDerecha(t->ti->f), COD_IMP, ExtraerIzquierda(t->ti->f));
    derecha->sig = t->ti->f->sig;
    izquierda->sig = derecha;
    t->ti->f = izquierda;
  }
  else {
    aux = BuscarOracion(busqueda,aux,t->ti);
    izquierda = Unir(ExtraerIzquierda(aux->sig), COD_IMP, ExtraerDerecha(aux->sig));
    derecha = Unir(ExtraerDerecha(aux->sig), COD_IMP, ExtraerIzquierda(aux->sig));
    derecha->sig = aux->sig->sig;
    izquierda->sig = derecha;
    aux->sig = izquierda;
  }
  Resolver(t->ti);
}
//FIN_Funciones para resolver alfa formulas

//Funciones para resolver beta formulas
void andNegado(Tableaux t,int busqueda) {
  Formula aux;
  Formula izquierda;
  Formula derecha;

  t->ti = CrearTableaux(CopiarFormula(t->f));
  if(busqueda == 0) {
    izquierda = NegarFormula(ExtraerIzquierda(t->ti->f)); //Unica diferencia con el OR
    izquierda->sig = t->ti->f->sig;
    t->ti->f = izquierda;
  }
  else {
    aux = BuscarOracion(busqueda,aux,t->ti);
    izquierda = NegarFormula(ExtraerIzquierda(aux->sig)); //Unica diferencia con el OR
    izquierda->sig = aux->sig->sig;
    aux->sig = izquierda;
  }
  Resolver(t->ti);

  t->td = CrearTableaux(CopiarFormula(t->f));
  if(busqueda == 0) {
    derecha = NegarFormula(ExtraerDerecha(t->td->f)); //Unica diferencia con el OR
    derecha->sig = t->td->f->sig;
    t->td->f = derecha;
  }
  else {
    aux = BuscarOracion(busqueda,aux,t->td);
    derecha = NegarFormula(ExtraerDerecha(aux->sig)); //Unica diferencia con el OR
    derecha->sig = aux->sig->sig;
    aux->sig = derecha;
  }
  Resolver(t->td);
}
void or(Tableaux t,int busqueda) {
  Formula aux;
  Formula izquierda;
  Formula derecha;

  t->ti = CrearTableaux(CopiarFormula(t->f));
  if(busqueda == 0) {
    izquierda = ExtraerIzquierda(t->ti->f);
    izquierda->sig = t->ti->f->sig;
    t->ti->f = izquierda;
  }
  else {
    aux = BuscarOracion(busqueda,aux,t->ti);
    izquierda = ExtraerIzquierda(aux->sig);
    izquierda->sig = aux->sig->sig;
    aux->sig = izquierda;
  }
  Resolver(t->ti);

  t->td = CrearTableaux(CopiarFormula(t->f));
  if(busqueda == 0) {
    derecha = ExtraerDerecha(t->td->f);
    derecha->sig = t->td->f->sig;
    t->td->f = derecha;
  }
  else {
    aux = BuscarOracion(busqueda,aux,t->td);
    derecha = ExtraerDerecha(aux->sig);
    derecha->sig = aux->sig->sig;
    aux->sig = derecha;
  }
  Resolver(t->td);
}

void imp(Tableaux t,int busqueda) {
  Formula aux;
  Formula izquierda;
  Formula derecha;

  t->ti = CrearTableaux(CopiarFormula(t->f));
  if(busqueda == 0) {
    izquierda = ExtraerIzquierda(t->ti->f);
    izquierda = NegarFormula(izquierda);
    izquierda->sig = t->ti->f->sig;
    t->ti->f = izquierda;
  }
  else {
    aux = BuscarOracion(busqueda,aux,t->ti);
    izquierda = ExtraerIzquierda(aux->sig);
    izquierda = NegarFormula(izquierda);
    izquierda->sig = aux->sig->sig;
    aux->sig = izquierda;
  }
  Resolver(t->ti);

  t->td = CrearTableaux(CopiarFormula(t->f));
  if(busqueda == 0) {
    derecha = ExtraerDerecha(t->td->f);
    derecha->sig = t->td->f->sig;
    t->td->f = derecha;
  }
  else {
    aux = BuscarOracion(busqueda,aux,t->td);
    derecha = ExtraerDerecha(aux->sig);
    derecha->sig = aux->sig->sig;
    aux->sig = derecha;
  }
  Resolver(t->td);
}

void dobleImpNegado(Tableaux t,int busqueda) {
  Formula aux;
  Formula izquierda;
  Formula derecha;

  t->ti = CrearTableaux(CopiarFormula(t->f));
  if(busqueda == 0) {
    izquierda = Unir(ExtraerIzquierda(t->ti->f), COD_IMP, ExtraerDerecha(t->ti->f));
    izquierda->sig = t->ti->f->sig;
    t->ti->f = izquierda;
    t->ti->f->not = NEGADO;
  }
  else {
    aux = BuscarOracion(busqueda,aux,t->ti);
    izquierda = Unir(ExtraerIzquierda(aux->sig), COD_IMP, ExtraerDerecha(aux->sig));
    izquierda->sig = aux->sig->sig;
    izquierda->not = NEGADO;
    aux->sig = izquierda;
  }
  Resolver(t->ti);

  t->td = CrearTableaux(CopiarFormula(t->f));
  if(busqueda == 0) {
    izquierda = Unir(ExtraerDerecha(t->td->f), COD_IMP, ExtraerIzquierda(t->td->f));
    izquierda->sig = t->td->f->sig;
    t->td->f = izquierda;
    t->td->f->not = NEGADO;
  }
  else {
    aux = BuscarOracion(busqueda,aux,t->td);
    izquierda = Unir(ExtraerDerecha(aux->sig), COD_IMP, ExtraerIzquierda(aux->sig));
    izquierda->sig = aux->sig->sig;
    izquierda->not = NEGADO;
    aux->sig = izquierda;
  }
  Resolver(t->td);
}
//FIN_Funciones para resolver beta formulas

void ResolverTableaux(Formula oracion) {
  Tableaux t = CrearTableaux(oracion);
  Resolver(t);
  /*
  printf("Solucion: \n\n\n");
  showTableauxTree(t);
  printf("\n\n\n");
  if(TableauxCerrado(t))printf(RED "El tableaux esta cerrado\n" RESET "La expresion inicial es insatisfacible\n");
  else printf(GREEN "El tableaux esta abierto\n" RESET "La expresion inicial es satisfacible\n");
  */
  //showTableauxTree(t);
  showTableauxSVG(t);
}
