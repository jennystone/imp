/**
 *  \file CoreQuadsRestraint.h
 *  \brief Apply a QuadScore to each Quad in a list.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2010 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPCORE_INTERNAL_CORE_QUADS_RESTRAINT_H
#define IMPCORE_INTERNAL_CORE_QUADS_RESTRAINT_H

#include "../core_config.h"

#include <IMP/DecomposableRestraint.h>
#include <IMP/Pointer.h>
#include <IMP/QuadScore.h>
#include <IMP/QuadContainer.h>

#include <iostream>

IMPCORE_BEGIN_INTERNAL_NAMESPACE

//! Applies a QuadScore to each Quad in a list.
/** This restraint stores the used particles in a ParticleQuadsTemp.
    The container used can be set so that the list can be shared
    with other containers (or a nonbonded list can be used). By default
    a ListQuadContainer is used and the
    {add_, set_, clear_}particle_quad{s} methods can be used.

    \see QuadRestraint
 */
class IMPCOREEXPORT CoreQuadsRestraint :
  public DecomposableRestraint
{
  IMP::internal::OwnerPointer<QuadScore> ss_;
  IMP::internal::OwnerPointer<QuadContainer> pc_;
  mutable double score_;
public:

 //! Create the restraint with a shared container
  /** \param[in] ss The function to apply to each particle.
      \param[in] pc The container containing the stored particles. This
      container is not copied.
      \param[in] name The object name
   */
  CoreQuadsRestraint(QuadScore *ss,
                      QuadContainer *pc,
                      std::string name="QuadsRestraint %1%");

  IMP_INCREMENTAL_RESTRAINT(CoreQuadsRestraint);

  //! Get the container used to store Particles
  QuadContainer* get_quad_container() const {
    return pc_;
  }

  QuadScore* get_quad_score() const {
    return ss_;
  }

  Restraints get_decomposition() const;
};

IMPCORE_END_INTERNAL_NAMESPACE

#endif  /* IMPCORE_INTERNAL_CORE_QUADS_RESTRAINT_H */
