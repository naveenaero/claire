#ifndef _TAOINTERFACEREGISTRATION_CPP
#define _TAOINTERFACEREGISTRATION_CPP_

#include "TaoInterfaceRegistration.hpp"




namespace reg
{




/****************************************************************************
 * @brief general purpose function to evaluate objective
 * @param tao pointer to tao solver
 * @param x iterate x objective is to be evaluated at
 * @param Jx objective value J(x)
 * @param ptr pointer to optimziation problem (has to be implemented by user)
 ****************************************************************************/
#undef __FUNCT__
#define __FUNCT__ "EvaluateObjective"
PetscErrorCode EvaluateObjective(Tao tao,Vec x,ScalarType* Jx,void* ptr)
{
    PetscErrorCode ierr;
    OptimizationProblem *optprob = NULL;

    PetscFunctionBegin;

    (void)tao;
    optprob = (OptimizationProblem*)ptr;
    ierr=Assert(optprob!=NULL,"null pointer"); CHKERRQ(ierr);

    // compute objective value
    ierr=optprob->EvaluateObjective(Jx,x); CHKERRQ(ierr);

    PetscFunctionReturn(0);
}




/****************************************************************************
 * @brief general purpose function to evaluate the gradient
 * @param tao pointer to tao solver
 * @param x iterate x gradient is to be evaluated at
 * @param gx gradient of objective functional, i.e. g(x)
 * @param ptr pointer to actual optimziation problem
 ****************************************************************************/
#undef __FUNCT__
#define __FUNCT__ "EvaluateGradient"
PetscErrorCode EvaluateGradient(Tao tao, Vec x, Vec gx, void* ptr)
{
    PetscErrorCode ierr;
    OptimizationProblem* optprob = NULL;

    PetscFunctionBegin;

    (void)tao;
    optprob = (OptimizationProblem*)ptr;
    ierr=Assert(optprob!=NULL,"user is null pointer"); CHKERRQ(ierr);

    // compute gradient
    ierr=optprob->EvaluateGradient(gx,x); CHKERRQ(ierr);

    PetscFunctionReturn(0);
}




/****************************************************************************
 * @brief general purpose function to evaluate objective and gradient
 * @param tao pointer to tao solver
 * @param x iterate x gradient is to be evaluated at
 * @param Jx objective value J(x)
 * @param gx gradient of objective functional, i.e. g(x)
 * @param  ptr pointer to actual optimziation problem
 ****************************************************************************/
#undef __FUNCT__
#define __FUNCT__ "EvaluateObjectiveGradient"
PetscErrorCode EvaluateObjectiveGradient(Tao tao, Vec x, ScalarType* Jx, Vec gx, void* ptr)
{
    PetscErrorCode ierr;
    OptimizationProblem* optprob = NULL;
    (void)tao;

    PetscFunctionBegin;

    // cast pointer
    optprob = (OptimizationProblem*)ptr;
    ierr=Assert(optprob!=NULL,"user is null pointer"); CHKERRQ(ierr);

    // evaluate objective and gradient
    ierr=optprob->EvaluateObjective(Jx,x); CHKERRQ(ierr);
    ierr=optprob->EvaluateGradient(gx,x); CHKERRQ(ierr);

    PetscFunctionReturn(0);
}




/****************************************************************************
 * @brief this function is called before the hessian matvec; I used to have
 * the initialization of the krylov method here; it's now moved to the
 * preprocessing routine for the krylov method;
 * @param tao pointer to tao solver
 * @param x iterate x objective is to be evaluated at
 * @param H hessian matrix (can be matrix free)
 * @param Hpre preconditioner (can be matrix free)
 * @param ptr pointer to optimzation problem (has to be implemented by user)
 ****************************************************************************/
#undef __FUNCT__
#define __FUNCT__ "EvaluateHessian"
PetscErrorCode EvaluateHessian(Tao tao,Vec x,Mat H,Mat Hpre,void* ptr)
{
    PetscErrorCode ierr=0;
    (void)tao; (void)x; (void)H; (void)Hpre; (void)ptr;

    PetscFunctionReturn(ierr);
}




/****************************************************************************
 * @brief computes the hessian matrix vector product Hx = H*xtilde
 ****************************************************************************/
#undef __FUNCT__
#define __FUNCT__ "HessianMatVec"
PetscErrorCode HessianMatVec(Mat H, Vec x, Vec Hx)
{
    PetscErrorCode ierr;
    void* ptr;
    OptimizationProblem *optprob = NULL;

    PetscFunctionBegin;

    ierr=MatShellGetContext(H,(void**)&ptr); CHKERRQ(ierr);
    optprob = (OptimizationProblem*)ptr;
    ierr=Assert(optprob!=NULL,"null pointer"); CHKERRQ(ierr);

    // apply hessian
    ierr=optprob->HessianMatVec(Hx,x); CHKERRQ(ierr);

    PetscFunctionReturn(0);
}




/****************************************************************************
 * @brief computes the matrix vector product Px
 ****************************************************************************/
#undef __FUNCT__
#define __FUNCT__ "PrecondMatVec"
PetscErrorCode PrecondMatVec(PC Hpre, Vec x, Vec Hprex)
{
    PetscErrorCode ierr;
    void* ptr;
    PrecondReg *preconditioner = NULL;

    PetscFunctionBegin;

    ierr=PCShellGetContext(Hpre,&ptr); CHKERRQ(ierr);
    preconditioner = (PrecondReg*)ptr;
    ierr=Assert(preconditioner!=NULL,"null pointer"); CHKERRQ(ierr);

    // apply hessian
    ierr=preconditioner->MatVec(Hprex,x); CHKERRQ(ierr);

    PetscFunctionReturn(0);
}




/****************************************************************************
 * @brief convergence test for optimization
 * @param tao pointer to tao solver
 * @param ptr pointer to optimziation problem (has to be implemented by user)
 ****************************************************************************/
#undef __FUNCT__
#define __FUNCT__ "CheckConvergence"
PetscErrorCode CheckConvergence(Tao tao, void* ptr)
{
    PetscErrorCode ierr;
    OptimizationProblem* optprob=NULL;
    IntType iter,maxiter;
    ScalarType J,gnorm,step,gatol,grtol,gttol,g0norm,minstep;

    PetscFunctionBegin;

    optprob = static_cast<OptimizationProblem*>(ptr);
    ierr=Assert(optprob!=NULL,"null pointer"); CHKERRQ(ierr);

    minstep = std::pow(2,10);
    minstep = 1.0/minstep;

    // get initial gradient
    g0norm = optprob->GetInitialGradNorm();
    g0norm = (g0norm > 0.0) ? g0norm : 1.0;

#if (PETSC_VERSION_MAJOR >= 3) && (PETSC_VERSION_MINOR >= 7)
    ierr=TaoGetTolerances(tao,&gatol,&grtol,&gttol); CHKERRQ(ierr);
#else
    ierr=TaoGetTolerances(tao,NULL,NULL,&gatol,&grtol,&gttol); CHKERRQ(ierr);
#endif

    ierr=TaoGetMaximumIterations(tao,&maxiter); CHKERRQ(ierr);
    ierr=TaoGetSolutionStatus(tao,&iter,&J,&gnorm,NULL,&step,NULL); CHKERRQ(ierr);

    // check for NaN value
    if ( PetscIsInfOrNanReal(J) ){
        ierr=WrngMsg("objective value is NaN"); CHKERRQ(ierr);
        ierr=TaoSetConvergedReason(tao,TAO_DIVERGED_NAN); CHKERRQ(ierr);
        PetscFunctionReturn(0);
    }

    // check for NaN value
    if ( PetscIsInfOrNanReal(gnorm) ){
        ierr=WrngMsg("||g|| is NaN"); CHKERRQ(ierr);
        ierr=TaoSetConvergedReason(tao,TAO_DIVERGED_NAN); CHKERRQ(ierr);
        PetscFunctionReturn(0);
    }

    // convergence criterium met
    if (iter > maxiter){
        ierr=TaoSetConvergedReason(tao,TAO_DIVERGED_MAXITS); CHKERRQ(ierr);
        PetscFunctionReturn(0);
    }

    // convergence criterium met
    if ( gnorm < gatol ){
        ierr=TaoSetConvergedReason(tao,TAO_CONVERGED_GATOL); CHKERRQ(ierr);
        PetscFunctionReturn(0);
    }

    // convergence criterium met
    if ( gnorm < gttol*g0norm ){
        ierr=TaoSetConvergedReason(tao,TAO_CONVERGED_GTTOL); CHKERRQ(ierr);
        PetscFunctionReturn(0);
    }

    // step size to small (essentially means, line search failed)
    if ( step < minstep ){
        ierr=TaoSetConvergedReason(tao,TAO_CONVERGED_STEPTOL); CHKERRQ(ierr);
        PetscFunctionReturn(0);
    }

    // if we're here, we're good to go
    ierr=TaoSetConvergedReason(tao,TAO_CONTINUE_ITERATING); CHKERRQ(ierr);

    // go home
    PetscFunctionReturn(0);

}




/****************************************************************************
 * @brief monitor the optimization process
 * @param tao pointer to tao solver
 * @param ptr pointer to optimziation problem (has to be implemented by user)
 ****************************************************************************/
#undef __FUNCT__
#define __FUNCT__ "OptimizationMonitor"
PetscErrorCode OptimizationMonitor(Tao tao, void* ptr)
{
    PetscErrorCode ierr;
    IntType iter,nl,ng;
    int iterdisp;
    char msg[256];
    ScalarType J,gnorm,step,D,J0,D0,gnorm0;
    OptimizationProblem* optprob = NULL;
    TaoConvergedReason convreason;
    TaoLineSearch ls=NULL;
    TaoLineSearchConvergedReason lsconvreason;
    Vec x=NULL,g=NULL;

    PetscFunctionBegin;

    optprob = static_cast<OptimizationProblem*>(ptr);
    ierr=Assert(optprob!=NULL,"null pointer"); CHKERRQ(ierr);

    // reinit the initialization of the preconditioner
    optprob->GetOptions()->PrecondSetupDone(false);

    if (optprob->GetOptions()->GetVerbosity() > 1){

        nl = optprob->GetOptions()->GetDomainPara().nlocal;
        ng = optprob->GetOptions()->GetDomainPara().nglobal;

        ierr=VecCreate(x,3*nl,3*ng); CHKERRQ(ierr);
        ierr=VecCreate(g,3*nl,3*ng); CHKERRQ(ierr);

        ierr=TaoGetLineSearch(tao,&ls); CHKERRQ(ierr);
        ierr=TaoLineSearchGetSolution(ls,x,&J,g,&step,&lsconvreason); CHKERRQ(ierr);
        ierr=DispLSConvReason(lsconvreason); CHKERRQ(ierr);

        if (x != NULL){ ierr=VecDestroy(&x); CHKERRQ(ierr); x=NULL;};
        if (g != NULL){ ierr=VecDestroy(&g); CHKERRQ(ierr); g=NULL;};

    }

    // get current iteration, objective value, norm of gradient, norm of
    // contraint, step length / trust region radius and termination reason
    ierr=TaoGetSolutionStatus(tao,&iter,&J,&gnorm,NULL,&step,&convreason); CHKERRQ(ierr);

    // remember current iterate
    optprob->IncrementIterations();

    // tao: display convergence reason
    if (optprob->GetOptions()->GetVerbosity() > 0){
        ierr=DispTaoConvReason(convreason); CHKERRQ(ierr);
    }

    // compute l2 distance at current iteration
    ierr=optprob->EvaluateDistanceMeasure(&D); CHKERRQ(ierr);
    D*=optprob->GetOptions()->GetLebesqueMeasure();

    // get initial gradient
    gnorm0 = optprob->GetInitialGradNorm();
    gnorm0 = (gnorm0 > 0.0) ? gnorm0 : 1.0;

    // get initial l2 distance
    D0 = optprob->GetInitialDistanceVal();
    D0 = (D0 > 0.0) ? D0 : 1.0;

    // get initial objective value
    J0 = optprob->GetInitialObjVal();
    J0 = (J0 > 0.0) ? J0 : 1.0;

    // get the solution vector and finalize the iteration
    ierr=TaoGetSolutionVector(tao,&x); CHKERRQ(ierr);
    ierr=optprob->FinalizeIteration(x); CHKERRQ(ierr);

    // display progress to user
    iterdisp = static_cast<int>(iter);
    sprintf(msg,"  %03d  %-20.12E %-20.12E %-20.12E %-20.12E %.6f",iterdisp,J/J0,D/D0,gnorm/gnorm0,gnorm,step);
    PetscPrintf(MPI_COMM_WORLD,"%-80s\n",msg);


    // go home
    PetscFunctionReturn(0);
}




/****************************************************************************
 * @brief display the convergence reason of the KSP method
 ****************************************************************************/
#undef __FUNCT__
#define __FUNCT__ "DispLSConvReason"
PetscErrorCode DispLSConvReason(TaoLineSearchConvergedReason flag)
{
    PetscErrorCode ierr;
    std::string msg;

    PetscFunctionBegin;

    switch(flag){
        case TAOLINESEARCH_FAILED_INFORNAN:
        {
            msg="line search: function evaluation gave INF or NAN";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAOLINESEARCH_FAILED_BADPARAMETER:
        {
            msg="line search: bad parameter detected";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAOLINESEARCH_FAILED_ASCENT:
        {
            msg="line search: search direction is not a descent direction";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAOLINESEARCH_HALTED_MAXFCN:
        {
            msg="line search: maximum number of function evaluations reached";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAOLINESEARCH_HALTED_UPPERBOUND:
        {
            msg="line search: step size reached upper bound";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAOLINESEARCH_HALTED_LOWERBOUND:
        {
            msg="line search: step size reached lower bound";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAOLINESEARCH_HALTED_RTOL:
        {
            msg="line search: range of uncertainty is smaller than given tolerance";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAOLINESEARCH_HALTED_OTHER:
        {
            msg="line search: line search stopped (other)";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAOLINESEARCH_CONTINUE_ITERATING:
        {
            // do nothing, cause everything's fine
            break;
        }
        case TAOLINESEARCH_SUCCESS:
        {
            msg="line search was successfull";
            ierr=DbgMsg(msg); CHKERRQ(ierr);
            break;
        }
        default:
        {
            msg="LS: reason not defined";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }

    }

    PetscFunctionReturn(0);
}




/****************************************************************************
 * @brief display the convergence reason of the optimizer
 ****************************************************************************/
#undef __FUNCT__
#define __FUNCT__ "DispTaoConvReason"
PetscErrorCode DispTaoConvReason(TaoConvergedReason flag)
{
    PetscErrorCode ierr;
    std::string msg;

    PetscFunctionBegin;

    switch(flag){
        case TAO_CONVERGED_GATOL:
        {
            msg="solver converged: ||g(x)|| <= tol";
            ierr=DbgMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAO_CONVERGED_GRTOL:
        {
            msg="solver converged: ||g(x)||/J(x) <= tol";
            ierr=DbgMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAO_CONVERGED_GTTOL:
        {
            msg="solver converged: ||g(x)||/||g(x0)|| <= tol";
            ierr=DbgMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAO_CONVERGED_STEPTOL:
        {
            msg="step size too small";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAO_CONVERGED_MINF:
        {
            msg="objective value to small";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAO_CONVERGED_USER:
        {
            msg="user defined convergence criteria met";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAO_DIVERGED_MAXITS:
        {
            msg="maximum number of iterations reached";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAO_DIVERGED_NAN:
        {
            msg="numerical problems (NAN detected)";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAO_DIVERGED_MAXFCN:
        {
            msg="maximal number of function evaluations reached";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAO_DIVERGED_LS_FAILURE:
        {
            msg="line search failed";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAO_DIVERGED_TR_REDUCTION:
        {
            msg="trust region failed";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAO_DIVERGED_USER:
        {
            msg="user defined divergence criterion met";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }
        case TAO_CONTINUE_ITERATING:
        {
            // display nothing
            break;
        }
        default:
        {
            msg="convergence reason not defined";
            ierr=WrngMsg(msg); CHKERRQ(ierr);
            break;
        }

    }

    PetscFunctionReturn(0);
}




} // end of name space




#endif // _TAOINTERFACEREGISTRATION_H_
