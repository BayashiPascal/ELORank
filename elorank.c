// ============ ELORANK.C ================

// ================= Include =================

#include "elorank.h"
#if BUILDMODE == 0
#include "elorank-inline.c"
#endif

// ================ Functions declaration ====================

// Create a new ELOEntity
static ELOEntity* ELOEntityCreate(void* data);

// Return the GSetElem in 'that'->_set for the entity 'data'
static GSetElem* ELORankGetElem(ELORank* that, void* data);

// ================ Functions implementation ====================

// Create a new ELORank
ELORank* ELORankCreate(void) {
  // Allocate memory
  ELORank* that = PBErrMalloc(ELORankErr, sizeof(ELORank));
  // Set the default coefficient
  that->_k = ELORANK_K;
  // Create the set of entities
  that->_set = GSetCreateStatic();
  // Return the new ELORank
  return that;
}

// Free memory used by an ELORank
void ELORankFree(ELORank** that) {
  // Check the argument
  if (that == NULL || *that == NULL) return;
  // Empty the set of entities
  GSet* set = &((*that)->_set);
  while (GSetNbElem(set) > 0) {
    ELOEntity *ent = GSetPop(set);
    ELOEntityFree(&ent);
  }
  // Free memory
  free(*that);
  // Set the pointer to null
  *that = NULL;
}

// Free memory used by an ELOEntity
void ELOEntityFree(ELOEntity** that) {
  // Check the argument
  if (that == NULL || *that == NULL) return;
  // Free memory
  free(*that);
  // Set the pointer to null
  *that = NULL;  
}

// Add the entity 'data' to 'that' 
void ELORankAdd(ELORank* that, void* data) {
#if BUILDMODE == 0
  // Check arguments
  if (that == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'that' is null");
    PBErrCatch(ELORankErr);
  }
  if (data == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'data' is null");
    PBErrCatch(ELORankErr);
  }
#endif
  // Create a new ELOEntity
  ELOEntity *ent = ELOEntityCreate(data);
  // Add the new entity to the set with a default score
  GSetAddSort(&(that->_set), ent, ELORANK_STARTELO);
}

// Create a new ELOEntity
static ELOEntity* ELOEntityCreate(void* data) {
#if BUILDMODE == 0
  // Check argument
  if (data == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'data' is null");
    PBErrCatch(ELORankErr);
  }
#endif
  // Allocate memory
  ELOEntity *that = PBErrMalloc(ELORankErr, sizeof(ELOEntity));
  // Set properties
  that->_data = data;
  that->_nbRun = 0;
  // Return the new ELOEntity
  return that;
}

// Remove the entity 'data' from 'that' 
void ELORankRemove(ELORank* that, void* data) {
#if BUILDMODE == 0
  // Check arguments
  if (that == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'that' is null");
    PBErrCatch(ELORankErr);
  }
  if (data == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'data' is null");
    PBErrCatch(ELORankErr);
  }
#endif
  // Search the entity
  GSetElem* elem = ELORankGetElem(that, data);
  // If we have found the entity
  if (elem != NULL) {
    // Free the memory 
    ELOEntityFree((ELOEntity**)(&(elem->_data)));
    // Remove the element
    GSetRemoveElem(&(that->_set), &elem);
  }
}

// Return the GSetElem in 'that'->_set for the entity 'data'
static GSetElem* ELORankGetElem(ELORank* that, void* data) {
#if BUILDMODE == 0
  // Check arguments
  if (that == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'that' is null");
    PBErrCatch(ELORankErr);
  }
  if (data == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'data' is null");
    PBErrCatch(ELORankErr);
  }
#endif
  // Search the element
  GSetElem* elem = that->_set._head;
  while (elem != NULL && ((ELOEntity*)(elem->_data))->_data != data)
    elem = elem->_next;
  // Return the element
  return elem;
}

// Update the ranks in 'that' with results 'res' given as a GSet of 
// pointers toward entities (_data in GSetElem equals _data in 
// ELOEntity) in winning order
// The _sortVal of the GSet represents the score (and so position)
// of the entities for this update (thus, equal _sortVal means tie)
// The set of results must contain at least 2 elements
// Elements in the result set must be in the ELORank 
void ELORankUpdate(ELORank* that, GSet* res) {
#if BUILDMODE == 0
  // Check arguments
  if (that == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'that' is null");
    PBErrCatch(ELORankErr);
  }
  if (res == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'res' is null");
    PBErrCatch(ELORankErr);
  }
  if (GSetNbElem(res) < 2) {
    ELORankErr->_type = PBErrTypeInvalidArg;
    sprintf(ELORankErr->_msg, 
      "Number of elements in result set invalid (%d>=2)",
      GSetNbElem(res));
    PBErrCatch(ELORankErr);
  }
#endif
  // Declare a variable to memorise the delta of elo of each entity
  VecFloat* deltaElo = VecFloatCreate(GSetNbElem(res));
  // Calculate the delta of elo for each pair of entity
  GSetElem* elemA = res->_head;
  int iElem = 0;
  while (elemA != NULL) {
    GSetElem* elemAElo = ELORankGetElem(that, elemA->_data);
#if BUILDMODE == 0
    if (elemAElo == NULL) {
      ELORankErr->_type = PBErrTypeNullPointer;
      sprintf(ELORankErr->_msg, 
        "Entity in the result set can't be found in the ELORank.");
      PBErrCatch(ELORankErr);
    }
#endif
    GSetElem* elemB = res->_head;
    while (elemB != NULL) {
      // Ignore tie and match with itself
      if (ISEQUALF(elemA->_sortVal, elemB->_sortVal) == false) {
        GSetElem* elemBElo = ELORankGetElem(that, elemB->_data);
#if BUILDMODE == 0
        if (elemBElo == NULL) {
          ELORankErr->_type = PBErrTypeNullPointer;
          sprintf(ELORankErr->_msg, 
            "Entity in the result set can't be found in the ELORank.");
          PBErrCatch(ELORankErr);
        }
#endif
        // If elemA has won
        if (elemA->_sortVal > elemB->_sortVal) {
          float winnerELO = elemAElo->_sortVal;
          float looserELO = elemBElo->_sortVal;
          float a = 
            1.0 / (1.0 + pow(10.0, (looserELO - winnerELO) / 400.0));
          VecSet(deltaElo, iElem, 
            VecGet(deltaElo, iElem) + that->_k * (1.0 - a));
        // Else, if elemA has lost
        } else {
          float winnerELO = elemBElo->_sortVal;
          float looserELO = elemAElo->_sortVal;
          float b = 
            1.0 / (1.0 + pow(10.0, (winnerELO - looserELO) / 400.0));
          VecSet(deltaElo, iElem, 
            VecGet(deltaElo, iElem) - that->_k * b);
        }
      }
      elemB = elemB->_next;
    }
    elemA = elemA->_next;
    ++iElem;
  }
  // Apply the delta of elo and update the number of run
  GSetElem* elem = res->_head;
  iElem = 0;
  while (elem != NULL) {
    GSetElem* elemElo = ELORankGetElem(that, elem->_data);
#if BUILDMODE == 0
    if (elemElo == NULL) {
      ELORankErr->_type = PBErrTypeNullPointer;
      sprintf(ELORankErr->_msg, 
        "Entity in the result set can't be found in the ELORank.");
      PBErrCatch(ELORankErr);
    }
#endif
    elemElo->_sortVal += VecGet(deltaElo, iElem);
    ++(((ELOEntity*)(elemElo->_data))->_nbRun);
    ++iElem;
    elem = elem->_next;
  }
  // Free memory
  VecFree(&deltaElo);
  // Sort the ELORank
  GSetSort(&(that->_set));
}

// Get the current rank of the entity 'data' (starts at 0)
int ELORankGetRank(ELORank* that, void* data) {
#if BUILDMODE == 0
  // Check arguments
  if (that == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'that' is null");
    PBErrCatch(ELORankErr);
  }
  if (data == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'data' is null");
    PBErrCatch(ELORankErr);
  }
#endif
  // Declare a variable to memorize the rank
  int rank = 0;
  // Search the element
  GSetElem* elem = that->_set._tail;
  while (elem != NULL && ((ELOEntity*)(elem->_data))->_data != data) {
    elem = elem->_prev;
    ++rank;
  }
#if BUILDMODE == 0
  if (elem == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, 
      "Entity requested can't be found in the ELORank.");
    PBErrCatch(ELORankErr);
  }
#endif  
  // Return the element
  return rank;
}

// Get the current ELO of the entity 'data'
float ELORankGetELO(ELORank* that, void* data) {
#if BUILDMODE == 0
  // Check arguments
  if (that == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'that' is null");
    PBErrCatch(ELORankErr);
  }
  if (data == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'data' is null");
    PBErrCatch(ELORankErr);
  }
#endif
  // Declare a variable to memorize the ELO
  float elo = ELORANK_STARTELO;
  // Search the element
  GSetElem* elem = that->_set._head;
  elo = elem->_sortVal;
  while (elem != NULL && ((ELOEntity*)(elem->_data))->_data != data) {
    elem = elem->_next;
    elo = elem->_sortVal;
  }
#if BUILDMODE == 0
  if (elem == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, 
      "Entity requested can't be found in the ELORank.");
    PBErrCatch(ELORankErr);
  }
#endif  
  // Return the element
  return elo;
}

// Get the 'rank'-th entity according to current ELO of 'that'  
// (starts at 0)
ELOEntity* ELORankGetRanked(ELORank* that, int rank) {
#if BUILDMODE == 0
  // Check arguments
  if (that == NULL) {
    ELORankErr->_type = PBErrTypeNullPointer;
    sprintf(ELORankErr->_msg, "'that' is null");
    PBErrCatch(ELORankErr);
  }
  if (rank < 0 || rank >= GSetNbElem(&(that->_set))) {
    ELORankErr->_type = PBErrTypeInvalidArg;
    sprintf(ELORankErr->_msg, "'rank' is invalid (0<=%d<%d)", rank, 
      GSetNbElem(&(that->_set)));
    PBErrCatch(ELORankErr);
  }
#endif
  GSetElem* elem = that->_set._tail;
  for (int i = rank; i--;)
    elem = elem->_prev;
  return (ELOEntity*)(elem->_data);
}
