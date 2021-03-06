/**
 *  \file CloseBipartitePairContainer.cpp   \brief A list of
 *kernel::ParticlePairs.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2013 IMP Inventors. Close rights reserved.
 *
 */

#include "IMP/container/CloseBipartitePairContainer.h"

IMPCONTAINER_BEGIN_NAMESPACE

CloseBipartitePairContainer::CloseBipartitePairContainer(
    SingletonContainerAdaptor a, SingletonContainerAdaptor b, double distance,
    double slack, std::string name)
    : P(a, b, distance, slack, name) {}

CloseBipartitePairContainer::CloseBipartitePairContainer(
    SingletonContainerAdaptor a, SingletonContainerAdaptor b, double distance,
    core::ClosePairsFinder *, double slack, std::string name)
    : P(a, b, distance, slack, name) {}
IMPCONTAINER_END_NAMESPACE
