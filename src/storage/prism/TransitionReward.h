#ifndef STORM_STORAGE_PRISM_TRANSITIONREWARD_H_
#define STORM_STORAGE_PRISM_TRANSITIONREWARD_H_

#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace prism {
        class TransitionReward {
        public:
            /*!
             * Creates a transition reward for the transitions with the given name emanating from states satisfying the
             * given expression with the value given by another expression.
             *
             * @param actionName The name of the command that obtains this reward.
             * @param statePredicateExpression The predicate that needs to hold before taking a transition with the previously
             * specified name in order to obtain the reward.
             * @param rewardValueExpression An expression specifying the values of the rewards to attach to the transitions.
             */
            TransitionReward(std::string const& actionName, storm::expressions::Expression const& statePredicateExpression, storm::expressions::Expression const& rewardValueExpression);
            
            // Create default implementations of constructors/assignment.
            TransitionReward() = default;
            TransitionReward(TransitionReward const& otherVariable) = default;
            TransitionReward& operator=(TransitionReward const& otherVariable)= default;
            TransitionReward(TransitionReward&& otherVariable) = default;
            TransitionReward& operator=(TransitionReward&& otherVariable) = default;
            
            /*!
             * Retrieves the action name that is associated with this transition reward.
             *
             * @return The action name that is associated with this transition reward.
             */
            std::string const& getActionName() const;
            
            /*!
             * Retrieves the state predicate expression that is associated with this state reward.
             *
             * @return The state predicate expression that is associated with this state reward.
             */
            storm::expressions::Expression const& getStatePredicateExpression() const;
            
            /*!
             * Retrieves the reward value expression associated with this state reward.
             *
             * @return The reward value expression associated with this state reward.
             */
            storm::expressions::Expression const& getRewardValueExpression() const;
            
            friend std::ostream& operator<<(std::ostream& stream, TransitionReward const& transitionReward);

        private:
            // The name of the command this transition-based reward is attached to.
            std::string commandName;
            
            // A predicate that needs to be satisfied by states for the reward to be obtained (by taking
            // a corresponding command transition).
            storm::expressions::Expression statePredicateExpression;
            
            // The expression specifying the value of the reward obtained along the transitions.
            storm::expressions::Expression rewardValueExpression;
        };
        
    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_TRANSITIONREWARD_H_ */