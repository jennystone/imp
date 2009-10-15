/**
 *  \file PairsScoreState.cpp
 *  \brief Use a PairModifier applied to a PairContainer to
 *  maintain an invariant
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-9 Sali Lab. All rights reserved.
 *
 */

#include "IMP/core/PairsScoreState.h"
#include "IMP/internal/container_helpers.h"
#include <utility>

IMPCORE_BEGIN_NAMESPACE

PairsScoreState::PairsScoreState(PairContainer *c,
                                           PairModifier *before,
                                           PairModifier *after,
                                           std::string name):
  ScoreState(name), c_(c) {
  if (before) f_=before;
  if (after) af_=after;
}


void PairsScoreState::do_before_evaluate()
{
  IMP_OBJECT_LOG;
  if (!f_) return;
  IMP_LOG(TERSE, "Begin PairsScoreState::update" << std::endl);
  IMP_CHECK_OBJECT(f_);
  IMP_CHECK_OBJECT(c_);
  c_->apply(f_);
  IMP_LOG(TERSE, "End PairsScoreState::update" << std::endl);
}

void PairsScoreState::do_after_evaluate(DerivativeAccumulator *da)
{
  IMP_OBJECT_LOG;
  if (!af_) return;
  IMP_LOG(TERSE, "Begin PairsScoreState::after_evaluate" << std::endl);
  IMP_CHECK_OBJECT(af_);
  IMP_CHECK_OBJECT(c_);
  if (da) c_->apply(af_, *da);
  IMP_LOG(TERSE, "End PairsScoreState::after_evaluate" << std::endl);
}

ParticlesList PairsScoreState::get_interacting_particles() const {
  ParticlesList ret0, ret1;
  if (f_) ret0 = IMP::internal::get_interacting_particles(c_, f_.get());
  if (af_) ret1= IMP::internal::get_interacting_particles(c_, af_.get());
  ret0.insert(ret0.end(), ret1.begin(), ret1.end());
  return ret0;
}


ParticlesTemp PairsScoreState::get_read_particles() const {
  ParticlesTemp ret0, ret1;
  if (f_) ret0= IMP::internal::get_read_particles(c_, f_.get());
  if (af_) ret1= IMP::internal::get_read_particles(c_, af_.get());
  ret0.insert(ret0.end(), ret1.begin(), ret1.end());
  return ret0;
}

ParticlesTemp PairsScoreState::get_write_particles() const {
  ParticlesTemp ret0, ret1;
  if (f_) ret0= IMP::internal::get_write_particles(c_, f_.get());
  if (af_) ret1= IMP::internal::get_write_particles(c_, af_.get());
  ret0.insert(ret0.end(), ret1.begin(), ret1.end());
  return ret0;
}

void PairsScoreState::show(std::ostream &out) const {
  out << "PairsScoreState base" << std::endl;
}

IMPCORE_END_NAMESPACE
