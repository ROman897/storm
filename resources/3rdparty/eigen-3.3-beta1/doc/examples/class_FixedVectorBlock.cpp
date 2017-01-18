#include <Eigen/Core>
#include <iostream>
using namespace StormEigen;
using namespace std;

template<typename Derived>
StormEigen::VectorBlock<Derived, 2>
firstTwo(MatrixBase<Derived>& v)
{
  return StormEigen::VectorBlock<Derived, 2>(v.derived(), 0);
}

template<typename Derived>
const StormEigen::VectorBlock<const Derived, 2>
firstTwo(const MatrixBase<Derived>& v)
{
  return StormEigen::VectorBlock<const Derived, 2>(v.derived(), 0);
}

int main(int, char**)
{
  Matrix<int,1,6> v; v << 1,2,3,4,5,6;
  cout << firstTwo(4*v) << endl; // calls the const version
  firstTwo(v) *= 2;              // calls the non-const version
  cout << "Now the vector v is:" << endl << v << endl;
  return 0;
}