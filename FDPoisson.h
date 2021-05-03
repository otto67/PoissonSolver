#include <iostream>
#include <vector>
#include <memory>
#include <tuple>

class DscPrms{

public:
  float dx, dy, x_max, x_min, y_max, y_min;
  int nno_x, nno_y;
  
  DscPrms(int=1600, float=0.0, float=0.0, float=1.0, float=1.0);
  ~DscPrms(){std::cout<<"Deleting DSCPrms object \n";}
};

class FDPoisson {

 protected:
  std::shared_ptr<DscPrms> params;
  std::unique_ptr<std::vector<double>> solution, r_h_s;
  std::unique_ptr<std::vector<std::vector<double>>> A;
  std::unique_ptr<std::vector<std::vector<int>>> bic;
  
  void assembleEqSys();
  void fillEssBC();
  void plot();
  
  float analytic(float, float);

  virtual float RHS(float x, float y);
  virtual float essBC(float x, float y);
  
  
 public:
  FDPoisson(std::shared_ptr<DscPrms> prms) : params(prms){std::cout<<"Creating FD \n";}
  FDPoisson(){std::cout<<"FD empty constructor called \n";}
  ~FDPoisson(){ std::cout<<"Deleting FD object"<<std::endl;}

  virtual void solve();
  void setBoinds(std::vector<std::tuple<int, int, int, int>>);
  
};
