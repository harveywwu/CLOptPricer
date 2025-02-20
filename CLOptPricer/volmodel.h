#ifndef VOLMODEL_H
#define VOLMODEL_H
#include <vector>
#include <cmath>
#include <string>
#include "interpolator.h"
#include "math_utils.h"
#include "normdist.h"
#include "ci.h"

double SamuelsonFactor(double a, double b, double t, double T, double mat);

class VolNode {
public:
	virtual double GetVolByStrike(const double strike, const double dmat = 0) { 
		return this->GetVolByMoneyness( 0.0, dmat); 
	}
	
	virtual double GetVolByDelta(const double delta, const double dmat = 0) {
		return this->GetVolByMoneyness( 0.0, dmat);
	}

	virtual double GetVolByMoneyness(const double ratio, const double dmat = 0) {
		return _atmVol;
	}

	virtual double GetInstVol(const double d = 0) {
		return _atmVol;
	}

	virtual void setAtm( double atm ) { _atmVol = atm; }
	virtual void setToday( double dtoday ) { _dtoday = dtoday; }
	void setExp( double dexp ) { _dexp = dexp; }

	double atmVol_() { return _atmVol; } 
	double dtoday_() { return _dtoday; }
	double dexp_() { return _dexp; }
	std::string accrual_() { return _accrual; }
	double expiry_();
	double time2expiry_(const double dtoday, const double dexp);
	double nextwkday_(const double dtoday);

	VolNode() :_atmVol(0.0), _dexp(0.0), _dtoday(0.0), _accrual("act365"){}
	VolNode(double vol, double dtoday, double dexp, 
		std::string accrual = "act365") :_atmVol(vol), _dtoday(dtoday), _dexp(dexp), _accrual(accrual){}
private:
	double _atmVol;
	double _dtoday;
	double _dexp;
	std::string _accrual;
};

class SamuelVolNode: public VolNode {
public:
	SamuelVolNode(const double dtoday,
				  const double dexp, 
				  const double atm,
				  const double alpha,
				  const double beta,
				  const std::string accrual = "act365") :VolNode(atm, dtoday, dexp, accrual), _alpha(alpha), _beta(beta) {}
	double alpha_() { return _alpha; }
	double beta_() { return _beta; }
	void setAlpha( double alpha ) { _alpha = alpha; }
	void setBeta( double beta ) { _beta = beta; }

	double GetVolByMoneyness(const double ratio, const double dmat);
	double GetInstVol(const double d);
private:
	double _alpha;
	double _beta;
};

class FXVol {
public:
	FXVol(const double dtoday, 
		  const DblVector fxTenors,
		  const DblVector fxVols);
	~FXVol() { delete _volInterp; }	  
	double fxAtmVol(unsigned int i);
	double GetFXVolByDate(double dmat) { return _volInterp->InterpByExpiry(dmat); }
	DblVector& fxAtmVols_() { return _fxAtmVols; }
	DblVector& fxTenors_()  { return _fxTenors; }
	void setFxAtmVols(double vol, unsigned int index) { if (index <= _fxAtmVols.size()) _fxAtmVols[index] = vol;}
	virtual void setToday( double dtoday ) { _volInterp->setToday(dtoday); }
private:
	DblVector _fxTenors;
	DblVector _fxAtmVols;
	VolInterp* _volInterp;
};

class FXSamuelVolNode: public SamuelVolNode, public FXVol {
public:
	FXSamuelVolNode(const double dtoday, const double dexp, 
				const double atm, const double alpha, const double beta,
				const std::string accrual,
				DblVector fxTenors, DblVector fxVols, const double corr) :
				SamuelVolNode(dtoday, dexp, atm, alpha, beta, accrual),
				FXVol(dtoday, fxTenors, fxVols), _corr(corr) {}
	double corr_() { return _corr; }
	void setToday( double dtoday );
	double GetVolByMoneyness(const double ratio, const double dmat);
private:
	double _corr;
};

class Delta5VolNode: public VolNode {
public:
	Delta5VolNode(const double dtoday,
				  const double dexp, 
				  const double fwd,
				  const double atm, 
				  const double v90, 
				  const double v75, 
				  const double v25, 
				  const double v10,
				  const std::string accrual = "act365");
	//Delta5VolNode( const Delta5VolNode & other);
	~Delta5VolNode() { delete _interp; }

	void initialize();
	virtual void setAtm( double atm );
	void setFwd( double fwd ) { _fwd = fwd; }
	void setD10Vol( double d10Vol ) { _d10Vol = d10Vol; }
	void setD25Vol( double d25Vol ) { _d25Vol = d25Vol; }
	void setD75Vol( double d75Vol ) { _d75Vol = d75Vol; }
	void setD90Vol( double d90Vol ) { _d90Vol = d90Vol; }
	
	double d10Vol_() { return _d10Vol; } 
	double d25Vol_() { return _d25Vol; } 
	double d75Vol_() { return _d75Vol; } 
	double d90Vol_() { return _d90Vol; } 
	double fwd_() { return _fwd; }
	double omega_() { return _omega; }
	double GetVolByStrike(const double strike, const double dmat=0);
	double GetVolByDelta(const double delta, const double dmat=0);
	virtual double GetVolByMoneyness(const double ratio, const double dmat=0);

private:
	ConvInterpolator *_interp;
	double _fwd;
	double _d10Vol;
	double _d25Vol;
	double _d75Vol;
	double _d90Vol;
	double _omega;
};

class SamuelDelta5VolNode: public Delta5VolNode {
public:
	SamuelDelta5VolNode(const double dtoday,
				  const double dexp, 
				  const double fwd,
				  const double atm, 
				  const double v90, 
				  const double v75, 
				  const double v25, 
				  const double v10,
				  const double alpha,
				  const double beta,
				  const std::string accrual = "act365");
	double alpha_() { return _alpha; }
	double beta_() { return _beta; }
	void setAlpha( double alpha ) { _alpha = alpha; }
	void setBeta( double beta ) { _beta = beta; }
	double GetVolByMoneyness(const double ratio, const double dmat);
	double GetInstVol(const double d);
private:
	double _alpha;
	double _beta;
};

//class FXSmileVol: public FXVol {
//public:
//	FXSmileVol(const double dtoday, 
//		  const DblVector fxTenors,
//		  const DblVector fxAtmVols,
//		  const DblVector fxD10Vols,
//		  const DblVector fxD25Vols,
//		  const DblVector fxD75Vols,
//		  const DblVector fxD90Vols);
//	~FXSmileVol();	  
//	double fxAtmVol(unsigned int i);
//	double fxD10Vol(unsigned int i);
//	double fxD25Vol(unsigned int i);
//	double fxD75Vol(unsigned int i);
//	double fxD90Vol(unsigned int i);
//	double GetFXVolByDate(double dmat) { return _volInterp->InterpByExpiry(dmat); }
//	DblVector& fxAtmVols_() { return _fxAtmVols; }
//	DblVector& fxTenors_()  { return _fxTenors; }
//	void setFxAtmVols(double vol, unsigned int index) { if (index <= _fxAtmVols.size()) _fxAtmVols[index] = vol;}
//	virtual void setToday( double dtoday ) { _volInterp->setToday(dtoday); }
//private:
//	DblVector _fxTenors;
//	DblVector _fxAtmVols;
//	VolInterp* _volInterp;
//};

#endif
