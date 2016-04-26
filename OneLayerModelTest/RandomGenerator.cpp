#include "RandomGenerator.h"

default_random_engine RandomNumberGenerator::m_Generator(rand());

RandomNumberGenerator::RandomNumberGenerator(double prob):m_BinomialDistribution(INIT_BUDGET, prob){
	m_DropRange = 0;
	m_Prob = prob;
	m_iniProb = prob;
}

RandomNumberGenerator::~RandomNumberGenerator(){

}

double RandomNumberGenerator::getSample(){

	static double lastResult = 0.0;

	m_Counter++;
	binomial_distribution<int,double> temp(INIT_BUDGET+m_Counter,m_Prob);
	m_BinomialDistribution.param(temp.param());
	double result = m_BinomialDistribution(m_Generator);
	result /= (double)(INIT_BUDGET+m_Counter);

	bool isUseTrap = (m_DropRange != 0.0);
	bool shouldDropAfterEnoughBudgetAllocated = (m_Counter > CHANGE_PROB_BOUNDARY);
	bool hasNotDropped = (m_Prob == m_iniProb);
	if(isUseTrap && shouldDropAfterEnoughBudgetAllocated && hasNotDropped){
		m_Prob -= m_DropRange;
	}

	return result;
}


double RandomNumberGenerator::getProb(){

	bool isUseTrap = (m_DropRange != 0.0);
	bool shouldDropAfterEnoughBudgetAllocated = (m_Counter > CHANGE_PROB_BOUNDARY);

	if(isUseTrap && !shouldDropAfterEnoughBudgetAllocated) return (m_Prob-m_DropRange);
	else return m_Prob;
}

void RandomNumberGenerator::reset(){
	m_Counter = 0;

	// Binomial
	m_BinomialDistribution.reset();

	// 
	m_Prob = m_iniProb;
};

void RandomNumberGenerator::setTrap(double dropRange){
	m_DropRange = dropRange;
};

