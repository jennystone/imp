/**
 *  \file SingletonContainerSet.cpp
 *  \brief A set of SingletonContainers.
 *
 *  WARNING This file was generated from NAMEContainerSet.cc
 *  in tools/maintenance/container_templates/container
 *  by tools/maintenance/make-container.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include "IMP/container/SingletonContainerSet.h"
#include <IMP/internal/container_helpers.h>
#include <algorithm>


IMPCONTAINER_BEGIN_NAMESPACE

SingletonContainerSet
::SingletonContainerSet(Model *m, std::string name):
  SingletonContainer(m, name) {
}


SingletonContainerSet
::SingletonContainerSet(const SingletonContainersTemp& in,
                        std::string name):
    SingletonContainer(IMP::internal::get_model(in), name){
  set_singleton_containers(in);
}

void SingletonContainerSet::do_show(std::ostream &out) const {
  IMP_CHECK_OBJECT(this);
  out << get_number_of_particles()
      << " containers" << std::endl;
}


ParticleIndexes SingletonContainerSet::get_indexes() const {
  ParticleIndexes sum;
  for (SingletonContainerConstIterator it= singleton_containers_begin();
       it != singleton_containers_end(); ++it) {
    ParticleIndexes cur=(*it)->get_indexes();
    sum.insert(sum.end(), cur.begin(), cur.end());
  }
  return sum;
}

ParticleIndexes SingletonContainerSet::get_all_possible_indexes() const {
  ParticleIndexes sum;
  for (SingletonContainerConstIterator it= singleton_containers_begin();
       it != singleton_containers_end(); ++it) {
    ParticleIndexes cur=(*it)->get_all_possible_indexes();
    sum.insert(sum.end(), cur.begin(), cur.end());
  }
  return sum;
}

IMP_LIST_IMPL(SingletonContainerSet,
              SingletonContainer,
              singleton_container,
              SingletonContainer*,
              SingletonContainers);


void SingletonContainerSet::apply(const SingletonModifier *sm) const {
  for (unsigned int i=0; i< get_number_of_singleton_containers(); ++i) {
    get_singleton_container(i)->apply(sm);
  }
}

void SingletonContainerSet::apply(const SingletonDerivativeModifier *sm,
                               DerivativeAccumulator &da) const {
  for (unsigned int i=0; i< get_number_of_singleton_containers(); ++i) {
    get_singleton_container(i)->apply(sm, da);
  }
}

double SingletonContainerSet::evaluate(const SingletonScore *s,
                                       DerivativeAccumulator *da) const {
  return template_evaluate(s, da);
}

double SingletonContainerSet::evaluate_if_good(const SingletonScore *s,
                                               DerivativeAccumulator *da,
                                               double max) const {
  return template_evaluate_if_good(s, da, max);
}


ParticlesTemp SingletonContainerSet::get_all_possible_particles() const {
  ParticlesTemp ret;
  for (unsigned int i=0; i< get_number_of_singleton_containers(); ++i) {
    ParticlesTemp cur= get_singleton_container(i)
        ->get_all_possible_particles();
    ret+=cur;
  }
  return ret;
}

bool SingletonContainerSet::get_is_changed() const {
  for (unsigned int i=0; i< get_number_of_singleton_containers(); ++i) {
    if (get_singleton_container(i)->get_is_changed()) return true;
  }
  return Container::get_is_changed();
}


ContainersTemp SingletonContainerSet::get_input_containers() const {
  return ContainersTemp(singleton_containers_begin(),
                        singleton_containers_end());
}
void SingletonContainerSet::do_before_evaluate() {
}

IMPCONTAINER_END_NAMESPACE
