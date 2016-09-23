#include "JaniProgramGraphBuilder.h"
#include "src/storage/jani/EdgeDestination.h"

namespace storm {
    namespace builder {
        unsigned JaniProgramGraphBuilder::janiVersion = 1;
        
        storm::jani::OrderedAssignments JaniProgramGraphBuilder::buildOrderedAssignments(storm::jani::Automaton& automaton, storm::ppg::DeterministicProgramAction const& act) {
            std::vector<storm::jani::Assignment> vec;
            uint64_t level = 0;
            for(auto const& group : act) {
                for(auto const& assignment : group) {
                    vec.emplace_back(*(variables.at(assignment.first)), assignment.second, level);
                }
                ++level;
            }
            return storm::jani::OrderedAssignments(vec);
        }
        
        std::vector<storm::jani::EdgeDestination> JaniProgramGraphBuilder::buildProbabilisticDestinations(storm::jani::Automaton& automaton, storm::ppg::ProgramEdge const& edge ) {
            storm::ppg::ProbabilisticProgramAction const& act = static_cast<storm::ppg::ProbabilisticProgramAction const&>(edge.getAction());
            std::vector<storm::jani::EdgeDestination> vec;
            for(auto const& assign : act ) {
                storm::jani::Assignment  assignment(automaton.getVariables().getVariable(act.getVariableName()), expManager->integer(assign.value) ,0);
                vec.emplace_back(janiLocId.at(edge.getTargetId()), assign.probability, assignment);
            }
            return vec;
        }
    
        std::vector<storm::jani::EdgeDestination> JaniProgramGraphBuilder::buildDestinations(storm::jani::Automaton& automaton, storm::ppg::ProgramEdge const& edge ) {
            if (edge.getAction().isProbabilistic()) {
                return buildProbabilisticDestinations(automaton, edge);
            } else {
                storm::jani::OrderedAssignments oa = buildOrderedAssignments(automaton, static_cast<storm::ppg::DeterministicProgramAction const&>(edge.getAction()));
                storm::jani::EdgeDestination dest(janiLocId.at(edge.getTargetId()), expManager->rational(1.0), oa);
                return {dest};
            }
        }
        
        storm::expressions::Expression simplifyExpression(storm::expressions::Expression const& in) {
            // TODO use bound restrictions etc.
            return in.simplify();
        }
        
        std::pair<std::vector<storm::jani::Edge>, storm::expressions::Expression> JaniProgramGraphBuilder::addVariableChecks(storm::ppg::ProgramEdge const& edge) {
            std::vector<storm::jani::Edge> edges;
            storm::expressions::Expression newGuard;
            newGuard = expManager->boolean(true);
            if (edge.getAction().isProbabilistic()) {
            
            } else {
                storm::ppg::DeterministicProgramAction const& act = static_cast<storm::ppg::DeterministicProgramAction const&>(edge.getAction());
                STORM_LOG_THROW(act.nrLevels() <= 1, storm::exceptions::NotSupportedException, "Multi-level assignments with user variable bounds not supported");
                for(auto const& group : act) {
                    for(auto const& assignment : group) {
                        if (isUserRestrictedVariable(assignment.first)) {
                            assert(variableRestrictions.count(assignment.first) == 1);
                            storm::storage::IntegerInterval const& bound = variableRestrictions.at(assignment.first);
                            if (!assignment.second.containsVariables()) {
                                // Constant assignments can be checked statically.
                                // TODO we might still want to allow assignments which go out of bounds.
                                STORM_LOG_THROW(bound.contains(assignment.second.evaluateAsInt()), storm::exceptions::NotSupportedException, "User provided bounds must contain all constant expressions");
                            } else {
                                // TODO currently only fully bounded restrictions are supported;
                                assert(variableRestrictions.at(assignment.first).hasLeftBound() && variableRestrictions.at(assignment.first).hasRightBound());
                                storm::expressions::Expression newCondition = simplifyExpression(edge.getCondition() && (assignment.second > bound.getRightBound().get() || assignment.second < bound.getLeftBound().get()));
                                storm::jani::EdgeDestination dest(varOutOfBoundsLocations.at(assignment.first), expManager->rational(1.0), storm::jani::OrderedAssignments());
                                storm::jani::Edge e(janiLocId.at(edge.getSourceId()), storm::jani::Model::SILENT_ACTION_INDEX, boost::none, newCondition, {dest});
                                edges.push_back(e);
                                newGuard = newGuard && assignment.second <= bound.getRightBound().get() && assignment.second >= bound.getLeftBound().get();
                            }
                        }
                    }
                }
            }
            return {edges, newGuard};
        }
        
        
        void JaniProgramGraphBuilder::addEdges(storm::jani::Automaton& automaton) {
            for(auto it = programGraph.locationBegin(); it != programGraph.locationEnd(); ++it) {
                ppg::ProgramLocation const& loc = it->second;
                if (loc.nrOutgoingEdgeGroups() == 0) {
                    // TODO deadlock!
                } else if (loc.nrOutgoingEdgeGroups() == 1) {
                    for(auto const& edge :  **(loc.begin())) {
                        std::pair<std::vector<storm::jani::Edge>, storm::expressions::Expression> checks = addVariableChecks(*edge);
                        for(auto const& check : checks.first) {
                            automaton.addEdge(check);
                        }
                        storm::jani::Edge e(janiLocId.at(loc.id()), storm::jani::Model::SILENT_ACTION_INDEX, boost::none, simplifyExpression(edge->getCondition() && checks.second), buildDestinations(automaton, *edge));
                        automaton.addEdge(e);
                    }
                } else {
                    // We have probabilistic branching
                    if(loc.hasNonDeterminism())
                    {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Combi of nondeterminism and probabilistic choices within a loc not supported yet");
                    } else {
                        std::vector<storm::jani::EdgeDestination> destinations;
                        for(auto const& eg : loc) {
                            // TODO add assignments
                            assert(eg->nrEdges() < 2); // Otherwise, non-determinism occurs.
                            assert(eg->nrEdges() > 0); // Empty edge groups should not occur in input.
                            uint64_t target = janiLocId.at((*eg->begin())->getTargetId());
                            destinations.emplace_back(target, eg->getProbability());
                        }
                        storm::jani::Edge e(janiLocId.at(it->second.id()), storm::jani::Model::SILENT_ACTION_INDEX, boost::none,  expManager->boolean(true), destinations);
                        automaton.addEdge(e);
                    }
                }
            }
        }

    }
}
