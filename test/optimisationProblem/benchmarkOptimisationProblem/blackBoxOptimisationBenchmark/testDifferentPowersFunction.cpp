// Catch
#include <catch.hpp>
#include "catchExtension.hpp"

// C++ standard library
#include <string>

// Armadillo
#include <armadillo>

// Mantella
#include <mantella>

extern std::string testDirectory;

class TestDifferentPowersFunction : public mant::bbob::DifferentPowersFunction {
 public:
  using mant::bbob::DifferentPowersFunction::DifferentPowersFunction;

  // Increases the visibility of the internal objective function, to bypass general modification, made by the parent class.
  using mant::bbob::DifferentPowersFunction::objectiveFunction_;
};

TEST_CASE("bbob::DifferentPowersFunction") {
  // All expected objective values where generated for a 40-dimensional problem instance.
  TestDifferentPowersFunction optimisationProblem(40);

  SECTION(".objectiveFunction_") {
    SECTION("Returns the expected objective value") {
      // *Note:* There is no need to *CAPTURE* anything below, as everything is stored on the hard disk, and easily accessible.

      arma::Mat<double> parameters;
      REQUIRE(parameters.load(rootTestDataDirectory + "/optimisationProblem/benchmarkOptimisationProblem/blackBoxOptimisationBenchmark/_parameters_40x100.input"));

      arma::Col<double> expectedObjectiveValues;
      REQUIRE(expectedObjectiveValues.load(rootTestDataDirectory + "/optimisationProblem/benchmarkOptimisationProblem/blackBoxOptimisationBenchmark/differentPowersFunction_dim40_1x100.expected"));

      for (arma::uword n = 0; n < parameters.n_cols; ++n) {
        CHECK(optimisationProblem.objectiveFunction_(parameters.col(n)) == Approx(expectedObjectiveValues(n)));
      }
    }
  }
}
