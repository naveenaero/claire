/**
 *  Copyright (c) 2015-2016.
 *  All rights reserved.
 *  This file is part of the XXX library.
 *
 *  XXX is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  XXX is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XXX.  If not, see <http://www.gnu.org/licenses/>.
 *
*/


#ifndef _REGOPT_H_
#define _REGOPT_H_

//global includes
#include <sstream>

// local includes
#include "RegUtils.hpp"


namespace reg
{




// flags for hyperbolic PDE solvers
enum PDESolver
{
    RK2,  ///< flag for RK2 solver
    RK2A, ///< flag for stabilized RK2 solver
    SL,   ///< flag for SL
};


// flags for regularization norms
enum RegNormType
{
    L2,   ///< flag for L2-norm
    H1,   ///< flag for H1-norm
    H2,   ///< flag for H2-norm
    H3,   ///< flag for H3-norm
    H1SN, ///< flag for H1-seminorm
    H2SN, ///< flag for H2-seminorm
    H3SN, ///< flag for H3-seminorm
};


enum ParaContType
{
    PCONTOFF,
    PCONTBINSEARCH,
    PCONTREDUCESEARCH,
    PCONTINUATION
};


/*! hessian mat vec type */
enum HessianMatVecType
{
    DEFAULTMATVEC,
    PRECONDMATVEC,
    PRECONDMATVECSYM,
};


// flags for optimization methods
enum OptMeth
{
    GAUSSNEWTON, ///< Gauss-Newton approximation
    FULLNEWTON,  ///< full Newton
    GRADDESCENT, ///< gradient descent (gradient in sobolev space)
};


// flags for preconditioner methods
enum PrecondMeth
{
    INVREG,  ///< inverse regularization operator
    TWOLEVEL, ///< 2 level preconditioner
    NOPC,    ///< no preconditioner
};


// flags for optimization
enum FSeqType
{
    QDFS, ///< quadratic forcing sequence
    SLFS, ///< superliner forcing sequence
    NOFS, ///< no forcing sequence
};


// flags for preconditioners
enum KrylovSolverType
{
    PCG,    ///< pcg
    FCG,    ///< flexible cg
    CHEB,   ///< chebyshev method
    GMRES,  ///< gmres
    FGMRES, ///< flexible gmres
};


// high level flags for solver
enum SolveType
{
    FAST_AGG, // H1-regularization; few iterations; low accuracy
    FAST_SMOOTH, // H2-regularization; few iterations; low accuracy
    ACC_AGG, // H1-regularization; accurate solution;
    ACC_SMOOTH, // H2-regularization; accurate solution;
    NOTSET, // no predefined solver
};


// flags for timers
enum TimerType
{
    T2SEXEC = 0, ///< time to solution (execution time)
    PDEEXEC,     ///< pde solves (execution time)
    HMVEXEC,     ///< hessian mat vec (execution time)
    PMVSETUP,    ///< setup time for preconditioner
    PMVEXEC,     ///< precond mat vec (execution time)
    GRADEXEC,    ///< gradient evaluation (execution time)
    OBJEXEC,     ///< objective evluation (execution time)
    FFTSETUP,    ///< fft setup time
    FFTEXEC,     ///< fft execution time
    NTIMERS,     ///< to allocate the timers
};


// counters (number of operations)
enum CounterType
{
    PDESOLVE = 0, ///< PDE solves
    HESSMATVEC,   ///< # hessian matvecs
    PCMATVEC,     ///< preconditioner matvecs
    OBJEVAL,      ///< objective evaluations
    GRADEVAL,     ///< gradient evaluations
    IPVEC,        ///< interpolation execution time
    IP,           ///< interpolation execution time
    FFT,          ///< fft evaluations
    ITERATIONS,   ///< number of outer iterations
    NCOUNTERS,    ///< to allocate the counters
};


enum RegModel
{
    COMPRESSIBLE,
    STOKES,
    RELAXEDSTOKES
};







/* parameters for domain */
struct Domain{
    IntType isize[3]; ///< size of grid in spatial domain for mpi proc
    IntType istart[3]; ///< start index in spatial domain for mpi proc
    IntType nlocal; ///< number of grid points for each mpi proc
    IntType nglobal; ///< number of grid points (global)
    ScalarType hx[3]; ///< spatial grid cell size
    IntType nx[3]; ///< spatial grid size
    IntType nt; ///< number of time points
    ScalarType timehorizon[2]; ///< time horizon
};


/* parameters for optimization */
struct Optimization{
    int maxit; ///< maximal number of (outer) iterations
    ScalarType tol[3]; ///< tolerances for optimization
    OptMeth method; ///< optimization method
};


/* parameters for krylov solver */
struct KrylovSolver{
    int maxit;
    ScalarType tol[3];
    FSeqType fseqtype; ///<forcing sequence type
    KrylovSolverType solver;
    std::string name;
    ScalarType reltol;
    PrecondMeth pctype;
    std::string pcname;
    KrylovSolverType pcsolver;
    ScalarType pcsolvertol;
    ScalarType g0norm;
    bool g0normset;
    int pcsolvermaxit;
};


/* parameter for parameter continuation (regularization parameter) */
struct ParCont{
    static const ScalarType betavminh1=1E-3; ///< minimal regularization parameter for h1 type norm
    static const ScalarType betavminh2=1E-6; ///< minimal regularization parameter for h2 type norm
    static const int maxsteps = 10; ///< max number of steps
    static const ScalarType betascale = 1E-1; ///< default reduction factor (one order of magnitude)
    static const ScalarType dbetascale = 1E-2; ///< default reduction factor (one order of magnitude)
    ParaContType strategy; ///< flag for parameter continuation strategy
    bool enabled; ///< flag: parameter continuation using different strategies
    ScalarType targetbeta; ///< target regularization parameter
};


/* parameter for scale continuation */
struct ScaleCont{
    bool enabled;
    static const int maxlevel=6;
    ScalarType sigma[3][maxlevel];
};


/* parameter for grid continuation */
struct GridCont{
    bool enabled;
    static const int minlevels=3;
    std::vector<std::vector<IntType>> nx;
    std::vector<std::vector<IntType>> isize;
    std::vector<std::vector<IntType>> istart;
    std::vector<std::vector<IntType>> osize;
    std::vector<std::vector<IntType>> ostart;
    std::vector<IntType> nlocal;
    std::vector<IntType> nglobal;
    std::vector<IntType> nalloc;
    int nlevels;
};


/* parameters for parameter continuation */
struct RegMonitor{
    bool JAC; ///< flag to monitor jacobian during iterations
    bool CFL; ///< flag to monitor CFL condition during iterations
    ScalarType jacmin; ///< min value of jacobian
    ScalarType jacmax; ///< max value of jacobian
    ScalarType jacmean; ///< mean value of jacobian
    ScalarType jacbound; ///< lower bound for jacobian
};


struct RegNorm{
    ScalarType beta[3]; ///< regularization parameter
    RegNormType type; ///< flag for regularization norm
};


struct FourierTransform{
    accfft_plan* plan;
    MPI_Comm mpicomm;
    IntType nalloc;
    IntType osize[3]; ///< size of grid in fourier domain for mpi proc
    IntType ostart[3]; ///< start index in fourier domain for mpi proc
};


struct RegFlags{
    bool readimages;
    bool smoothingenabled;
    bool storetimeseries;
    bool storeiterates;
    bool storedefgrad;
    bool storedefmap;
    bool storeresults;
    bool storeinterresults;
    bool loggingenabled;
    bool runpostproc;
    bool resampledata;
};




class RegOpt
{

public:

    typedef RegOpt Self;

    RegOpt();
    RegOpt(int,char**,int id=0);
    RegOpt(const RegOpt&);
    ~RegOpt();

    // spatial grid
    inline void SetNumGridPoints(int i,IntType nx){this->m_Domain.nx[i] = nx;};
    inline IntType GetNumGridPoints(int i){return this->m_Domain.nx[i];};
    inline ScalarType GetLebesqueMeasure(void)
    {
        return  this->m_Domain.hx[0]
               *this->m_Domain.hx[1]
               *this->m_Domain.hx[2];
    }

    inline Domain GetDomainPara(){return this->m_Domain;};
    inline GridCont GetGridContPara(){return this->m_GridCont;};
    inline ScaleCont GetScaleContPara(){return this->m_ScaleCont;};
    inline ParCont GetParaCont(){return this->m_ParaCont;};
    ScalarType GetBetaMinParaCont();
    inline FourierTransform GetFFT(){return this->m_FFT;};
    inline RegFlags GetRegFlags(){return this->m_RegFlags;};
    inline RegMonitor GetRegMonitor(){return this->m_RegMonitor;};
    inline void DisableSmoothing(){ this->m_RegFlags.smoothingenabled=false; };
    inline void EnableSmoothing(){ this->m_RegFlags.smoothingenabled=true; };

    PetscErrorCode GetSizes(IntType*,IntType&,IntType&);
    PetscErrorCode GetSizes(IntType*,IntType*,IntType*);

    // time horizon, step size, and ....
    inline void SetNumTimePoints(IntType nt){ this->m_Domain.nt=nt; };
    inline ScalarType GetTimeStepSize(void){
        return (this->m_Domain.timehorizon[1] - this->m_Domain.timehorizon[0])
               /static_cast<ScalarType>(this->m_Domain.nt);
    };

    // control input and output
    inline std::string GetXFolder(void){return this->m_XFolder;};
    inline std::string GetIFolder(void){return this->m_IFolder;};
    inline std::string GetXExtension(void){return this->m_XExtension;};

    inline std::string GetTemplateFN(void){return this->m_TemplateFN;};
    inline std::string GetReferenceFN(void){return this->m_ReferenceFN;};

    // registration model
    inline RegModel GetRegModel(void){return this->m_RegModel;};

    /* do setup for grid continuation */
    PetscErrorCode SetupGridCont();

    // regularization
    inline RegNorm GetRegNorm(){return this->m_RegNorm; };
    inline void SetRegularizationWeight(ScalarType beta){
        this->m_RegNorm.beta[0]=beta;
        this->m_RegNorm.beta[1]=beta;
    };

    // smoothing
    inline ScalarType GetSigma(int i){return this->m_Sigma[i];};
    inline void SetSigma(int i,ScalarType sigma){this->m_Sigma[i]=sigma;};

    // solver flags
    inline PDESolver GetPDESolver(void){return this->m_PDESolver;};
    inline HessianMatVecType GetHessianMatVecType(){return this->m_HessianMatVecType;}
    inline KrylovSolver GetKrylovSolverPara(){ return this->m_KrylovSolverPara;};
    inline void SetRelTolKrylovMethod(ScalarType value){this->m_KrylovSolverPara.reltol=value;};
    inline void SetInitialGradNormKrylovMethod(ScalarType value){
        this->m_KrylovSolverPara.g0norm=value;
        this->m_KrylovSolverPara.g0normset=true;
    }
    inline void InitialGradNormSet(bool flag){this->m_KrylovSolverPara.g0normset=flag;};

    // jacobians
    inline void SetJacMin(ScalarType value){this->m_RegMonitor.jacmin=value;};
    inline void SetJacMax(ScalarType value){this->m_RegMonitor.jacmax=value;};
    inline void SetJacMean(ScalarType value){this->m_RegMonitor.jacmean=value;};

    // flag for setup
    inline bool SetupDone(){return this->m_SetupDone;};

    // timers and counters
    inline unsigned int GetCounter(CounterType id){return this->m_Counter[id];};
    inline void IncrementCounter(CounterType id){this->m_Counter[id]++;};
    inline void IncrementCounter(CounterType id, unsigned int i){this->m_Counter[id]+= i;};

    inline void GetTimer(TimerType id,double* wtime){
        wtime[0] = this->m_Timer[id][MIN];
        wtime[1] = this->m_Timer[id][MAX];
        wtime[2] = this->m_Timer[id][AVG];
    };

    unsigned int GetNumThreads(){return this->m_NumThreads;};
    inline int GetNetworkDims(int i){return this->m_CartGridDims[i];};
    inline void IncreaseFFTTimers(double timers[5]){
        for(int i=0; i < 5; ++i) this->m_FFTTimers[i][LOG]+=timers[i];
    };
    inline void IncreaseInterpTimers(double timers[4]){
        for(int i=0; i < 4; ++i) this->m_InterpTimers[i][LOG]+=timers[i];
    };

    inline ScalarType GetOptTol(int i){return this->m_OptPara.tol[i];};
    inline int GetOptMaxit(){return this->m_OptPara.maxit;};
    inline OptMeth GetOptMeth(void){return this->m_OptPara.method;};

    inline int GetVerbosity(){return this->m_Verbosity;};

    int GetLineLength(){return this->m_LineLength;};

    ScalarType ComputeFFTScale();

    PetscErrorCode StartTimer(TimerType);
    PetscErrorCode StopTimer(TimerType);
    PetscErrorCode ResetTimers(void);
    PetscErrorCode ResetTimer(TimerType);
    PetscErrorCode ResetCounters(void);
    PetscErrorCode ResetCounter(CounterType);
    PetscErrorCode ProcessTimers(void);

    PetscErrorCode DisplayOptions(void);
    PetscErrorCode DisplayTimeToSolution(void);
    PetscErrorCode WriteLogFile(void);
    PetscErrorCode DoSetup(bool dispteaser=true);

private:

    PetscErrorCode Initialize(void);
    PetscErrorCode ClearMemory(void);
    PetscErrorCode ParseArgumentsRegistration(int,char**);
    PetscErrorCode ParseArgumentsPostProcessing(int,char**);
    PetscErrorCode UsageRegistration(bool advanced=false);
    PetscErrorCode UsagePostProcessing(bool advanced=false);
    PetscErrorCode CheckArgumentsRegistration(void);
    PetscErrorCode CheckArgumentsPostProcessing(void);
    PetscErrorCode SetPresetParameters();

    enum TimerValue{LOG=0,MIN,MAX,AVG,NVALTYPES};

    Optimization m_OptPara; ///< optimization parameters
    PDESolver m_PDESolver; ///< flag for PDE solver
    KrylovSolver m_KrylovSolverPara; ///< parameters for krylov solver
    RegMonitor m_RegMonitor; ///< monitor for registration
    RegNorm m_RegNorm; ///< parameters for regularization model
    ParCont m_ParaCont; ///< flags for parameter continuation
    GridCont m_GridCont; ///< flags for grid continuation
    ScaleCont m_ScaleCont; ///< flags for scale continuation
    Domain m_Domain; ///< parameters for spatial domain
    RegModel m_RegModel; ///< flag for particular registration model
    FourierTransform m_FFT; ///< parameters for FFT/accfft
    RegFlags m_RegFlags; ///< flags for registration
    HessianMatVecType m_HessianMatVecType;

    std::string m_XFolder; ///< identifier for folder to write results to
    std::string m_IFolder; ///< identifier for folder to read in results from
    std::string m_XExtension; ///< identifier for extension of files to be written to file
    std::string m_TemplateFN; ///< template image file name
    std::string m_ReferenceFN; ///< reference image file name
    std::string m_VelocityX1FN; ///< x1-velocity field file name
    std::string m_VelocityX2FN; ///< x2-velocity field file name
    std::string m_VelocityX3FN; ///< x3-velocity field file name

    double m_Timer[NTIMERS][NVALTYPES];
    double m_TempTimer[NTIMERS];
    bool m_TimerIsRunning[NTIMERS];
    unsigned int m_Counter[NCOUNTERS];
    double m_FFTTimers[5][NVALTYPES];
    double m_InterpTimers[4][NVALTYPES];

    int m_CartGridDims[2];
    unsigned int m_NumThreads;
    const unsigned int m_LineLength = 101;

    bool m_SetupDone;

    ScalarType m_Sigma[3];
    SolveType m_SolveType;

    int m_Verbosity;

};

} // end of namespace


#endif
