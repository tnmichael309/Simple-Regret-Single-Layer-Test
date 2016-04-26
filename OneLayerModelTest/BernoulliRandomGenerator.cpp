#include "BernoulliRandomGenerator.h"

default_random_engine RandomNumberGenerator::m_Generator(rand());

RandomNumberGenerator::RandomNumberGenerator(double prob):m_Prob(prob),m_Distribution(prob),m_GaussianDistribution(prob,0.5),m_Dev(0.5),m_BinomialDistribution(100, prob){
	m_iniDev = m_Dev;
	m_DropRange = 0;
	m_iniProb = m_Prob;
}

RandomNumberGenerator::~RandomNumberGenerator(){

}

double RandomNumberGenerator::getSample(){
	//return  m_Distribution(m_Generator);
	/*m_Counter++;
	
	double randomNum = m_GaussianDistribution(m_Generator);
	if(m_Counter%50 == 0){ // decrease the deviation
		m_Dev *= 0.9;
		m_Dev >= 0.1 ? m_Dev : 0.1;

		//cerr << "Dev: " << m_Dev << endl;
		normal_distribution<double> temp(m_Prob,m_Dev);
		m_GaussianDistribution.param(temp.param());
	}

	randomNum = randomNum > 1 ? 1.0 : randomNum;
	randomNum = randomNum < 0 ? 0.0 : randomNum;
	return randomNum;*/

	static double lastResult = 0.0;

	m_Counter++;
	binomial_distribution<int,double> temp(100+m_Counter,m_Prob);
	m_BinomialDistribution.param(temp.param());
	double result = m_BinomialDistribution(m_Generator);
	result /= (double)(100+m_Counter);

	/*if(m_Counter == 1) lastResult = result;
	else {
		result = lastResult*0.2 + result*0.8;
		lastResult = result;
	}*/

	if(m_DropRange != 0.0 && m_Counter > 125 && m_Prob == m_iniProb){
		m_Prob -= m_DropRange;
	}

	return result;
}


double RandomNumberGenerator::getProb(){
	if(m_DropRange != 0.0 && m_Counter <= 125) return (m_Prob-m_DropRange);
	else return m_Prob;
}

void RandomNumberGenerator::reset(){
	m_Counter = 0;

	// Gaussian
	m_GaussianDistribution.reset();

	normal_distribution<double> temp(m_Prob,m_iniDev);
	m_GaussianDistribution.param(temp.param());
	m_iniDev = m_Dev;

	// Bernoulli
	m_Distribution.reset();

	// Binomial
	m_BinomialDistribution.reset();

	// 
	m_Prob = m_iniProb;
};

void RandomNumberGenerator::setTrap(double dropRange){
	m_DropRange = dropRange;
};

