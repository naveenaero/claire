/*************************************************************************
 *  Copyright (c) 2018.
 *  All rights reserved.
 *  This file is part of the CLAIRE library.
 *
 *  CLAIRE is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CLAIRE is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CLAIRE.  If not, see <http://www.gnu.org/licenses/>.
 ************************************************************************/

#ifndef _DISTANCEMEASURESL2_CPP_
#define _DISTANCEMEASURESL2_CPP_

#include "DistanceMeasureSL2.hpp"




namespace reg {




/********************************************************************
 * @brief default constructor
 *******************************************************************/
DistanceMeasureSL2::DistanceMeasureSL2() : SuperClass() {
}




/********************************************************************
 * @brief default destructor
 *******************************************************************/
DistanceMeasureSL2::~DistanceMeasureSL2() {
    this->ClearMemory();
}




/********************************************************************
 * @brief constructor
 *******************************************************************/
DistanceMeasureSL2::DistanceMeasureSL2(RegOpt* opt) : SuperClass(opt) {
}




/********************************************************************
 * @brief clean up
 *******************************************************************/
PetscErrorCode DistanceMeasureSL2::ClearMemory() {
    PetscErrorCode ierr = 0;
    PetscFunctionBegin;

    PetscFunctionReturn(ierr);
}




/********************************************************************
 * @brief evaluate the functional (i.e., the distance measure)
 * D = (1/2)*||m1 - mR||_L2
 *******************************************************************/
PetscErrorCode DistanceMeasureSL2::EvaluateFunctional(ScalarType* D) {
    PetscErrorCode ierr = 0;
    ScalarType *p_mr = NULL, *p_m = NULL, *p_w = NULL;
    IntType nt, nc, nl, l;
    int rval;
    ScalarType dr, value, l2distance;

    PetscFunctionBegin;

    this->m_Opt->Enter(__func__);

    ierr = Assert(this->m_StateVariable != NULL, "null pointer"); CHKERRQ(ierr);
    ierr = Assert(this->m_ReferenceImage != NULL, "null pointer"); CHKERRQ(ierr);

    // get sizes
    nt = this->m_Opt->m_Domain.nt;
    nc = this->m_Opt->m_Domain.nc;
    nl = this->m_Opt->m_Domain.nl;

    ierr = VecGetArray(this->m_StateVariable, &p_m); CHKERRQ(ierr);
    ierr = VecGetArray(this->m_ReferenceImage, &p_mr); CHKERRQ(ierr);

    l = nt*nl*nc;
    value = 0.0;
    if (this->m_Mask != NULL) {
        // mask objective functional
        ierr = VecGetArray(this->m_Mask, &p_w); CHKERRQ(ierr);
        for (IntType i = 0; i < nc*nl; ++i) {
            dr = (p_mr[i] - p_m[l+i]);
            value += p_w[i]*dr*dr;
        }
        ierr = VecRestoreArray(this->m_Mask, &p_w); CHKERRQ(ierr);
    } else {
        for (IntType i = 0; i < nc*nl; ++i) {
            dr = (p_mr[i] - p_m[l+i]);
            value += dr*dr;
        }
    }
    // all reduce
    rval = MPI_Allreduce(&value, &l2distance, 1, MPIU_REAL, MPI_SUM, PETSC_COMM_WORLD);
    ierr = Assert(rval == MPI_SUCCESS, "mpi error"); CHKERRQ(ierr);

    ierr = VecRestoreArray(this->m_ReferenceImage, &p_mr); CHKERRQ(ierr);
    ierr = VecRestoreArray(this->m_StateVariable, &p_m); CHKERRQ(ierr);

    // objective value
    *D = 0.5*l2distance/static_cast<ScalarType>(nc);

    this->m_Opt->Exit(__func__);

    PetscFunctionReturn(ierr);
}




/********************************************************************
 * @brief set final condition for adjoint equaiton (varies for
 * different distance measres)
 *******************************************************************/
PetscErrorCode DistanceMeasureSL2::SetFinalCondition() {
    PetscErrorCode ierr = 0;
    IntType nl, nc, nt, l, ll;
    ScalarType *p_mr = NULL, *p_m = NULL, *p_l = NULL, *p_w = NULL;
    PetscFunctionBegin;

    this->m_Opt->Enter(__func__);

    ierr = Assert(this->m_ReferenceImage != NULL, "null pointer"); CHKERRQ(ierr);
    ierr = Assert(this->m_StateVariable != NULL, "null pointer"); CHKERRQ(ierr);
    ierr = Assert(this->m_AdjointVariable != NULL, "null pointer"); CHKERRQ(ierr);

    nt = this->m_Opt->m_Domain.nt;
    nc = this->m_Opt->m_Domain.nc;
    nl = this->m_Opt->m_Domain.nl;

    // index for final condition
    if (this->m_Opt->m_OptPara.method == FULLNEWTON) {
        ll = nt*nc*nl;
    } else {
        ll = 0;
    }

    ierr = VecGetArray(this->m_StateVariable, &p_m); CHKERRQ(ierr);
    ierr = VecGetArray(this->m_ReferenceImage, &p_mr); CHKERRQ(ierr);
    ierr = VecGetArray(this->m_AdjointVariable, &p_l); CHKERRQ(ierr);

    l = nt*nc*nl;
    // compute terminal condition \lambda_1 = -(m_1 - m_R) = m_R - m_1
    if (this->m_Mask != NULL) {
        // mask objective functional
        ierr = VecGetArray(this->m_Mask, &p_w); CHKERRQ(ierr);
#pragma omp parallel
{
#pragma omp for
        for (IntType i = 0; i < nc*nl; ++i) {
            p_l[ll+i] = p_w[i]*(p_mr[i] - p_m[l+i]);
        }
}  // omp
        ierr = VecRestoreArray(this->m_Mask, &p_w); CHKERRQ(ierr);
    } else {
#pragma omp parallel
{
#pragma omp for
        for (IntType i = 0; i < nc*nl; ++i) {
            p_l[ll+i] = p_mr[i] - p_m[l+i];
        }
}  // omp
    }
    ierr = VecRestoreArray(this->m_AdjointVariable, &p_l); CHKERRQ(ierr);
    ierr = VecRestoreArray(this->m_ReferenceImage, &p_mr); CHKERRQ(ierr);
    ierr = VecRestoreArray(this->m_StateVariable, &p_m); CHKERRQ(ierr);

    this->m_Opt->Exit(__func__);

    PetscFunctionReturn(ierr);
}




}  // namespace reg




#endif  // _DISTANCEMEASURESL2_CPP_
