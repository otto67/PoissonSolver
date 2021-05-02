#include <iostream>
#include "Simulator.h"


void Simulator::setRHS(std::vector<float> cfs){
    rhs_coeff.resize(cfs.size());
    for (unsigned int i = 0; i < cfs.size(); i++)
        rhs_coeff[i] = cfs[i];
}

void Simulator::setBCs(std::vector<float> bcis){
    this->bc_vals.resize(bcis.size());
    for (unsigned int i = 0; i < bcis.size(); i++)
        bc_vals[i] = bcis[i];
}

// To be implemented
float Simulator::RHS(float x, float y) {

    unsigned int n_cfs = rhs_coeff.size();

    if (n_cfs == 1) // Const rhs
        return rhs_coeff[0];
    if (n_cfs == 3) // Linear rhs
        return rhs_coeff[0]*x + rhs_coeff[1]*y + rhs_coeff[2];
    if (n_cfs == 6) // Quadratic rhs
        return rhs_coeff[0]*x*x + rhs_coeff[1]*y*y + rhs_coeff[2]*x*y + rhs_coeff[2]*x + rhs_coeff[4]*y + rhs_coeff[5];

    // Error, return 0
    return 0.0;
}


float Simulator::essBC(float x, float y) { 

    // Considering implementing proper boundary indices later
    if (x < params->x_min + params->dx)
        return bc_vals[2];
    if (x > params->x_max - params->dx)
        return bc_vals [0];
    if (y < params->y_min + params->dy)
        return bc_vals [3];
    if (y < params->y_max + params->dy)
        return bc_vals [1];

    // Error, return 0
    return 0.0;
}
