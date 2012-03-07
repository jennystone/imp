/**
 *  \file ScoringFunction.h   \brief Storage of a model, its restraints,
 *                         constraints and particles.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPKERNEL_SCORING_FUNCTION_H
#define IMPKERNEL_SCORING_FUNCTION_H

#include "kernel_config.h"
#include "Restraint.h"
#include "RestraintSet.h"
#include "ScoreState.h"
#include <IMP/base/tracking.h>

#include <limits>


IMP_BEGIN_NAMESPACE
class Model;


/** A ScoringFunction represents a scoring function on the model.
    The Model has a default scoring function
    (Model::get_model_scoring_function()), but it can be useful to use
    others in different contexts during a samping process.

    \note This may eventually become an abstract base class to support
    certain optimizations.
*/
class IMPEXPORT ScoringFunction:
#if defined(IMP_DOXYGEN) || defined(SWIG)
    public base::Object
#else
    public base::TrackedObject<ScoringFunction, Model>
#endif
 {
   typedef  base::TrackedObject<ScoringFunction, Model> Tracked;

  friend class Model;
  // kept alive in model
  ScoreStatesTemp ss_;
  double last_score_;
  bool last_was_good_;
  void ensure_dependencies();
public:
  typedef std::pair<double, bool> ScoreIsGoodPair;
protected:
  virtual ScoreIsGoodPair do_evaluate_if_good(bool derivatives,
                                              const ScoreStatesTemp &ss)=0;
  virtual ScoreIsGoodPair do_evaluate(bool derivatives,
                                      const ScoreStatesTemp &ss)=0;
  virtual ScoreIsGoodPair do_evaluate_if_below(bool derivatives,
                                               double max,
                                               const ScoreStatesTemp &ss)=0;
 public:
  ScoringFunction(Model *m, std::string name);
  IMP_OBJECT_INLINE(ScoringFunction, out << get_restraints(),);
  Model *get_model() const {
    return Tracked::get_tracker();
  }
  double evaluate_if_good(bool derivatives);
  double evaluate(bool derivatives);
  double evaluate_if_below(bool derivatives, double max);
  /** Return true if the last evaluate satisfied all the restraint
      thresholds.*/
  bool get_had_good_score() const {
    return last_was_good_;
  }
  double get_last_score() const {
    return last_score_;
  }
  virtual RestraintsTemp get_restraints() const=0;
  const ScoreStatesTemp& get_score_states();
};
IMP_OBJECTS(ScoringFunction, ScoringFunctions);



/** Create a scoring function on a list of restraints.
*/
class IMPEXPORT RestraintsScoringFunction: public ScoringFunction {
  friend class Model;
  Restraints rs_;
  RestraintSets rss_;
  double weight_;
  double max_;
 public:
  RestraintsScoringFunction(const RestraintsTemp &rs,
                            double weight=1.0,
                            double max=NO_MAX);
  IMP_SCORING_FUNCTION(RestraintsScoringFunction);
};

/** Return a list of ScoringFunction objects where each is as simple
    as possible and evaluating the sum (and anding the good score bits)
    is exactly like evaluating the one ScoringFunction.*/
IMPEXPORT ScoringFunctions create_decomposition(ScoringFunction *sf);


/** Return a list of ScoringFunction objects where each is as simple
    as possible and evaluating the sum (and anding the good score bits)
    is exactly like evaluating the one ScoringFunction.

    These scoring functions are optimized for incremental evaluation.
*/
IMPEXPORT ScoringFunctions
create_incremental_decomposition(ScoringFunction *sf);

/** This class is to provide a backward compatible interface for things
    that take ScoringFunctions as arguments, but used to take
    RestraintsTemp or a RestraintSet. */
class IMPEXPORT ScoringFunctionInput
#ifndef SWIG
: public OwnerPointer<ScoringFunction>
#endif
{
  typedef OwnerPointer<ScoringFunction> P;
 public:
  ScoringFunctionInput(ScoringFunction *sf): P(sf){}
  ScoringFunctionInput(Model *sf);
  ScoringFunctionInput(const RestraintsTemp &sf);
  ScoringFunctionInput(RestraintSet *sf);
};

//! Print the hierarchy of restraints
IMPEXPORT void show_restraint_hierarchy(ScoringFunctionInput rs,
                                     std::ostream &out=std::cout);


IMP_END_NAMESPACE

#endif  /* IMPKERNEL_SCORING_FUNCTION_H */
