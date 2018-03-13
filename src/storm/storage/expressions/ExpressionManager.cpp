#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/storage/expressions/Expressions.h"
#include "storm/storage/expressions/Variable.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace expressions {

        VariableIterator::VariableIterator(ExpressionManager const& manager, std::unordered_map<std::string, uint_fast64_t>::const_iterator nameIndexIterator, std::unordered_map<std::string, uint_fast64_t>::const_iterator nameIndexIteratorEnd, VariableSelection const& selection) : manager(manager), nameIndexIterator(nameIndexIterator), nameIndexIteratorEnd(nameIndexIteratorEnd), selection(selection) {
            moveUntilNextSelectedElement(false);
        }
        
        bool VariableIterator::operator==(VariableIterator const& other) {
            return this->nameIndexIterator == other.nameIndexIterator;
        }
        
        bool VariableIterator::operator!=(VariableIterator const& other) {
            return !(*this == other);
        }
        
        VariableIterator::value_type& VariableIterator::operator*() {
            return currentElement;
        }
        
        VariableIterator& VariableIterator::operator++(int) {
            moveUntilNextSelectedElement();
            return *this;
        }
        
        VariableIterator& VariableIterator::operator++() {
            moveUntilNextSelectedElement();
            return *this;
        }
        
        void VariableIterator::moveUntilNextSelectedElement(bool atLeastOneStep) {
            if (atLeastOneStep && nameIndexIterator != nameIndexIteratorEnd) {
                ++nameIndexIterator;
            }
            
            // Move the underlying iterator forward until a variable matches the selection.
            while (nameIndexIterator != nameIndexIteratorEnd
                   && (selection == VariableSelection::OnlyRegularVariables && (nameIndexIterator->second & ExpressionManager::auxiliaryMask) != 0)
                   && (selection == VariableSelection::OnlyAuxiliaryVariables && (nameIndexIterator->second & ExpressionManager::auxiliaryMask) == 0)) {
                ++nameIndexIterator;
            }
            
            if (nameIndexIterator != nameIndexIteratorEnd) {
                currentElement = std::make_pair(Variable(manager.getSharedPointer(), nameIndexIterator->second), manager.getVariableType(nameIndexIterator->second));
            }
        }
        
        ExpressionManager::ExpressionManager() : nameToIndexMapping(), indexToNameMapping(), indexToTypeMapping(), numberOfBooleanVariables(0), numberOfIntegerVariables(0), numberOfBitVectorVariables(0), numberOfRationalVariables(0), numberOfAuxiliaryVariables(0), numberOfAuxiliaryBooleanVariables(0), numberOfAuxiliaryIntegerVariables(0), numberOfAuxiliaryBitVectorVariables(0), numberOfAuxiliaryRationalVariables(0), freshVariableCounter(0) {
            // Intentionally left empty.
        }
        
        ExpressionManager::~ExpressionManager() {
            // Intentionally left empty.
        }
        
        std::shared_ptr<ExpressionManager> ExpressionManager::clone() const {
            return std::shared_ptr<ExpressionManager>(new ExpressionManager(*this));
        }
        
        Expression ExpressionManager::boolean(bool value) const {
            return Expression(std::make_shared<BooleanLiteralExpression>(*this, value));
        }

        Expression ExpressionManager::integer(int_fast64_t value) const {
            return Expression(std::make_shared<IntegerLiteralExpression>(*this, value));
        }

        Expression ExpressionManager::rational(double value) const {
            return Expression(std::make_shared<RationalLiteralExpression>(*this, value));
        }
        
        Expression ExpressionManager::rational(storm::RationalNumber const& value) const {
            return Expression(std::make_shared<RationalLiteralExpression>(*this, value));
        }

        // Roman code
        /*Expression ExpressionManager::NonlinearDistribution(NonlinearDistributionTypes type, Expression const& exp1, Expression const& exp2) const{
            // TODO(Roman): not sure if this will be properly used
            // TODO FIX this does not work as it is
            return Expression(std::make_shared<NonlinearDistributionExpression>(*this, std::shared_ptr<BaseType>( new Expression(exp1)), std::shared_ptr<BaseType>( new Expression(exp2))));
        }*/

        /*Expression ExpressionManager::NonlinearDistribution(NonlinearDistributionTypes type, storm::RationalNumber const& param1) const{
            // TODO(Roman): not sure if this will be properly used
            return Expression(std::make_shared<NonlinearDistributionExpression>(*this, std::shared_ptr<BaseType>( new Expression(exp1))));
        }*/
        // end code

        bool ExpressionManager::operator==(ExpressionManager const& other) const {
            return this == &other;
        }
        
        Type const& ExpressionManager::getBooleanType() const {
            if (!booleanType) {
                booleanType = Type(this->getSharedPointer(), std::shared_ptr<BaseType>(new BooleanType()));
            }
            return booleanType.get();
        }
        
        Type const& ExpressionManager::getIntegerType() const {
            if (!integerType) {
                integerType = Type(this->getSharedPointer(), std::shared_ptr<BaseType>(new IntegerType()));
            }
            return integerType.get();
        }
        
        Type const& ExpressionManager::getBitVectorType(std::size_t width) const {
            Type type(this->getSharedPointer(), std::shared_ptr<BaseType>(new BitVectorType(width)));
            auto typeIterator = bitvectorTypes.find(type);
            if (typeIterator == bitvectorTypes.end()) {
                auto iteratorBoolPair = bitvectorTypes.insert(type);
                return *iteratorBoolPair.first;
            }
            return *typeIterator;
        }
        
        Type const& ExpressionManager::getRationalType() const {
            if (!rationalType) {
                rationalType = Type(this->getSharedPointer(), std::shared_ptr<BaseType>(new RationalType()));
            }
            return rationalType.get();
        }

        Type const& ExpressionManager::getNonlinearDistributionType() const {
            if (!nonlinearDistributionType) {
                nonlinearDistributionType = Type(this->getSharedPointer(), std::shared_ptr<BaseType>(new NonlinearDistributionType()));
            }
            return nonlinearDistributionType.get();   
        }

        Type const& ExpressionManager::getEventType() const {
            if (!eventType) {
                eventType = Type(this->getSharedPointer(), std::shared_ptr<BaseType>(new EventType()));
            }
            return eventType.get();
        }

        
        bool ExpressionManager::isValidVariableName(std::string const& name) {
            return name.size() < 2 || name.at(0) != '_' || name.at(1) != '_';
        }

        bool ExpressionManager::variableExists(std::string const& name) const {
            auto nameIndexPair = nameToIndexMapping.find(name);
            return nameIndexPair != nameToIndexMapping.end();
        }
        
        Variable ExpressionManager::declareVariableCopy(Variable const& variable) {
            return declareFreshVariable(variable.getType(), true, "_" + variable.getName() + "_");
        }
        
        Variable ExpressionManager::declareVariable(std::string const& name, storm::expressions::Type const& variableType, bool auxiliary) {
            STORM_LOG_THROW(!variableExists(name), storm::exceptions::InvalidArgumentException, "Variable with name '" << name << "' already exists.");
            return declareOrGetVariable(name, variableType, auxiliary);
        }
        
        Variable ExpressionManager::declareBooleanVariable(std::string const& name, bool auxiliary) {
            Variable var = this->declareVariable(name, this->getBooleanType(), auxiliary);
            return var;
        }
        
        Variable ExpressionManager::declareIntegerVariable(std::string const& name, bool auxiliary) {
            return this->declareVariable(name, this->getIntegerType(), auxiliary);
        }

        Variable ExpressionManager::declareBitVectorVariable(std::string const& name, std::size_t width, bool auxiliary) {
            return this->declareVariable(name, this->getBitVectorType(width), auxiliary);
        }
        
        Variable ExpressionManager::declareRationalVariable(std::string const& name, bool auxiliary) {
            return this->declareVariable(name, this->getRationalType(), auxiliary);
        }

        Variable ExpressionManager::declareNonlinearDistributionVariable(std::string const& name, bool auxiliary){
            return this->declareVariable(name, this->getNonlinearDistributionType(), auxiliary);
        }

        Variable ExpressionManager::declareEventVariable(std::string const& name, bool auxiliary) {
            return this->declareVariable(name, this->getEventType(), auxiliary);    
        }

        Variable ExpressionManager::declareOrGetVariable(std::string const& name, storm::expressions::Type const& variableType, bool auxiliary) {
            return declareOrGetVariable(name, variableType, auxiliary, true);
        }

        Variable ExpressionManager::declareOrGetVariable(std::string const& name, storm::expressions::Type const& variableType, bool auxiliary, bool checkName) {
            STORM_LOG_THROW(!checkName || isValidVariableName(name), storm::exceptions::InvalidArgumentException, "Invalid variable name '" << name << "'.");
            auto nameIndexPair = nameToIndexMapping.find(name);
            if (nameIndexPair != nameToIndexMapping.end()) {
                return Variable(this->getSharedPointer(), nameIndexPair->second);
            } else {
                uint_fast64_t offset = 0;
                if (auxiliary) {
                    if (variableType.isBooleanType()) {
                        offset = numberOfBooleanVariables++;
                    } else if (variableType.isIntegerType()) {
                        offset = numberOfIntegerVariables++ + numberOfBitVectorVariables;
                    } else if (variableType.isBitVectorType()) {
                        offset = numberOfBitVectorVariables++ + numberOfIntegerVariables;
                    } else if (variableType.isRationalType()) {
                        offset = numberOfRationalVariables++;
                        // Roman modification
                    } else if (variableType.isEventDistributionType()) {
                        offset = numberOfNonlinearDistributionVariables++;
                    } else if (variableType.isEventType()) {
                        offset = numberOfEventVariables++;
                    }
                } else {
                    if (variableType.isBooleanType()) {
                        offset = numberOfBooleanVariables++;
                    } else if (variableType.isIntegerType()) {
                        offset = numberOfIntegerVariables++ + numberOfBitVectorVariables;
                    } else if (variableType.isBitVectorType()) {
                        offset = numberOfBitVectorVariables++ + numberOfIntegerVariables;
                    } else if (variableType.isRationalType()) {
                        offset = numberOfRationalVariables++;
                        // Roman modification
                    } else if (variableType.isEventDistributionType()) {
                        offset = numberOfNonlinearDistributionVariables++;
                    } else if (variableType.isEventType()) {
                        offset = numberOfEventVariables++;
                    }
                }
                
                // Compute the index of the new variable.
                uint_fast64_t newIndex = offset | variableType.getMask() | (auxiliary ? auxiliaryMask : 0);
                
                // Properly insert the variable into the data structure.
                nameToIndexMapping[name] = newIndex;
                indexToNameMapping[newIndex] = name;
                indexToTypeMapping[newIndex] = variableType;
                Variable result(this->getSharedPointer(), newIndex);
                variableSet.insert(result);
                return result;
            }
        }
        
        Variable ExpressionManager::getVariable(std::string const& name) const {
            auto nameIndexPair = nameToIndexMapping.find(name);
            STORM_LOG_THROW(nameIndexPair != nameToIndexMapping.end(),  storm::exceptions::InvalidArgumentException, "Unknown variable '" << name << "'.");
            return Variable(this->getSharedPointer(), nameIndexPair->second);
        }
        
        std::set<Variable> const& ExpressionManager::getVariables() const {
            return variableSet;
        }
        
        Expression ExpressionManager::getVariableExpression(std::string const& name) const {
            return Expression(getVariable(name));
        }
        
        bool ExpressionManager::hasVariable(std::string const& name) const {
            return nameToIndexMapping.find(name) != nameToIndexMapping.end();
        }

        Variable ExpressionManager::declareFreshVariable(storm::expressions::Type const& variableType, bool auxiliary, std::string const& prefix) {
            std::string newName = prefix + std::to_string(freshVariableCounter++);
            return declareOrGetVariable(newName, variableType, auxiliary, false);
        }
        
        Variable ExpressionManager::declareFreshBooleanVariable(bool auxiliary, const std::string& prefix) {
            return declareFreshVariable(this->getBooleanType(), auxiliary, prefix);
        }
        
        Variable ExpressionManager::declareFreshIntegerVariable(bool auxiliary, const std::string& prefix) {
            return declareFreshVariable(this->getIntegerType(), auxiliary, prefix);
        }
        
        Variable ExpressionManager::declareFreshRationalVariable(bool auxiliary, const std::string& prefix) {
            return declareFreshVariable(this->getRationalType(), auxiliary, prefix);
        }
        // Roman code
        Variable ExpressionManager::declareFreshNonlinearDistributionVariable(bool auxiliary, const std::string& prefix) {
            return declareFreshVariable(this->getNonlinearDistributionType(), auxiliary, prefix);
        }

        Variable ExpressionManager::declareFreshEventVariable(bool auxiliary, std::string const& prefix) {
            return declareFreshVariable(this->getEventType(), auxiliary, prefix);
        }

        
        uint_fast64_t ExpressionManager::getNumberOfVariables(storm::expressions::Type const& variableType) const {
            if (variableType.isBooleanType()) {
                return numberOfBooleanVariables;
            } else if (variableType.isIntegerType()) {
                return numberOfIntegerVariables;
            } else if (variableType.isBitVectorType()) {
                return numberOfBitVectorVariables;
            } else if (variableType.isRationalType()) {
                return numberOfRationalVariables;
                // Roman modification
            } else if (variableType.isEventDistributionType()){
                return numberOfNonlinearDistributionVariables;
            }
            return 0;
        }
        
        uint_fast64_t ExpressionManager::getNumberOfVariables() const {
            return numberOfBooleanVariables + numberOfIntegerVariables + numberOfBitVectorVariables + 
            numberOfRationalVariables + numberOfNonlinearDistributionVariables;
        }
        
        uint_fast64_t ExpressionManager::getNumberOfBooleanVariables() const {
            return numberOfBooleanVariables;
        }
        
        uint_fast64_t ExpressionManager::getNumberOfIntegerVariables() const {
            return numberOfIntegerVariables;
        }
        
        uint_fast64_t ExpressionManager::getNumberOfBitVectorVariables() const {
            return numberOfBitVectorVariables;
        }
        
        uint_fast64_t ExpressionManager::getNumberOfRationalVariables() const {
            return numberOfRationalVariables;
        }

        // Roman code
        uint_fast64_t ExpressionManager::getNumberOfNonlinearDistributionVariables() const{
            return numberOfNonlinearDistributionVariables;
        }

        
        std::string const& ExpressionManager::getVariableName(uint_fast64_t index) const {
            auto indexTypeNamePair = indexToNameMapping.find(index);
            STORM_LOG_THROW(indexTypeNamePair != indexToNameMapping.end(), storm::exceptions::InvalidArgumentException, "Unknown variable index '" << index << "'.");
            return indexTypeNamePair->second;
        }
        
        Type const& ExpressionManager::getVariableType(uint_fast64_t index) const {
            auto indexTypePair = indexToTypeMapping.find(index);
            STORM_LOG_ASSERT(indexTypePair != indexToTypeMapping.end(), "Unable to retrieve type of unknown variable index.");
            return indexTypePair->second;
        }
        
        uint_fast64_t ExpressionManager::getOffset(uint_fast64_t index) const {
            return index & offsetMask;
        }
        
        ExpressionManager::const_iterator ExpressionManager::begin() const {
            return ExpressionManager::const_iterator(*this, this->nameToIndexMapping.begin(), this->nameToIndexMapping.end(), const_iterator::VariableSelection::OnlyRegularVariables);
        }
        
        ExpressionManager::const_iterator ExpressionManager::end() const {
            return ExpressionManager::const_iterator(*this, this->nameToIndexMapping.end(), this->nameToIndexMapping.end(), const_iterator::VariableSelection::OnlyRegularVariables);
        }
        
        std::shared_ptr<ExpressionManager> ExpressionManager::getSharedPointer() {
            return this->shared_from_this();
        }

        std::shared_ptr<ExpressionManager const> ExpressionManager::getSharedPointer() const {
            return this->shared_from_this();
        }
        
        std::ostream& operator<<(std::ostream& out, ExpressionManager const& manager) {
            out << "manager {" << std::endl;
            
            for (auto const& variableTypePair : manager) {
                std::cout << "\t" << variableTypePair.second << " " << variableTypePair.first.getName() << " [offset " << variableTypePair.first.getOffset() << "]" << std::endl;
            }
            
            out << "}" << std::endl;
            
            return out;
        }
        
    } // namespace expressions
} // namespace storm
