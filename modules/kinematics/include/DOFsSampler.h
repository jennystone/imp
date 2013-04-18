/**
 * \file DOFsSampler \brief
 *
 * Copyright 2007-2010 Sali Lab. All rights reserved.
 *
 */

#ifndef IMPKINEMATICS_DO_FS_SAMPLER_H
#define IMPKINEMATICS_DO_FS_SAMPLER_H

#include "kinematics_config.h"
#include "DOFValues.h"

IMPKINEMATICS_BEGIN_NAMESPACE

class IMPKINEMATICSEXPORT DOFsSampler {
public:
  DOFsSampler() {}

  virtual DOFValues get_sample() const = 0;

  virtual void apply(const DOFValues& values) = 0;

  void apply_last_sample() { apply(last_sample_); }

  void sample_and_apply() { apply(get_sample()); }

 protected:
  DOFValues last_sample_;
};

IMPKINEMATICS_END_NAMESPACE

#endif /* IMPKINEMATICS_DO_FS_SAMPLER_H */
