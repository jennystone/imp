/**
 *  \file Constraint.cpp \brief Shared score state.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 *
 */

#include "IMP/kernel/Constraint.h"
#include "IMP/kernel/Model.h"
#include "IMP/kernel/internal/utility.h"

IMPKERNEL_BEGIN_NAMESPACE

Constraint::Constraint(kernel::Model *m, std::string name)
    : ScoreState(m, name) {}

IMPKERNEL_END_NAMESPACE
