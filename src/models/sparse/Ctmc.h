#ifndef STORM_MODELS_SPARSE_CTMC_H_
#define STORM_MODELS_SPARSE_CTMC_H_

#include <memory>
#include <vector>

#include "src/models/sparse/DeterministicModel.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            /*!
             * This class represents a continuous-time Markov chain.
             */
            template <typename ValueType>
            class Ctmc : public DeterministicModel<ValueType> {
            public:
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param rateMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param optionalStateRewardVector The reward values associated with the states.
                 * @param optionalTransitionRewardMatrix The reward values associated with the transitions of the model.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                Ctmc(storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::models::sparse::StateLabeling const& stateLabeling,
                     boost::optional<std::vector<ValueType>> const& optionalStateRewardVector = boost::optional<std::vector<ValueType>>(),
                     boost::optional<storm::storage::SparseMatrix<ValueType>> const& optionalTransitionRewardMatrix = boost::optional<storm::storage::SparseMatrix<ValueType>>(),
                     boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> const& optionalChoiceLabeling = boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
                
                /*!
                 * Constructs a model by moving the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param optionalStateRewardVector The reward values associated with the states.
                 * @param optionalTransitionRewardMatrix The reward values associated with the transitions of the model.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                Ctmc(storm::storage::SparseMatrix<ValueType>&& rateMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                     boost::optional<std::vector<ValueType>>&& optionalStateRewardVector = boost::optional<std::vector<ValueType>>(),
                     boost::optional<storm::storage::SparseMatrix<ValueType>>&& optionalTransitionRewardMatrix = boost::optional<storm::storage::SparseMatrix<ValueType>>(),
                     boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>&& optionalChoiceLabeling = boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
                
                Ctmc(Ctmc<ValueType> const& ctmc) = default;
                Ctmc& operator=(Ctmc<ValueType> const& ctmc) = default;
                
#ifndef WINDOWS
                Ctmc(Ctmc<ValueType>&& ctmc) = default;
                Ctmc& operator=(Ctmc<ValueType>&& ctmc) = default;
#endif

            };
            
        } // namespace sparse
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SPARSE_CTMC_H_ */
