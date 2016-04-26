#ifndef ALG_H
#define ALG_H


#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <boost/math/distributions/students_t.hpp>
#include "MultiArmBandits.h"

using namespace std;
using namespace boost::math;

#define isResetVariance 0
#define isResetHalf 0
#define USE_TRAP 0
#define TRAP_INDEX 0

class armInfo{
public:

	// implement for different algorithm
	static bool armInfoCompare(const armInfo& arm1, const armInfo& arm2);

	int m_originalIndex;
	int m_nPulls;
	double mean;
	double variance;
	double meanOfMean;
};

class Algorithms{
public:
	Algorithms(int expNum, int numBudget, double* probs, int nArms);
	virtual ~Algorithms();

	// to be inherited and implemented by each algorithm
	virtual int select() = 0;
	virtual void update(double result, int armNum) = 0;
	virtual int recommend() = 0;
	virtual void reset() = 0;
	virtual void showLog() = 0;

	double pullArm(int i);
	void resetArmInfos();
	void runExperiments();

protected:
	MultiArmBandits m_MABs;
	vector<armInfo> m_Arms;
	int m_nBudgets;

private:
	int m_nExp;
};

#endif