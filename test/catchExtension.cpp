#include "catchExtension.hpp"

// C++ standard library
#include <algorithm>
#include <cmath>
#include <random>

// Mantella
#include <mantella>

std::string rootTestDataDirectory("");
int nodeRank(0);
int numberOfNodes(1);

arma::Mat<double> getContinuousRandomNumbers(
    const arma::uword numberOfRows,
    const arma::uword numberOfColumns) {
  return arma::randu<arma::Mat<double>>(numberOfRows, numberOfColumns) * 200.0 - 100.0;
}

arma::Mat<arma::uword> getDiscreteRandomNumbers(
    const arma::uword numberOfRows,
    const arma::uword numberOfColumns) {
  return arma::randi<arma::Mat<arma::uword>>(numberOfRows, numberOfColumns, arma::distr_param(1, 10));
}

arma::Col<double> getContinuousRandomNumbers(
    const arma::uword numberOfDimensions) {
  return arma::randu<arma::Col<double>>(numberOfDimensions) * 200.0 - 100.0;
}

arma::Col<arma::uword> getDiscreteRandomNumbers(
    const arma::uword numberOfElements) {
  return arma::randi<arma::Col<arma::uword>>(numberOfElements, arma::distr_param(1, 10));
}

double getContinuousRandomNumber() {
  return std::uniform_real_distribution<double>(-100.0, 100.0)(mant::Rng::getGenerator());
}

arma::uword getDiscreteRandomNumber() {
  return std::uniform_int_distribution<arma::uword>(1, 10)(mant::Rng::getGenerator());
}

arma::uword getDifferentDiscreteRandomNumber(
    const arma::uword discreteRandomNumber) {
  arma::uword randomNumber = std::uniform_int_distribution<arma::uword>(1, 9)(mant::Rng::getGenerator());
  if (randomNumber >= discreteRandomNumber) {
    ++randomNumber;
  }

  return randomNumber;
}

arma::Mat<double> SYNCHRONISED(
    const arma::Mat<double>& data) {
#if defined(SUPPORT_MPI)
  arma::Mat<double> synchronisedData = data;
  MPI_Bcast(synchronisedData.memptr(), static_cast<int>(synchronisedData.n_elem), MPI_DOUBLE, 0, MPI_COMM_WORLD);
  return synchronisedData;
#else
  return data;
#endif
}

arma::Mat<arma::uword> SYNCHRONISED(
    const arma::Mat<arma::uword>& data) {
#if defined(SUPPORT_MPI)
  arma::Mat<unsigned int> synchronisedData = arma::conv_to<arma::Mat<unsigned int>>::from(data);
  MPI_Bcast(synchronisedData.memptr(), static_cast<int>(synchronisedData.n_elem), MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  return arma::conv_to<arma::Mat<arma::uword>>::from(synchronisedData);
#else
  return data;
#endif
}

arma::Col<double> SYNCHRONISED(
    const arma::Col<double>& data) {
#if defined(SUPPORT_MPI)
  arma::Col<double> synchronisedData = data;
  MPI_Bcast(synchronisedData.memptr(), static_cast<int>(synchronisedData.n_elem), MPI_DOUBLE, 0, MPI_COMM_WORLD);
  return synchronisedData;
#else
  return data;
#endif
}

arma::Col<arma::uword> SYNCHRONISED(
    const arma::Col<arma::uword>& data) {
#if defined(SUPPORT_MPI)
  arma::Col<unsigned int> synchronisedData = arma::conv_to<arma::Col<unsigned int>>::from(data);
  MPI_Bcast(synchronisedData.memptr(), static_cast<int>(synchronisedData.n_elem), MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  return arma::conv_to<arma::Col<arma::uword>>::from(synchronisedData);
#else
  return data;
#endif
}

arma::Row<double> SYNCHRONISED(
    const arma::Row<double>& data) {
#if defined(SUPPORT_MPI)
  arma::Row<double> synchronisedData = data;
  MPI_Bcast(synchronisedData.memptr(), static_cast<int>(synchronisedData.n_elem), MPI_DOUBLE, 0, MPI_COMM_WORLD);
  return synchronisedData;
#else
  return data;
#endif
}

arma::Row<arma::uword> SYNCHRONISED(
    const arma::Row<arma::uword>& data) {
#if defined(SUPPORT_MPI)
  arma::Row<unsigned int> synchronisedData = arma::conv_to<arma::Row<unsigned int>>::from(data);
  MPI_Bcast(synchronisedData.memptr(), static_cast<int>(synchronisedData.n_elem), MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  return arma::conv_to<arma::Row<arma::uword>>::from(synchronisedData);
#else
  return data;
#endif
}

double SYNCHRONISED(
    const double data) {
#if defined(SUPPORT_MPI)
  double synchronisedData = data;
  MPI_Bcast(&synchronisedData, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  return synchronisedData;
#else
  return data;
#endif
}

arma::uword SYNCHRONISED(
    const arma::uword data) {
#if defined(SUPPORT_MPI)
  unsigned int synchronisedData = static_cast<unsigned int>(data);
  MPI_Bcast(&synchronisedData, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  return static_cast<arma::uword>(synchronisedData);
#else
  return data;
#endif
}

std::chrono::microseconds SYNCHRONISED(
    const std::chrono::microseconds data) {
#if defined(SUPPORT_MPI)
  unsigned int synchronisedData = static_cast<unsigned int>(data.count());
  MPI_Bcast(&synchronisedData, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  return std::chrono::microseconds(synchronisedData);
#else
  return data;
#endif
}

void HAS_SAME_SAMPLES(
    const std::unordered_map<arma::Col<double>, double, mant::Hash, mant::IsEqual>& actualSamples,
    const std::unordered_map<arma::Col<double>, double, mant::Hash, mant::IsEqual>& expectedSamples) {
  REQUIRE(actualSamples.size() == expectedSamples.size());

  for (const auto& expectedSample : expectedSamples) {
    const auto actualSample = actualSamples.find(expectedSample.first);

    REQUIRE(actualSample != actualSamples.cend());
    REQUIRE(actualSample->second == Approx(expectedSample.second));
  }
}

void HAS_SAME_PARAMETERS(
    const std::vector<arma::Col<double>>& actualParameters,
    const std::vector<arma::Col<double>>& expectedParameters) {
  REQUIRE(actualParameters.size() == expectedParameters.size());

  for (const auto& expectedParameter : expectedParameters) {
    bool found = false;
    for (const auto& actualParameter : actualParameters) {
      if (expectedParameter.n_elem != actualParameter.n_elem) {
        continue;
      }

      if (arma::all(expectedParameter == actualParameter)) {
        found = true;
        break;
      }
    }

    REQUIRE(found == true);
  }
}

void HAS_SAME_PARAMETERS(
    const std::vector<arma::Col<arma::uword>>& actualParameters,
    const std::vector<arma::Col<arma::uword>>& expectedParameters) {
  REQUIRE(actualParameters.size() == expectedParameters.size());

  for (const auto& expectedParameter : expectedParameters) {
    bool found = false;
    for (const auto& actualParameter : actualParameters) {
      if (expectedParameter.n_elem != actualParameter.n_elem) {
        continue;
      }

      if (arma::all(expectedParameter == actualParameter)) {
        found = true;
        break;
      }
    }

    REQUIRE(found == true);
  }
}

void HAS_SAME_PARAMETERS(
    const std::vector<std::pair<arma::Col<double>, double>>& actualSamples,
    const std::vector<arma::Col<double>>& expectedParameters) {
  REQUIRE(actualSamples.size() == expectedParameters.size());

  for (const auto& expectedParameter : expectedParameters) {
    bool found = false;
    for (const auto& actualSample : actualSamples) {
      if (expectedParameter.n_elem != actualSample.first.n_elem) {
        continue;
      }

      if (arma::all(arma::abs(expectedParameter - actualSample.first) < 1e-12 * arma::max(arma::ones(arma::size(expectedParameter)), arma::abs(expectedParameter)))) {
        found = true;
        break;
      }
    }

    REQUIRE(found == true);
  }
}

void HAS_SAME_ELEMENTS(
    const arma::Col<double>& actualElements,
    const arma::Col<double>& expectedElements) {
  REQUIRE(actualElements.n_elem == expectedElements.n_elem);

  for (const auto& expectedElement : expectedElements) {
    bool found = false;
    for (const auto& actualElement : actualElements) {
      if (std::abs(expectedElement - actualElement) < 1e-12 * std::max(1.0, std::abs(expectedElement))) {
        found = true;
        break;
      }
    }

    REQUIRE(found == true);
  }
}

void IS_EQUAL(
    const std::vector<std::pair<arma::Col<double>, double>>& actual,
    const std::vector<std::pair<arma::Col<double>, double>>& expected) {
  REQUIRE(actual.size() == expected.size());

  for (arma::uword n = 0; n < actual.size(); ++n) {
    const std::pair<arma::Col<double>, double> actualSample = actual.at(n);
    const std::pair<arma::Col<double>, double> expectedSample = actual.at(n);

    IS_EQUAL(actualSample.first, expectedSample.first);
    CHECK(actualSample.second == Approx(expectedSample.second));
  }
}

void IS_EQUAL(
    const std::vector<arma::Col<double>>& actual,
    const std::vector<arma::Col<double>>& expected) {
  REQUIRE(actual.size() == expected.size());

  for (arma::uword n = 0; n < actual.size(); ++n) {
    IS_EQUAL(actual.at(n), expected.at(n));
  }
}

void IS_EQUAL(
    const std::vector<arma::Col<double>::fixed<3>>& actual,
    const std::vector<arma::Col<double>>& expected) {
  REQUIRE(actual.size() == expected.size());

  for (arma::uword n = 0; n < actual.size(); ++n) {
    IS_EQUAL(actual.at(n), expected.at(n));
  }
}

void IS_EQUAL(
    const std::vector<arma::Col<double>::fixed<2>>& actual,
    const std::vector<arma::Col<double>>& expected) {
  REQUIRE(actual.size() == expected.size());

  for (arma::uword n = 0; n < actual.size(); ++n) {
    IS_EQUAL(actual.at(n), expected.at(n));
  }
}

void IS_EQUAL(
    const std::vector<arma::Col<arma::uword>>& actual,
    const std::vector<arma::Col<arma::uword>>& expected) {
  REQUIRE(actual.size() == expected.size());

  for (arma::uword n = 0; n < actual.size(); ++n) {
    IS_EQUAL(actual.at(n), expected.at(n));
  }
}

void IS_EQUAL(
    const arma::Cube<double>& actual,
    const arma::Cube<double>& expected) {
  REQUIRE(actual.n_rows == expected.n_rows);
  REQUIRE(actual.n_cols == expected.n_cols);
  REQUIRE(actual.n_slices == expected.n_slices);

  for (arma::uword n = 0; n < expected.n_elem; ++n) {
    REQUIRE(std::isfinite(actual(n)));
    REQUIRE(actual(n) == Approx(expected(n)));
  }
}

void IS_EQUAL(
    const arma::Cube<arma::uword>& actual,
    const arma::Cube<arma::uword>& expected) {
  REQUIRE(actual.n_rows == expected.n_rows);
  REQUIRE(actual.n_cols == expected.n_cols);
  REQUIRE(actual.n_slices == expected.n_slices);

  for (arma::uword n = 0; n < expected.n_elem; ++n) {
    REQUIRE(actual(n) == expected(n));
  }
}

void IS_EQUAL(
    const arma::Mat<double>& actual,
    const arma::Mat<double>& expected) {
  REQUIRE(actual.n_rows == expected.n_rows);
  REQUIRE(actual.n_cols == expected.n_cols);

  for (arma::uword n = 0; n < expected.n_elem; ++n) {
    REQUIRE(std::isfinite(actual(n)));
    REQUIRE(actual(n) == Approx(expected(n)));
  }
}

void IS_EQUAL(
    const arma::Mat<arma::uword>& actual,
    const arma::Mat<arma::uword>& expected) {
  REQUIRE(actual.n_rows == expected.n_rows);
  REQUIRE(actual.n_cols == expected.n_cols);

  for (arma::uword n = 0; n < expected.n_elem; ++n) {
    REQUIRE(actual(n) == expected(n));
  }
}

void IS_EQUAL(
    const arma::Col<double>& actual,
    const arma::Col<double>& expected) {
  REQUIRE(actual.n_elem == expected.n_elem);

  for (arma::uword n = 0; n < expected.n_elem; ++n) {
    REQUIRE(std::isfinite(actual(n)));
    REQUIRE(actual(n) == Approx(expected(n)));
  }
}

void IS_EQUAL(
    const arma::Col<arma::uword>& actual,
    const arma::Col<arma::uword>& expected) {
  REQUIRE(actual.n_elem == expected.n_elem);

  for (arma::uword n = 0; n < expected.n_elem; ++n) {
    REQUIRE(actual(n) == expected(n));
  }
}

void IS_EQUAL(
    const arma::Row<double>& actual,
    const arma::Row<double>& expected) {
  REQUIRE(actual.n_elem == expected.n_elem);

  for (arma::uword n = 0; n < expected.n_elem; ++n) {
    REQUIRE(std::isfinite(actual(n)));
    REQUIRE(actual(n) == Approx(expected(n)));
  }
}

void IS_EQUAL(
    const arma::Row<arma::uword>& actual,
    const arma::Row<arma::uword>& expected) {
  REQUIRE(actual.n_elem == expected.n_elem);

  for (arma::uword n = 0; n < expected.n_elem; ++n) {
    REQUIRE(actual(n) == expected(n));
  }
}

void IS_UNIFORM(
    const arma::Col<double>& actualData,
    const double expectedLowerBound,
    const double expectedUpperBound) {
  REQUIRE(arma::all(expectedLowerBound <= actualData));
  REQUIRE(arma::all(actualData <= expectedUpperBound));

  const arma::uword numberOfBins = 10;
  CAPTURE(numberOfBins);

  const double binDistance = (expectedUpperBound - expectedLowerBound) / numberOfBins;
  CAPTURE(binDistance);

  const arma::Col<double>& bins = arma::linspace<arma::Col<double>>(expectedLowerBound, expectedUpperBound - binDistance, numberOfBins) + binDistance / 2;
  CAPTURE(bins);

  const arma::Col<arma::uword>& histogram = arma::hist(actualData, bins);
  CAPTURE(histogram);

  REQUIRE((histogram.max() - histogram.min()) < static_cast<arma::uword>(0.05 * static_cast<double>(actualData.n_elem)));
}

void IS_UNIFORM(
    const arma::Col<arma::uword>& actualData,
    const arma::uword expectedLowerBound,
    const arma::uword expectedUpperBound) {
  REQUIRE(arma::all(expectedLowerBound <= actualData));
  REQUIRE(arma::all(actualData <= expectedUpperBound));

  const arma::Col<arma::uword>& bins = arma::linspace<arma::Col<arma::uword>>(expectedLowerBound, expectedUpperBound, expectedUpperBound - expectedLowerBound + 1);
  CAPTURE(bins);

  const arma::Col<arma::uword>& histogram = arma::hist(actualData, bins);
  CAPTURE(histogram);

  REQUIRE((histogram.max() - histogram.min()) < static_cast<arma::uword>(0.05 * static_cast<double>(actualData.n_elem)));
}
