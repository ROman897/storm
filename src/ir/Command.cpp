/*
 * Command.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>
#include <iostream>

#include "Command.h"
#include "src/parser/prismparser/VariableState.h"

namespace storm {
    namespace ir {
        
        Command::Command() : actionName(), guardExpression(), updates() {
            // Nothing to do here.
        }
        
        Command::Command(uint_fast64_t globalIndex, std::string const& actionName, std::shared_ptr<storm::ir::expressions::BaseExpression> guardExpression, std::vector<storm::ir::Update> const& updates)
        : actionName(actionName), guardExpression(guardExpression), updates(updates), globalIndex(globalIndex) {
            // Nothing to do here.
        }
        
        Command::Command(Command const& oldCommand, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState& variableState)
        : actionName(oldCommand.getActionName()), guardExpression(oldCommand.guardExpression->clone(renaming, variableState)), globalIndex(newGlobalIndex) {
            auto renamingPair = renaming.find(this->actionName);
            if (renamingPair != renaming.end()) {
                this->actionName = renamingPair->first;
            }
            this->updates.reserve(oldCommand.getNumberOfUpdates());
            for (Update const& update : oldCommand.updates) {
                this->updates.emplace_back(update, variableState.getNextGlobalUpdateIndex(), renaming, variableState);
                variableState.nextGlobalUpdateIndex++;
            }
        }
        
        std::string const& Command::getActionName() const {
            return this->actionName;
        }
        
        std::shared_ptr<storm::ir::expressions::BaseExpression> const& Command::getGuard() const {
            return guardExpression;
        }
        
        uint_fast64_t Command::getNumberOfUpdates() const {
            return this->updates.size();
        }
        
        storm::ir::Update const& Command::getUpdate(uint_fast64_t index) const {
            return this->updates[index];
        }
        
        uint_fast64_t Command::getGlobalIndex() const {
            return this->globalIndex;
        }
        
        std::string Command::toString() const {
            std::stringstream result;
            result << "[" << actionName << "] " << guardExpression->toString() << " -> ";
            for (uint_fast64_t i = 0; i < updates.size(); ++i) {
                result << updates[i].toString();
                if (i < updates.size() - 1) {
                    result << " + ";
                }
            }
            result << ";";
            return result.str();
        }
        
    } // namespace ir
} // namespace storm