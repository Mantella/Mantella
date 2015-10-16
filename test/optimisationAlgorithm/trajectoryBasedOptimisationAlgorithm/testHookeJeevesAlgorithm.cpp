// Catch
#include <catch.hpp>

// C++ Standard Library
#include <memory>

// Mantella
#include <mantella>

TEST_CASE(
    "HookeJeevesAlgorithm") {
  SECTION(
      ".optimise") {
    // TODO
  }

  SECTION(
      ".toString") {
    SECTION(
        "Returns a (filesystem friendly) name for the class.") {
      std::shared_ptr<mant::OptimisationProblem> optimisationProblem(new mant::bbob::SphereFunction(2));
      CHECK(mant::HookeJeevesAlgorithm(optimisationProblem).toString() ==
            "hooke_jeeves_algorithm");
    }
  }
}
