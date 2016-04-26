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
	int m_TrapIndex;
};

class SequentialHalving:public Algorithms{
public:

	SequentialHalving(int expNum, int numBudget, double* probs, int nArms);
	~SequentialHalving();

	void reset();
	int select() ;
	void update(double result, int armNum);
	int recommend();
	void showLog();

private:

	int m_totalRounds;
	int m_currentRounds;
	int m_bugetForEachRound;
	int m_selectNums;
	int m_selectRange;
};

class StochasticModel:public Algorithms{
public:
	StochasticModel(int expNum, int numBudget, double* probs, int nArms);
	~StochasticModel();

	void reset();
	int select() ;
	void update(double result, int armNum);
	int recommend();
	void showLog();

private:

	////////
	//student-t's helper functions
	////////
	double calculateStudentTPDF(double tValue, double degreeOfFreedom);
	// separability
	double calculateStudentTCDF(double tValue, double degreeOfFreedom);
	// win rate
	double calculateStudentTWinRate(double tValue, double degreeOfFreedom);
	double calculateStudentTBestProbability(int iIndex, vector<double> &vMeans, vector<double> &vDeviations, vector<int> &vMoveCounts);

	int m_lastBestMove;
	int m_lastSelectedChild;
};

class UCBE:public Algorithms{
public:

	UCBE(int expNum, int numBudget, double* probs, int nArms, int param);
	~UCBE();

	void reset();
	int select() ;
	void update(double result, int armNum);
	int recommend();
	void showLog();

private:

	int m_Param;
};

class SuccessiveReject:public Algorithms{
public:

	SuccessiveReject(int expNum, int numBudget, double* probs, int nArms);
	~SuccessiveReject();

	void reset();
	int select() ;
	void update(double result, int armNum);
	int recommend();
	void showLog();

private:
	int m_bugetForEachRound;
	int m_selectRange;
	int m_selectNums;
	int m_currentRound;

	double avgLogK(int k);
};

#endif