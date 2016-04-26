#ifndef BRG_H
#define BRG_H

#include <iostream>
#include <random>
using namespace std;

#define INIT_BUDGET 100
#define CHANGE_PROB_BOUNDARY 125

class RandomNumberGenerator{
public:

	RandomNumberGenerator(double prob);
	~RandomNumberGenerator();
	double getSample();
	double getProb();
	void reset();
	void setTrap(double dropRange);

private:
	double m_DropRange;
	double m_Prob;
	double m_iniProb;
	int m_Counter;
	static default_random_engine m_Generator;
	binomial_distribution<int, double> m_BinomialDistribution;
};

#endif