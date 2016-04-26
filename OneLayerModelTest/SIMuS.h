#include "Algorithms.h"


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
