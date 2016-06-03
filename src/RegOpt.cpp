#ifndef _REGOPT_CPP_
#define _REGOPT_CPP_

#include <fstream>
#include "RegOpt.hpp"




namespace reg
{



/********************************************************************
 * Name: RegOpt
 * Description: default constructor
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "RegOpt"
RegOpt::RegOpt()
{
    this->Initialize();
}




/********************************************************************
 * Name: RegOpt
 * Description: constructor
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "RegOpt"
RegOpt::RegOpt(int argc, char** argv)
{
    this->Initialize();
    this->ParseArguments(argc,argv);
}




/********************************************************************
 * Name: ParseArguments
 * Description: parse user arguments
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "ParseArguments"
PetscErrorCode RegOpt::ParseArguments(int argc, char** argv)
{
    PetscErrorCode ierr;
    std::string msg;
    std::vector<unsigned int> nx;
    std::vector<unsigned int> np;
    std::vector<unsigned int> sigma;
    PetscFunctionBegin;

    while(argc > 1){

        if ( (strcmp(argv[1],"-help") == 0)
            ||(strcmp(argv[1],"-h") == 0)
            ||(strcmp(argv[1],"-HELP") == 0) ){
            ierr=this->Usage(); CHKERRQ(ierr);
        }
        if ( strcmp(argv[1],"-advanced") == 0 ){
            ierr=this->Usage(true); CHKERRQ(ierr);
        }
        else if(strcmp(argv[1],"-nx") == 0){

            argc--; argv++;

            const std::string nxinput = argv[1];

            // strip the "x" in the string to get the numbers
            nx = String2Vec( nxinput );

            if (nx.size() == 1){
                for(unsigned int i=0; i < 3; ++i){
                    this->m_Domain.nx[i] = static_cast<IntType>(nx[0]);
                }
            }
            else if(nx.size() == 3){
                for(unsigned int i=0; i < 3; ++i){
                    this->m_Domain.nx[i] = static_cast<IntType>(nx[i]);
                }
            }
            else{
                msg="\n\x1b[31m error in grid size argument: %s\x1b[0m\n";
                ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str(),argv[1]); CHKERRQ(ierr);
                ierr=this->Usage(); CHKERRQ(ierr);
            }

        }
        else if(strcmp(argv[1],"-nt") == 0){
            argc--; argv++;
            this->m_Domain.nt = static_cast<IntType>(atoi(argv[1]));
        }
        else if(strcmp(argv[1],"-sigma") == 0){

            argc--; argv++;

            const std::string sigmainput = argv[1];

            // strip the "x" in the string to get the numbers
            sigma = String2Vec( sigmainput );

            if (sigma.size() == 1){
                for(unsigned int i=0; i < 3; ++i){
                    this->m_Sigma[i] = static_cast<ScalarType>(sigma[0]);
                }
            }
            else if(sigma.size() == 3){
                for(unsigned int i=0; i < 3; ++i){
                    this->m_Sigma[i] = static_cast<IntType>(sigma[i]);
                }
            }
            else{
                msg="\n\x1b[31m error in smoothing kernel size: %s\x1b[0m\n";
                ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str(),argv[1]); CHKERRQ(ierr);
                ierr=this->Usage(); CHKERRQ(ierr);
            }

        }
        else if(strcmp(argv[1],"-nthreads") == 0){
            argc--; argv++;
            this->m_NumThreads = atoi(argv[1]);
        }
        else if(strcmp(argv[1],"-np") == 0){

            argc--; argv++;
            const std::string npinput = argv[1];

            // strip the "x" in the string to get the numbers
            np = String2Vec( npinput );

            if (np.size() == 1){
                for(unsigned int i=0; i < 2; ++i){
                    this->m_CartGridDims[i] = static_cast<unsigned int>(np[0]);
                }
            }
            else if (np.size() == 2){
                for(unsigned int i=0; i < 2; ++i){
                    this->m_CartGridDims[i] = static_cast<unsigned int>(np[i]);
                }
            }
            else{
                msg="\n\x1b[31m error in number of procs: %s\x1b[0m\n";
                ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str(),argv[1]); CHKERRQ(ierr);
                ierr=this->Usage(); CHKERRQ(ierr);
            }
        }
        else if(strcmp(argv[1],"-mr") == 0){
            argc--; argv++;
            this->m_ReferenceFN = argv[1];
        }
        else if(strcmp(argv[1],"-mt") == 0){
            argc--; argv++;
            this->m_TemplateFN = argv[1];
        }
        else if(strcmp(argv[1],"-x") == 0){
            argc--; argv++;
            this->m_XFolder = argv[1];
        }
        else if(strcmp(argv[1],"-preset") == 0){
            argc--; argv++;
            if (strcmp(argv[1],"fast-aggressive") == 0){
                this->m_SolveType = FAST_AGG;
            }
            else if (strcmp(argv[1],"fast-smooth") == 0){
                this->m_SolveType = FAST_SMOOTH;
            }
            else if (strcmp(argv[1],"accurate-aggressive") == 0){
                this->m_SolveType = ACC_AGG;
            }
            else if (strcmp(argv[1],"accurate-smooth") == 0){
                this->m_SolveType = ACC_SMOOTH;
            }
            else {
                msg="\n\x1b[31m high level solver flag not available: %s\x1b[0m\n";
                ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str(),argv[1]); CHKERRQ(ierr);
                ierr=this->Usage(); CHKERRQ(ierr);
            }
        }
        else if(strcmp(argv[1],"-ic") == 0){
            this->m_RegModel = STOKES;
        }
        else if(strcmp(argv[1],"-ric") == 0){
            this->m_RegModel = RELAXEDSTOKES;
        }
        else if(strcmp(argv[1],"-xresults") == 0){
            this->m_WriteImages = true;
        }
        else if(strcmp(argv[1],"-xlog") == 0){
            this->m_WriteLogFiles = true;
        }
        else if (strcmp(argv[1],"-optmeth") == 0){
            argc--; argv++;
            if (strcmp(argv[1],"fn") == 0){
                this->m_OptPara.method = FULLNEWTON;
            }
            else if (strcmp(argv[1],"gn") == 0){
                this->m_OptPara.method = GAUSSNEWTON;
            }
            else {
                msg="\n\x1b[31m optimization method not defined: %s\x1b[0m\n";
                ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str(),argv[1]); CHKERRQ(ierr);
                ierr=this->Usage(); CHKERRQ(ierr);
            }
        }
        else if(strcmp(argv[1],"-maxit") == 0){
            argc--; argv++;
            this->m_OptPara.maxit = atoi(argv[1]);
        }
        else if(strcmp(argv[1],"-gabs") == 0){
            argc--; argv++;
            this->m_OptPara.tol[0] = atof(argv[1]);
        }
        else if(strcmp(argv[1],"-grel") == 0){
            argc--; argv++;
            this->m_OptPara.tol[2] = atof(argv[1]);
        }
        else if(strcmp(argv[1],"-jbound") == 0){
            argc--; argv++;
            this->m_RegMonitor.jacbound = atof(argv[1]);
        }
        else if(strcmp(argv[1],"-krylovmaxit") == 0){
            argc--; argv++;
            this->m_KKTSolverPara.maxit = atoi(argv[1]);
        }
        else if(strcmp(argv[1],"-krylovfseq") == 0){
            argc--; argv++;
            if (strcmp(argv[1],"none") == 0){
                this->m_KKTSolverPara.fseqtype = NOFS;
            }
            else if (strcmp(argv[1],"quadratic") == 0){
                this->m_KKTSolverPara.fseqtype = QDFS;
            }
            else if (strcmp(argv[1],"suplinear") == 0){
                this->m_KKTSolverPara.fseqtype = SLFS;
            }
            else {
                msg="\n\x1b[31m optimization method not defined: %s\x1b[0m\n";
                ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str(),argv[1]); CHKERRQ(ierr);
                ierr=this->Usage(); CHKERRQ(ierr);
            }
        }
        else if(strcmp(argv[1],"-x") == 0){
            argc--; argv++;
            this->m_XFolder = argv[1];
        }
        else if (strcmp(argv[1],"-pdesolver") == 0){
            argc--; argv++;
            if (strcmp(argv[1],"rk2") == 0){
                this->m_PDESolver = RK2;
            }
            else if (strcmp(argv[1],"sl") == 0){
                this->m_PDESolver = SL;
            }
            else {
                msg="\n\x1b[31m pde solver not implemented: %s\x1b[0m\n";
                ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str(),argv[1]); CHKERRQ(ierr);
                ierr=this->Usage(); CHKERRQ(ierr);
            }
        }
        else if (strcmp(argv[1],"-regnorm") == 0){
            argc--; argv++;
            if (strcmp(argv[1],"h1s") == 0){
                this->m_Regularization.norm = H1SN;
            }
            else if (strcmp(argv[1],"h2s") == 0){
                this->m_Regularization.norm = H2SN;
            }
            else if (strcmp(argv[1],"h1") == 0){
                this->m_Regularization.norm = H1;
            }
            else if (strcmp(argv[1],"h2") == 0){
                this->m_Regularization.norm = H2;
            }
            else if (strcmp(argv[1],"l2") == 0){
                this->m_Regularization.norm = L2;
            }
            else {
                msg="\n\x1b[31m regularization norm not available: %s\x1b[0m\n";
                ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str(),argv[1]); CHKERRQ(ierr);
                ierr=this->Usage(); CHKERRQ(ierr);
            }
        }
        else if(strcmp(argv[1],"-betav") == 0){
            argc--; argv++;
            this->m_Regularization.beta[0] = atof(argv[1]);
            this->m_Regularization.beta[1] = atof(argv[1]);
        }
        else if(strcmp(argv[1],"-betaw") == 0){
            argc--; argv++;
            this->m_Regularization.beta[2] = atof(argv[1]);
        }
        else if(strcmp(argv[1],"-train") == 0){

            if (this->m_ParaCont.enabled){
                msg="\n\x1b[31m you can't do training and continuation simultaneously\x1b[0m\n";
                ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str()); CHKERRQ(ierr);
                ierr=this->Usage(); CHKERRQ(ierr);
            }

            argc--; argv++;
            if (strcmp(argv[1],"binary") == 0){
                this->m_ParaCont.strategy = PCONTBINSEARCH;
                this->m_ParaCont.enabled = true;
            }
            else if (strcmp(argv[1],"reduce") == 0){
                this->m_ParaCont.strategy = PCONTREDUCESEARCH;
                this->m_ParaCont.enabled = true;
            }
            else {
                msg="\n\x1b[31m training method not implemented: %s\x1b[0m\n";
                ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str(),argv[1]); CHKERRQ(ierr);
                ierr=this->Usage(); CHKERRQ(ierr);
            }

        }
        else if(strcmp(argv[1],"-betavcont") == 0){

           if (this->m_ParaCont.enabled){
                msg="\n\x1b[31m you can't do training and continuation simultaneously\x1b[0m\n";
                ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str()); CHKERRQ(ierr);
                ierr=this->Usage(); CHKERRQ(ierr);
            }

            this->m_ParaCont.strategy = PCONTINUATION;
            this->m_ParaCont.enabled = true;

            argc--; argv++;
            this->m_ParaCont.targetbeta = atof(argv[1]);
        }
        else if(strcmp(argv[1],"-scalecont") == 0){
            this->m_ScaleCont.enabled = true;
        }
        else if(strcmp(argv[1],"-verbosity") == 0){
            argc--; argv++;
            this->m_Verbosity = atoi(argv[1]);
        }
        else if(strcmp(argv[1],"-jmonitor") == 0){
            this->m_RegMonitor.monitorJAC = true;
        }
        else if(strcmp(argv[1],"-storeiter") == 0){
            this->m_StoreIterates = true;
        }
        else {
            msg="\n\x1b[31m argument not valid: %s\x1b[0m\n";
            ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str(),argv[1]); CHKERRQ(ierr);
            ierr=this->Usage(); CHKERRQ(ierr);
        }
        argc--; argv++;
    }

    // check the arguments/parameters set by the user
    ierr=this->CheckArguments(); CHKERRQ(ierr);

    if (this->m_SolveType != NOTSET){
        ierr=this->SetPresetParameters(); CHKERRQ(ierr);
    }

    PetscFunctionReturn(0);
}




/********************************************************************
 * Name: RegOpt
 * Description: default constructor
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "~RegOpt"
RegOpt::~RegOpt()
{
    this->ClearMemory();
}




/********************************************************************
 * Name: ClearMemory
 * Description: clean up
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "ClearMemory"
PetscErrorCode RegOpt::ClearMemory()
{
    PetscFunctionBegin;

    if(this->m_FFTPlan!= NULL){
        accfft_destroy_plan(this->m_FFTPlan);
        accfft_cleanup();
        this->m_FFTPlan = NULL;
    }

    if (this->m_Comm != NULL){
        MPI_Comm_free(&this->m_Comm);
    }

    PetscFunctionReturn(0);

}




/********************************************************************
 * Name: Initialize
 * Description: initialize class variables
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "Initialize"
PetscErrorCode RegOpt::Initialize()
{
    PetscErrorCode ierr;
    PetscFunctionBegin;

    this->m_SetupDone = false;

    this->m_FFTPlan = NULL;
    this->m_Comm = NULL;

    this->m_Domain.nt = 4;
    this->m_Domain.nx[0] = 32;
    this->m_Domain.nx[1] = 32;
    this->m_Domain.nx[2] = 32;

    this->m_Regularization.norm = H2SN;
    this->m_Regularization.beta[0] = 1E-2;
    this->m_Regularization.beta[1] = 1E-2;
    this->m_Regularization.beta[2] = 1E-4;

    this->m_Verbosity = 1;
    this->m_TimeHorizon[0] = 0.0;
    this->m_TimeHorizon[1] = 1.0;
    this->m_PDESolver = SL;
    this->m_PrecondMeth = INVREG;
    this->m_RegModel = COMPRESSIBLE;

    // smoothing
    this->m_Sigma[0] = 1.0;
    this->m_Sigma[1] = 1.0;
    this->m_Sigma[2] = 1.0;

    this->m_WriteImages = false;
    this->m_WriteLogFiles = false;

    this->m_KKTSolverPara.tol[0] = 1E-12; // relative tolerance
    this->m_KKTSolverPara.tol[1] = 1E-12; // absolute tolerance
    this->m_KKTSolverPara.tol[2] = 1E+06; // divergence tolerance
    this->m_KKTSolverPara.maxit  = 1000; // maximal iterations
    this->m_KKTSolverPara.fseqtype = QDFS;

    this->m_OptPara.tol[0] = 1E-6;  // grad abs tol
    this->m_OptPara.tol[1] = 1E-16; // grad rel tol
    this->m_OptPara.tol[2] = 1E-2;  // grad rel tol
    this->m_OptPara.maxit = 1000; // max number of iterations
    this->m_OptPara.method = GAUSSNEWTON;

    this->m_SolveType = NOTSET;

    this->m_ReadImagesFromFile = false;
    this->m_StoreIterates = false;
    this->m_StoreTimeSeries = false;

    // parameter continuation
    this->m_ParaCont.strategy = PCONTOFF;
    this->m_ParaCont.enabled = false;
    this->m_ParaCont.targetbeta = 0.0;

    // scale continuation
    this->m_ScaleCont.enabled=false;

    // monitor for registration
    this->m_RegMonitor.monitorJAC = false;
    this->m_RegMonitor.monitorCFL = false;
    this->m_RegMonitor.jacmin = 0.0;
    this->m_RegMonitor.jacmax = 0.0;
    this->m_RegMonitor.jacmean = 0.0;
    this->m_RegMonitor.jacbound = 2E-1;

    this->m_NumThreads=1;
    this->m_CartGridDims[0]=1;
    this->m_CartGridDims[1]=1;

    ierr=this->ResetTimers(); CHKERRQ(ierr);
    ierr=this->ResetCounters(); CHKERRQ(ierr);

    PetscFunctionReturn(0);
}




/********************************************************************
 * Name: Usage
 * Description: display usage message for binary
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "Usage"
PetscErrorCode RegOpt::Usage(bool advanced)
{

    PetscErrorCode ierr;
    int rank;
    std::string line;
    PetscFunctionBegin;

    MPI_Comm_rank(PETSC_COMM_WORLD,&rank);

    line = std::string(this->m_LineLength,'-');

    if (rank == 0){

        std::cout << std::endl;
        std::cout << line << std::endl;
        std::cout << " usage: runcoldreg [options] " <<std::endl;
        std::cout << line << std::endl;
        std::cout << " where [options] is one or more of the following"<<std::endl;
        std::cout << line << std::endl;
        std::cout << " -mr <file>                reference image (*.nii, *.nii.gz, *.hdr)"<<std::endl;
        std::cout << " -mt <file>                template image (*.nii, *.nii.gz, *.hdr)"<<std::endl;

        // ####################### advanced options #######################
        if (advanced)
        {
        std::cout << " -sigma <int>x<int>x<int>  size of gaussian smoothing kernel applied to input images (e.g., 1x2x1;"<<std::endl;
        std::cout << "                           units: voxel size; if only one parameter is set"<<std::endl;
        std::cout << "                           uniform smoothing is assumed)"<<std::endl;
        }
        // ####################### advanced options #######################

        std::cout << line << std::endl;
        std::cout << " -x <path>                 output path (what's written out is controlled by the flags below)"<<std::endl;
        std::cout << "                           a prefix can be added by doing '-x </out/put/path/prefix_>"<<std::endl;
        std::cout << " -xresults                 flag: write results to file (requires -x option)"<<std::endl;

        // ####################### advanced options #######################
        if (advanced)
        {
        std::cout << " -xlog                     flag: write log files (requires -x option)"<<std::endl;
        std::cout << line << std::endl;
        std::cout << " optimization specific parameters"<<std::endl;
        std::cout << line << std::endl;
        std::cout << " -optmeth <type>           optimization method"<<std::endl;
        std::cout << "                           <type> is one of the following"<<std::endl;
        std::cout << "                               gn           Gauss-Newton (default)"<<std::endl;
        std::cout << "                               fn           full Newton"<<std::endl;
        std::cout << " -grel <dbl>               tolerance for optimization (default: 1E-2)"<<std::endl;
        std::cout << "                               relative change of gradient"<<std::endl;
        std::cout << "                               optimization stops if ||g_k||/||g_0|| <= tol"<<std::endl;
        std::cout << " -gabs <dbl>               tolerance for optimization (default: 1E-6)"<<std::endl;
        std::cout << "                               lower bound for gradient"<<std::endl;
        std::cout << "                               optimization stops if ||g_k|| <= tol"<<std::endl;
        std::cout << "                               tol <= ||g||/||g_init||"<<std::endl;
        std::cout << " -maxit <int>              maximum number of (outer) Newton iterations (default: 50)"<<std::endl;
        std::cout << " -krylovmaxit <int>        maximum number of (inner) Krylov iterations (default: 50)"<<std::endl;
        std::cout << " -krylovfseq <type>        forcing sequence for Krylov solver (tolerance for inner iterations)"<<std::endl;
        std::cout << "                           where <types> are"<<std::endl;
        std::cout << "                               quadratic     quadratic (default)"<<std::endl;
        std::cout << "                               suplinear     super-linear"<<std::endl;
        std::cout << "                               none          exact solve (expensive)"<<std::endl;
        std::cout << line << std::endl;
        std::cout << " regularization/constraints"<<std::endl;
        std::cout << line << std::endl;
        std::cout << " -regnorm <type>           regularization norm for velocity field"<<std::endl;
        std::cout << "                           <type> is one of the following"<<std::endl;
        std::cout << "                               h1s          H1-seminorm"<<std::endl;
        std::cout << "                               h2s          H2-seminorm (default)"<<std::endl;
        std::cout << "                               h1           H1-norm"<<std::endl;
        std::cout << "                               h2           H2-norm"<<std::endl;
        std::cout << "                               l2           l2-norm (discouraged)"<<std::endl;
        std::cout << " -betav <dbl>              regularization parameter (velocity field; default: 1E-2)"<<std::endl;
        std::cout << " -betaw <dbl>              regularization parameter (mass source map; default: 1E-4; enable relaxed"<<std::endl;
        std::cout << "                           incompressibility to use this parameter via '-ric' option; see below)"<<std::endl;
        std::cout << " -ic                       enable incompressibility constraint (det(grad(y))=1)"<<std::endl;
        std::cout << " -ric                      enable relaxed incompressibility (control jacobians; det(grad(y)) ~ 1)"<<std::endl;
        std::cout << " -scalecont                enable scale continuation (continuation in smoothness of images;"<<std::endl;
        std::cout << "                           i.e., use a multi-scale scheme to solve optimization problem)"<<std::endl;
        std::cout << " -gridcont                 enable grid continuation (continuation in resolution of images;"<<std::endl;
        std::cout << "                           i.e., use multi-resultion scheme to solve optimization probelm)"<<std::endl;
        }
        // ####################### advanced options #######################

        std::cout << " -train <type>             estimate regularization parameter (use 'jbound' to set bound"<<std::endl;
        std::cout << "                           for det(grad(y)) used during estimation)"<<std::endl;
        std::cout << "                           <type> is one of the following"<<std::endl;
        std::cout << "                               binary       perform binary search (recommended)"<<std::endl;
        std::cout << "                               reduce       reduce parameter by one order until bound is breached"<<std::endl;
        std::cout << " -jbound <dbl>             lower bound on determinant of deformation gradient (default: 2E-1)"<<std::endl;
        std::cout << " -betavcont <dbl>          do parameter continuation in betav until target regularization"<<std::endl;
        std::cout << "                           parameter betav=<dbl> is reached (betav must be in (0,1))"<<std::endl;

        // ####################### advanced options #######################
        if (advanced)
        {
        std::cout << " -jmonitor                 enable monitor for det(grad(y))"<<std::endl;
        std::cout << line << std::endl;
        std::cout << " solver specific parameters (numerics)"<<std::endl;
        std::cout << line << std::endl;
        std::cout << " -pdesolver <type>         numerical time integrator for transport equations"<<std::endl;
        std::cout << "                           <type> is one of the following"<<std::endl;
        std::cout << "                               sl           semi-Lagrangian method (default; unconditionally stable)"<<std::endl;
        std::cout << "                               rk2          rk2 time integrator (conditionally stable)"<<std::endl;
        std::cout << " -nt <int>                 number of time points (for time integration; default: 4)"<<std::endl;
        std::cout << line << std::endl;
        std::cout << " memory distribution and parallelism"<<std::endl;
        std::cout << line << std::endl;
        std::cout << " -nthreads <int>           number of threads (default: 1)"<<std::endl;
        std::cout << " -np <int>x<int>           distribution of mpi tasks (cartesian grid) (example: -np 2x4 results"<<std::endl;
        std::cout << "                           results in MPI distribution of size (nx1/2,nx2/4,nx3) for each mpi task)"<<std::endl;
        std::cout << line << std::endl;
        std::cout << " other parameters"<<std::endl;
        std::cout << line << std::endl;
        std::cout << " -verbosity <int>          verbosity level (ranges from 0 to 3; default: 1)"<<std::endl;
        std::cout << " -storeiter                store iterates (deformed template image and velocity field)"<<std::endl;
        std::cout << " -nx <int>x<int>x<int>     grid size (e.g., 32x64x32); allows user to control grid size for synthetic"<<std::endl;
        std::cout << "                           problems; assumed to be uniform if single integer is provided"<<std::endl;
        }
        // ####################### advanced options #######################

        std::cout << line << std::endl;
        std::cout << " -help                     display a brief version of the user message"<<std::endl;
        std::cout << " -advanced                 display this message"<<std::endl;
        std::cout << line << std::endl;
        std::cout << line << std::endl;
    }

    ierr=PetscFinalize(); CHKERRQ(ierr);
    exit(0);

    PetscFunctionReturn(0);


}




/********************************************************************
 * Name: CheckArguments
 * Description: check the arguments set by user
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "CheckArguments"
PetscErrorCode RegOpt::CheckArguments()
{
    PetscErrorCode ierr;
    bool readmR=false,readmT=false;
    ScalarType betav;

    std::string msg;
    PetscFunctionBegin;

    if(!this->m_TemplateFN.empty()){ readmT=true; }
    if(!this->m_ReferenceFN.empty()){ readmR=true; }

    if (readmT && readmR){

        // check if files exist
        msg = "file " + this->m_TemplateFN + "does not exist";
        ierr=Assert(FileExists(this->m_TemplateFN),msg); CHKERRQ(ierr);

        msg = "file " + this->m_ReferenceFN + "does not exist";
        ierr=Assert(FileExists(this->m_ReferenceFN),msg); CHKERRQ(ierr);

        this->ReadImagesFromFile(true);
    }
    else if( (readmT == false) && readmR ) {
        msg="\x1b[31m you need to also assign a template image\x1b[0m\n";
        ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str()); CHKERRQ(ierr);
        ierr=this->Usage(); CHKERRQ(ierr);
    }
    else if( readmT && (readmR == false) ) {
        msg="\x1b[31m you need to also assign a reference image\x1b[0m\n";
        ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str()); CHKERRQ(ierr);
        ierr=this->Usage(); CHKERRQ(ierr);
    }
    else if( (readmT == false) && (readmR == false) ){
        this->ReadImagesFromFile(false);
    }

    this->m_XExtension = ".nii.gz";

    if (this->m_ParaCont.strategy==PCONTINUATION){
        betav=this->m_ParaCont.targetbeta;
        if(betav <= 0.0){
            msg="\x1b[31m target betav <= 0.0 \x1b[0m\n";
            ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str()); CHKERRQ(ierr);
            ierr=this->Usage(); CHKERRQ(ierr);
        }
        if(betav > 1.0){
            msg="\x1b[31m target betav >= 1.0 \x1b[0m\n";
            ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str()); CHKERRQ(ierr);
            ierr=this->Usage(); CHKERRQ(ierr);
        }
        this->m_Regularization.beta[0] = betav;
        this->m_Regularization.beta[1] = betav;
    }

    if (this->m_ScaleCont.enabled && this->m_ParaCont.enabled){
        msg="\x1b[31m combined parameter and scale continuation not available \x1b[0m\n";
        ierr=PetscPrintf(PETSC_COMM_WORLD,msg.c_str()); CHKERRQ(ierr);
        ierr=this->Usage(); CHKERRQ(ierr);
    }

    PetscFunctionReturn(0);
}




/********************************************************************
 * Name: DoSetup
 * Description: setup options and accfft
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "DoSetup"
PetscErrorCode RegOpt::DoSetup()
{
    PetscErrorCode ierr;
    int nx[3],isize[3],istart[3],osize[3],ostart[3],ompthreads,nprocs,np;
    IntType alloc_max;
    ScalarType *u=NULL, fftsetuptime;
    Complex *uk=NULL;
    std::stringstream ss;

    PetscFunctionBegin;

    // set number of threads
    ierr=reg::Assert(this->m_NumThreads > 0,"omp threads < 0"); CHKERRQ(ierr);

    omp_set_dynamic(0);
    omp_set_num_threads(this->m_NumThreads);

    // check if number of threads is consistent with user options
    ompthreads=omp_get_max_threads();
    ss << "max number of openmp threads is not a match (user,set)=("
       << this->m_NumThreads <<"," << ompthreads <<")\n";
    ierr=Assert(ompthreads == this->m_NumThreads,ss.str().c_str()); CHKERRQ(ierr);
    ss.str( std::string() );
    ss.clear();

    // set up MPI/cartesian grid
    MPI_Comm_size(PETSC_COMM_WORLD, &nprocs);
    np = this->m_CartGridDims[0]*this->m_CartGridDims[1];

    // check number of procs
    if(np!=nprocs){

        if (this->m_Verbosity > 2){
            ss << "cartesian grid setup wrong: px1*px2="
                << this->m_CartGridDims[0] <<"*"<<this->m_CartGridDims[1]
                <<"="<< np << " != " << nprocs << "=p";
            ierr=DbgMsg(ss.str()); CHKERRQ(ierr);
            ss.str( std::string() ); ss.clear();
        }

        // update cartesian grid layout
        this->m_CartGridDims[0]=0;
        this->m_CartGridDims[1]=0;
        MPI_Dims_create(nprocs,2,this->m_CartGridDims);

        if (this->m_Verbosity > 2){
            ss << "switching to cartesian grid setup: "
                << this->m_CartGridDims[0] <<"x"<<this->m_CartGridDims[1];
            ierr=DbgMsg(ss.str()); CHKERRQ(ierr);
            ss.str( std::string() ); ss.clear();
        }
    }


    // initialize accft
    accfft_create_comm(PETSC_COMM_WORLD,this->m_CartGridDims,&this->m_Comm);
    accfft_init(this->m_NumThreads);

    // parse grid size for setup
    for (int i = 0; i < 3; ++i){
        nx[i] = static_cast<int>(this->m_Domain.nx[i]);
        this->m_Domain.hx[i] = PETSC_PI*2.0/static_cast<ScalarType>(nx[i]);
    }

    if (this->m_FFTPlan != NULL){
        accfft_destroy_plan(this->m_FFTPlan);
        this->m_FFTPlan = NULL;
        accfft_cleanup();
    }

    alloc_max = accfft_local_size_dft_r2c(nx,isize,istart,osize,ostart,this->m_Comm);
    u  = (ScalarType*)accfft_alloc(alloc_max);
    uk = (Complex*)accfft_alloc(alloc_max);

    // compute global and local size
    this->m_Domain.nlocal = 1;
    this->m_Domain.nglobal = 1;
    for (int i = 0; i < 3; ++i){

        this->m_Domain.isize[i] = static_cast<IntType>(isize[i]);
        this->m_Domain.istart[i] = static_cast<IntType>(istart[i]);

        this->m_Domain.osize[i] = static_cast<IntType>(osize[i]);
        this->m_Domain.ostart[i] = static_cast<IntType>(ostart[i]);

        this->m_Domain.nlocal *= static_cast<IntType>(isize[i]);
        this->m_Domain.nglobal *= this->m_Domain.nx[i];
    }

    // check if sizes are ok
    ierr=reg::Assert(this->m_Domain.nlocal > 0,"bug in setup"); CHKERRQ(ierr);
    ierr=reg::Assert(this->m_Domain.nglobal > 0,"bug in setup"); CHKERRQ(ierr);


    // set up the fft
    fftsetuptime=-MPI_Wtime();
    this->m_FFTPlan = accfft_plan_dft_3d_r2c(nx,u,(double*)uk,this->m_Comm,ACCFFT_MEASURE);
    fftsetuptime+=MPI_Wtime();

    // set the fft setup time
    this->m_Timer[FFTSETUP][LOG] = fftsetuptime;

    this->m_SetupDone=true;

    // display the options to the user
    ierr=this->DisplayOptions(); CHKERRQ(ierr);

    // clean up
    accfft_free(u);
    accfft_free(uk);


    PetscFunctionReturn(0);
}




/********************************************************************
 * Name: SetPresetParameters
 * Description: set preset parameters
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "SetPresetParameters"
PetscErrorCode RegOpt::SetPresetParameters()
{
    PetscErrorCode ierr;

    PetscFunctionBegin;

    if (this->m_SolveType == FAST_AGG){

        // use fast and aggressive method
        this->m_KKTSolverPara.fseqtype = NOFS;
        this->m_Regularization.norm = H1SN;
        this->m_KKTSolverPara.maxit = 5;
        this->m_OptPara.maxit = 20;
        this->m_OptPara.tol[2] = 1E-2;

    }
    else if (this->m_SolveType == ACC_AGG){

        // use slow and aggressive method
        this->m_Regularization.norm = H1SN;
        this->m_KKTSolverPara.fseqtype = QDFS;
        this->m_KKTSolverPara.maxit = 50;
        this->m_OptPara.maxit = 50;
        this->m_OptPara.tol[2] = 1E-2;

    }
    else if (this->m_SolveType == FAST_SMOOTH){

        // use fast and smooth method
        this->m_Regularization.norm = H2SN;
        this->m_KKTSolverPara.fseqtype = NOFS;
        this->m_KKTSolverPara.maxit = 10;
        this->m_OptPara.maxit = 20;
        this->m_OptPara.tol[2] = 1E-2;

    }
    else if (this->m_SolveType == ACC_SMOOTH){

        // use slow and smooth method
        this->m_Regularization.norm = H2SN;
        this->m_KKTSolverPara.fseqtype = QDFS;
        this->m_KKTSolverPara.maxit = 50;
        this->m_OptPara.maxit = 50;
        this->m_OptPara.tol[2] = 1E-2;

    }
    else { ierr=ThrowError("flag not defined"); CHKERRQ(ierr); }

    PetscFunctionReturn(0);
}




/********************************************************************
 * Name: DisplayOptions
 * Description: display registration options
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "GetBetaMinParaCont"
ScalarType RegOpt::GetBetaMinParaCont()
{
    if (this->m_Regularization.norm == H1)        return this->m_ParaCont.betavminh1;
    else if (this->m_Regularization.norm == H1SN) return this->m_ParaCont.betavminh1;
    else if (this->m_Regularization.norm == H2)   return this->m_ParaCont.betavminh2;
    else if (this->m_Regularization.norm == H2SN) return this->m_ParaCont.betavminh2;
    else return 1E-9;
}




/********************************************************************
 * Name: DisplayOptions
 * Description: display registration options
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "DisplayOptions"
PetscErrorCode RegOpt::DisplayOptions()
{
    PetscErrorCode ierr;
    int rank,indent,align;
    std::string msg, line;
    bool newtontype;

    PetscFunctionBegin;

    MPI_Comm_rank(PETSC_COMM_WORLD,&rank);

    indent = 40;
    align  = 30;
    line = std::string(this->m_LineLength,'-');

    // display the parameters (only on rank 0)
    if (rank == 0){

        std::cout<<std::endl;

        std::cout<< line << std::endl;
        std::cout<< " Constrained Large Deformation Diffeomorphic Registration"<<std::endl;
        std::cout<< line << std::endl;
        std::cout<< " Parallel Algorithms for Data Analysis and Simulation Group"<<std::endl;
        std::cout<< " The Institute of Computational Engineering and Sciences"<<std::endl;
        std::cout<< " The University of Texas at Austin"<<std::endl;
        std::cout<< line << std::endl;
        std::cout<< " problem setup"<<std::endl;
        std::cout<< line << std::endl;

        std::cout<< std::left << std::setw(indent) <<" problem dimensions"
                    << "(nx1,nx2,nx3,nt)=(" << this->m_Domain.nx[0] <<","
                    <<  this->m_Domain.nx[1] <<","
                    <<  this->m_Domain.nx[2] <<","
                    <<  this->m_Domain.nt <<")" <<std::endl;
        std::cout<< std::left << std::setw(indent) <<" network dimensions"
                    << this->m_CartGridDims[0] <<"x"
                    << this->m_CartGridDims[1]<<std::endl;
        std::cout<< std::left << std::setw(indent) <<" threads"
                    << this->m_NumThreads<<std::endl;
        std::cout<< std::left << std::setw(indent) <<" (ng,nl)"
                    << "(" << this->m_Domain.nglobal <<","
                    <<  this->m_Domain.nlocal <<")" <<std::endl;

        std::cout<< line << std::endl;
        std::cout<< " parameters"<<std::endl;
        std::cout<< line << std::endl;

        // display regularization model
        std::cout<< std::left << std::setw(indent) <<" regularization model v";

        if( this->m_ParaCont.strategy == PCONTBINSEARCH
            || this->m_ParaCont.strategy == PCONTREDUCESEARCH){
            switch(this->m_Regularization.norm){
                case L2:
                {
                    std::cout<<"L2-norm (betav estimated)"<<std::endl;
                    break;
                }
                case H1:
                {
                    std::cout<<"H1-norm (betav estimated)"<<std::endl;
                    break;
                }
                case H2:
                {
                    std::cout<<"H2-norm (betav estimated)"<<std::endl;
                    break;
                }
                case H1SN:
                {
                    std::cout<<"H1-seminorm (betav estimated)"<<std::endl;
                    break;
                }
                case H2SN:
                {
                    std::cout<<"H2-seminorm (betav estimated)"<<std::endl;
                    break;
                }
                default:
                {
                    ierr=ThrowError("regularization model not implemented"); CHKERRQ(ierr);
                    break;
                }
            }

            // display parameters and tolerances
            std::cout<< std::left << std::setw(indent) <<" parameter continuation";
            if( this->m_ParaCont.strategy == PCONTBINSEARCH){
                std::cout << "binary search" << std::endl;
            }
            else if( this->m_ParaCont.strategy == PCONTREDUCESEARCH){
                std::cout << "search by reduction" << std::endl;
            }
            std::cout<< std::left << std::setw(indent) <<" "
                      << std::setw(align) <<"bound det(grad(y))"
                      << this->m_RegMonitor.jacbound << std::endl;
        }
        else{
            switch(this->m_Regularization.norm){
                case L2:
                {
                    std::cout   << std::scientific << "L2-norm (betav="
                                << this->m_Regularization.beta[0]
                                << ")" <<std::endl;
                    break;
                }
                case H1:
                {
                    std::cout   << std::scientific << "H1-norm (betav="
                                << this->m_Regularization.beta[0]
                                << ", "<< this->m_Regularization.beta[1]
                                << ") "<<std::endl;
                    break;
                }
                case H2:
                {
                    std::cout   << std::scientific << "H2-norm (betav="
                                << this->m_Regularization.beta[0]
                                << ", "<< this->m_Regularization.beta[1]
                                << ")" <<std::endl;
                    break;
                }
                case H1SN:
                {
                    std::cout   << std::scientific <<  "H1-seminorm (betav="
                                <<  this->m_Regularization.beta[0]
                                << ")" <<std::endl;
                    break;
                }
                case H2SN:
                {
                    std::cout   << std::scientific << "H2-seminorm (betav="
                                << this->m_Regularization.beta[0]
                                << ")" <<std::endl;
                    break;
                }
                default:{ ierr=ThrowError("regularization model not implemented"); CHKERRQ(ierr); break; }
            }

            // display parameters and tolerances
            std::cout<< std::left << std::setw(indent) <<" parameter continuation";
            if( this->m_ParaCont.strategy == PCONTINUATION){
                std::cout << "enabled" << std::endl;
            }
            else if( this->m_ParaCont.strategy == PCONTOFF){
                std::cout << "disabled" << std::endl;
            }

        }
        if (this->m_RegModel == reg::RELAXEDSTOKES){
            // display regularization model
            std::cout<< std::left << std::setw(indent) <<" regularization model w";
            std::cout   <<  "H1-seminorm (betaw="
                        <<  this->m_Regularization.beta[2]<< ")" <<std::endl;
        }

        // display regularization model
        std::cout<< std::left << std::setw(indent) <<" PDE solver (hyperbolic)";
        switch(this->m_PDESolver){
            case RK2:
            {
                std::cout<<"second order RK method"<<std::endl;
                break;
            }
            case SL:
            {
                std::cout<<"semi-lagrangian method"<<std::endl;
                break;
            }
            default:
            {
                ierr=ThrowError("solver not implemented"); CHKERRQ(ierr);
                break;
            }
        }
        // display type of optimization method
        newtontype=false;
        std::cout<< std::left << std::setw(indent) <<" optimization method";
        switch(this->m_OptPara.method){
            case GRADDESCENT:
            {
                std::cout<< "preconditioned gradient descent method" << std::endl;
                break;
            }
            case FULLNEWTON:
            {
                std::cout<< "full newton method" << std::endl;
                newtontype=true;
                break;
            }
            case GAUSSNEWTON:
            {
                std::cout<< "gauss newton method" << std::endl;
                newtontype=true;
                break;
            }
            default:
            {
                ierr=ThrowError("optimization method not implemented"); CHKERRQ(ierr);
                break;
            }
        }

        std::cout << std::left << std::setw(indent) <<" maximal # iterations"
                  << this->m_OptPara.maxit << std::endl;

        // display optimization tolerances
        std::cout << std::left << std::setw(indent) <<" convergence tolerances"
                  << std::setw(align) <<"||g(v)|| <= tol"
                  << this->m_OptPara.tol[0] << std::endl;
        std::cout << std::left << std::setw(indent) <<" "
                  << std::setw(align) <<"||g(v)||/|J(v)| <= tol"
                  << this->m_OptPara.tol[1] << std::endl;
        std::cout << std::left << std::setw(indent) <<" "
                  << std::setw(align) <<"||g(v)||/||g(v0)|| <= tol"
                  << this->m_OptPara.tol[2] << std::endl;

        // display parameters for newton type optimization methods
        if ( newtontype ){

            std::cout << std::left << std::setw(indent)
                      <<" hessian sytem"
                      << std::setw(align) << "solver"
                      << "PCG" << std::endl;

            std::cout << std::left << std::setw(indent) <<" "
                      << std::setw(align) << "forcing sequence";

            switch(this->m_KKTSolverPara.fseqtype){
                case NOFS:
                {
                    std::cout << std::setw(align) << "disabled" << std::endl;
                    std::cout << std::left << std::setw(indent) <<" "
                              << std::setw(align) <<"absolute"
                              << this->m_KKTSolverPara.tol[0] << std::endl;
                    std::cout << std::left << std::setw(indent) <<" "
                              << std::setw(align) <<"relative"
                              << this->m_KKTSolverPara.tol[1] << std::endl;
                    break;
                }
                case QDFS:
                {
                    std::cout << "quadratic" << std::endl;
                    break;
                }
                case SLFS:
                {
                    std::cout << "superlinear" << std::endl;
                    break;
                }
                default:
                {
                    ierr=ThrowError("forcing sequence not implemented"); CHKERRQ(ierr);
                    break;
                }
            }
            std::cout << std::left << std::setw(indent) <<" "
                      << std::setw(align) <<"divergence"
                      << this->m_KKTSolverPara.tol[2] << std::endl;

            std::cout << std::left << std::setw(indent) <<" "
                      << std::setw(align) <<"maxit"
                      << this->m_KKTSolverPara.maxit << std::endl;

        }

        std::cout<< line << std::endl;

    }

    PetscFunctionReturn(0);
}





/********************************************************************
 * Name: ComputeFFTScale
 * Description: compute weight for FFT
 *******************************************************************/
ScalarType RegOpt::ComputeFFTScale()
{

    ScalarType scale = 1.0;
    for (unsigned int i=0; i < 3; ++i){
        scale *= static_cast<ScalarType>(this->m_Domain.nx[i]);
    }
    return 1.0/scale;

};





/********************************************************************
 * Name: ResetTimers
 * Description: resets all timers
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "ResetTimers"
PetscErrorCode RegOpt::ResetTimers()
{
    PetscFunctionBegin;

    for (int i = 0; i < NTIMERS; ++i){
        if (i != FFTSETUP){
            for (int j = 0; j < NVALTYPES; ++j){
                this->m_Timer[i][j] = 0.0;
            }
        }
        this->m_TimerIsRunning[i] = false;
        this->m_TempTimer[i] = 0.0;
    }

    for(int i = 0; i < 5; ++i){
        for (int j = 0; j < NVALTYPES; ++j){
            this->m_FFTTimers[i][j] = 0.0;
        }
    }

    for(int i = 0; i < 4; ++i){
        for (int j = 0; j < NVALTYPES; ++j){
            this->m_InterpTimers[i][j] = 0.0;
        }
    }

    PetscFunctionReturn(0);
}




/********************************************************************
 * Name: ResetTimer
 * Description: resets timer
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "ResetTimer"
PetscErrorCode RegOpt::ResetTimer(TimerType id)
{
    PetscFunctionBegin;

    for (int j = 0; j < NVALTYPES; ++j){
        this->m_Timer[id][j] = 0.0;
    }
    this->m_TimerIsRunning[id] = false;
    this->m_TempTimer[id] = 0.0;

    PetscFunctionReturn(0);
}




/********************************************************************
 * Name: StartTimer
 * Description: start the timer (checks if running)
 * Author: Andreas Mang
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "StartTimer"
PetscErrorCode RegOpt::StartTimer(TimerType id)
{

    PetscErrorCode ierr;
    std::string msg;

    PetscFunctionBegin;

    msg="fatal error: timer has already been started";
    ierr=Assert(this->m_TimerIsRunning[id]==false,msg); CHKERRQ(ierr);

    this->m_TempTimer[id] = -MPI_Wtime();
    this->m_TimerIsRunning[id] = true;

    PetscFunctionReturn(0);

}




/********************************************************************
 * Name: StopTimer
 * Description: stop setup timer (checks if running)
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "StopTimer"
PetscErrorCode RegOpt::StopTimer(TimerType id)
{
    PetscErrorCode ierr;
    std::string msg;

    PetscFunctionBegin;

    msg="fatal error: timer has not been started";
    ierr=Assert(this->m_TimerIsRunning[id],msg); CHKERRQ(ierr);

    this->m_TempTimer[id] += MPI_Wtime();
    this->m_Timer[id][LOG] += this->m_TempTimer[id];

    // tell the world that we stop the timer
    this->m_TimerIsRunning[id] = false;

    PetscFunctionReturn(0);
}





/********************************************************************
 * Name: ProcessTimers
 * Description: process the timers
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "ProcessTimers"
PetscErrorCode RegOpt::ProcessTimers()
{
    PetscErrorCode ierr;
    int rval,rank,nproc;
    double ival=0.0,xval=0.0;

    PetscFunctionBegin;

    MPI_Comm_rank(PETSC_COMM_WORLD,&rank);
    MPI_Comm_size(PETSC_COMM_WORLD,&nproc);

    for(int id = 0; id < NTIMERS; ++id){

        // remember input value
        ival=this->m_Timer[id][LOG];

        // get maximal execution time
        rval=MPI_Reduce(&ival,&xval,1,MPI_DOUBLE,MPI_MIN,0,PETSC_COMM_WORLD);
        ierr=Assert(rval==MPI_SUCCESS,"mpi reduce returned error"); CHKERRQ(ierr);
        this->m_Timer[id][MIN]=xval;

        // get maximal execution time
        rval=MPI_Reduce(&ival,&xval,1,MPI_DOUBLE,MPI_MAX,0,PETSC_COMM_WORLD);
        ierr=Assert(rval==MPI_SUCCESS,"mpi reduce returned error"); CHKERRQ(ierr);
        this->m_Timer[id][MAX] = xval;

        // get mean execution time
        rval=MPI_Reduce(&ival,&xval,1,MPI_DOUBLE,MPI_SUM,0,PETSC_COMM_WORLD);
        ierr=Assert(rval==MPI_SUCCESS,"mpi reduce returned error"); CHKERRQ(ierr);
        this->m_Timer[id][AVG]=xval;

        if (rank == 0){ this->m_Timer[id][AVG] /= static_cast<double>(nproc); }

    }

    for(int i = 0; i < 5; ++i){

        // remember input value
        ival=this->m_FFTTimers[i][LOG];

        // get maximal execution time
        rval=MPI_Reduce(&ival,&xval,1,MPI_DOUBLE,MPI_MIN,0,PETSC_COMM_WORLD);
        ierr=Assert(rval==MPI_SUCCESS,"mpi reduce returned error"); CHKERRQ(ierr);
        this->m_FFTTimers[i][MIN] = xval;

        // get maximal execution time
        rval=MPI_Reduce(&ival,&xval,1,MPI_DOUBLE,MPI_MAX,0,PETSC_COMM_WORLD);
        ierr=Assert(rval==MPI_SUCCESS,"mpi reduce returned error"); CHKERRQ(ierr);
        this->m_FFTTimers[i][MAX] = xval;

        // get mean execution time
        rval=MPI_Reduce(&ival,&xval,1,MPI_DOUBLE,MPI_SUM,0,PETSC_COMM_WORLD);
        ierr=Assert(rval==MPI_SUCCESS,"mpi reduce returned error"); CHKERRQ(ierr);
        this->m_FFTTimers[i][AVG] = xval;

        if (rank == 0){ this->m_FFTTimers[i][AVG] /= static_cast<double>(nproc); }

    }

    for(int i = 0; i < 4; ++i){

        // remember input value
        ival=this->m_InterpTimers[i][LOG];

        // get maximal execution time
        rval=MPI_Reduce(&ival,&xval,1,MPI_DOUBLE,MPI_MIN,0,PETSC_COMM_WORLD);
        ierr=Assert(rval==MPI_SUCCESS,"mpi reduce returned error"); CHKERRQ(ierr);
        this->m_InterpTimers[i][MIN] = xval;

        // get maximal execution time
        rval=MPI_Reduce(&ival,&xval,1,MPI_DOUBLE,MPI_MAX,0,PETSC_COMM_WORLD);
        ierr=Assert(rval==MPI_SUCCESS,"mpi reduce returned error"); CHKERRQ(ierr);
        this->m_InterpTimers[i][MAX] = xval;

        // get mean execution time
        rval=MPI_Reduce(&ival,&xval,1,MPI_DOUBLE,MPI_SUM,0,PETSC_COMM_WORLD);
        ierr=Assert(rval==MPI_SUCCESS,"mpi reduce returned error"); CHKERRQ(ierr);
        this->m_InterpTimers[i][AVG] = xval;

        if (rank == 0){ this->m_InterpTimers[i][AVG] /= static_cast<double>(nproc); }

    }


    PetscFunctionReturn(0);

}




/********************************************************************
 * Name: ResetCounter
 * Description: resets counters
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "ResetCounters"
PetscErrorCode RegOpt::ResetCounters()
{
    PetscFunctionBegin;

    for(int i = 0; i < NCOUNTERS; ++i){
        this->m_Counter[i] = 0;
    }

    PetscFunctionReturn(0);
}




/********************************************************************
 * Name: ResetCounter
 * Description: resets counter
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "ResetCounter"
PetscErrorCode RegOpt::ResetCounter(CounterType id)
{
    PetscFunctionBegin;

    this->m_Counter[id] = 0;

    PetscFunctionReturn(0);
}




/********************************************************************
 * Name: WriteLogFile
 * Description: write log results to file
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "WriteLogFile"
PetscErrorCode RegOpt::WriteLogFile()
{
    PetscErrorCode ierr;
    std::string filename,fn,line;
    std::ofstream logwriter;
    std::stringstream ss, ssnum;
    int rank, nnum, nstr, nproc;

    PetscFunctionBegin;

    // get rank
    MPI_Comm_rank(PETSC_COMM_WORLD,&rank);
    MPI_Comm_size(PETSC_COMM_WORLD,&nproc);


    line = std::string(this->m_LineLength,'-');

    // write out logfile
    if (rank == 0){

        nnum = 20; nstr = 20;
        filename = this->m_XFolder + "registration-performance";
        fn = filename + ".log";


        // create output file
        logwriter.open(fn.c_str());
        ierr=Assert(logwriter.is_open(),"could not open file for writing"); CHKERRQ(ierr);

        logwriter << std::scientific;
        logwriter << line <<std::endl;
        logwriter << "# problem setup" <<std::endl;
        logwriter << line <<std::endl;
        ss  << this->m_Domain.nx[0] << " x "
            << this->m_Domain.nx[1] << " x "
            << this->m_Domain.nx[2];

        logwriter << std::left
                  << std::setw(nstr) << " nx" << std::right
                  << std::setw(nnum) << ss.str() << std::endl;
        ss.clear(); ss.str(std::string());

        logwriter << std::left
                  << std::setw(nstr) << " nt" << std::right
                  << std::setw(nnum) << this->m_Domain.nt << std::endl;

        logwriter << std::left
                  << std::setw(nstr) << " n" << std::right
                  << std::setw(nnum) << this->GetNGlobal() << std::endl;

        logwriter << std::left
                  << std::setw(nstr) << " nl" << std::right
                  << std::setw(nnum) << this->GetNLocal() << std::endl;

        logwriter << std::left
                  << std::setw(nstr) << " nmpi" << std::right
                  << std::setw(nnum) << nproc << std::endl;

        ss << this->m_CartGridDims[0] << " x " << this->m_CartGridDims[1];
        logwriter << std::left
                  << std::setw(nstr) << " proc grid" << std::right
                  << std::setw(nnum) << ss.str() << std::endl;
        ss.clear(); ss.str(std::string());

        logwriter << std::left
                  << std::setw(nstr) << " num threads" << std::right
                  << std::setw(nnum) << this->m_NumThreads << std::endl;

        logwriter << std::endl;
        logwriter << line <<std::endl;
        logwriter << "# timers" <<std::endl;
        logwriter << line <<std::endl;

        // write heading
        ss  << std::scientific << std::left
            << std::setw(nstr) << " " << std::right
            << std::setw(nnum) << "min(p)"
            << std::setw(nnum) << "max(p)"
            << std::setw(nnum) << "mean(p)"
            << std::setw(nnum) << "max(p)/numeval";
        logwriter << ss.str() << std::endl;
        ss.clear(); ss.str(std::string());


        // if time has been logged
        if (this->m_Timer[T2SEXEC][LOG] > 0.0){
            ss  << std::scientific << std::left
                << std::setw(nstr) << " time to solution" << std::right
                << std::setw(nnum) << this->m_Timer[T2SEXEC][MIN]
                << std::setw(nnum) << this->m_Timer[T2SEXEC][MAX]
                << std::setw(nnum) << this->m_Timer[T2SEXEC][AVG]
                << std::setw(nnum) << "n/a";
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }


        // if time has been logged
        if (this->m_Timer[PDEEXEC][LOG] > 0.0){
            ierr=Assert(this->m_Counter[PDESOLVE] > 0,"bug in counter"); CHKERRQ(ierr);
            ss  << std::scientific << std::left
                << std::setw(nstr) << " pde solves" << std::right
                << std::setw(nnum) << this->m_Timer[PDEEXEC][MIN]
                << std::setw(nnum) << this->m_Timer[PDEEXEC][MAX]
                << std::setw(nnum) << this->m_Timer[PDEEXEC][AVG]
                << std::setw(nnum) << this->m_Timer[PDEEXEC][MAX]/static_cast<ScalarType>(this->m_Counter[PDESOLVE]);
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        // if time has been logged
        if (this->m_Timer[OBJEXEC][LOG] > 0.0){
            ierr=Assert(this->m_Counter[OBJEVAL] > 0,"bug in counter"); CHKERRQ(ierr);
            ss  << std::scientific << std::left
                << std::setw(nstr) << " obj eval" << std::right
                << std::setw(nnum) << this->m_Timer[OBJEXEC][MIN]
                << std::setw(nnum) << this->m_Timer[OBJEXEC][MAX]
                << std::setw(nnum) << this->m_Timer[OBJEXEC][AVG]
                << std::setw(nnum) << this->m_Timer[PDEEXEC][MAX]/static_cast<ScalarType>(this->m_Counter[OBJEVAL]);
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        // if time has been logged
        if (this->m_Timer[GRADEXEC][LOG] > 0.0){
            ss  << std::scientific << std::left
                << std::setw(nstr) << " grad eval" << std::right
                << std::setw(nnum) << this->m_Timer[GRADEXEC][MIN]
                << std::setw(nnum) << this->m_Timer[GRADEXEC][MAX]
                << std::setw(nnum) << this->m_Timer[GRADEXEC][AVG]
                << std::setw(nnum) << this->m_Timer[GRADEXEC][MAX]/static_cast<ScalarType>(this->m_Counter[GRADEVAL]);
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        // if time has been logged
        if (this->m_Timer[HMVEXEC][LOG] > 0.0){
            ierr=Assert(this->m_Counter[HESSMATVEC] > 0,"bug in counter"); CHKERRQ(ierr);
            ss  << std::scientific << std::left
                << std::setw(nstr) << " hess mat vec" << std::right
                << std::setw(nnum) << this->m_Timer[HMVEXEC][MIN]
                << std::setw(nnum) << this->m_Timer[HMVEXEC][MAX]
                << std::setw(nnum) << this->m_Timer[HMVEXEC][AVG]
                << std::setw(nnum) << this->m_Timer[HMVEXEC][MAX]/static_cast<ScalarType>(this->m_Counter[HESSMATVEC]);
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        // if time has been logged
        if (this->m_Timer[PMVEXEC][LOG] > 0.0){
            ierr=Assert(this->m_Counter[PCMATVEC] > 0,"bug in counter"); CHKERRQ(ierr);
            ss  << std::scientific << std::left
                << std::setw(nstr) << " pc mat vec" << std::right
                << std::setw(nnum) << this->m_Timer[PMVEXEC][MIN]
                << std::setw(nnum) << this->m_Timer[PMVEXEC][MAX]
                << std::setw(nnum) << this->m_Timer[PMVEXEC][AVG]
                << std::setw(nnum) << this->m_Timer[PMVEXEC][MAX]/static_cast<ScalarType>(this->m_Counter[PCMATVEC]);
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }


        // if time has been logged
        if (this->m_Timer[FFTSETUP][LOG] > 0.0){
            ss  << std::scientific << std::left
                << std::setw(nstr) << " FFT setup" << std::right
                << std::setw(nnum) << this->m_Timer[FFTSETUP][MIN]
                << std::setw(nnum) << this->m_Timer[FFTSETUP][MAX]
                << std::setw(nnum) << this->m_Timer[FFTSETUP][AVG]
                << std::setw(nnum) << "n/a";
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        // if time has been logged
        if (this->m_FFTTimers[2][LOG] > 0.0){
            ierr=Assert(this->m_Counter[FFT] > 0,"bug in counter"); CHKERRQ(ierr);
            ss  << std::scientific << std::left
                << std::setw(nstr) << " FFT communication" << std::right
                << std::setw(nnum) << this->m_FFTTimers[2][MIN]
                << std::setw(nnum) << this->m_FFTTimers[2][MAX]
                << std::setw(nnum) << this->m_FFTTimers[2][AVG]
                << std::setw(nnum) << this->m_FFTTimers[2][MAX]/static_cast<ScalarType>(this->m_Counter[FFT]);
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        // if time has been logged
        if (this->m_FFTTimers[4][LOG] > 0.0){
            ierr=Assert(this->m_Counter[FFT] > 0,"bug in counter"); CHKERRQ(ierr);
            ss  << std::scientific << std::left
                << std::setw(nstr) << " FFT execution" << std::right
                << std::setw(nnum) << this->m_FFTTimers[4][MIN]
                << std::setw(nnum) << this->m_FFTTimers[4][MAX]
                << std::setw(nnum) << this->m_FFTTimers[4][AVG]
                << std::setw(nnum) << this->m_FFTTimers[4][MAX]/static_cast<ScalarType>(this->m_Counter[FFT]);
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        // if time has been logged
        if (this->m_InterpTimers[0][LOG] > 0.0){
            ierr=Assert(this->m_Counter[IP] > 0 && this->m_Counter[IPVEC] > 0,"bug in counter"); CHKERRQ(ierr);
            ss  << std::scientific << std::left
                << std::setw(nstr) << " interp comm" << std::right
                << std::setw(nnum) << this->m_InterpTimers[0][MIN]
                << std::setw(nnum) << this->m_InterpTimers[0][MAX]
                << std::setw(nnum) << this->m_InterpTimers[0][AVG]
                << std::setw(nnum) << this->m_InterpTimers[0][MAX]/static_cast<ScalarType>(this->m_Counter[IP] + this->m_Counter[IPVEC]);
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        // if time has been logged
        if (this->m_InterpTimers[1][LOG] > 0.0){
            ierr=Assert(this->m_Counter[IP] > 0 && this->m_Counter[IPVEC] > 0,"bug in counter"); CHKERRQ(ierr);
            ss  << std::scientific << std::left
                << std::setw(nstr) << " interp exec" << std::right
                << std::setw(nnum) << this->m_InterpTimers[1][MIN]
                << std::setw(nnum) << this->m_InterpTimers[1][MAX]
                << std::setw(nnum) << this->m_InterpTimers[1][AVG]
                << std::setw(nnum) << this->m_InterpTimers[1][MAX]/static_cast<ScalarType>(this->m_Counter[IP] + this->m_Counter[IPVEC]);
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        // if time has been logged
        if (this->m_InterpTimers[2][LOG] > 0.0){
            ierr=Assert(this->m_Counter[IP] > 0 && this->m_Counter[IPVEC] > 0,"bug in counter"); CHKERRQ(ierr);
            ss  << std::scientific << std::left
                << std::setw(nstr) << " interp alloc" << std::right
                << std::setw(nnum) << this->m_InterpTimers[2][MIN]
                << std::setw(nnum) << this->m_InterpTimers[2][MAX]
                << std::setw(nnum) << this->m_InterpTimers[2][AVG]
                << std::setw(nnum) << this->m_InterpTimers[2][MAX]/static_cast<ScalarType>(this->m_Counter[IP] + this->m_Counter[IPVEC]);
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        // if time has been logged
        if (this->m_InterpTimers[3][LOG] > 0.0){
            ierr=Assert(this->m_Counter[IP] > 0 && this->m_Counter[IPVEC] > 0,"bug in counter"); CHKERRQ(ierr);
            ss  << std::scientific << std::left
                << std::setw(nstr) << " interp sort" << std::right
                << std::setw(nnum) << this->m_InterpTimers[3][MIN]
                << std::setw(nnum) << this->m_InterpTimers[3][MAX]
                << std::setw(nnum) << this->m_InterpTimers[3][AVG]
                << std::setw(nnum) << this->m_InterpTimers[3][MAX]/static_cast<ScalarType>(this->m_Counter[IP] + this->m_Counter[IPVEC]);
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        logwriter << std::endl;
        logwriter << line <<std::endl;
        logwriter << "# counters" <<std::endl;
        logwriter << line <<std::endl;

        if (this->m_Counter[OBJEVAL] > 0){
            ss  << std::scientific << std::left
                << std::setw(nstr) << " objective evals" << std::right
                << std::setw(nnum) << this->m_Counter[OBJEVAL];
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        if (this->m_Counter[GRADEVAL] > 0){
            ss  << std::scientific << std::left
                << std::setw(nstr) << " grad evals" << std::right
                << std::setw(nnum) << this->m_Counter[GRADEVAL];
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        if (this->m_Counter[PDESOLVE] > 0){
            ss  << std::scientific << std::left
                << std::setw(nstr) << " PDE solves" << std::right
                << std::setw(nnum) << this->m_Counter[PDESOLVE];
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        if (this->m_Counter[HESSMATVEC] > 0){
            ss  << std::scientific << std::left
                << std::setw(nstr) << " hess mat vecs" << std::right
                << std::setw(nnum) << this->m_Counter[HESSMATVEC];
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        if (this->m_Counter[PCMATVEC] > 0){
            ss  << std::scientific << std::left
                << std::setw(nstr) << " pc mat vecs" << std::right
                << std::setw(nnum) << this->m_Counter[PCMATVEC];
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());

        }

        if (this->m_Counter[IP] > 0){
            ss  << std::scientific << std::left
                << std::setw(nstr) << " ips sca" << std::right
                << std::setw(nnum) << this->m_Counter[IP];
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        if (this->m_Counter[IPVEC] > 0){
            ss  << std::scientific << std::left
                << std::setw(nstr) << " ips vec" << std::right
                << std::setw(nnum) << this->m_Counter[IPVEC];
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }

        if (this->m_Counter[FFT] > 0){
            ss  << std::scientific << std::left
                << std::setw(nstr) << " ffts" << std::right
                << std::setw(nnum) << this->m_Counter[FFT];
            logwriter << ss.str() << std::endl;
            ss.clear(); ss.str(std::string());
        }


    }

    PetscFunctionReturn(0);
}




/********************************************************************
 * Name: DisplayTimeToSolution
 * Description: displays the global exection time
 *******************************************************************/
#undef __FUNCT__
#define __FUNCT__ "DisplayTimeToSolution"
PetscErrorCode RegOpt::DisplayTimeToSolution()
{

    PetscErrorCode ierr;
    double hours,minutes,seconds,millisec,time;
    int rank;
    std::stringstream ss;
    std::string line;
    PetscFunctionBegin;

    time = this->m_Timer[T2SEXEC][MAX];

    hours    = time / 3600.0;
    minutes  = (hours   - floor(hours)  ) *   60.0;
    seconds  = (minutes - floor(minutes)) *   60.0;
    millisec = (seconds - floor(seconds)) * 1000.0;

    MPI_Comm_rank(PETSC_COMM_WORLD,&rank);

    line=std::string(this->m_LineLength,'-');

    MPI_Comm_rank(PETSC_COMM_WORLD,&rank);
    ierr=PetscPrintf(PETSC_COMM_WORLD,"%s\n",line.c_str()); CHKERRQ(ierr);

    ss  << "computation finished ( elapsed cpu time "
        << floor(hours) << " h "
        << floor(minutes) << " m "
        << floor(seconds) << " s "
        << floor(millisec) << " ms )";

    ierr=Msg(ss.str()); CHKERRQ(ierr);
    ierr=PetscPrintf(PETSC_COMM_WORLD,"%s\n",line.c_str()); CHKERRQ(ierr);

    PetscFunctionReturn(0);
}



} // end of namespace

#endif //_REGOPT_CPP_
