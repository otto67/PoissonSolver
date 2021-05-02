#include <iostream>
#include <vector>

using namespace std;

typedef vector<double> Vec;
typedef vector<Vec> Mat;

void gausselim(Mat &a, Vec &b){

  int i, j, k;
  int N = a.size();
  float temp, s;
  
   //make above matrix upper triangular Matrix

  for(j=0; j<N-1; j++){
    for(i=j+1; i<N; i++)
      {
	temp=a[i][j]/a[j][j];
	
	for(k=0; k<N+1; k++)
	  a[i][k]-=a[j][k]*temp;
      }
  }

  
    
  //find values of x,y,z using back substitution
  for(i=N-1; i>=0; i--){
    s=0;
    for(j=i+1; j<N; j++)
      s += a[i][j]*b[j];
    b[i]=(a[i][N]-s)/a[i][i];
  }
  
  //print values of x,y,z
}
