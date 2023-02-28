#ifndef SINPRODSF_H
#define SINPRODSF_H

#include "ScalarField.hpp"
#include "Constants.hpp"

class SinProductSF : public ScalarField {
	public:
		SinProductSF(Mesh *m, double frequency = 2, double amplitude = 1,
			bool fadeV = false);
		void evaluate(double u, double v, double &f, double &fu, double &fv,
			double &fuu, double &fuv, double &fvv) const;

	private:
		const double freq, ampl;
		const bool fadeV;
};

#endif