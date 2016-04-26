#include "Algorithms.h"


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