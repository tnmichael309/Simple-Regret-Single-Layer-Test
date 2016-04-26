#include "Algorithms.h"

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
