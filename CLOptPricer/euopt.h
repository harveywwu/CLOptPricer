#ifndef EUOPT_H
#define EUOPT_H

#include <cmath>
#include <string>
#include "normdist.h"

using namespace std;

double BlackPrice(double F,double K,double vol,double t_exp,double df, std::string PutCall);

double BSPrice(double spot, double K, double vol, double t_exp, double r, double q, std::string PutCall);

double BlackDelta(double F,double K,double vol,double t_exp,double df, std::string PutCall);

double BSDigital(double spot,double K,double vol,double t_exp, double r, double q, std::string PutCall);

double BlackDigital(double F, double K, double vol, double t_exp, double df, std::string PutCall);

double BachelierPrice(double F,double K,double vol,double t_exp,double df, std::string PutCall);

double BlackImpliedVol(double MktPrice, double F,double K,double r,double T, std::string PutCall);

double BSImpliedVol(double MktPrice, double S,double K,double r,double T, std::string PutCall);

double BachelierImpliedVol(double MktPrice, double F,double K,double r,double T, std::string PutCall);

#endif