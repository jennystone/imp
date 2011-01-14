/**
 *  \file PairFilter.cpp   \brief Filter for pair.
 *
 *  This file is generated by a script (core/tools/make-filter).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 *
 */

#include "IMP/PairFilter.h"
#include "IMP/internal/utility.h"
#include "IMP/PairModifier.h"
#include <algorithm>

IMP_BEGIN_NAMESPACE

PairFilter::PairFilter(std::string name): Object(name) {
}
namespace {
struct PairGCP {
  const PairFilter *back_;
  PairGCP(const PairFilter *n): back_(n){}
  template <class T>
  bool operator()(const T &p) const {
    return back_->get_contains_particle_pair(p);
  }
};
}
void PairFilter
::filter_in_place(ParticlePairsTemp &ps) const {
  ps.erase(std::remove_if(ps.begin(), ps.end(),
                          PairGCP(this)),
           ps.end());

}

IMP_END_NAMESPACE
