#include "common.h"

int U(int min, int max){
	return min +(rand() % (max-min+1));
}

double U(double min, double max){
	return min + (max-min)*rand()*(1.0/RAND_MAX);
}

double gaussianNumber(double mu, double sigma){
	const double epsilon = std::numeric_limits<double>::min();
	const double two_pi = 2.0*3.14159265358979323846;

	static double z0, z1;
	static bool generate;
	generate = !generate;

	if (!generate)
		return z1 * sigma + mu;

	double u1, u2;
	do
	{
		u1 = rand() * (1.0 / RAND_MAX);
		u2 = rand() * (1.0 / RAND_MAX);
	}
	while ( u1 <= epsilon );

	z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
	z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);
	return z0 * sigma + mu;
}


int Xi2Number(int k){
	double Z=gaussianNumber(0, 1);
	double Z2=Z*Z;

	float accum=1;
	double U;
	for(int i=0;i<k;i++){
		srand (time (NULL));
		U=((double) rand() / (RAND_MAX));
		accum*=(U);
	}
	return (int)(Z2-2*log(accum));
}

int gammaNumber(float alpha){
	double U,V;
	double X;
	double reject;
	while(true){

		//srand (time (NULL));
		U=((double) rand() / (RAND_MAX));
		X=-2*log(1-pow(U,(1/alpha)));
		//srand (time (NULL));
		V=((double) rand() / (RAND_MAX));

		reject=(pow(X,(alpha-1)*exp(-X/2)))/(pow(2,(alpha-1))*pow((1-exp(-X/2)),(alpha-1)));
		if(V<=reject) break;
	}
	return (int)X;
}
