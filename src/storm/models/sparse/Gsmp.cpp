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
                    : DeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Gsmp, components), eventVariables(components.eventVariables.get()), eventToStatesMapping(components.eventToStatesMapping.get()), stateToEventsMapping(components.stateToEventsMapping.get()) {
                STORM_LOG_WARN("built gsmp from conponents lval");
                // Intentionally left empty
            }
            
           template <typename ValueType, typename RewardModelType>
            Gsmp<ValueType, RewardModelType>::Gsmp(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>&& components)
                    : DeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Gsmp, std::move(components)), eventVariables(std::move(components.eventVariables.get())), eventToStatesMapping(std::move(components.eventToStatesMapping.get())), stateToEventsMapping(std::move(components.stateToEventsMapping.get())) {
                STORM_LOG_WARN("built gsmp from components rval");
                for (auto const& tt : eventToStatesMapping) {

                    STORM_LOG_WARN("--------------event id:" << tt.first << "------------------");
                    for (auto s : tt.second) {
                        STORM_LOG_WARN("active at row: " << s);
                    }
                    STORM_LOG_WARN("-----------------------------------------------------------");
                }
                // Intentionally left empty
            }
   
            template<typename ValueType, typename RewardModelType>
            void Gsmp<ValueType, RewardModelType>::reduceToStateBasedRewards() {
                for (auto& rewardModel : this->getRewardModels()) {
                    rewardModel.second.reduceToStateBasedRewards(this->getTransitionMatrix(), true);
                }
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
