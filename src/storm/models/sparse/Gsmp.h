#ifndef STORM_MODELS_SPARSE_GSMP_H_
#define STORM_MODELS_SPARSE_GSMP_H_

#include "storm/models/sparse/DeterministicModel.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            /*!
             * This class represents a discrete-time Markov chain.
             */
            template<class ValueType, typename RewardModelType = StandardRewardModel<ValueType>>
            class Gsmp : public DeterministicModel<ValueType, RewardModelType> {
            public:
                    /*!
                 * Constructs a model from the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 */
                Gsmp(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                    storm::models::sparse::StateLabeling const& stateLabeling,
                    std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>());
                
                /*!
                 * Constructs a model by moving the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 */
                Gsmp(storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                    storm::models::sparse::StateLabeling&& stateLabeling,
                    std::unordered_map<std::string, RewardModelType>&& rewardModels = std::unordered_map<std::string, RewardModelType>());
                
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param components The components for this model.
                 */
                Gsmp(storm::storage::sparse::ModelComponents<ValueType, RewardModelType> const& components);
                Gsmp(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>&& components);
                
                
                Gsmp(Gsmp<ValueType, RewardModelType> const& gsmp) = default;
                Gsmp& operator=(Gsmp<ValueType, RewardModelType> const& gsmp) = default;
                
                Gsmp(Gsmp<ValueType, RewardModelType>&& gsmp) = default;
                Gsmp& operator=(Gsmp<ValueType, RewardModelType>&& gsmp) = default;

                virtual void reduceToStateBasedRewards() override;

            private:
                std::vector<generator::EventVariableInformation> eventVariables;
                std::map<uint_fast64_t, std::vector<uint_fast64_t>> eventToStatesMapping;
                std::map<uint_fast64_t, std::vector<uint_fast64_t>> stateToEventsMapping;


            };
            
        } // namespace sparse
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SPARSE_GSMP_H_ */
