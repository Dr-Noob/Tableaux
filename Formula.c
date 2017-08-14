#include "Formula.h"
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

int SoloTieneUnAtomo(Formula f) { //1 true, 0 false
  if(f->f1 == NULL) {
    if(f->a1 != NULL && f->a2 != NULL)return 0;
    if(f->f2 == NULL)return 1;
    else return 0;
  }
  return 0;
}

Formula NegarFormula(Formula f) {
  if(SoloTieneUnAtomo(f)){
    if(f->a1 == NULL)f->a2->not = NEGADO;
    else f->a1->not = NEGADO;
  }
  else f->not = NEGADO;
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

Formula CrearFormulaParentesis(Formula original) {
  /*
  Formula f = malloc(sizeof(struct FormulaRep));
  Formula anidada = malloc(sizeof(struct FormulaRep));

  if(original->a1 == NULL) { //A la izquierda hay una formula
    anidada->f1 = CopiarFormula(original->f1);
    if(original->a2 == NULL) { // A la derecha hay una formula
      anidada->f2 = CopiarFormula(original->f2);
    }
    else { // A la derecha no hay una formula
      anidada->a2 = CopiarAtomo(original->a2);
    }
  }
  else { //Ni la izquierda ni a la derecha hay formulas
    anidada->a1 = CopiarAtomo(original->a1);
    anidada->a2 = CopiarAtomo(original->a2);
  }

  f->f1 = anidada;
  f->sig = original->sig;
  //Liberar original
	return f;
  */
  return original;
  Formula f = malloc(sizeof(struct FormulaRep));
  f->f1 = original;
  return f;
}

Formula Unir(Formula formula1,int operador,Formula formula2) {
  Formula f = malloc(sizeof(struct FormulaRep));

  if(formula1 == NULL || formula2 == NULL) {
    printf("ERROR: Formula nula\n");
    return NULL;
  }

  /*
  show(formula1);
  printf("\n");
  show(formula2);
  printf("\n");
  */

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

//Dejar f2 al final de f1
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
  return t;
}

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

void showTableaux(Tableaux t) {
  show(t->f);
  if(t->ti != NULL)showTableaux(t->ti);
  if(t->td != NULL)showTableaux(t->td);
}

Formula ExtraerIzquierda(Formula f) {
  if(f->f1 == NULL)return CrearFormula(f->a1);
  return CopiarFormula(f->f1);
}

Formula ExtraerDerecha(Formula f) {
  if(f->f2 == NULL)return CrearFormula(f->a2);
  return CopiarFormula(f->f2);
}

Formula BuscarOracion(int busqueda, Formula aux, Tableaux t) {
  aux = t->f;
  for(int i=0;i<busqueda-1;i++)aux = aux->sig;
  return aux;
}

void Resolver(Tableaux t) {
  int busqueda = 0;
  show(t->f);
  printf("\n");
  Formula oracion = t->f;

  //Busqueda del siguiente
  while(oracion->sig != NULL && SoloTieneUnAtomo(oracion)) {
    busqueda++;
    oracion = oracion->sig;
  }
  //Si es nodo, no seguir
  if(oracion->sig == NULL && SoloTieneUnAtomo(oracion)) { //Añadir a list
    return;
  }
  //Si no lo es, ramificar
  else {
    switch(oracion->COD_OP) {
      Formula aux;
      Formula izquierda;
      Formula derecha;

      case COD_DIMP:
        t->ti = CrearTableaux(CopiarFormula(t->f));
        Formula tmp1 = malloc(sizeof(struct FormulaRep));
        Formula tmp2 = malloc(sizeof(struct FormulaRep));
        if(busqueda == 0) {
          izquierda = ExtraerIzquierda(t->ti->f);
          derecha = ExtraerDerecha(t->ti->f);
          tmp1 = Unir(izquierda, COD_IMP, derecha);
          tmp2 = Unir(derecha, COD_IMP, izquierda);
          tmp2->sig = t->ti->f->sig;
          tmp1->sig = tmp2;
          t->ti->f = tmp1;
        }
        else {
          aux = BuscarOracion(busqueda,aux,t->ti);
          izquierda = ExtraerIzquierda(aux->sig);
          derecha = ExtraerDerecha(aux->sig);
          tmp1 = Unir(izquierda, COD_DIMP, derecha);
          tmp2 = Unir(derecha, COD_DIMP, izquierda);
          tmp2->sig = t->ti->f->sig;
          tmp1->sig = tmp2;
          t->ti->f = tmp1;
        }
        Resolver(t->ti);

        break;


      case COD_AND:
        t->ti = CrearTableaux(CopiarFormula(t->f));
        if(busqueda == 0) {
          izquierda = ExtraerIzquierda(t->ti->f);
          derecha = ExtraerDerecha(t->ti->f);
          derecha->sig = t->ti->f->sig;
          izquierda->sig = derecha;
          t->ti->f = derecha;
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

        break;


      case COD_IMP:
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

        break;


      case COD_OR:
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

        break;
    }
  }
}

void ResolverTableaux(Formula oracion) {
  Tableaux t = CrearTableaux(oracion);
  Resolver(t);
}
