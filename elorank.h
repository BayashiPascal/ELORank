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
#define ELORANK_STARTELO 0.0

// ================= Data structure ===================

typedef struct ELOEntity {
  // Pointer toward user struct
  void* _data;
  // Number of evaluation
  long _nbRun;
  // Sum of evalutation
  float _sumSoftElo;
  // Flag to memorize if the entity is a milestone
  // (whose elo is blocked)
  bool _isMilestone;
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
static inline
#endif
void ELORankSetK(ELORank* const that, const float k);

// Get the K coefficient of 'that' 
#if BUILDMODE != 0
static inline
#endif
float ELORankGetK(const ELORank* const that);

// Add the entity 'data' to 'that' 
void ELORankAdd(ELORank* const that, void* const data);

// Remove the entity 'data' from 'that' 
void ELORankRemove(ELORank* const that, void* data);

// Get the number of entity in 'that'
#if BUILDMODE != 0
static inline
#endif
int ELORankGetNb(const ELORank* const that);

// Update the ranks in 'that' with results 'res' given as a GSet of 
// pointers toward entities (_data in GSetElem equals _data in 
// ELOEntity) in winning order
// The _sortVal of the GSet represents the score (and so position)
// of the entities for this update (thus, equal _sortVal means tie)
// The set of results must contain at least 2 elements
// Elements in the result set must be in the ELORank 
void ELORankUpdate(ELORank* const that, const GSet* const res);

// Get the current rank of the entity 'data' (starts at 0)
int ELORankGetRank(const ELORank* const that, const void* const data);

// Get the current ELO of the entity 'data'
float ELORankGetELO(const ELORank* const that, const void* const data);

// Get the current soft ELO (average of elo over nb of evaluation) 
// of the entity 'data'
float ELORankGetSoftELO(const ELORank* const that, 
  const void* const data);

// Set the current ELO of the entity 'data' to 'elo'
void ELORankSetELO(const ELORank* const that, const void* const data, 
  const float elo);

// Set the milestone flag of the entity 'data' to 'flag'
void ELORankSetIsMilestone(const ELORank* const that, 
  const void* const data, const bool flag);

// Reset the milestone flag of all the entitities to false
void ELORankResetAllMilestone(const ELORank* const that);

// Reset the current ELO of the entity 'data'
void ELORankResetELO(const ELORank* const that, const void* const data);

// Get the 'rank'-th entity according to current ELO of 'that'  
// (starts at 0)
const ELOEntity* ELORankGetRanked(const ELORank* const that, const int rank);

// ================ static inliner ====================

#if BUILDMODE != 0
#include "elorank-inline.c"
#endif


#endif
