/**
 *  \file BondSingletonScore.cpp
 *  \brief A score based a bond decorator.
 *
 *  Copyright 2007-9 Sali Lab. All rights reserved.
 */

#include "IMP/atom/BondSingletonScore.h"
#include "IMP/core/internal/evaluate_distance_pair_score.h"
#include "IMP/atom/bond_decorators.h"
#include "IMP/core/XYZ.h"
#include <boost/lambda/lambda.hpp>

IMPATOM_BEGIN_NAMESPACE

BondSingletonScore::BondSingletonScore(UnaryFunction *f): f_(f){}

Float BondSingletonScore::evaluate(Particle *b,
                                   DerivativeAccumulator *da) const
{
  IMP_IF_CHECK(USAGE_AND_INTERNAL) {
    Bond::decorate_particle(b);
  }
  Bond bd(b);
  Float l= bd.get_length();
  Float s= bd.get_stiffness();
  if (l < 0) {
    IMP_WARN("Bond does not have a length: " << bd << std::endl);
    return 0;
  }
  if (s <0) s=1;
  Particle *pa=NULL, *pb=NULL;
  try {
    pa = bd.get_bonded(0).get_particle();
    pb = bd.get_bonded(1).get_particle();
  } catch (const IndexException &e) {
    IMP_WARN("Problem processing bond: " << bd << std::endl);
    IMP_WARN(e.what() << std::endl);
    return 0;
  }
  return
    IMP::core::internal::
    evaluate_distance_pair_score(IMP::core::XYZ(pa),
                                 IMP::core::XYZ(pb),
                                 da,
                                 f_.get(),
                                 s*(boost::lambda::_1-l));
}

ParticlesList BondSingletonScore::get_interacting_particles(Particle *p) const {
  ParticlesList ret(1,ParticlesTemp(2));
  Bond bd(p);
  ret[0][0]= bd.get_bonded(0);
  ret[0][1]= bd.get_bonded(1);
  return ret;
}

ParticlesTemp BondSingletonScore::get_used_particles(Particle *p) const {
  ParticlesTemp ret(3);
  Bond bd(p);
  ret[0]= bd.get_bonded(0);
  ret[1]= bd.get_bonded(1);
  ret[2]= p;
  return ret;
}

void BondSingletonScore::show(std::ostream &out) const
{
  out << "BondSingletonScore using ";
  f_->show(out);
  out << std::endl;
}

IMPATOM_END_NAMESPACE
