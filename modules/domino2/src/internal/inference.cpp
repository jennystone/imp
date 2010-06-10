/**
 *  \file RestraintGraph.cpp
 *  \brief creates a MRF from a set of particles and restraints
 *
 *  Copyright 2007-2010 IMP Inventors. All rights reserved.
 *
 */

#include <IMP/domino2/internal/inference.h>

#include <algorithm>
#include <boost/graph/copy.hpp>
#include <boost/pending/indirect_cmp.hpp>


IMPDOMINO2_BEGIN_INTERNAL_NAMESPACE

ParticleIndex get_index(Subset *s) {
  ParticleIndex ret;
  for (unsigned int i=0; i< s->get_number_of_particles(); ++i) {
    ret[s->get_particle(i)]=i;
  }
  return ret;
}


Subset* get_intersection(Subset *a, Subset *b) {
  ParticlesTemp pa= a->get_particles();
  ParticlesTemp pb= b->get_particles();
  std::sort(pa.begin(), pa.end());
  std::sort(pb.begin(), pb.end());
  ParticlesTemp rs;
  std::set_intersection(pa.begin(), pa.end(),
                        pb.begin(), pb.end(),
                        std::back_inserter(rs));
   Subset *ret= new Subset(rs);
   std::ostringstream oss;
   for (unsigned int j=0; j< rs.size(); ++j) {
     oss << rs[j]->get_name();
     if (j != rs.size()-1) {
       oss << ", ";
     }
    }
    ret->set_name(oss.str());
    return ret;
}

NodeData get_node_data(Subset *s,
                       const SubsetEvaluatorTable *eval,
                       const SubsetStatesTable *states,
                       double max_score) {
  NodeData ret;
  IMP::internal::OwnerPointer<SubsetStates> ss
    = states->get_subset_states(s);
  IMP::internal::OwnerPointer<SubsetEvaluator> se
    = eval->get_subset_evaluator(s);
  for (unsigned int i=0; i< ss->get_number_of_states(); ++i) {
    SubsetState state(ss->get_state(i));
    double score= se->get_score(state);
    if (score < max_score) {
      ret.scores[state]= score;
    }
  }
  return ret;
}

// indexes[i] is the index in is of the ith component
SubsetState get_subset_state(const IncompleteStates &is,
                             Subset *s,
                             const ParticleIndex &index) {
  SubsetState ret(s->get_number_of_particles());
  for (unsigned int i=0; i< ret.size(); ++i) {
    Particle *p= s->get_particle(i);
    ret[i]= is[index.find(p)->second];
  }
  for (unsigned int i=0; i< ret.size(); ++i) {
    IMP_USAGE_CHECK(ret[i] >=0, "not initialized " << ret);
  }
  return ret;
}


PropagatedData get_merged(const PropagatedData &da,
                          const PropagatedData &db,
                          const ParticleIndex &all_index,
                          const EdgeData &ed,
                          double max_score) {
  PropagatedData ret;
  for (PropagatedData::Scores::const_iterator ita = da.scores.begin();
       ita != da.scores.end(); ++ita) {
    SubsetState edge_state_a= get_subset_state(ita->first, ed.subset,
                                               all_index);
    double edge_score=ed.scores.find(edge_state_a)->second;
    for (PropagatedData::Scores::const_iterator itb = db.scores.begin();
         itb != db.scores.end(); ++itb) {
      /*IMP_LOG(VERBOSE, "Lists are " << ita->first << "(" << ita->second << ")"
        << " " << itb->first << " (" << itb->second << ")");*/
      SubsetState edge_state_b= get_subset_state(itb->first, ed.subset,
                                                 all_index);
      if (edge_state_b != edge_state_a) {
        //IMP_LOG(VERBOSE, " Rejected" << std::endl);
      } else {
        /*IMP_LOG(VERBOSE, " edge " << edge_state_a
          << " ("<< edge_score << ")");*/
        double nscore= ita->second+itb->second-edge_score;
        if (nscore < max_score) {
          IncompleteStates merged= get_merged(ita->first, itb->first);
          /*IMP_LOG(VERBOSE, " ok " << merged << " with score "
            << nscore <<std::endl);*/
          ret.scores[merged]= nscore;
        } else {
          //IMP_LOG(VERBOSE, " Rejected" << std::endl);
        }
      }
    }
  }
  return ret;
}

PropagatedData get_propagated_data(const ParticleIndex& all_particles,
                                   Subset* subset,
                                   const NodeData &nd) {
  PropagatedData ret;
  for (NodeData::Scores::const_iterator it= nd.scores.begin();
       it != nd.scores.end(); ++it) {
    IncompleteStates is= get_incomplete_states(all_particles,
                                               subset, it->first);
    ret.scores[is]= it->second;
  }
  return ret;
}



namespace {

EdgeData get_edge_data(const ParticleIndex &all,
                       const SubsetEvaluatorTable *eval,
                       Subset *a,
                       Subset *b,
                       const NodeData &nda) {
  EdgeData ret;
  ret.subset= get_intersection(a,b);
  ret.subset->set_was_used(true);
  IMP::internal::OwnerPointer<SubsetEvaluator> edge_eval
    = eval->get_subset_evaluator(ret.subset);
  IMP_IF_LOG(VERBOSE) {
    IMP_LOG(VERBOSE, "Edge from node " << a->get_name()
            << " to " << b->get_name()
            << " is " << ret.subset->get_name() << std::endl);
  }
  ParticleIndex a_index= get_index(a);
  // could be done better
  for (NodeData::Scores::const_iterator it = nda.scores.begin();
       it != nda.scores.end(); ++it) {
    SubsetState es(ret.subset->get_number_of_particles());
    for (unsigned int i=0; i< es.size(); ++i) {
      Particle *p=ret.subset->get_particle(i);
      int v= it->first[a_index[p]];
      es[i]=v;
    }
    if (ret.scores.find(es) == ret.scores.end()) {
      //std::cout << "Found state " << es << " with score std::endl;
      // compute score and store it
      double score= edge_eval->get_score(es);
      ret.scores[es]=score;
    }
  }
  IMP_LOG(VERBOSE,"Data is:\n" << ret << std::endl);
  return ret;
}


  PropagatedData
  get_best_conformations_internal(const SubsetGraph &jt,
                                  unsigned int root,
                                  unsigned int parent,
                                  const ParticleIndex &all_index,
                                  const SubsetEvaluatorTable *eval,
                                  const SubsetStatesTable *states,
                                  double max_score) {
    boost::property_map< SubsetGraph, boost::vertex_name_t>::const_type
      subset_map= boost::get(boost::vertex_name, jt);
    const NodeData nd= get_node_data(boost::get(subset_map, root), eval, states,
                                     max_score);
    IMP_LOG(VERBOSE, "For node " << root
            << " local data is:\n" << nd << std::endl);
    double local_max_score=0;
    for (NodeData::Scores::const_iterator it= nd.scores.begin();
         it != nd.scores.end(); ++it) {
      local_max_score= std::max(it->second, local_max_score);
    }
    PropagatedData pd= get_propagated_data(all_index, subset_map[root], nd);
    ParticleIndex root_index= get_index(subset_map[root]);
    typedef boost::graph_traits<SubsetGraph>::adjacency_iterator
      NeighborIterator;
    for (std::pair<NeighborIterator, NeighborIterator> be
           = boost::adjacent_vertices(root, jt);
         be.first != be.second; ++be.first) {
      if (*be.first == parent) continue;
      EdgeData ed= get_edge_data(all_index, eval, boost::get(subset_map, root),
                                 boost::get(subset_map, *be.first), nd);

      // compute intersection set and index map in one direction
      // for each pattern of that in me, compute subset score
      // subtract the min of mine (assume scores positive)
      // for merged score, subtract off edge value
      const PropagatedData cpd
        = get_best_conformations_internal(jt, *be.first, root,
                                          all_index, eval, states,
                                          max_score-local_max_score);
      IMP_LOG(VERBOSE, "For child " << *be.first
              << " returned data is:\n" << cpd << std::endl);
      pd= get_merged(pd, cpd, all_index, ed, max_score);
      IMP_LOG(VERBOSE, "For child " << *be.first
              << " merged data is:\n" << pd << std::endl);
    }
    return pd;
  }
}

PropagatedData get_best_conformations(const SubsetGraph &jt,
                                      int root,
                                      Subset* all_particles,
                                      const SubsetEvaluatorTable *eval,
                                      const SubsetStatesTable *states,
                                      double max_score) {
  ParticleIndex all_index=get_index(all_particles);
  const PropagatedData pd= get_best_conformations_internal(jt, root, root,
                                                           all_index, eval,
                                                           states,
                                                           max_score);
  // check if is tree
  IMP_IF_CHECK(USAGE_AND_INTERNAL) {
    for (PropagatedData::Scores::const_iterator it =pd.scores.begin();
         it != pd.scores.end(); ++it) {
      IncompleteStates is= it->first;
      for (unsigned int i=0; i< is.size(); ++i) {
        IMP_INTERNAL_CHECK(is[i] >= 0, "Entry " << i << " is still unknown");
      }
    }
  }
  return pd;
}


IMPDOMINO2_END_INTERNAL_NAMESPACE
