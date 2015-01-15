#pragma once

// C++ Standard Library
#include <array>
#include <vector>

// Mantella
#include <mantella_bits/optimisationAlgorithm/populationBasedAlgorithm/parallelAlgorithm.hpp>
#include <mantella_bits/distanceFunction/euclideanDistance.hpp>

namespace mant {
  class ParallelStandardParticleSwarmOptimisation2011 : public ParallelAlgorithm<double, EuclideanDistance> {
    public:
      explicit ParallelStandardParticleSwarmOptimisation2011(
          const std::shared_ptr<OptimisationProblem<double>> optimisationProblem,
          const unsigned int& populationSize) noexcept;

      ParallelStandardParticleSwarmOptimisation2011(const ParallelStandardParticleSwarmOptimisation2011&) = delete;
      ParallelStandardParticleSwarmOptimisation2011& operator=(const ParallelStandardParticleSwarmOptimisation2011&) = delete;

      void setNeighbourhoodProbability(
          const double& neighbourhoodProbability) noexcept;
      void setAcceleration(
          const double& acceleration) noexcept;
      void setLocalAttraction(
          const double& localAttraction) noexcept;
      void setGlobalAttraction(
          const double& globalAttraction) noexcept;
      void setCommunicationSteps(
          const unsigned int& communicationSteps) noexcept;

      std::string to_string() const noexcept override;

    protected:
      double neighbourhoodProbability_;
      double acceleration_;
      double localAttraction_;
      double globalAttraction_;

      unsigned int communicationSteps_;

      void parallelOptimiseImplementation() noexcept override;
  };
}
