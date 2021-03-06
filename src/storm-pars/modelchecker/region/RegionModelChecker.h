#pragma once

#include <memory>

#include "storm-pars/modelchecker/results/RegionCheckResult.h"
#include "storm-pars/modelchecker/results/RegionRefinementCheckResult.h"
#include "storm-pars/modelchecker/region/RegionResult.h"
#include "storm-pars/modelchecker/region/RegionResultHypothesis.h"
#include "storm-pars/storage/ParameterRegion.h"

#include "storm/models/ModelBase.h"
#include "storm/modelchecker/CheckTask.h"

namespace storm {
    
    class Environment;
    
    namespace modelchecker{
        
        template<typename ParametricType>
        class RegionModelChecker {
        public:
            
            typedef typename storm::storage::ParameterRegion<ParametricType>::CoefficientType CoefficientType;
            typedef typename storm::storage::ParameterRegion<ParametricType>::VariableType VariableType;

            RegionModelChecker();
            virtual ~RegionModelChecker() = default;
            
            virtual bool canHandle(std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, ParametricType> const& checkTask) const = 0;
            virtual void specify(Environment const& env, std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, ParametricType> const& checkTask, bool generateRegionSplitEstimates, bool allowModelSimplifications = true) = 0;

            
            /*!
             * Analyzes the given region.
             * @param hypothesis if not 'unknown', the region checker only tries to show the hypothesis
             * @param initialResult encodes what is already known about this region
             * @param sampleVerticesOfRegion enables sampling of the vertices of the region in cases where AllSat/AllViolated could not be shown.
             */
            virtual RegionResult analyzeRegion(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, RegionResultHypothesis const& hypothesis = RegionResultHypothesis::Unknown, RegionResult const& initialResult = RegionResult::Unknown, bool sampleVerticesOfRegion = false) = 0;
            
             /*!
             * Analyzes the given regions.
             * @param hypothesis if not 'unknown', we only try to show the hypothesis for each region

             * If supported by this model checker, it is possible to sample the vertices of the regions whenever AllSat/AllViolated could not be shown.
             */
            std::unique_ptr<storm::modelchecker::RegionCheckResult<ParametricType>> analyzeRegions(Environment const& env, std::vector<storm::storage::ParameterRegion<ParametricType>> const& regions, std::vector<RegionResultHypothesis> const& hypotheses, bool sampleVerticesOfRegion = false) ;

            virtual ParametricType getBoundAtInitState(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dirForParameters);
            
            /*!
             * Iteratively refines the region until the region analysis yields a conclusive result (AllSat or AllViolated).
             * @param region the considered region
             * @param coverageThreshold if given, the refinement stops as soon as the fraction of the area of the subregions with inconclusive result is less then this threshold
             * @param depthThreshold if given, the refinement stops at the given depth. depth=0 means no refinement.
             * @param hypothesis if not 'unknown', it is only checked whether the hypothesis holds within the given region.
             *
             */
            std::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ParametricType>> performRegionRefinement(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, boost::optional<ParametricType> const& coverageThreshold, boost::optional<uint64_t> depthThreshold = boost::none, RegionResultHypothesis const& hypothesis = RegionResultHypothesis::Unknown);
            
            /*!
             * Returns true if region split estimation (a) was enabled when model and check task have been specified and (b) is supported by this region model checker.
             */
            virtual bool isRegionSplitEstimateSupported() const;
            
            /*!
             * Returns an estimate of the benefit of splitting the last checked region with respect to each parameter. This method should only be called if region split estimation is supported and enabled.
             * If a parameter is assigned a high value, we should prefer splitting with respect to this parameter.
             */
            virtual std::map<VariableType, double> getRegionSplitEstimate() const;
            
        };

    } //namespace modelchecker
} //namespace storm
