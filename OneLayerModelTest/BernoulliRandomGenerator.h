#ifndef BRG_H
#define BRG_H

#include <iostream>
#include <random>
using namespace std;

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
	double m_Dev;
	double m_iniDev;
	int m_Counter;
	static default_random_engine m_Generator;
	bernoulli_distribution m_Distribution;
	normal_distribution<double> m_GaussianDistribution;
	binomial_distribution<int, double> m_BinomialDistribution;
};

#endif