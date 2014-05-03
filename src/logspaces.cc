#include<cmath>

double toLog(double a){
	return log(x);
}
double logadd(double a, double b){
    if(a == ZERO)
        return b;
    else if(b == ZERO)
        return a;
    else if(a > b)
        return a + log1p(exp(b - a));
    else
        return b + log1p(exp(a - b));
};
