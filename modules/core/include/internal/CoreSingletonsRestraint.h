/**
 *  \file CoreSingletonsRestraint.h
 *  \brief Apply a SingletonScore to each Particle in a list.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2010 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPCORE_INTERNAL_CORE_SINGLETONS_RESTRAINT_H
#define IMPCORE_INTERNAL_CORE_SINGLETONS_RESTRAINT_H

#include "../core_config.h"

#include <IMP/DecomposableRestraint.h>
#include <IMP/Pointer.h>
#include <IMP/SingletonScore.h>
#include <IMP/SingletonContainer.h>

#include <iostream>

IMPCORE_BEGIN_INTERNAL_NAMESPACE

//! Applies a SingletonScore to each Particle in a list.
/** This restraint stores the used particles in a Particles.
    The container used can be set so that the list can be shared
    with other containers (or a nonbonded list can be used). By default
    a ListSingletonContainer is used and the
    {add_, set_, clear_}particle{s} methods can be used.

    \see SingletonRestraint
 */
class IMPCOREEXPORT CoreSingletonsRestraint :
  public DecomposableRestraint
{
  IMP::internal::OwnerPointer<SingletonScore> ss_;
  IMP::internal::OwnerPointer<SingletonContainer> pc_;
  mutable double score_;
public:

 //! Create the restraint with a shared container
  /** \param[in] ss The function to apply to each particle.
      \param[in] pc The container containing the stored particles. This
      container is not copied.
      \param[in] name The object name
   */
  CoreSingletonsRestraint(SingletonScore *ss,
                      SingletonContainer *pc,
                      std::string name="SingletonsRestraint %1%");

  IMP_INCREMENTAL_RESTRAINT(CoreSingletonsRestraint);

  //! Get the container used to store Particles
  SingletonContainer* get_singleton_container() {
    return pc_;
  }

  double unprotected_evaluate_subset(DerivativeAccumulator *accum) const;

  SingletonScore* get_singleton_score() const {
    return ss_;
  }

  Restraints get_decomposition() const;
};

IMPCORE_END_INTERNAL_NAMESPACE

#endif  /* IMPCORE_INTERNAL_CORE_SINGLETONS_RESTRAINT_H */
