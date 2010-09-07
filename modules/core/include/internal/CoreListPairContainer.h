/**
 *  \file ListPairContainer.h    \brief Store a list of ParticlePairsTemp
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2010 IMP Inventors. All rights reserved.
 */

#ifndef IMPCORE_INTERNAL_CORE_LIST_PAIR_CONTAINER_H
#define IMPCORE_INTERNAL_CORE_LIST_PAIR_CONTAINER_H

#include "../core_config.h"
#include <IMP/PairContainer.h>
#include <IMP/internal/container_helpers.h>
#include "pair_helpers.h"
#include <IMP/ScoreState.h>

IMPCORE_BEGIN_INTERNAL_NAMESPACE


class IMPCOREEXPORT CoreListPairContainer:
public internal::ListLikePairContainer
{
  IMP_ACTIVE_CONTAINER_DECL(CoreListPairContainer);
public:
  CoreListPairContainer(Model *m, std::string name);
  CoreListPairContainer(Model *m, const char *name);
  CoreListPairContainer();
  void add_particle_pair(const ParticlePair& vt);
  void add_particle_pairs(const ParticlePairsTemp &c);
  void remove_particle_pairs(const ParticlePairsTemp &c);
  void set_particle_pairs(ParticlePairsTemp c);
  void clear_particle_pairs();
#ifndef IMP_DOXYGEN
  bool get_is_up_to_date() const {
    return true;
  }
  bool get_contained_particles_changed() const;
  ParticlesTemp get_contained_particles() const;
#endif
  IMP_LISTLIKE_PAIR_CONTAINER(CoreListPairContainer);
};


IMPCORE_END_INTERNAL_NAMESPACE

#endif  /* IMPCORE_INTERNAL_CORE_LIST_PAIR_CONTAINER_H */
