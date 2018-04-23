// ============ ELORANK.H ================

#ifndef ELORANK_H
#define ELORANK_H

// ================= Include =================

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "pberr.h"
#include "gset.h"
#include "pbmath.h"

// ================= Define ==================

#define ELORANK_K 8.0
#define ELORANK_STARTELO 100.0

// ================= Data structure ===================

typedef struct ELOEntity {
  // Pointer toward user struct
  void* _data;
  // Number of evaluation
  int _nbRun;
} ELOEntity;

typedef struct ELORank {
  // ELO coefficient
  float _k;
  // Set of ELO entities
  GSet _set;
} ELORank;


// ================ Functions declaration ====================

// Create a new ELORank
ELORank* ELORankCreate(void);
/*#if BUILDMODE == 0
ELORank ELORankCreateStatic(void);
#endif*/

// Free memory used by an ELORank
void ELORankFree(ELORank** that);

// Free memory used by an ELOEntity
void ELOEntityFree(ELOEntity** that);

// Set the K coefficient of 'that' to 'k' 
#if BUILDMODE != 0
inline
#endif
void ELORankSetK(ELORank* that, float k);

// Get the K coefficient of 'that' 
#if BUILDMODE != 0
inline
#endif
float ELORankGetK(ELORank* that);

// Add the entity 'data' to 'that' 
void ELORankAdd(ELORank* that, void* data);

// Remove the entity 'data' from 'that' 
void ELORankRemove(ELORank* that, void* data);

// Get the number of entity in 'that'
#if BUILDMODE != 0
inline
#endif
int ELORankGetNb(ELORank* that);

// Update the ranks in 'that' with results 'res' given as a GSet of 
// pointers toward entities (_data in GSetElem equals _data in 
// ELOEntity) in winning order
// The _sortVal of the GSet represents the score (and so position)
// of the entities for this update (thus, equal _sortVal means tie)
// The set of results must contain at least 2 elements
// Elements in the result set must be in the ELORank 
void ELORankUpdate(ELORank* that, GSet* res);

// Get the current rank of the entity 'data' (starts at 0)
int ELORankGetRank(ELORank* that, void* data);

// Get the current ELO of the entity 'data'
float ELORankGetELO(ELORank* that, void* data);

// Get the 'rank'-th entity according to current ELO of 'that'  
// (starts at 0)
ELOEntity* ELORankGetRanked(ELORank* that, int rank);

// ================ Inliner ====================

#if BUILDMODE != 0
#include "elorank-inline.c"
#endif


#endif
