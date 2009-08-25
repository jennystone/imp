/**
 *  \file RestraintSet.h     \brief Used to hold a set of related restraints.
 *
 *  Copyright 2007-9 Sali Lab. All rights reserved.
 *
 */

#ifndef IMPCORE_RESTRAINT_SET_H
#define IMPCORE_RESTRAINT_SET_H

#include "config.h"

#include <IMP/Restraint.h>

#include <string>

IMPCORE_BEGIN_NAMESPACE

//! Container used to hold a set of restraints
/**
 */
class IMPCOREEXPORT RestraintSet : public Restraint
{
public:
  //! Create an empty set
  RestraintSet(const std::string& name="RestraintSet %1%");

  IMP_RESTRAINT(RestraintSet, get_module_version_info())
 /** @name Methods to control the nested Restraint objects

     This container manages a set of Restraint objects. To
     manipulate the stored set use the methods below.
  */
  /**@{*/
    IMP_LIST(public, Restraint, restraint, Restraint*, Restraints);
  /**@}*/
 public:

  //! Set weight for all restraints contained by this set.
  /** Setting the weight to 0 disables the restraints in the set.

      \param[in] weight The new value of the weight.
    */
  void set_weight(Float weight) { weight_ = weight; }

  //! Get weight for all restraints contained by this set.
  Float get_weight() const { return weight_; }

  virtual ParticlesList get_interacting_particles() const;


  void set_model(Model *m);
private:

  //! Weight for all restraints.
  Float weight_;
};

IMPCORE_END_NAMESPACE

#endif  /* IMPCORE_RESTRAINT_SET_H */
