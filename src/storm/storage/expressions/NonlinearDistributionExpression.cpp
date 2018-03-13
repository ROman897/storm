#include "storm/storage/expressions/RationalLiteralExpression.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/ExpressionVisitor.h"
#include "storm/storage/expressions/NonlinearDistributionExpression.h"
#include "storm/exceptions/InvalidAccessException.h"

#include "storm/utility/constants.h"

namespace storm {
    namespace expressions {

            const std::map<NonlinearDistributionTypes, std::string> NonlinearDistributionExpression::distributionTypeToString =
                {{NonlinearDistributionTypes::Exp, "Exp"}, {NonlinearDistributionTypes::Weibul, "Weibul"},
                {NonlinearDistributionTypes::Uniform, "Uniform"}, {NonlinearDistributionTypes::Dirac, "Dirac"}};     

            NonlinearDistributionExpression::NonlinearDistributionExpression(ExpressionManager const& manager, NonlinearDistributionTypes type, std::shared_ptr<BaseExpression const> const& param1, std::shared_ptr<BaseExpression const> const& param2) : 
                BaseExpression(manager, manager.getNonlinearDistributionType()), distributionType(type), param1(param1), param2(param2){
                // intentionally left empty
            }

            NonlinearDistributionExpression::NonlinearDistributionExpression(ExpressionManager const& manager, NonlinearDistributionTypes type, std::shared_ptr<BaseExpression const> const& param1) : 
                BaseExpression(manager, manager.getNonlinearDistributionType()), distributionType(type), param1(param1) {
                    // intentionally left empty
                }

            /*NonlinearDistributionExpression::NonlinearDistributionExpression(ExpressionManager const& manager, std::string const& valueAsString){

            }*/

            /*DistributionLiteral_d NonlinearDistributionExpression::evaluateAsDistribution_d(Valuation const* valuation = nullptr) const{
                auto v1 = param1->evaluateAsRational();
                auto v2 = param2->evaluateAsRational();
                return {distributionType, v1, v2};
            }
            DistributionLiteral_r NonlinearDistributionExpression::evaluateAsDistribution_r(Valuation const* valuation = nullptr) const{
                auto v1 = param1->evaluateAsDouble();
                auto v2 = param2->evaluateAsDouble();
                return {distributionType, v1, v2};   
            }*/
            
             bool NonlinearDistributionExpression::isLiteral() const {
                return param1->isLiteral() && param2->isLiteral();
            }
            void NonlinearDistributionExpression::gatherVariables(std::set<storm::expressions::Variable>& variables) const {
                param1->gatherVariables(variables);
                if (param2){
                    param2->gatherVariables(variables);
                }
            }

            std::shared_ptr<BaseExpression const> NonlinearDistributionExpression::simplify() const {
                auto p1 = param1->simplify();
                if (param2){
                    auto p2 = param2->simplify();
                    return std::shared_ptr<BaseExpression>(new NonlinearDistributionExpression(getManager(), distributionType, p1, p2));
                }
                return std::shared_ptr<BaseExpression>(new NonlinearDistributionExpression(getManager(), distributionType, p1));
            }
            boost::any NonlinearDistributionExpression::accept(ExpressionVisitor& visitor, boost::any const& data) const {
                return visitor.visit(*this, data);
                // return boost::any();
            }
            /*bool NonlinearDistributionExpression::isEventDistributionExpression() const {
                return true;
            }*/
            uint_fast64_t NonlinearDistributionExpression::getArity() const {
                return param2 ? 2 : 1;
            }

            std::shared_ptr<BaseExpression const> NonlinearDistributionExpression::NonlinearDistributionExpression::getParam1() const{
                return param1;
            }
        
            std::shared_ptr<BaseExpression const> NonlinearDistributionExpression::getParam2() const {
                STORM_LOG_THROW(static_cast<bool>(param2), storm::exceptions::InvalidAccessException, "Unable to access operand 2 in expression of arity 1.");
                return param2;
            }

            NonlinearDistributionTypes NonlinearDistributionExpression::getDistributionType() const {
                return distributionType;
            }


            /*!
             * Retrieves the value of the double literal.
             *
             * @return The value of the double literal.
             */
            /*DistributionLiteral_d NonlinearDistributionExpression::getValueAsDistributionLiteral_d() const{
                return evaluateAsDistribution_d();
            }*/

            /*!
             * Retrieves the value of the double literal.
             *
             * @return The value of the double literal.
             */
            /*DistributionLiteral_r NonlinearDistributionExpression::getValue() const{
                return evaluateAsDistribution_r();
            }*/
            
            // Override base class method.
            void NonlinearDistributionExpression::printToStream(std::ostream& stream) const {
                stream << distributionTypeToString.at(distributionType) << "(" << *param1;
                if (param2){
                    stream << ", " << *param2;
                }
                stream << ")";
            }
        }
    }
