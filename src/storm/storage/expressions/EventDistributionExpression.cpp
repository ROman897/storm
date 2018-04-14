#include "storm/storage/expressions/RationalLiteralExpression.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/ExpressionVisitor.h"
#include "storm/storage/expressions/EventDistributionExpression.h"
#include "storm/exceptions/InvalidAccessException.h"

#include "storm/utility/constants.h"

namespace storm {
    namespace expressions {

            const std::map<EventDistributionTypes, std::string> EventDistributionExpression::distributionTypeToString =
                {{EventDistributionTypes::Exp, "Exponentian"}, {EventDistributionTypes::Weibull, "Weibull"},
                {EventDistributionTypes::Uniform, "Uniform"}, {EventDistributionTypes::Dirac, "Dirac"},
                {EventDistributionTypes::Erlang, "Erlang"}};     

            EventDistributionExpression::EventDistributionExpression(ExpressionManager const& manager, EventDistributionTypes type, std::shared_ptr<BaseExpression const> const& param1, std::shared_ptr<BaseExpression const> const& param2) : 
                BaseExpression(manager, manager.getEventDistributionType()), distributionType(type), param1(param1), param2(param2){
                // intentionally left empty
            }

            EventDistributionExpression::EventDistributionExpression(ExpressionManager const& manager, EventDistributionTypes type, std::shared_ptr<BaseExpression const> const& param1) : 
                BaseExpression(manager, manager.getEventDistributionType()), distributionType(type), param1(param1) {
                    // intentionally left empty
                }

            /*EventDistributionExpression::EventDistributionExpression(ExpressionManager const& manager, std::string const& valueAsString){

            }*/

            /*DistributionLiteral_d EventDistributionExpression::evaluateAsDistribution_d(Valuation const* valuation = nullptr) const{
                auto v1 = param1->evaluateAsRational();
                auto v2 = param2->evaluateAsRational();
                return {distributionType, v1, v2};
            }
            DistributionLiteral_r EventDistributionExpression::evaluateAsDistribution_r(Valuation const* valuation = nullptr) const{
                auto v1 = param1->evaluateAsDouble();
                auto v2 = param2->evaluateAsDouble();
                return {distributionType, v1, v2};   
            }*/
            
             bool EventDistributionExpression::isLiteral() const {
                return param1->isLiteral() && param2->isLiteral();
            }
            void EventDistributionExpression::gatherVariables(std::set<storm::expressions::Variable>& variables) const {
                param1->gatherVariables(variables);
                if (param2){
                    param2->gatherVariables(variables);
                }
            }

            std::shared_ptr<BaseExpression const> EventDistributionExpression::simplify() const {
                auto p1 = param1->simplify();
                if (param2){
                    auto p2 = param2->simplify();
                    return std::shared_ptr<BaseExpression>(new EventDistributionExpression(getManager(), distributionType, p1, p2));
                }
                return std::shared_ptr<BaseExpression>(new EventDistributionExpression(getManager(), distributionType, p1));
            }
            boost::any EventDistributionExpression::accept(ExpressionVisitor& visitor, boost::any const& data) const {
                return visitor.visit(*this, data);
                // return boost::any();
            }
            /*bool EventDistributionExpression::isEventDistributionExpression() const {
                return true;
            }*/
            uint_fast64_t EventDistributionExpression::getArity() const {
                return param2 ? 2 : 1;
            }

            std::shared_ptr<BaseExpression const> EventDistributionExpression::EventDistributionExpression::getParam1() const{
                return param1;
            }
        
            std::shared_ptr<BaseExpression const> EventDistributionExpression::getParam2() const {
                STORM_LOG_THROW(static_cast<bool>(param2), storm::exceptions::InvalidAccessException, "Unable to access operand 2 in expression of arity 1.");
                return param2;
            }

            EventDistributionTypes EventDistributionExpression::getDistributionType() const {
                return distributionType;
            }


            /*!
             * Retrieves the value of the double literal.
             *
             * @return The value of the double literal.
             */
            /*DistributionLiteral_d EventDistributionExpression::getValueAsDistributionLiteral_d() const{
                return evaluateAsDistribution_d();
            }*/

            /*!
             * Retrieves the value of the double literal.
             *
             * @return The value of the double literal.
             */
            /*DistributionLiteral_r EventDistributionExpression::getValue() const{
                return evaluateAsDistribution_r();
            }*/
            
            // Override base class method.
            void EventDistributionExpression::printToStream(std::ostream& stream) const {
                stream << distributionTypeToString.at(distributionType) << "(" << *param1;
                if (param2){
                    stream << ", " << *param2;
                }
                stream << ")";
            }
        }
    }
