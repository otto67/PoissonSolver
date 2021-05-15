#include <math.h>
#include "FDPoisson.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <QDir>
#include <QThread>
#include <QProcess>
#include <QMessageBox>

// #include <QtOpenGL>

extern void gausselim(std::vector<std::vector<double>> &a,
		      std::vector<double> &x);

DscPrms::DscPrms(int nno , float xmin, float ymin,
         float xmax, float ymax){
  x_min = xmin;
  y_min = ymin;
  x_max = xmax;
  y_max = ymax;

  nno_x = int(sqrt((nno*(x_max - x_min)/(y_max - y_min))));
  nno_y = int(nno/nno_x);

  dx = (x_max - x_min)/(nno_x - 1);
  dy = (y_max - y_min)/(nno_y - 1);
}

void FDPoisson::assembleEqSys(){
  
   float dx = params->dx;
   float dy = params->dy;
   int nnox = params->nno_x;
   int nnoy = params->nno_y;
    
   if (!A){
    A = std::make_unique<std::vector<std::vector<double>>>();
    A->resize(nnox*nnoy);
    for (unsigned int i=0; i < A->size(); i++){
      (*A)[i] = std::vector<double>(nnox*nnoy);
    }   
   }
   
  if (!r_h_s){
    r_h_s = std::make_unique<std::vector<double>>();
    r_h_s->resize(nnox*nnoy);
  }

  if (!solution){
    solution = std::make_unique<std::vector<double>>();
    solution->resize(nnox*nnoy);
  }

  // Assume Dirichlet bc's over the entire boundary
  // Avoid having to  compensate for the -1 index
  // from the second derivatives
    for (int i=nnox; i < nnox*(nnoy - 1); i++){
      
      (*A)[i][i] = -2*((1 / (dx*dx)) + (1 / (dy*dy)));
      (*A)[i][i-nnox] = 1 / (dy*dy);
      (*A)[i][i+nnox] = 1 / (dy*dy);
      (*A)[i][i-1] = 1 / (dx*dx);
      (*A)[i][i+1] = 1 / (dx*dx);
    }

    for (int i=0; i<nnoy; i++){
      for (int j=0; j<nnox; j++){
	(*r_h_s)[i * nnox + j] = RHS(j*dx, i*dy);
      }
    }
}

void FDPoisson::fillEssBC(){

  float dx = params->dx;
  float dy = params->dy;
  int nnox = params->nno_x;
  int nnoy = params->nno_y;
  
  // Note: Need to check boundary indicators later
  for (int i= 0; i<nnox; i++){
    for (int j= 0; j<nnox*nnoy; j++){
      (*A)[i][j] = 0.0;
      (*A)[nnox*(nnoy-1)+i][j] = 0.0;
    }
    (*A)[i][i] = 1.0;
    (*A)[nnox*(nnoy-1)+i][nnox*(nnoy-1)+i] = 1.0;
    (*r_h_s)[i] = essBC(i*dx,params->y_min);
    (*r_h_s)[nnox *(nnoy - 1)+i] = essBC(i*dx, params->y_max);
  }
  
   for (int i=0; i<nnoy; i++){
    for (int j=0; j<nnox*nnoy; j++){
      (*A)[i*nnox][j] = 0.0;
      (*A)[(nnox-1)+(i * nnox)][j] = 0.0;
    }
    (*A)[i*nnox][i*nnox] = 1.0;
    (*A)[(nnox-1)+i*nnox][(nnox-1)+ i*nnox] = 1.0;
    (*r_h_s)[i*nnoy] = essBC(params->x_min, i*dy);
    (*r_h_s)[(nnox - 1)+ (i*nnox)] = essBC(params->x_max, i*dy);
   }

    /*
  std::for_each(bic->begin(), bic->end(),[](std::vector<int> s){
      for (int v : s) (v > 0 ?  std::cout<<"boundary number  is "<<v<<std::endl :
		       std::cout<<"Negative"<<std::endl);
    });
    */
}

float FDPoisson::analytic(float x, float y){
  return 2*x*x - y*y;
}

float FDPoisson::RHS(float, float){
  return 2;
}

float FDPoisson::essBC(float x, float y){
  return analytic(x,y);
}

void FDPoisson::setBoinds(std::vector<std::tuple<int, int, int, int>> binds){
  
  if (!bic){
    bic = std::make_unique<std::vector<std::vector<int>>>();
  }

  for (unsigned int i=0; i < binds.size();i++){

    std::vector<int> temp(4);

    auto [b1, b2, b3, b4] = binds[i];
    if (b1 > 0)
      temp[0] = b1;
    if (b2 > 0)
      temp[1] = b2;
    if (b3 > 0)
      temp[2] = b3;
    if (b4 > 0)
      temp[3] = b4;
    
    bic->push_back(temp);
  }  
}

void FDPoisson::solve(){

  int i, j;

  assembleEqSys();
  fillEssBC();
  
  int size = A->size();

  std::vector<std::vector<double>> a(size);
  for(i = 0; i < size; i++){
    a[i] = std::vector<double>(size+1);
  }

  
  // Solve linear equation system. Then plot
  // Need to augment matrix to include right hand side
  // for library solver
  
  for (i = 0; i < size; i++){
    for (j = 0; j < size; j++){
      a[i][j] = (*A)[i][j];
    }
    a[i][size] = (*r_h_s)[i];
  }

  // Solve the linear system by simple gauss elimination
  // a contains matrix augmented with right hand side
  // gausselim fills into *solution
  gausselim(a, *solution);
  plot();
}

void FDPoisson::plot(){
  
  int nno_x = params->nno_x;
  int nno_y = params->nno_y;
  float dx = params->dx;
  float dy = params->dy;
  float x_min = params->x_min;
  float y_min = params->y_min;
  
  std::ofstream myfile;
  std::vector<std::vector<double>> u(nno_y);

  for(int i = 0; i < nno_y; i++){
    u[i] = std::vector<double>(nno_x);
  }
  
  for (int i=0; i<nno_x*nno_y; i++){
    u[floor(i/nno_x)][i % nno_x] = (*solution)[i]; 
  }

  try {

      myfile.open ("nodevals.nod");

    myfile <<"M "<<x_min<<';'<<y_min<<';'<<dx<<';'<<dy<<';'<<nno_x<<';'<<nno_y<<std::endl;

    for_each(u.begin(), u.end(), [&](std::vector<double> vec){
        for (double v : vec) myfile <<v<<";";

        myfile <<"\n";
  });
  myfile.close();
  }

  catch (std::exception const& e){
      std::cout << "There was an error: " << e.what() << std::endl;
  }


  QStringList arguments {"plotter.py"};
  QProcess p;
  p.startDetached("python ", arguments);
  QThread::sleep(6); // Wait for the script to write picture to file
}
