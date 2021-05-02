#include "FDPoisson.h" 
#include <iostream>

// Class to implement right hand side and essential bc's
// prescribed by user

class Simulator : public FDPoisson {

protected:
  std::vector<float> rhs_coeff;
  std::vector<float> bc_vals;

  float RHS(float x, float y) override;
  float essBC(float x, float y) override; 

public:
  Simulator(std::shared_ptr<DscPrms> prms) {
    this->params = prms;
    std::cout<<"Creating Simulator object \n";
  }
  
  ~Simulator(){ std::cout<<"Deleting Simulator object"<<std::endl;}

  void setRHS(std::vector<float> cfs);
  void setBCs(std::vector<float> bcs);

};
