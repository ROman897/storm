/*
 * BooleanConstantExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include "BooleanConstantExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            BooleanConstantExpression::BooleanConstantExpression(std::string const& constantName) : ConstantExpression<bool>(bool_, constantName) {
                // Nothing to do here.
            }
            
            BooleanConstantExpression::BooleanConstantExpression(BooleanConstantExpression const& booleanConstantExpression) : ConstantExpression(booleanConstantExpression) {
                // Nothing to do here.
            }

            std::shared_ptr<BaseExpression> BooleanConstantExpression::clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const {
                return std::shared_ptr<BaseExpression>(new BooleanConstantExpression(*this));
            }
            
            bool BooleanConstantExpression::getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (!this->isDefined()) {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                    << "Boolean constant '" << this->getConstantName() << "' is undefined.";
                } else {
                    return this->getValue();
                }
            }
            
            void BooleanConstantExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm