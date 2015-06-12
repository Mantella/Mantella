namespace mant {
  namespace bbob {
    template <typename T = double>
    class RosenbrockFunction : public BlackBoxOptimisationBenchmark<T> {
      static_assert(std::is_floating_point<T>::value, "The parameter type T must be a floating point type.");
    
      public:
        explicit RosenbrockFunction(
            const std::size_t numberOfDimensions) noexcept;

        std::string toString() const noexcept override;

      protected:
        const double max_;

        arma::Col<T> localParameterTranslation_;

        double getObjectiveValueImplementation(
            const arma::Col<T>& parameter) const noexcept override;
        
#if defined(MANTELLA_USE_MPI)
        // Grants direct access to the otherwise hidden .serialise() and .deserialise(...) methods.
        friend class OptimisationAlgorithm;

        // The type is intentionally fixed to ease usage with MPI_DOUBLE.
        std::vector<double> serialise() const noexcept;

        // The type is intentionally fixed to ease usage with MPI_DOUBLE.
        void deserialise(
            const std::vector<double>& serialisedOptimisationProblem);
#endif
    };

    //
    // Implementation
    //

    template <typename T>
    RosenbrockFunction<T>::RosenbrockFunction(
        const std::size_t numberOfDimensions) noexcept
      : BlackBoxOptimisationBenchmark<T>(numberOfDimensions),
        max_(std::max(1.0, std::sqrt(static_cast<T>(this->numberOfDimensions_)) / 8.0)) {
      this->setParameterTranslation(static_cast<T>(0.75L) * this->getRandomParameterTranslation());
    }

    template <typename T>
    double RosenbrockFunction<T>::getObjectiveValueImplementation(
        const arma::Col<T>& parameter) const noexcept {
      const arma::Col<T>& z = max_ * parameter + static_cast<T>(1.0L);

      return 100.0 * std::pow(static_cast<double>(arma::norm(arma::square(z.head(z.n_elem - 1)) - z.tail(z.n_elem - 1))), 2.0) + std::pow(static_cast<double>(arma::norm(z.head(z.n_elem - 1) - static_cast<T>(1.0L))), 2.0);
    }

    template <typename T>
    std::string RosenbrockFunction<T>::toString() const noexcept {
      return "bbob_rosenbrock_function";
    }
    
#if defined(MANTELLA_USE_MPI)
    template <typename T>
    std::vector<double> RosenbrockFunction<T>::serialise() const noexcept {
      return BlackBoxOptimisationBenchmark<T, T>::serialise();
    }

    template <typename T>
    void RosenbrockFunction<T>::deserialise(
        const std::vector<double>& serialisedOptimisationProblem) {
      BlackBoxOptimisationBenchmark<T, T>::deserialise(serialisedOptimisationProblem);
    }
#endif
  }
}