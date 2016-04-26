#include "Algorithms.h"


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
