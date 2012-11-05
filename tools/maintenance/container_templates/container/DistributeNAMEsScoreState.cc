/**
 *  \file PredicateCLASSNAMEsRestraint.cpp   \brief Container for LCCLASSNAME.
 *
 *  BLURB
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include "IMP/container/DistributeCLASSNAMEsScoreState.h"

IMPCONTAINER_BEGIN_NAMESPACE
DistributeCLASSNAMEsScoreState::
DistributeCLASSNAMEsScoreState(CLASSNAMEContainerAdaptor input,
                               std::string name): ScoreState(name) {
  input_=input;
  updated_=false;
}

ParticlesTemp DistributeCLASSNAMEsScoreState
::get_output_particles() const {
  return ParticlesTemp();
}
ContainersTemp DistributeCLASSNAMEsScoreState
::get_output_containers() const {
  ContainersTemp ret(data_.size());
  for (unsigned int i=0; i< data_.size(); ++i) {
    ret[i]=data_[i].get<0>();
  }
  return ret;
}

ParticlesTemp DistributeCLASSNAMEsScoreState
::get_input_particles() const {
  // not correct, but correct is complicated
  return input_->get_all_possible_particles();
}
ContainersTemp DistributeCLASSNAMEsScoreState
::get_input_containers() const {
  // List containers don't do anything interesting
  return ContainersTemp(1, input_);
}


void DistributeCLASSNAMEsScoreState::do_before_evaluate() {
  update_lists_if_necessary();
}
void DistributeCLASSNAMEsScoreState
::do_after_evaluate(DerivativeAccumulator *) {
}

void DistributeCLASSNAMEsScoreState
::update_lists_if_necessary() const {
  if (updated_ && !input_->get_is_changed()) return;
  updated_=true;

  base::Vector<PLURALINDEXTYPE> output(data_.size());
  IMP_FOREACH_HEADERNAME_INDEX(input_, {
      for (unsigned int i=0; i< data_.size(); ++i) {
        if (data_[i].get<1>()->get_value_index(get_model(), _1)
            == data_[i].get<2>()) {
          output[i].push_back(_1);
        }
      }
    });
  for (unsigned int i=0; i< output.size(); ++i) {
    data_[i].get<0>()->set(output[i]);
  }
}

void DistributeCLASSNAMEsScoreState::do_show(std::ostream &) const {
}
IMPCONTAINER_END_NAMESPACE
