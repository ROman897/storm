#include "storm/models/sparse/Gsmp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/constants.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template <typename ValueType, typename RewardModelType>
            Gsmp<ValueType, RewardModelType>::Gsmp(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::models::sparse::StateLabeling const& stateLabeling,
                                  std::unordered_map<std::string, RewardModelType> const& rewardModels)
                    : Gsmp<ValueType, RewardModelType>(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>(transitionMatrix, stateLabeling, rewardModels)) {
                // Intentionally left empty
                STORM_LOG_WARN("built gsmp from src lval");
            }
            
            template <typename ValueType, typename RewardModelType>
            Gsmp<ValueType, RewardModelType>::Gsmp(storm::storage::SparseMatrix<ValueType>&& transitionMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                                  std::unordered_map<std::string, RewardModelType>&& rewardModels)
                    : Gsmp<ValueType, RewardModelType>(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>(std::move(transitionMatrix), std::move(stateLabeling), std::move(rewardModels))) {
                // Intentionally left empty
                STORM_LOG_WARN("built gsmp from src rval");
            }
            
            template <typename ValueType, typename RewardModelType>
            Gsmp<ValueType, RewardModelType>::Gsmp(storm::storage::sparse::ModelComponents<ValueType, RewardModelType> const& components)
                    : DeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Gsmp, components), eventVariables(components.eventVariables.get()), eventToStatesMapping(components.eventToStatesMapping.get()), stateToEventsMapping(components.stateToEventsMapping.get()), eventNameToId(components.eventNameToId.get()) {
                STORM_LOG_WARN("built gsmp from conponents lval");
                // Intentionally left empty
            }
            
            template <typename ValueType, typename RewardModelType>
            Gsmp<ValueType, RewardModelType>::Gsmp(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>&& components)
                    : DeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Gsmp, std::move(components)), eventVariables(std::move(components.eventVariables.get())), eventToStatesMapping(std::move(components.eventToStatesMapping.get())), stateToEventsMapping(std::move(components.stateToEventsMapping.get())), eventNameToId(std::move(components.eventNameToId.get())) {
                STORM_LOG_WARN("built gsmp from components rval");
                for (auto const& tt : eventToStatesMapping) {

                    STORM_LOG_WARN("--------------event id:" << tt.first << "------------------");
                    for (auto s : tt.second) {
                        STORM_LOG_WARN("active at row: " << s);
                    }
                    STORM_LOG_WARN("-----------------------------------------------------------");
                }
                for (auto const& tt : eventNameToId) {
                    STORM_LOG_WARN("event name: " << tt.first << "---------->" << tt.second << std::endl);
                    STORM_LOG_WARN(getTransitionMatrixForEvent(tt.first) << std::endl);
                    STORM_LOG_WARN("----------------------------------------" << std::endl);
                }
                // Intentionally left empty
            }
   
            template<typename ValueType, typename RewardModelType>
            void Gsmp<ValueType, RewardModelType>::reduceToStateBasedRewards() {
                for (auto& rewardModel : this->getRewardModels()) {
                    rewardModel.second.reduceToStateBasedRewards(this->getTransitionMatrix(), true);
                }
            }

            template<typename ValueType, typename RewardModelType>
            bool Gsmp<ValueType, RewardModelType>::hasEvent(std::string const& event) {
                return eventNameToId.find(event) != eventNameToId.end();
            }


            template<typename ValueType, typename RewardModelType>
            storm::storage::SparseMatrix<ValueType> Gsmp<ValueType, RewardModelType>::getTransitionMatrixForEvent(std::string const& eventName) const {
                auto id = eventNameToId.at(eventName);
                auto const& transitionMatrix = this->getTransitionMatrix();
                storm::storage::SparseMatrixBuilder<ValueType> matrixBuilder;

                auto activeStates = eventToStatesMapping.at(id);
                for (auto const& groupRowIdPair : activeStates) {
                    auto const& row = transitionMatrix.getRow(groupRowIdPair.second);
                    for (auto const& entry : row) {
                        matrixBuilder.addNextValue(groupRowIdPair.first, entry.getColumn(), entry.getValue());
                    }
                }
                return matrixBuilder.build();
            }
            
            template class Gsmp<double>;

#ifdef STORM_HAVE_CARL
            template class Gsmp<storm::RationalNumber>;

            template class Gsmp<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class Gsmp<storm::RationalFunction>;
#endif
        } // namespace sparse
    } // namespace models
} // namespace storm
