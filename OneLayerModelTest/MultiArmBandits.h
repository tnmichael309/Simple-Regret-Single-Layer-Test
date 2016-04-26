#ifndef MAB_H
#define MAB_H

#include <iostream>
#include <vector>
#include "RandomGenerator.h"

using namespace std;

class MultiArmBandits{
public:
	MultiArmBandits(double* probs, int nArms);
	~MultiArmBandits();
	double pullArm(int i);
	int getBestArm();
	void reset();
	void setTrap(int i);

private:
	vector<double> m_probs;
	vector<RandomNumberGenerator> m_Arms;
	int m_nArms;

};

#endif