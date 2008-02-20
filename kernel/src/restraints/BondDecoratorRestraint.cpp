/**
 *  \file BondDecoratorRestraint.cpp 
 *  \brief A bond restraint.
 *
 *  Copyright 2007-8 Sali Lab. All rights reserved.
 *
 */

#include "IMP/restraints/BondDecoratorRestraint.h"
#include "IMP/UnaryFunction.h"
#include "IMP/score_states/BondDecoratorListScoreState.h"
#include "IMP/decorators/bond_decorators.h"
#include "IMP/pair_scores/DistancePairScore.h"

namespace IMP
{

BondDecoratorRestraint::BondDecoratorRestraint(BondDecoratorListScoreState *s,
                                               UnaryFunction *f): bl_(s),
                                                                  f_(f){}

Float BondDecoratorRestraint::evaluate(DerivativeAccumulator *accum)
{
  Float sum=0;
  for (BondDecoratorListScoreState::BondIterator bi= bl_->bonds_begin();
       bi != bl_->bonds_end(); ++bi) {
    BondDecorator bd= *bi;
    Float l= bd.get_length();
    Float s= bd.get_stiffness();
    if (l < 0) {
      IMP_WARN("Bond does not have a length: " << bd << std::endl);
      continue;
    }
    if (s <0) s=1;
    try {
      /*IMP_LOG(VERBOSE, "Bonded pair " 
              << bd.get_bonded(0).get_particle()->get_index()
              << " " << bd.get_bonded(1).get_particle()->get_index()
              << " with length " << l << " and stiffness " << s << std::endl);*/
      sum+= 
        internal::evaluate_distance_pair_score(bd.get_bonded(0).get_particle(),
                                               bd.get_bonded(1).get_particle(),
                                               accum,
                                               f_.get(),
                                               l, s);
    } catch (...) {
      IMP_WARN("Problem processing bond: " << bd << std::endl);
    }
  }
  return sum;
}

void BondDecoratorRestraint::show(std::ostream& out) const
{
  out << "Bond decorator restraint with unary function ";
  f_->show(out);
  out << " on " << bl_->number_of_bonds() << " bonds";
  out << std::endl;
}

} // namespace IMP
