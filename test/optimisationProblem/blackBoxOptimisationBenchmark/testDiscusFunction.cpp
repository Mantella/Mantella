// Catch
#include <catch.hpp>

// C++ Standard Library
#include <cstdlib>
#include <string>

// Armadillo
#include <armadillo>

// Mantella
#include <mantella>

extern std::string testDirectory;

TEST_CASE("bbob::DiscusFunction", "") {
  for (const auto& numberOfDimensions : {2, 40}) {
    mant::bbob::DiscusFunction<> discusFunction(numberOfDimensions);

    arma::Mat<double> parameters;
    REQUIRE(parameters.load(testDirectory + "/data/optimisationProblem/blackBoxOptimisationBenchmark/_parameters_" + std::to_string(numberOfDimensions) + "x10.input"));

    arma::Col<double> translation;
    REQUIRE(translation.load(testDirectory + "/data/optimisationProblem/blackBoxOptimisationBenchmark/_translation_" + std::to_string(numberOfDimensions) + "x1.input"));

    arma::Mat<double> rotationR;
    REQUIRE(rotationR.load(testDirectory + "/data/optimisationProblem/blackBoxOptimisationBenchmark/_randomRotationMatrix_" + std::to_string(numberOfDimensions) + "x" + std::to_string(numberOfDimensions) + "_2.input"));

    arma::Col<double> expected;
    REQUIRE(expected.load(testDirectory + "/data/optimisationProblem/blackBoxOptimisationBenchmark/bbob_discusFunction_dim" + std::to_string(numberOfDimensions) +".expected"));

    discusFunction.setObjectiveValueTranslation(0);
    discusFunction.setParameterTranslation(translation);
    discusFunction.setParameterRotation(rotationR);

    for (std::size_t n = 0; n < parameters.n_cols; ++n) {
      CHECK(discusFunction.getObjectiveValue(parameters.col(n)) == Approx(expected.at(n)));
    }
  }

  SECTION("Returns the specified class name.") {
    CHECK(mant::bbob::DiscusFunction<>(5).toString() == "bbob_discus_function");
  }
}
