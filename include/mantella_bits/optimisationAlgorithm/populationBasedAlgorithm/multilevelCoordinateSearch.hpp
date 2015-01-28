namespace mant {

  template<class DistanceFunction>
  class MultilevelCoordinateSearch : public PopulationBasedAlgorithm<double, DistanceFunction> {
  public:
    //lower boundaries are expected in the first col of "boundaries", upper boundaries in the second col of "boundaries.
    //initialPointIndex is the index inside initialPopulation_ which is used as the starting point.
    explicit MultilevelCoordinateSearch(const std::shared_ptr<OptimisationProblem<double>> optimisationProblem, const unsigned int& populationSize, arma::Col<arma::uword> initialPointIndex, unsigned int boxDivisions = 0, unsigned int maxLocalSearchSteps = 50) noexcept;
    MultilevelCoordinateSearch(const MultilevelCoordinateSearch&) = delete;
    MultilevelCoordinateSearch& operator=(const MultilevelCoordinateSearch&) = delete;

    void setLocalSearch(const std::shared_ptr<OptimisationAlgorithm<double, DistanceFunction>> localSearch) noexcept;

    std::string to_string() const noexcept override;
  protected:
    void optimiseImplementation() override;

    std::shared_ptr<OptimisationAlgorithm<double, DistanceFunction>> localSearch_;

    unsigned int step1_ = 10000;
    unsigned int step = 1000; //TODO: could be moved to constructor as a definable variable
    unsigned int dim = 10000;

    unsigned int boxDivisions_; //smax
    arma::Mat<double> boundaries_; //u,v - with u=col(0) and v=col(1)
    unsigned int maxLocalSearchSteps_; //local

    arma::Col<arma::uword> isplit_;
    arma::Col<arma::uword> level_;
    arma::Col<arma::uword> ipar_;
    arma::Col<arma::uword> ichild_;
    arma::Mat<double> boxBaseVertexFunctionValues_; //f
    arma::Mat<double> z_;
    arma::Col<arma::uword> nogain_;

    arma::Col<arma::uword> initialPointIndex_; //l
    arma::Mat<double> x0_;
    arma::Mat<double> initListEvaluations_; //f0
    arma::Col<arma::uword> bestPointIndex_; //istar

    arma::Col<double> baseVertex_; //x
    arma::Col<double> oppositeVertex_; //v1, sometimes called y in subfunctions

    arma::Col<arma::uword> record_;
    unsigned int nboxes_ = 1;
    unsigned int nbasket_ = 0;
    unsigned int nbasket0_ = 0;
    unsigned int m_;
    unsigned int nloc_ = 0;
    arma::Mat<double> xloc_;

    void initBoxes(); //initbox.m
    arma::Col<arma::uword> variabilityRanking_; //p

    arma::Mat<double> pointsInBasket_; //xmin
    arma::Col<double> pointsInBasketValue_; //fmi

    //helper methods
    void genBox(int nbox, int par, int level, int nchild, double baseVertexFunctionValue); //genbox.m

    arma::Col<double> quadraticPolynomialInterpolation(arma::Col<double> supportPoints, arma::Col<double> functionValues); //polint.m

    double minimumQuadraticPolynomial(double a, double b, arma::Col<double> d, arma::Mat<double> x0_fragment); //quadmin.m

    double quadraticPolynomial(double x, arma::Col<double> d, arma::Mat<double> x0_fragment); //quadpol.m

    //split1.m 
    //splits the interval [x1,x2] according to the golden section rule
    double splitByGoldenSectionRule(double x1, double x2, double f1, double f2);

    //splrnk.m 
    //determines the splitting index and splitting value for splitting a box by rank
    void splitByRank(unsigned int par, unsigned int numberOfDimensions, arma::Col<arma::uword> n0);

    //split2.m 
    //determines a value x1 for splitting the interval [min(x,y),max(x,y)] 
    //is modeled on the function subint with safeguards for infinite y
    double splitBySubint(double x, double y);

    //splinit.m 
    //splits box # par at level s according to the initialization list in the 
    //ith coordinate and inserts its children and their parameters in the list.
    //returns isFinished() || isTerminated()
    bool splitByInitList(unsigned int splittingIndex, unsigned int minimalLevel, unsigned int par, arma::Col<arma::uword> n0, arma::Col<double> x1, arma::Col<double> x2, arma::Mat<double> pointsInBasket, arma::Col<double> pointsInBasketValue);

    //split.m 
    //splits box # par at level s in its ith coordinate into two or three 
    //children and inserts its children and their parameters in the list
    //returns isFinished() || isTerminated()
    bool split(unsigned int splittingIndex, unsigned int minimalLevel, unsigned int par, arma::Col<arma::uword> n0, arma::Col<double> x1, arma::Col<double> x2, arma::Mat<double> pointsInBasket, arma::Col<double> pointsInBasketValue);

    //exgain.m 
    //determines the splitting index, the splitting value and the expected 
    //gain vector e for (potentially) splitting a box by expected gain
    arma::Col<double> expectedGainOfSplit(unsigned int par, unsigned int numberOfDimensions, arma::Col<arma::uword> n0, arma::Col<double> x1, arma::Col<double> x2, arma::Col<double> f1, arma::Col<double> f2);

    unsigned int startSweep(); //strtsw.m

    //vertex.m 
    void vertex(unsigned int par, arma::Col<double> x1, arma::Col<double> x2, arma::Col<double> f1, arma::Col<double> f2, arma::Col<arma::uword> n0);

    void vert1(int updateIndex, unsigned int j, unsigned int m, arma::Col<double> x, arma::Col<double> x1, arma::Col<double> x2, arma::Col<double> f1, arma::Col<double> f2); //vert1.m

    void vert2(int updateIndex, unsigned int j, unsigned int m, arma::Col<double> x, arma::Col<double> x1, arma::Col<double> x2, arma::Col<double> f1, arma::Col<double> f2); //vert2.m

    void vert3(int updateIndex, unsigned int j, unsigned int m, unsigned int f0Index, arma::Col<double> x1, arma::Col<double> x2, arma::Col<double> f1, arma::Col<double> f2); //vert3.m

    void updtf(unsigned int numberOfDimensions, unsigned int splittingIndex, double fold, arma::Col<double> x1, arma::Col<double> x2, arma::Col<double> f1, arma::Col<double> f2, double baseVertexValueCurrentBox); //updtf.m

    //subint.m 
    //computes for real x and real or infinite y two points x1 and x2 in 
    //[min(x,y),max(x,y)] that are neither too close nor too far away from x
    arma::Col<double> subint(double x, double y);

    //updtrec.m 
    //updates the pointer record(s) to the best non-split box at level s.
    //f=vector containing the base vertex function values of the already defined boxes
    void updateRecord(unsigned int label, int level, arma::Col<double> f);

    //chkloc.m 
    //checks whether a point has already been used as starting point for a local search
    bool checkLocationNotUsed(arma::Col<double> location);

    //addloc.m 
    //adds a new point to the list of starting points for local search
    void addLocation(arma::Col<double> loc);

    //basket.m 
    //checks whether a candidate for local search lies in the 'domain of 
    //attraction' of a point in the 'shopping basket'
    bool pointInsideDomainOfAttraction(arma::Col<double> loc, std::shared_ptr<double> valueAtLoc, double nbasket);

    //basket1.m 
    //checks whether a candidate for local search lies in the 'domain of 
    //attraction' of a point in the 'shopping basket'
    bool candidateInsideDomainOfAttraction(arma::Col<double> candidate, double valueAtCandidate, double nbasket);
  };

  //
  // Implementation
  //
  // <editor-fold>

  template<class DistanceFunction>
  MultilevelCoordinateSearch<DistanceFunction>::MultilevelCoordinateSearch(const std::shared_ptr<OptimisationProblem<double>> optimisationProblem,
      const unsigned int& populationSize, arma::Col<arma::uword> initialPointIndex, unsigned int boxDivisions, unsigned int maxLocalSearchSteps) noexcept
  : PopulationBasedAlgorithm<double, DistanceFunction>(optimisationProblem, populationSize), boxDivisions_(boxDivisions),
  maxLocalSearchSteps_(maxLocalSearchSteps), initialPointIndex_(initialPointIndex) {
    std::cout << "starting constructor" << std::endl;
    //for convenience
    unsigned int numberOfDimensions = optimisationProblem->getNumberOfDimensions();

    std::shared_ptr<OptimisationAlgorithm<double, DistanceFunction >> test(new HillClimbing<double, DistanceFunction>(optimisationProblem));

    setLocalSearch(test);

    std::cout << "assigning boundary" << std::endl;

    boundaries_ = arma::Mat<double>(numberOfDimensions, 2);
    boundaries_.col(0) = optimisationProblem->getLowerBounds();
    boundaries_.col(1) = optimisationProblem->getUpperBounds();

    //assigning standard values for variables. Can't do in header-file since dependent on input variable "boundaries"
    if (boxDivisions_ == 0) {
      boxDivisions_ = 50 * boundaries_.col(0).n_elem + 10;
    }

    //length of u and v should equal numberOfDimensions
    if (numberOfDimensions != boundaries_.col(0).n_elem) {
      std::cout << "lower boundaries dimensions don't match!" << std::endl;
    }
    if (numberOfDimensions != boundaries_.col(1).n_elem) {
      std::cout << "upper boundaries dimensions don't match!" << std::endl;
    }
    //Check if bounds are malformed
    for (std::size_t i = 0; i < numberOfDimensions; i++) {
      if (boundaries_(i, 0) >= boundaries_(i, 1)) {
        std::cout << "boundary malformed! u: " << boundaries_(i, 0) << ", v:" << boundaries_(i, 1) << std::endl;
      }
    }

    std::cout << "initting large arrays" << std::endl;
    //init of large arrays
    //TODO: type completely unclear right now, pdf page 6f
    isplit_ = arma::Col<arma::uword>(step1_, arma::fill::zeros);
    level_ = arma::Col<arma::uword>(step1_, arma::fill::zeros);
    ipar_ = arma::Col<arma::uword>(step1_, arma::fill::zeros);
    ichild_ = arma::Col<arma::uword>(step1_, arma::fill::zeros);
    boxBaseVertexFunctionValues_ = arma::Mat<double>(2, step1_, arma::fill::zeros);
    z_ = arma::Mat<double>(2, step1_, arma::fill::zeros);
    nogain_ = arma::Col<arma::uword>(step1_, arma::fill::zeros);

    //initialization list
    //this is the equivalent of iinit = 0 in matlab
    //TODO: this is basically initialpopulation
    x0_ = arma::Mat<double>(numberOfDimensions, 3);
    x0_.col(0) = boundaries_.col(0);
    x0_.col(1) = (boundaries_.col(0) + boundaries_.col(1)) / 2.0;
    x0_.col(2) = boundaries_.col(1);
    initListEvaluations_ = arma::Mat<double>(this->populationSize_, numberOfDimensions);
    bestPointIndex_ = arma::Col<arma::uword>(numberOfDimensions);
    variabilityRanking_ = arma::Col<arma::uword>(numberOfDimensions);

    //TODO: for custom initialisation lists there is a check here to see if they violate the boundaries


    //l_ L and x0_ are the custom initialisation list variables
    //l_ is supposed to point to the initial point x^0 in x0_ 
    //l_ also never gets changed in matlab as far as i could see
    //L gives the amount of predefined values per dimension (basically this->populationSize_ with more finetuning possible)
    //TODO: mcs.m does infinity check on x0_ here, not sure if needed

    arma::Col<double> initialPoint(numberOfDimensions, arma::fill::zeros);
    for (std::size_t i = 0; i < numberOfDimensions; i++) {
      initialPoint(i) = x0_(i, initialPointIndex_(i));
    }

    std::cout << "setting up bestparameter" << std::endl;

    this->bestObjectiveValue_ = this->optimisationProblem_->getObjectiveValue(initialPoint);
    this->bestParameter_ = initialPoint;
    initListEvaluations_(0, initialPointIndex_(0)) = this->bestObjectiveValue_;

    std::cout << "starting work on initpointindex" << std::endl;

    for (std::size_t r = 0; r < numberOfDimensions; r++) {
      std::cout << "r = " << r << std::endl;
      bestPointIndex_(r) = initialPointIndex_(r);
      for (std::size_t c = 0; c < this->populationSize_; c++) {
        std::cout << "c = " << c << std::endl;
        if (c == initialPointIndex_(r)) {
          std::cout << "first if taken" << std::endl;
          if (r != 0) {
            initListEvaluations_(c, r) = initListEvaluations_(bestPointIndex_(r - 1), r - 1);
          }
        } else {
          std::cout << "first if not taken" << std::endl;
          initialPoint(r) = x0_(r, c);
          initListEvaluations_(c, r) = this->optimisationProblem_->getObjectiveValue(initialPoint);
          this->numberOfIterations_++;
          if (initListEvaluations_(c, r) < this->bestObjectiveValue_) {
            std::cout << "inner if, of not taken, taken" << std::endl;
            this->bestObjectiveValue_ = initListEvaluations_(c, r);
            this->bestParameter_ = initialPoint;
            bestPointIndex_(r) = c;
          }
        }
      }
      initialPoint(r) = x0_(r, bestPointIndex_(r));
    }
    //in init.m all operations are done transposed (reason unknown yet), we simply do it at the end
    //TODO: About halfway through the code this still seems to make no sense
    //initListEvaluations_ = initListEvaluations_.t();

    std::cout << "initting base and opposite vertex" << std::endl;
    //base vertex and opposite vertex init
        //TODO: check if this->populationSize_ comparisons need to be '-1'd
    baseVertex_ = arma::Col<double>(numberOfDimensions);
    oppositeVertex_ = arma::Col<double>(numberOfDimensions);
    for (std::size_t i = 0; i < numberOfDimensions; i++) {
      baseVertex_(i) = x0_(i, initialPointIndex_(i));

      //if true, use u, else use v
      if (std::abs(baseVertex_(i) - boundaries_.col(0)(i)) > std::abs(baseVertex_(i) - boundaries_.col(1)(i))) {
        oppositeVertex_(i) = boundaries_.col(0)(i);
      } else {
        oppositeVertex_(i) = boundaries_.col(1)(i);
      }
    }

    //init of record list, nboxes, nbasket,nbasket0, m, nloc, xloc
    //"flag" is not needed since it is equal to optProblem->isFinished()
    //values not listed here are defined in header
    //nsweep not needed also
    record_ = arma::Col<arma::uword>(boxDivisions_ - 1, arma::fill::zeros);
    m_ = numberOfDimensions;
    record_(0) = 1;
    xloc_ = arma::Mat<double>(numberOfDimensions, step1_);

    //generate boxes
    std::cout << "initting boxes" << std::endl;
    initBoxes();
  }

  template<class DistanceFunction>
  void MultilevelCoordinateSearch<DistanceFunction>::optimiseImplementation() {
    //for convenience
    unsigned int numberOfDimensions = this->optimisationProblem_->getNumberOfDimensions();

    //TODO: find better value than step1_...
    pointsInBasket_ = arma::Mat<double>(numberOfDimensions, step1_); //xmin
    pointsInBasketValue_ = arma::Col<double>(step1_); //fmi

    // s- the vector record is updated, and the minimal level s containing non-split boxes is computed
    unsigned int minimalLevel = startSweep();

    //TODO: mcs.m checks for "minimalLevel < boxDivisions" as a while condition.
    //Which makes sense, since we cannot calculate anything if we cannot divide further.
    while (!this->isFinished() && !this->isTerminated() && minimalLevel < boxDivisions_) {
      unsigned int par = record_(minimalLevel); //the best box at level s is the current box

      arma::Col<double> x1 = arma::Col<double>(numberOfDimensions);
      x1.fill(arma::datum::inf);
      arma::Col<double> x2 = arma::Col<double>(numberOfDimensions);
      x2.fill(arma::datum::inf);
      arma::Col<double> f1 = arma::Col<double>(numberOfDimensions, arma::fill::zeros);
      arma::Col<double> f2 = arma::Col<double>(numberOfDimensions, arma::fill::zeros);
      arma::Col<arma::uword> n0 = arma::Col<arma::uword>(numberOfDimensions, arma::fill::zeros);
      vertex(par, x1, x2, f1, f2, n0);

      bool doSplit = false; //splt
      if (minimalLevel > 2 * numberOfDimensions * (arma::min(n0) + 1)) {
        splitByRank(par, numberOfDimensions, n0);
        doSplit = true;
      } else {
        //TODO: this if should be unnecessary in c++. else be !if
        if (nogain_(par)) {
          doSplit = false;
        } else {
          arma::Col<double> expectedGain = expectedGainOfSplit(par, numberOfDimensions, n0, x1, x2, f1, f2); //e
          //index again so use 0, matlab=1
          double fexp = boxBaseVertexFunctionValues_(0, par) + arma::min(expectedGain);
          if (fexp < this->bestObjectiveValue_) {
            doSplit = true;
          } else {
            doSplit = false;
            nogain_(par) = 1;
          }
        }
      }
      if (doSplit) {
        int i = isplit_(par);
        level_(par) = 0;
        //index again so use 1, matlab=2
        if (z_(1, par) == arma::datum::inf) {
          m_++;
          //index again so use 1, matlab=2
          z_(1, par) = m_;
          //little different than matlab, if this returns true = isFinished() | isTerminated()
          if (splitByInitList(i, minimalLevel, par, n0, x1, x2, pointsInBasket_, pointsInBasketValue_)) {
            break; //should break out of major while loop
          }
        } else {
          //index again so use 0, matlab=1
          z_(0, par) = baseVertex_(i);
          //little different than matlab, if this returns true = isFinished() | isTerminated()
          if (split(i, minimalLevel, par, n0, x1, x2, pointsInBasket_, pointsInBasketValue_)) {
            break;
          }
        }
        //if the pre-assigned size of the `large' arrays has already been exceeded, these arrays are made larger
        if (nboxes_ > dim) {
          //TODO: are the additional elements automatically set to zero? if not, need to do that
          isplit_.resize(nboxes_ + step);
          level_.resize(nboxes_ + step);
          ipar_.resize(nboxes_ + step);
          ichild_.resize(nboxes_ + step);
          z_.resize(z_.n_rows, nboxes_ + step);
          nogain_.resize(nboxes_ + step);
          boxBaseVertexFunctionValues_.resize(z_.n_rows, nboxes_ + step);
          dim = nboxes_ + step;
        }
        if (this->isFinished() || this->isTerminated()) {
          break;
        }
      } else {//no splitting, increase the level by 1
        if (minimalLevel + 1 < boxDivisions_) {
          level_(par) = minimalLevel + 1;
          //index again so use 0, matlab=1
          updateRecord(par, minimalLevel + 1, boxBaseVertexFunctionValues_.row(0));
        } else {
          level_(par) = 0;
          nbasket_++;
          pointsInBasket_.col(nbasket_) = baseVertex_;
          //index again so use 0, matlab=1
          pointsInBasketValue_(nbasket_) = boxBaseVertexFunctionValues_(0, par);
        }
      }
      //of prepare for splitting
      minimalLevel++;
      while (minimalLevel < boxDivisions_) {
        if (record_(minimalLevel) == 0) {
          minimalLevel++;
        } else {
          break;
        }
      }
      //if smax is reached, a new sweep is started 
      if (minimalLevel == boxDivisions_) {
        if (maxLocalSearchSteps_ > 0) {
          //original matlab sort: [fmi(nbasket0+1:nbasket),j] = sort(fmi(nbasket0+1:nbasket));
          //the j is never used, so we don't retrieve the sort-index
          pointsInBasketValue_.rows(nbasket0_ + 1, nbasket_) = arma::sort(pointsInBasketValue_.rows(nbasket0_ + 1, nbasket_));

          for (int j = nbasket0_ + 1; j < nbasket_; j++) {
            //programmatically it seems dumb to overwrite a global variable, but they do it in matlab.
            baseVertex_ = pointsInBasket_.col(j);

            if (checkLocationNotUsed(baseVertex_)) {
              //unfortunately multiple return values of primitives are needed later, so we have to use pointers
              //TODO: Something is strange/wrong here.
              //f1 is already a vector earlier. Why is he overwriting it in matlab??
              std::shared_ptr<double> f1(new double(pointsInBasketValue_(j)));

              addLocation(baseVertex_);

              bool isInside = pointInsideDomainOfAttraction(baseVertex_, f1, nbasket0_);
              if (this->isFinished() || this->isTerminated()) {
                return;
              }
              if (isInside) {
                ///////////////////////////TODO: TEMPORARY WORKAROUND FOR LSEARCH///////////////////////////
                arma::Col<double> xminTemp(numberOfDimensions, arma::fill::zeros);
                double fmiTemp = 10;

                if (fmiTemp < this->bestObjectiveValue_) {
                  this->bestObjectiveValue_ = fmiTemp;
                  this->bestParameter_ = xminTemp;
                  if (this->isFinished() || this->isTerminated()) {
                    nbasket0_++;
                    nbasket_ = nbasket0_;
                    pointsInBasket_.col(nbasket_) = xminTemp;
                    pointsInBasketValue_(nbasket_) = fmiTemp;
                    return;
                  }
                }
                bool candidateInside = candidateInsideDomainOfAttraction(xminTemp, fmiTemp, nbasket0_);
                if (candidateInside) {
                  nbasket0_++;
                  pointsInBasket_.col(nbasket0_) = xminTemp;
                  pointsInBasketValue_(nbasket0_) = fmiTemp;
                  //TODO: this check seems redundant, should check if it ever gets called
                  if (pointsInBasketValue_(nbasket0_) < this->bestObjectiveValue_) {
                    this->bestObjectiveValue_ = pointsInBasketValue_(nbasket0_);
                    this->bestParameter_ = pointsInBasket_.col(nbasket0_);
                    if (this->isFinished() || this->isTerminated()) {
                      nbasket_ = nbasket0_;
                      return;
                    }
                  }
                }
              }
            }
          }
          nbasket_ = nbasket0_;
          if (this->isFinished() || this->isTerminated()) {
            return;
          }
        }
        minimalLevel = startSweep();
      }
    }
    //TODO: In matlab there is something done that looks like cleanup, i don't think we care about that
    /*
    if local,
      if length(fmi) > nbasket
        xmin(:,nbasket+1:length(fmi)) = [];
        fmi(nbasket+1:length(fmi)) = [];
      end
    end
     * */
  }

  template<class DistanceFunction>
  void MultilevelCoordinateSearch<DistanceFunction>::initBoxes() {
    //for convenience
    unsigned int numberOfDimensions = this->optimisationProblem_->getNumberOfDimensions();

    //parameter values of box 1
    ipar_(0) = 0;
    level_(0) = 1;
    ichild_(0) = 1;
    boxBaseVertexFunctionValues_(0, 0) = initListEvaluations_(initialPointIndex_(0), 0);

    int par = 0;

    arma::Col<double> var = arma::Col<double>(step1_);
    std::cout << "before outer for loop" << std::endl;

    for (std::size_t i = 0; i < numberOfDimensions; i++) {
      std::cout << "i = " << i << std::endl;
      isplit_(par) = -i; //boxes split in the init. procedure get a negative splitting index
      int nchild = 0;
      if (x0_(i, 1) > boundaries_.col(0)(i)) {
        nboxes_++;
        nchild++;
        std::cout << "first genbox" << std::endl;
        std::cout << initListEvaluations_(0, i) << std::endl;
        genBox(nboxes_, par, level_(par) + 1, -nchild, initListEvaluations_(0, i));
      }
      
      //TODO: in matlab this is L(i) instead of populationSize,
      //seems kind of pointless since we don't have dynamic popSize per dimension so this always or never true
      //TODO: also, this might need to be 2 instead of 3
      std::cout << "before v1" << std::endl;
      double v1 = 0;
      if (this->populationSize_ == 3) {
        v1 = boundaries_.col(1)(i);
      } else {
        v1 = x0_(i, 2);
      }
      std::cout << "before first minqp" << std::endl;
      arma::Col<double> d = quadraticPolynomialInterpolation(x0_.row(i).subvec(0,2).t(), initListEvaluations_.col(i).subvec(0,2));
      std::cout << "d set" << std::endl;
      double xl = minimumQuadraticPolynomial(boundaries_.col(0)(i), v1, d, x0_.submat(i, 0, i, 2));
      std::cout << "xl set" << std::endl;
      double fl = quadraticPolynomial(xl, d, x0_.submat(i, 0, i, 2));
      std::cout << "fl set" << std::endl;
      double xu = minimumQuadraticPolynomial(boundaries_.col(0)(i), v1, -d, x0_.submat(i, 0, i, 2));
      std::cout << "xu set" << std::endl;
      double fu = quadraticPolynomial(xu, d, x0_.submat(i, 0, i, 2));
      
      std::cout << "after first minqp" << std::endl;

      int par1 = 0;
      if (bestPointIndex_(i) == 0) {
        if (xl < x0_(i, 0)) {
          par1 = nboxes_; //label of the current box for the next coordinate
        } else {
          par1 = nboxes_ + 1;
        }
      }
      std::cout << "start of inner for loop" << std::endl;
      for (std::size_t j = 0; j < this->populationSize_ - 1; j++) {
        std::cout << "j = " << j << std::endl;
        nboxes_++;
        nchild++;
        int s = 0;
        std::cout << "first if reached" << std::endl;
        if (initListEvaluations_(j, i) <= initListEvaluations_(j + 1, i)) {
          s = 1;
        } else {
          s = 2;
        }
        std::cout << "second genbox" << std::endl;
        genBox(nboxes_, par, level_(par) + s, -nchild, initListEvaluations_(j, i));
        if (j >= 1) {
          std::cout << "inside first if" << std::endl;
          if (bestPointIndex_(i) == j) {
            if (xl <= x0_(i, j)) {
              par1 = nboxes_ - 1;
            } else {
              par1 = nboxes_;
            }
          }
          std::cout << "before second if" << std::endl;
          if (j < this->populationSize_ - 2) {
            
            d = quadraticPolynomialInterpolation(x0_.submat(i, j, i, j + 2), initListEvaluations_.submat(j, i, j + 2, i));
            double u1 = 0;
            if (j < this->populationSize_ - 2) {
              u1 = x0_(i, j + 2);
            } else {
              u1 = boundaries_.col(1)(i);
            }
            std::cout << "before second minqp" << std::endl;
            xl = minimumQuadraticPolynomial(x0_(i, j), u1, d, x0_.submat(i, j, i, j + 2));
            fl = std::min(quadraticPolynomial(xl, d, x0_.submat(i, j, i, j + 2)), fl);
            xu = minimumQuadraticPolynomial(x0_(i, j), u1, -d, x0_.submat(i, j, i, j + 2));
            fu = std::max(quadraticPolynomial(xu, d, x0_.submat(i, j, i, j + 2)), fu);
          }
        }
        std::cout << "after second if" << std::endl;
        nboxes_++;
        nchild++;
        std::cout << "third genbox" << std::endl;
        genBox(nboxes_, par, level_(par) + 3 - s, -nchild, initListEvaluations_(j + 1, i));
      }
      std::cout << "before third if" << std::endl;
      if (x0_(i, this->populationSize_-1) < boundaries_.col(0)(i)) {
        
        nboxes_++;
        nchild++;
        std::cout << "fourth genbox" << std::endl;
        genBox(nboxes_, par, level_(par) + 1, -nchild, initListEvaluations_(this->populationSize_-1, i));
      }
      std::cout << "before fourth if" << std::endl;
      if (bestPointIndex_(i) == this->populationSize_-1) {
        
        if (x0_(i, this->populationSize_-1) < boundaries_.col(0)(i)) {
          if (xl <= x0_(i, this->populationSize_-1)) {
            par1 = nboxes_ - 1;
          } else {
            par1 = nboxes_;
          }
        } else {
          par1 = nboxes_;
        }
      }
      std::cout << "before loop end" << std::endl;
      var(i) = fu - fl; // the quadratic model is taken as a crude measure of the variability in the ith component
      level_(par) = 0; //box is marked as split
      par = par1;
      //TODO: no idea what this splval in this next section is for. it is never used...
    }
    //from matlab: best function value after the init. procedure
    //doesnt make much sense to me since we never changed initListEvaluations
    this->bestObjectiveValue_ = initListEvaluations_(bestPointIndex_(numberOfDimensions-1), numberOfDimensions-1);
    double var0 = 0;
    for (std::size_t i = 0; i < numberOfDimensions; i++) {
      //TODO: next two lines should equal [var0,p(i)] = max(var); not sure if correct
      var0 = arma::max(var);
      variabilityRanking_(i) = var0;
      var(var0) = -1;
      this->bestParameter_(i) = x0_(i, bestPointIndex_(i)); //from matlab: best point after the init. procedure
    }
    std::cout << "initbox done" << std::endl;
  }

  template<class DistanceFunction>
  std::string MultilevelCoordinateSearch<DistanceFunction>::to_string() const noexcept {
    return "MultilevelCoordinateSearch";
  }

  template<class DistanceFunction>
  void MultilevelCoordinateSearch<DistanceFunction>::genBox(int nbox, int par, int level, int nchild, double baseVertexFunctionValue) {
    std::cout << "inside genbox, nbox = " << nbox << std::endl;
    ipar_(nbox) = par;
    level_(nbox) = level;
    //TODO: nchild probably needs to be decremented by 1, or all calls to genBox's nchild
    ichild_(nbox) = nchild;
    std::cout << "genbox cols done" << std::endl;
    boxBaseVertexFunctionValues_(0, nbox) = baseVertexFunctionValue;
    std::cout << "genbox done" << std::endl;
  }

  template<class DistanceFunction>
  arma::Col<double> MultilevelCoordinateSearch<DistanceFunction>::quadraticPolynomialInterpolation(arma::Col<double> supportPoints, arma::Col<double> functionValues) {
    std::cout << "qpi: before init d" << std::endl;
    arma::Col<double> d(3);
    std::cout << "qpi: init d" << std::endl;
    d(0) = functionValues(0);
    d(1) = (functionValues(1) - functionValues(0)) / (supportPoints(1) - supportPoints(0));
    double f23 = (functionValues(2) - functionValues(1)) / (supportPoints(2) - supportPoints(1));
    d(2) = (f23 - d(1)) / (supportPoints(2) - supportPoints(0));
    std::cout << "qpi: return" << std::endl;
    return d;
  }

  template<class DistanceFunction>
  double MultilevelCoordinateSearch<DistanceFunction>::minimumQuadraticPolynomial(double a, double b, arma::Col<double> d, arma::Mat<double> x0_fragment) {
    double x = 0;
    if (d(2) == 0) {
      if (d(1) == 0) {
        x = a;
      } else {
        x = b;
      }
      return x;
    } else if (d(2) > 0) {
      double x1 = 0.5 * (x0_fragment(0)) + x0_fragment(1) - 0.5 * (d(1) / d(2));
      if (a <= x1 && x1 <= b) {
        x = x1;
        return x;
      }
    }
    if (quadraticPolynomial(a, d, x0_fragment) < quadraticPolynomial(b, d, x0_fragment)) {
      x = a;
    } else {
      x = b;
    }
    return x;
  }

  template<class DistanceFunction>
  double MultilevelCoordinateSearch<DistanceFunction>::quadraticPolynomial(double x, arma::Col<double> d, arma::Mat<double> x0_fragment) {
    return d(0) + d(1)*(x - x0_fragment(0)) + d(2)*(x - x0_fragment(0))*(x - x0_fragment(1));
  }

  template<class DistanceFunction>
  double MultilevelCoordinateSearch<DistanceFunction>::splitByGoldenSectionRule(double x1, double x2, double f1, double f2) {
    if (f1 <= f2) {
      return x1 + 0.5 * (-1 + std::sqrt(5))*(x2 - x1);
    } else {
      return x1 + 0.5 * (3 - std::sqrt(5))*(x2 - x1);
    }
  }

  template<class DistanceFunction>
  void MultilevelCoordinateSearch<DistanceFunction>::splitByRank(unsigned int par, unsigned int numberOfDimensions, arma::Col<arma::uword> n0) {
    //index again so use 0, matlab=1 for all 3 variables
    int isplit = 0;
    int n1 = n0(0);
    int p1 = variabilityRanking_(0);
    //matlab starts at 2 obviously
    for (int i = 1; i < numberOfDimensions; i++) {
      if (n0(i) < n1 || (n0(i) == n1 && variabilityRanking_(i) < p1)) {
        isplit = i;
        n1 = n0(i);
        p1 = variabilityRanking_(i);
      }
    }
    if (n1 > 0) {
      //index again so use 1, matlab=2
      z_(1, par) = splitBySubint(baseVertex_(isplit), oppositeVertex_(isplit));
    } else {
      z_(1, par) = arma::datum::inf;
    }
  }

  template<class DistanceFunction>
  double MultilevelCoordinateSearch<DistanceFunction>::splitBySubint(double x, double y) {
    double x2 = y;
    if (x == 0 && std::abs(y) > 1000) {
      x2 = std::copysign(1.0, y);
    } else if (x != 0 && std::abs(y) > 100 * std::abs(x)) {
      //TODO: c++ standardlibraries have no signum. wat. using copysign instead...
      //original matlab: x2 = 10.*sign(y)*abs(x);
      x2 = 10 * std::copysign(x, y);
    }
    return x + 2 * (x2 - x) / 3.0;
  }

  template<class DistanceFunction>
  bool MultilevelCoordinateSearch<DistanceFunction>::splitByInitList(unsigned int splittingIndex, unsigned int minimalLevel, unsigned int par, arma::Col<arma::uword> n0, arma::Col<double> x1, arma::Col<double> x2, arma::Mat<double> pointsInBasket, arma::Col<double> pointsInBasketValue) {
    initListEvaluations_.col(m_).zeros();
    for (int j = 0; j < this->populationSize_; j++) {
      if (j != initialPointIndex_(splittingIndex)) {
        //TODO: why are we writing into the baseVertex? oO
        baseVertex_(splittingIndex) = x0_(splittingIndex, j);
        initListEvaluations_.col(m_)(j) = this->optimisationProblem_->getObjectiveValue(baseVertex_);
        this->numberOfIterations_++;
        if (initListEvaluations_.col(m_)(j) < this->bestObjectiveValue_) {
          this->bestObjectiveValue_ = initListEvaluations_.col(m_)(j);
          this->bestParameter_ = baseVertex_;
        }
        //In matlab this if is a little different and inside the if directly before this.
        //Our stopping conditions are a little different so it's here.
        if (this->isFinished() || this->isTerminated()) {
          return true;
        }
      } else {
        //index again so use 0, matlab=1
        initListEvaluations_.col(m_)(j) = boxBaseVertexFunctionValues_(0, par);
      }
    }

    if (minimalLevel + 1 < boxDivisions_) {
      int nchild = 0;
      //index again so use 0, matlab=1
      if (boundaries_.col(0)(splittingIndex) < x0_(splittingIndex, 0)) {//in that case the box at the boundary gets level s + 1
        nchild++;
        nboxes_++;
        //index again so use 0, matlab=1
        genBox(nboxes_, par, minimalLevel + 1, -nchild, initListEvaluations_.col(m_)(0));
        //index again so use 0, matlab=1
        updateRecord(nboxes_, level_(nboxes_), boxBaseVertexFunctionValues_.row(0));
      }
      for (int j = 0; j < this->populationSize_ - 1; j++) {
        nchild++;
        int level0 = 0;
        if (initListEvaluations_.col(m_)(j) <= initListEvaluations_.col(m_)(j + 1) || minimalLevel + 2 < boxDivisions_) {
          nboxes_++;
          if (initListEvaluations_.col(m_)(j) <= initListEvaluations_.col(m_)(j)) {
            level0 = minimalLevel + 1;
          } else {
            level0 = minimalLevel + 2;
          }
          genBox(nboxes_, par, level0, -nchild, initListEvaluations_.col(m_)(j));
          //index again so use 0, matlab=1
          updateRecord(nboxes_, level_(nboxes_), boxBaseVertexFunctionValues_.row(0));
        } else {
          baseVertex_(splittingIndex) = x0_(splittingIndex, j);
          nbasket_++;
          pointsInBasket.col(nbasket_) = baseVertex_;
          pointsInBasketValue_(nbasket_) = initListEvaluations_.col(m_)(j);
        }
        nchild++;
        if (initListEvaluations_.col(m_)(j + 1) < initListEvaluations_.col(m_)(j) || minimalLevel + 2 < boxDivisions_) {
          nboxes_++;
          if (initListEvaluations_.col(m_)(j + 1) < initListEvaluations_.col(m_)(j)) {
            level0 = minimalLevel + 1;
          } else {
            level0 = minimalLevel + 2;
          }
          genBox(nboxes_, par, level0, -nchild, initListEvaluations_.col(m_)(j + 1));
          //index again so use 0, matlab=1
          updateRecord(nboxes_, level_(nboxes_), boxBaseVertexFunctionValues_.row(0));
        } else {
          baseVertex_(splittingIndex) = x0_(splittingIndex, j + 1);
          nbasket_++;
          pointsInBasket.col(nbasket_) = baseVertex_;
          pointsInBasketValue_(nbasket_) = initListEvaluations_.col(m_)(j + 1);
        }
      }
      if (x0_(splittingIndex, this->populationSize_) < boundaries_.col(1)(splittingIndex)) {//in that case the box at the boundary gets level s + 1
        nchild++;
        nboxes_++;
        genBox(nboxes_, par, minimalLevel + 1, -nchild, initListEvaluations_.col(m_)(this->populationSize_));
        //index again so use 0, matlab=1
        updateRecord(nboxes_, level_(nboxes_), boxBaseVertexFunctionValues_.row(0));
      }
    } else {
      for (int j = 0; j < this->populationSize_; j++) {
        baseVertex_(splittingIndex) = x0_(splittingIndex, j);
        nbasket_++;
        pointsInBasket.col(nbasket_) = baseVertex_;
        pointsInBasketValue_(nbasket_) = initListEvaluations_.col(m_)(j);
      }
    }
    return this->isFinished() || this->isTerminated();
  }

  template<class DistanceFunction>
  bool MultilevelCoordinateSearch<DistanceFunction>::split(unsigned int splittingIndex, unsigned int minimalLevel, unsigned int par, arma::Col<arma::uword> n0, arma::Col<double> x1, arma::Col<double> x2, arma::Mat<double> pointsInBasket, arma::Col<double> pointsInBasketValue) {
    //index again so use 1, matlab=2
    baseVertex_(splittingIndex) = z_(1);
    //index again so use 1, matlab=2
    boxBaseVertexFunctionValues_(1, par) = this->optimisationProblem_->getObjectiveValue(baseVertex_);
    this->numberOfIterations_++;
    //index again so use 1, matlab=2
    if (boxBaseVertexFunctionValues_(1, par) < this->bestObjectiveValue_) {
      //index again so use 1, matlab=2
      this->bestObjectiveValue_ = boxBaseVertexFunctionValues_(1, par);
      this->bestParameter_ = baseVertex_;
      //Our stopping conditions are a little different
      if (this->isFinished() || this->isTerminated()) {
        return true;
      }
    }

    if (minimalLevel + 1 < boxDivisions_) {
      //index again, all decremented by 1
      if (boxBaseVertexFunctionValues_(0, par) < boxBaseVertexFunctionValues_(1, par)) {
        nboxes_++;
        //index again so use 0, matlab=1
        genBox(nboxes_, par, minimalLevel + 1, 1, boxBaseVertexFunctionValues_(0, par));
        //index again so use 0, matlab=1
        updateRecord(nboxes_, level_(nboxes_), boxBaseVertexFunctionValues_.row(0));
        if (minimalLevel + 2 < boxDivisions_) {
          nboxes_++;
          //index again so use 1, matlab=2
          genBox(nboxes_, par, minimalLevel + 2, 2, boxBaseVertexFunctionValues_(1, par));
          //index again so use 0, matlab=1
          updateRecord(nboxes_, level_(nboxes_), boxBaseVertexFunctionValues_.row(0));
        } else {
          //index again so use 1, matlab=2
          baseVertex_(splittingIndex) = z_(par, 1);
          nbasket_ = nbasket_ + 1;
          pointsInBasket.col(nbasket_) = baseVertex_;
          //index again so use 1, matlab=2
          pointsInBasketValue_(nbasket_) = boxBaseVertexFunctionValues_(1, par);
        }

      } else {
        if (minimalLevel + 2 < boxDivisions_) {
          nboxes_++;
          //index again so use 0, matlab=1
          genBox(nboxes_, par, minimalLevel + 2, 1, boxBaseVertexFunctionValues_(0, par));
          //index again so use 0, matlab=1
          updateRecord(nboxes_, level_(nboxes_), boxBaseVertexFunctionValues_.row(0));
        } else {
          //index again so use 0, matlab=1
          baseVertex_(splittingIndex) = z_(par, 0);
          nbasket_ = nbasket_ + 1;
          pointsInBasket.col(nbasket_) = baseVertex_;
          //index again so use 0, matlab=1
          pointsInBasketValue_(nbasket_) = boxBaseVertexFunctionValues_(0, par);
        }
        nboxes_++;
        //index again so use 1, matlab=2
        genBox(nboxes_, par, minimalLevel + 1, 2, boxBaseVertexFunctionValues_(1, par));
        //index again so use 0, matlab=1
        updateRecord(nboxes_, level_(nboxes_), boxBaseVertexFunctionValues_.row(0));
      }

      // if the third box is larger than the smaller of the other two boxes,
      // it gets level s + 1; otherwise it gets level s + 2
      //index again so use 1, matlab=2
      if (z_(par, 1) != oppositeVertex_(splittingIndex)) {
        //index again so use 1, matlab=2
        if (std::abs(z_(par, 1) - oppositeVertex_(splittingIndex)) > std::abs(z_(par, 1))*(3 - std::sqrt(5)*0.5)) {
          nboxes_++;
          //index again so use 1, matlab=2
          genBox(nboxes_, par, minimalLevel + 1, 3, boxBaseVertexFunctionValues_(1, par));
          //index again so use 0, matlab=1
          updateRecord(nboxes_, level_(nboxes_), boxBaseVertexFunctionValues_.row(0));
        } else {
          if (minimalLevel + 2 < boxDivisions_) {
            nboxes_++;
            //index again so use 1, matlab=2
            genBox(nboxes_, par, minimalLevel + 2, 3, boxBaseVertexFunctionValues_(1, par));
            //index again so use 0, matlab=1
            updateRecord(nboxes_, level_(nboxes_), boxBaseVertexFunctionValues_.row(0));
          } else {
            //index again so use 1, matlab=2
            baseVertex_(splittingIndex) = z_(par, 1);
            nbasket_ = nbasket_ + 1;
            pointsInBasket.col(nbasket_) = baseVertex_;
            //index again so use 1, matlab=2
            pointsInBasketValue_(nbasket_) = boxBaseVertexFunctionValues_(1, par);
          }
        }
      }
    } else {
      //index again so use 0, matlab=1
      baseVertex_(splittingIndex) = z_(par, 0);
      nbasket_ = nbasket_ + 1;
      pointsInBasket.col(nbasket_) = baseVertex_;
      //index again so use 0, matlab=1
      pointsInBasketValue_(nbasket_) = boxBaseVertexFunctionValues_(0, par);

      //index again so use 1, matlab=2
      baseVertex_(splittingIndex) = z_(par, 1);
      nbasket_ = nbasket_ + 1;
      pointsInBasket.col(nbasket_) = baseVertex_;
      //index again so use 1, matlab=2
      pointsInBasketValue_(nbasket_) = boxBaseVertexFunctionValues_(1, par);
    }
    return this->isFinished() || this->isTerminated();
  }

  template<class DistanceFunction>
  arma::Col<double> MultilevelCoordinateSearch<DistanceFunction>::expectedGainOfSplit(unsigned int par, unsigned int numberOfDimensions, arma::Col<arma::uword> n0, arma::Col<double> x1, arma::Col<double> x2, arma::Col<double> f1, arma::Col<double> f2) {
    double emin = arma::datum::inf;
    arma::Col<double> expectedGain = arma::Col<double>(numberOfDimensions);
    for (int i = 0; i < numberOfDimensions; i++) {
      if (n0(i) == 0) {
        //expected gain for splitting according to the initialization list
        expectedGain(i) = arma::min(initListEvaluations_.col(i)) - initListEvaluations_(initialPointIndex_(i), i);
        if (expectedGain(i) < emin) {
          emin = expectedGain(i);
          isplit_(par) = i;
          //index again so use 1, matlab=2
          z_(1, par) = arma::datum::inf;
        }
      } else {
        arma::Col<double> z1;
        z1 << baseVertex_(i) << x1(i) << x2(i) << arma::endr;
        //index again at "boxBaseVertexFunctionValues_(0,par)" so use 0, matlab=1
        arma::Col<double> z2;
        z2 << 0 << f1(i) - boxBaseVertexFunctionValues_(0, par) << f2(i) - boxBaseVertexFunctionValues_(0, par) << arma::endr;
        arma::Col<double> d = quadraticPolynomialInterpolation(z1, z2);
        arma::Col<double> eta = subint(baseVertex_(i), oppositeVertex_(i));
        //safeguard against splitting too close to x(i)
        double xi1 = arma::min(eta);
        double xi2 = arma::max(eta);
        double z = minimumQuadraticPolynomial(xi1, xi2, d, z1);
        expectedGain(i) = quadraticPolynomial(z, d, z1);
        if (expectedGain(i) < emin) {
          emin = expectedGain(i);
          isplit_(par) = i;
          //index again so use 1, matlab=2
          z_(1, par) = z;
        }
      }
    }
    return expectedGain;
  }

  template<class DistanceFunction>
  unsigned int MultilevelCoordinateSearch<DistanceFunction>::startSweep() {
    record_ = arma::Col<arma::uword>(boxDivisions_ - 1, arma::fill::zeros);
    unsigned int s = boxDivisions_;
    for (unsigned int i = 0; i < nboxes_; i++) {
      if (level_(i) > 0) {
        if (level_(i) < s) {
          s = level_(i);
        }
        if (!record_(level_(i))) {
          record_(level_(i)) = i;
        } else if (boxBaseVertexFunctionValues_(0, i) < boxBaseVertexFunctionValues_(0, record_(level_(i)))) {
          record_(level_(i)) = i;
        }
      }
    }
    return s;
  }

  template<class DistanceFunction>
  void MultilevelCoordinateSearch<DistanceFunction>::vertex(unsigned int par, arma::Col<double> x1, arma::Col<double> x2, arma::Col<double> f1, arma::Col<double> f2, arma::Col<arma::uword> n0) {
    //for convenience
    unsigned int numberOfDimensions = this->optimisationProblem_->getNumberOfDimensions();

    //TODO: check if this->populationSize_ comparisons need to be '-1'd
    baseVertex_ = arma::Col<double>(numberOfDimensions);
    baseVertex_.fill(arma::datum::inf);
    oppositeVertex_ = arma::Col<double>(numberOfDimensions);
    oppositeVertex_.fill(arma::datum::inf);


    double fold = boxBaseVertexFunctionValues_(0, par);
    unsigned int mVertex = par;
    while (mVertex > 1) {
      double i = std::abs(isplit_(ipar_(mVertex)));
      n0(i) = n0(i) + 1;
      //matlab checks for 1, since it's index use 0
      if (ichild_(mVertex) == 0) {
        if (baseVertex_(i) == arma::datum::inf || baseVertex_(i) == z_(0, ipar_(mVertex))) {
          //matlab passes 2, but it's used as an index so we need to use 1
          vert1(i, 1, mVertex, baseVertex_, x1, x2, f1, f2);
        } else {
          updtf(numberOfDimensions, i, fold, x1, x2, f1, f2, boxBaseVertexFunctionValues_(0, ipar_(mVertex)));
          fold = boxBaseVertexFunctionValues_(0, ipar_(mVertex));
          //matlab passes 1, but it's used as an index so we need to use 0
          vert2(i, 0, mVertex, baseVertex_, x1, x2, f1, f2);
        }
        //matlab checks for 2, since it's index use 1
      } else if (ichild_(mVertex) >= 1) {
        updtf(numberOfDimensions, i, fold, x1, x2, f1, f2, boxBaseVertexFunctionValues_(0, ipar_(mVertex)));
        fold = boxBaseVertexFunctionValues_(0, ipar_(mVertex));
        if (baseVertex_(i) == arma::datum::inf || baseVertex_(i) == z_(1, ipar_(mVertex))) {
          //matlab passes 1, but it's used as an index so we need to use 0
          vert1(i, 0, mVertex, baseVertex_, x1, x2, f1, f2);
        } else {
          //matlab passes 2, but it's used as an index so we need to use 1
          vert2(i, 1, mVertex, baseVertex_, x1, x2, f1, f2);
        }
      }
      //matlab checks for 1/2, since it's index use 0/1
      //original matlab code: 1 <= ichild(m) & ichild(m) <= 2 & y(i) == Inf
      if ((ichild_(mVertex) == 0 || ichild_(mVertex) == 1) && oppositeVertex_(i) == arma::datum::inf) {
        oppositeVertex_(i) = splitByGoldenSectionRule(z_(0, ipar_(mVertex)), z_(1, ipar_(mVertex)), boxBaseVertexFunctionValues_(0, ipar_(mVertex)), boxBaseVertexFunctionValues_(1, ipar_(mVertex)));
      }
      //box m was generated by splitting according to the init. list
      if (ichild_(mVertex) < 0) {
        int j1 = 0;
        int j2 = 0;
        int j3 = 0;
        int k = 0;
        if (boundaries_.col(0)(i) < x0_(i, 0)) {
          //TODO: since these are indexes too they also might need to be decremented
          //the if also needs to be adjusted. same in else below.
          j1 = std::ceil(std::abs(ichild_(mVertex)) / 2.0);
          j2 = std::floor(std::abs(ichild_(mVertex)) / 2.0);
          if ((std::abs(ichild_(mVertex)) / 2.0 < j1 && j1 > 1) || j1 == this->populationSize_) {
            j3 = -1;
          } else {
            //TODO: this probably needs to be 0. same below. 
            //but -1 should be correct (since that is a pseudoindex to recognize initlist boxes)
            j3 = 0;
          }
        } else {
          j1 = std::floor(std::abs(ichild_(mVertex)) / 2.0) + 1;
          j2 = std::ceil(std::abs(ichild_(mVertex)) / 2.0);
          if ((std::abs(ichild_(mVertex)) / 2.0 + 1) > j1 && j1 < this->populationSize_) {
            j3 = 0;
          } else {
            j3 = -1;
          }
        }
        //box m was generated in the init. procedure
        if (isplit_(ipar_(mVertex)) < 0) {
          k = i;
          //box m was generated by a later split according to the init.list
          //k points to the corresponding function values  
        } else {
          //matlab passes 2, but it's used as an index so we need to use 1
          k = z_(1, ipar_(mVertex));
        }
        if (j1 != initialPointIndex_(i) || (baseVertex_(i) != arma::datum::inf && baseVertex_(i) != x0_(i, (initialPointIndex_(i))))) {
          updtf(numberOfDimensions, i, fold, x1, x2, f1, f2, initListEvaluations_(initialPointIndex_(i), k));
          fold = initListEvaluations_(initialPointIndex_(i), k);
        }
        if (baseVertex_(i) == arma::datum::inf || baseVertex_(i) == x0_(i, j1)) {
          baseVertex_(i) = x0_(i, j1);
          if (x1(i) == arma::datum::inf) {
            vert3(i, j1, mVertex, k, x1, x2, f1, f2);
          } else if (x2(i) == arma::datum::inf && x1(i) != x0_(i, j1 + j3)) {
            x2(i) = x0_(i, j1 + j3);
            f2(i) = f2(i) + initListEvaluations_(j1 + j3, k);
          } else if (x2(i) == arma::datum::inf) {
            //matlab checks for 1, since it's index use 0
            if (j1 != 0 && j1 != this->populationSize_) {
              x2(i) = x0_(i, j1 - j3);
              f2(i) = f2(i) + initListEvaluations_(j1 - j3, k);
            } else {
              x2(i) = x0_(i, j1 + 2 * j3);
              f2(i) = f2(i) + initListEvaluations_(j1 + 2 * j3, k);
            }
          }
        } else {
          if (x1(i) == arma::datum::inf) {
            x1(i) = x0_(i, j1);
            f1(i) = f1(i) + initListEvaluations_(j1, k);
            if (baseVertex_(i) != x0_(i, j1 + j3)) {
              x2(i) = x0_(i, j1 + j3);
              f2(i) = f2(i) + initListEvaluations_(j1 + j3, k);
            }
          } else if (x2(i) == arma::datum::inf) {
            if (x1(i) != x0_(i, j1)) {
              x2(i) = x0_(i, j1);
              f2(i) = f2(i) + initListEvaluations_(j1, k);
            } else if (baseVertex_(i) != x0_(i, j1 + j3)) {
              x2(i) = x0_(i, j1 + j3);
              f2(i) = f2(i) + initListEvaluations_(j1 + j3, k);
            } else {
              //matlab checks for 1, since it's index use 0
              if (j1 != 0 && j1 != this->populationSize_) {
                x2(i) = x0_(i, j1 - j3);
                f2(i) = f2(i) + initListEvaluations_(j1 - j3, k);
              } else {
                x2(i) = x0_(i, j1 + 2 * j3);
                f2(i) = f2(i) + initListEvaluations_(j1 + 2 * j3, k);
              }
            }
          }
        }
        if (oppositeVertex_(i) == arma::datum::inf) {
          //TODO: why is there an index check for 0 in matlab?!!?
          if (j2 == 0) {
            oppositeVertex_(i) = boundaries_.col(0)(i);
          } else if (j2 == this->populationSize_) {
            oppositeVertex_(i) = boundaries_.col(1)(i);
          } else {
            oppositeVertex_(i) = splitByGoldenSectionRule(x0_(i, j2), x0_(i, j2 + 1), initListEvaluations_(j2, k), initListEvaluations_(j2 + 1, k));
          }
        }
      }
      mVertex = ipar_(mVertex);
    }
    for (int i = 0; i < numberOfDimensions; i++) {
      if (baseVertex_(i) == arma::datum::inf) {
        baseVertex_(i) = x0_(i, initialPointIndex_(i));
        vert3(i, initialPointIndex_(i), mVertex, i, x1, x2, f1, f2);
      }
      if (oppositeVertex_(i) == arma::datum::inf) {
        oppositeVertex_(i) = oppositeVertex_(i);
      }
    }
  }

  template<class DistanceFunction>
  void MultilevelCoordinateSearch<DistanceFunction>::vert1(int updateIndex, unsigned int j, unsigned int m, arma::Col<double> x, arma::Col<double> x1, arma::Col<double> x2, arma::Col<double> f1, arma::Col<double> f2) {
    int j1 = 0;
    //matlab checks for 1, since it's index use 0
    if (j == 0) {
      //also index again
      j1 = 1;
    } else {
      j1 = 0;
    }
    x(updateIndex) = z_(j1);
    if (x1(updateIndex) == arma::datum::inf) {
      x1(updateIndex) = z_(j);
      f1(updateIndex) = f1(updateIndex) + boxBaseVertexFunctionValues_(j);
    } else if (x2(updateIndex) == arma::datum::inf && x1(updateIndex) != z_(j)) {
      x2(updateIndex) = z_(j);
      f2(updateIndex) = f2(updateIndex) + boxBaseVertexFunctionValues_(j);
    }
  }

  template<class DistanceFunction>
  void MultilevelCoordinateSearch<DistanceFunction>::vert2(int updateIndex, unsigned int j, unsigned int m, arma::Col<double> x, arma::Col<double> x1, arma::Col<double> x2, arma::Col<double> f1, arma::Col<double> f2) {
    int j1 = 0;
    //matlab checks for 1, since it's index use 0
    if (j == 0) {
      //also index again
      j1 = 1;
    } else {
      j1 = 0;
    }
    if (x1(updateIndex) == arma::datum::inf) {
      x1(updateIndex) = z_(j);
      f1(updateIndex) = f1(updateIndex) + boxBaseVertexFunctionValues_(j);
      if (x(updateIndex) != z_(j1)) {
        x2(updateIndex) = z_(j1);
        f2(updateIndex) = f2(updateIndex) + boxBaseVertexFunctionValues_(j1);
      }
    } else if (x2(updateIndex) == arma::datum::inf && x1(updateIndex) != z_(j)) {
      x2(updateIndex) = z_(j);
      f2(updateIndex) = f2(updateIndex) + boxBaseVertexFunctionValues_(j);
    } else if (x2(updateIndex) == arma::datum::inf) {
      x2(updateIndex) = z_(j1);
      f2(updateIndex) = f2(updateIndex) + boxBaseVertexFunctionValues_(j1);
    }
  }

  template<class DistanceFunction>
  void MultilevelCoordinateSearch<DistanceFunction>::vert3(int updateIndex, unsigned int j, unsigned int m, unsigned int f0Index, arma::Col<double> x1, arma::Col<double> x2, arma::Col<double> f1, arma::Col<double> f2) {
    int k1 = 0;
    int k2 = 0;
    //matlab checks for 1, since it's index use 0
    if (j == 0) {
      //also index again
      k1 = 1;
      k2 = 2;
    } else if (j == this->populationSize_) {
      //TODO: might need to decrement one more
      k1 = this->populationSize_ - 2;
      k2 = this->populationSize_ - 1;
    } else {
      //TODO: might need to in-/decrement one more
      k1 = j - 1;
      k2 = j + 1;
    }
    x1(updateIndex) = x0_(k1);
    x2(updateIndex) = x0_(k2);
    f1(updateIndex) = f1(updateIndex) + boxBaseVertexFunctionValues_(k1);
    f2(updateIndex) = f2(updateIndex) + boxBaseVertexFunctionValues_(k2);
  }

  template<class DistanceFunction>
  void MultilevelCoordinateSearch<DistanceFunction>::updtf(unsigned int numberOfDimensions, unsigned int splittingIndex, double fold, arma::Col<double> x1, arma::Col<double> x2, arma::Col<double> f1, arma::Col<double> f2, double baseVertexValueCurrentBox) {
    for (int i = 0; i < numberOfDimensions; i++) {
      if (i != splittingIndex) {
        if (x1(i) == arma::datum::inf) {
          f1(i) = f1(i) + fold - baseVertexValueCurrentBox;
        }
        if (x2(i) == arma::datum::inf) {
          f2(i) = f2(i) + fold - baseVertexValueCurrentBox;
        }
      }
    }
    //updtf.m sets fold = f here. since the inputvalue fold never gets changed, this doesn't actually belong here.
  }

  template<class DistanceFunction>
  arma::Col<double> MultilevelCoordinateSearch<DistanceFunction>::subint(double x, double y) {
    int x2 = y;
    int f = 1000;
    if (f * std::abs(x) < 1) {
      if (std::abs(y) > f) {
        x2 = std::copysign(1.0, y);
      }
    } else {
      if (std::abs(y) > f * std::abs(x)) {
        //TODO: c++ standardlibraries have no signum. wat. using copysign instead...
        //original matlab: x2 = 10.*sign(y)*abs(x);
        x2 = 10 * std::copysign(x, y);
      }
    }
    return arma::Col<double>(x + (x2 - x) / 10.0, x2);
  }

  template<class DistanceFunction>
  void MultilevelCoordinateSearch<DistanceFunction>::updateRecord(unsigned int label, int level, arma::Col<double> f) {
    if (record_.n_elem < level) {
      record_(level) = label;
    } else if (record_(level) == 0) {
      record_(level) = label;
    } else if (f(label) < f(record_(level))) {
      record_(level) = label;
    }
  }

  template<class DistanceFunction>
  bool MultilevelCoordinateSearch<DistanceFunction>::checkLocationNotUsed(arma::Col<double> location) {
    for (int i = 0; i < nloc_; i++) {
      //TODO: This might not work correctly
      if (arma::all(location == xloc_.col(i))) {
        return false;
      }
    }
    return true;
  }

  template<class DistanceFunction>
  void MultilevelCoordinateSearch<DistanceFunction>::addLocation(arma::Col<double> loc) {
    nloc_++;
    if (xloc_.n_cols < nloc_) {
      xloc_.resize(xloc_.n_cols + step);
      xloc_.col(nloc_) = loc;
    } else {
      xloc_.col(nloc_) = loc;
    }
  }

  template<class DistanceFunction>
  void MultilevelCoordinateSearch<DistanceFunction>::setLocalSearch(const std::shared_ptr<OptimisationAlgorithm<double, DistanceFunction>> localSearch) noexcept {
    localSearch_ = localSearch;
  }

  template<class DistanceFunction>
  bool MultilevelCoordinateSearch<DistanceFunction>::pointInsideDomainOfAttraction(arma::Col<double> loc, std::shared_ptr<double> valueAtLoc, double nbasket) {
    if (nbasket == 0) {
      return true;
    }
    arma::Col<double> distancesToLoc(nbasket);
    for (int k = 0; k < nbasket; k++) {
      distancesToLoc(k) = arma::norm(loc - pointsInBasket_.col(k));
    }
    arma::Col<arma::uword> sortedDistances = arma::sort_index(distancesToLoc);
    for (int k = 0; k < nbasket; k++) {
      int i = sortedDistances(k);
      if (pointsInBasketValue_(i) <= *valueAtLoc) {
        arma::Col<double> p = pointsInBasket_(i) - loc;
        arma::Col<double> y1 = loc + (1 / 3.0) * p;
        double f1 = this->optimisationProblem_->getObjectiveValue(y1);
        this->numberOfIterations_++;
        if (f1 <= *valueAtLoc) {
          arma::Col<double> y2 = loc + (2 / 3.0) * p;
          double f2 = this->optimisationProblem_->getObjectiveValue(y2);
          if (f2 > std::max(f1, pointsInBasketValue_(i))) {
            if (f1 < *valueAtLoc) {
              loc = y1;
              *valueAtLoc = f1;
              if (*valueAtLoc < this->bestObjectiveValue_) {
                this->bestObjectiveValue_ = *valueAtLoc;
                this->bestParameter_ = loc;
                if (this->isFinished() || this->isTerminated()) {
                  return false;
                }
              }
            }
          } else {
            if (f1 < std::min(f2, pointsInBasketValue_(i))) {
              *valueAtLoc = f1;
              loc = y1;
              if (*valueAtLoc < this->bestObjectiveValue_) {
                this->bestObjectiveValue_ = *valueAtLoc;
                this->bestParameter_ = loc;
                if (this->isFinished() || this->isTerminated()) {
                  return false;
                }
              }
            } else if (f2 < std::min(f1, pointsInBasketValue_(i))) {
              *valueAtLoc = f2;
              loc = y2;
              if (*valueAtLoc < this->bestObjectiveValue_) {
                this->bestObjectiveValue_ = *valueAtLoc;
                this->bestParameter_ = loc;
                if (this->isFinished() || this->isTerminated()) {
                  return false;
                }
              }
            } else {
              return false;
            }
          }
        }
      }
    }
    return true;
  }

  template<class DistanceFunction>
  bool MultilevelCoordinateSearch<DistanceFunction>::candidateInsideDomainOfAttraction(arma::Col<double> candidate, double valueAtCandidate, double nbasket) {
    if (nbasket == 0) {
      return true;
    }
    arma::Col<double> distancesToCandidate(nbasket);
    for (int k = 0; k < nbasket; k++) {
      distancesToCandidate(k) = arma::norm(candidate - pointsInBasket_.col(k));
    }
    arma::Col<arma::uword> sortedDistances = arma::sort_index(distancesToCandidate);
    for (int k = 0; k < nbasket; k++) {
      int i = sortedDistances(k);
      arma::Col<double> p = pointsInBasket_.col(i) - candidate;
      arma::Col<double> y1 = candidate + (1 / 3.0) * p;
      double f1 = this->optimisationProblem_->getObjectiveValue(y1);
      this->numberOfIterations_++;
      if (f1 <= std::max(pointsInBasketValue_(i), valueAtCandidate)) {
        arma::Col<double> y2 = candidate + (2 / 3.0) * p;
        double f2 = this->optimisationProblem_->getObjectiveValue(y2);
        this->numberOfIterations_++;
        if (f2 <= std::max(f1, pointsInBasketValue_(i))) {
          if (valueAtCandidate < std::min(std::min(f1, f2), pointsInBasketValue_(i))) {
            pointsInBasketValue_(i) = valueAtCandidate;
            pointsInBasket_.col(i) = candidate;
            if (valueAtCandidate < this->bestObjectiveValue_) {
              this->bestObjectiveValue_ = valueAtCandidate;
              this->bestParameter_ = candidate;
              if (this->isFinished() || this->isTerminated()) {
                return false;
              }
            }
            return false;
          } else if (f1 < std::min(std::min(valueAtCandidate, f2), pointsInBasketValue_(i))) {
            pointsInBasketValue_(i) = f1;
            pointsInBasket_.col(i) = y1;
            if (valueAtCandidate < this->bestObjectiveValue_) {
              this->bestObjectiveValue_ = valueAtCandidate;
              this->bestParameter_ = candidate;
              if (this->isFinished() || this->isTerminated()) {
                return false;
              }
            }
            return false;
          } else if (f2 < std::min(std::min(valueAtCandidate, f1), pointsInBasketValue_(i))) {
            pointsInBasketValue_(i) = f2;
            pointsInBasket_.col(i) = y2;
            if (valueAtCandidate < this->bestObjectiveValue_) {
              this->bestObjectiveValue_ = valueAtCandidate;
              this->bestParameter_ = candidate;
              if (this->isFinished() || this->isTerminated()) {
                return false;
              }
            }
            return false;
          } else {
            return false;
          }
        }
      }
    }
    return true;
  }
  // </editor-fold>
}

