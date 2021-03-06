/**
 *  \file complex_assembly.cpp
 *  \brief A Score on the distance between a pair of particles.
 *
 *  Copyright 2007-2013 IMP Inventors. All rights reserved.
 */

#include <IMP/example/complex_assembly.h>
#include <IMP/example/optimizing.h>
#include <IMP/container/generic.h>
#include <IMP/algebra/vector_generators.h>
#include <IMP/core/ConjugateGradients.h>
#include <IMP/core/MonteCarlo.h>
#include <IMP/core/SerialMover.h>
#include <IMP/core/BallMover.h>
#include <IMP/core/HarmonicUpperBound.h>
#include <IMP/core/BoundingBox3DSingletonScore.h>
#include <IMP/container/ClosePairContainer.h>
#include <IMP/container/ListSingletonContainer.h>
#include <IMP/container/generic.h>
#include <IMP/core/rigid_bodies.h>
#include <IMP/base/log_macros.h>
#include <IMP/domino/subset_graphs.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include <IMP/scoped.h>

#include <IMP/container/generic.h>
#include <IMP/core/ConjugateGradients.h>
#include <IMP/core/MonteCarlo.h>
#include <IMP/core/SerialMover.h>
#include <IMP/core/BallMover.h>
#include <IMP/container/generic.h>
#include <IMP/container/ClosePairContainer.h>
#include <IMP/core/rigid_bodies.h>
#include <IMP/core/SphereDistancePairScore.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include <IMP/scoped.h>

IMPEXAMPLE_BEGIN_NAMESPACE

class AssemblyData {
  kernel::ParticlesTemp ps_;
  base::map<kernel::Particle *, int> index_;
  kernel::Restraints rs_;
  kernel::ParticlesTemps particles_;
  domino::InteractionGraph interactions_;

  int get_degree(unsigned int i, const kernel::ParticlesTemp &ps) const {
    int ret = 0;
    domino::InteractionGraphConstVertexName vm =
        boost::get(boost::vertex_name, interactions_);
    std::pair<domino::InteractionGraphTraits::adjacency_iterator,
              domino::InteractionGraphTraits::adjacency_iterator> be =
        boost::adjacent_vertices(i, interactions_);
    for (; be.first != be.second; ++be.first) {
      if (std::binary_search(ps.begin(), ps.end(), vm[*be.first])) {
        ++ret;
      }
    }
    return ret;
  }

 public:
  AssemblyData(kernel::ParticlesTemp ps, const kernel::RestraintsTemp &rs)
      : ps_(ps), rs_(rs.begin(), rs.end()) {
    std::sort(ps_.begin(), ps_.end());
    interactions_ = domino::get_interaction_graph(rs, ps);
    for (unsigned int i = 0; i < rs.size(); ++i) {
      kernel::ParticlesTemp cur = IMP::get_input_particles(rs[i]->get_inputs());
      std::sort(cur.begin(), cur.end());
      cur.erase(std::unique(cur.begin(), cur.end()), cur.end());
      kernel::ParticlesTemp used;
      std::set_intersection(cur.begin(), cur.end(), ps_.begin(), ps_.end(),
                            std::back_inserter(used));
      particles_.push_back(used);
    }
  }
  kernel::RestraintsTemp get_restraints(kernel::ParticlesTemp ps) const {
    std::sort(ps.begin(), ps.end());
    kernel::RestraintsTemp ret;
    for (unsigned int i = 0; i < rs_.size(); ++i) {
      kernel::ParticlesTemp used;
      std::set_intersection(particles_[i].begin(), particles_[i].end(),
                            ps.begin(), ps.end(), std::back_inserter(used));
      if (used.size() == particles_[i].size()) {
        ret.push_back(rs_[i]);
      }
    }
    return ret;
  }
  kernel::Particle *get_highest_degree_unused_particle(kernel::ParticlesTemp ps)
      const {
    std::sort(ps.begin(), ps.end());
    int md = 0;
    int mi = -1;
    domino::InteractionGraphConstVertexName vm =
        boost::get(boost::vertex_name, interactions_);
    for (unsigned int i = 0; i < boost::num_vertices(interactions_); ++i) {
      if (std::binary_search(ps.begin(), ps.end(), vm[i])) continue;
      int degree = get_degree(i, ps);
      if (degree >= md) {
        md = degree;
        mi = i;
      }
    }
    return vm[mi];
  }
};

/** Provide an example of a more involved protocol for assembly a complex. The
    protocol adds the particles one at a time based on how well connected they
    are to the already added particles (in the interaction graph). After each
    addition, the assembly is optimized. The protocol seems to work at
    assembling the residues of a protein from a truncated distance matrix.
*/
void optimize_assembly(kernel::Model *m,
                       const kernel::ParticlesTemp &components,
                       const kernel::RestraintsTemp &interactions,
                       const kernel::RestraintsTemp &other_restraints,
                       const algebra::BoundingBox3D &bb, PairScore *ev,
                       double cutoff, const PairPredicates &excluded) {
  IMP_NEW(core::ConjugateGradients, cg, (m));
  cg->set_log_level(IMP::base::SILENT);
  IMP_NEW(core::MonteCarlo, mc, (m));
  mc->set_log_level(IMP::base::SILENT);
  IMP_NEW(core::IncrementalScoringFunction, isf,
          (components, m->get_restraints()));
  mc->set_incremental_scoring_function(isf);
  AssemblyData ad(components, interactions);
  kernel::ParticlesTemp cur;
  IMP_NEW(container::ListSingletonContainer, active, (m));
  // fix distance
  IMP_NEW(container::ClosePairContainer, cpc, (active, 0, 4));
  cpc->set_pair_filters(excluded);
  IMP_NEW(core::SoftSpherePairScore, ssps, (10));
  base::Pointer<kernel::Restraint> evr =
      container::create_restraint(ssps.get(), cpc.get());
  IMP_NEW(core::HarmonicUpperBound, hub, (0, 10));
  IMP_NEW(core::BoundingBox3DSingletonScore, bbss, (hub, bb));
  base::Pointer<kernel::Restraint> bbr =
      container::create_restraint(bbss.get(), active.get());
  do {
    kernel::Particle *add = ad.get_highest_degree_unused_particle(cur);
    cur.push_back(add);
    core::XYZ(add).set_coordinates(algebra::get_random_vector_in(bb));
    mc->clear_movers();
    mc->add_mover(create_serial_mover(cur));
    isf->clear_close_pair_scores();
    isf->add_close_pair_score(ev, 0, cur, excluded);
    kernel::RestraintsTemp rs = other_restraints + ad.get_restraints(cur);
    IMP_LOG_TERSE("Current restraints are " << rs << " and particles " << cur
                                            << std::endl);
    mc->set_scoring_function(rs);
    cg->set_scoring_function(rs + kernel::RestraintsTemp(1, evr.get()) +
                             kernel::RestraintsTemp(1, bbr.get()));
    active->set_particles(cur);
    double e;
    for (int j = 0; j < 5; ++j) {
      mc->set_kt(100.0 / (3 * j + 1));
      mc->optimize(cur.size() * (j + 1) * 1000);
      e = cg->optimize(10 * cur.size());
      IMP_LOG_PROGRESS("Energy is " << e << std::endl);
    }
    if (e > cutoff) {
      IMP_THROW("Failure to converge", ModelException);
    }
  } while (cur.size() != components.size());
}

IMPEXAMPLE_END_NAMESPACE
