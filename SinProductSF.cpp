#include "SinProductSF.hpp"

SinProductSF::SinProductSF(Mesh* mesh, double freq, double ampl, bool fadeV) :
	ScalarField(mesh, 2), freq(freq), ampl(ampl), fadeV(fadeV) {
	const unsigned int vn = mesh->vertNum();
	// Compute field
	for(unsigned int i = 0; i < vn; ++i) {
		const double u = mesh->cAttrib(i, Mesh::Attribute::U);
		const double v = mesh->cAttrib(i, Mesh::Attribute::V);
		
		double f, fu, fv, fuu, fuv, fvv;
		evaluate(u, v, f, fu, fv, fuu, fuv, fvv);

		setValue(f, i, 0, 0);
		setValue(fu, i, 1, 0);
		setValue(fv, i, 0, 1);
		setValue(fuu, i, 2, 0);
		setValue(fuv, i, 1, 1);
		setValue(fvv, i, 0, 2);
	}
}

void SinProductSF::evaluate(double u, double v, double &f,
	double &fu, double &fv, double &fuu, double &fuv, double &fvv) const {
	const double freqpi = TWOPI * freq;
	f = ampl * sin(freqpi * u) * sin(freqpi * v);
	fu = ampl * freqpi * cos(freqpi * u) * sin(freqpi * v);
	fv = ampl * freqpi * sin(freqpi * u) * cos(freqpi * v);
	fuu = -pow(freqpi, 2) * f;
	fuv = pow(freqpi, 2) * ampl * cos(freqpi * u) * cos(freqpi * v);
	fvv = fuu;

	if (fadeV) {
		// Additional factor that goes to zero at the poles
		const double x = pow(2 * v - 1, 2);
		const double p = - 6*pow(x,5) + 15*pow(x,4)
			- 10*pow(x,3) + 1;
		const double pv = (- 30*pow(x,4) + 60*pow(x,3)
			- 30*pow(x,2)) * (8*v - 4);
		const double pvv = (- 120*pow(x,3) + 180*pow(x,2)
			- 60*x) * pow(8*v - 4, 2) + (- 30*pow(x,4)
			+ 60*pow(x,3) - 30*pow(x,2)) * 8;

		// Original values of f
		const double of = f, ofu = fu, ofv = fv,
			ofuu = fuu, ofuv = fuv, ofvv = fvv;

		// Product rule
		f = of * p;
		fu = ofu * p;
		fv = ofv * p + of * pv;
		fuu = ofuu * p;
		fuv = ofuv * p + ofu * pv;
		fvv = ofvv * p + 2 * ofv * pv + of * pvv;
	}
}