#ifndef STORM_STORAGE_EXPRESSIONS_EventDistributionExpression_H_
#define STORM_STORAGE_EXPRESSIONS_EventDistributionExpression_H_

#include "storm/storage/expressions/BaseExpression.h"
#include "storm/storage/expressions/EventDistributionTypes.h"

namespace storm {
    namespace expressions {

        struct DistributionLiteral_d{
            EventDistributionTypes type;
            double param1;
            double param2;
        };

        struct DistributionLiteral_r{
            EventDistributionTypes type;
            storm::RationalNumber param1;
            storm::RationalNumber param2;
        };

        class EventDistributionExpression : public BaseExpression {
        public:
            static const std::map<EventDistributionTypes, std::string> distributionTypeToString;
            /*!
             * Creates nonlinear distribution expression with the given expressions as parameters.
             *
             * @param manager The manager responsible for this expression.
             * @param type type of the distribution e.g exponential
             * @param type param1 first parameter descibing the distribution
             * @param type param2 second parameter descibing the distribution
             */
            EventDistributionExpression(ExpressionManager const& manager, EventDistributionTypes type, std::shared_ptr<BaseExpression const> const& param1, std::shared_ptr<BaseExpression const> const& param2);

            /*!
             * Creates nonlinear distribution expression with the given expression a parameter.
             *
             * @param manager The manager responsible for this expression.
             * @param type type of the distribution e.g exponential
             * @param type param1 first parameter descibing the distribution
             */
            EventDistributionExpression(ExpressionManager const& manager, EventDistributionTypes type, std::shared_ptr<BaseExpression const> const& param1);


            /*!
             * Creates an nonlinear distribution expression with the value given as a string.
             *
             * @param manager The manager responsible for this expression.
             * @param type representation of the value of the literal.
             */
            // EventDistributionExpression(ExpressionManager const& manager, std::string const& valueAsString);

            // Instantiate constructors and assignments with their default implementations.
            EventDistributionExpression(EventDistributionExpression const& other) = default;
            EventDistributionExpression& operator=(EventDistributionExpression const& other) = delete;
            EventDistributionExpression(EventDistributionExpression&&) = default;
            EventDistributionExpression& operator=(EventDistributionExpression&&) = delete;

            virtual ~EventDistributionExpression() = default;
            
            // Override base class methods.
            // virtual DistributionLiteral_d evaluateAsDistribution_d(Valuation const* valuation = nullptr) const override;
            // virtual DistributionLiteral_r evaluateAsDistribution_r(Valuation const* valuation = nullptr) const override;
            
            virtual bool isLiteral() const override;
            virtual void gatherVariables(std::set<storm::expressions::Variable>& variables) const override;
            virtual std::shared_ptr<BaseExpression const> simplify() const override;
            virtual boost::any accept(ExpressionVisitor& visitor, boost::any const& data) const override;
            // virtual bool isEventDistributionExpression() const override;
            virtual uint_fast64_t getArity() const override;
            std::shared_ptr<BaseExpression const> getParam1() const;
        
            std::shared_ptr<BaseExpression const> getParam2() const;

            EventDistributionTypes getDistributionType() const override;

            /*!
             * Retrieves the value of the double literal.
             *
             * @return The value of the double literal.
             */
            // DistributionLiteral_d getValueAsDistributionLiteral_d() const;

            /*!
             * Retrieves the value of the double literal.
             *
             * @return The value of the double literal.
             */
            // DistributionLiteral_r getValue() const;
            
        protected:
            // Override base class method.
            virtual void printToStream(std::ostream& stream) const override;
            
        private:
            EventDistributionTypes distributionType;
            // The first parameter describing the distribution
            std::shared_ptr<BaseExpression const> param1;

            // The second parameter describing the distribution
            // might not be used if the distribution requires only 1 parameter
            std::shared_ptr<BaseExpression const> param2;
        
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_EventDistributionExpression_H_ */
