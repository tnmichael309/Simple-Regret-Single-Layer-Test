#include "UCB-E.h"

UCBE::UCBE(int expNum, int numBudget, double* probs, int nArms, int param):Algorithms(expNum, numBudget, probs, nArms),m_Param(param){};
UCBE::~UCBE(){};

void UCBE::reset(){
};

int UCBE::select(){
	double maxBValue = 0;
	int selectIndex = 0;
	for(int i = 0 ; i < m_Arms.size(); i++){
		double bValue = m_Arms[i].m_nPulls == 0 ? 100000 : m_Arms[i].mean + sqrt((double)m_Param/(double)m_Arms[i].m_nPulls);
		if(bValue > maxBValue) {
			maxBValue = bValue;
			selectIndex = i;
		}
	}
	return m_Arms[selectIndex].m_originalIndex;
};

void UCBE::update(double result, int armNum){
	double reward = result;
	for(int i = 0 ; i < m_Arms.size(); i++){
		if(m_Arms[i].m_originalIndex == armNum) {
			m_Arms[i].mean = result;//(m_Arms[i].mean*m_Arms[i].m_nPulls + 1.0*reward)/(m_Arms[i].m_nPulls+1);
			m_Arms[i].m_nPulls++;
			break;
		}
	}
};

int UCBE::recommend(){
	double maxBValue = 0;
	int selectIndex = 0;
	for(int i = 0 ; i < m_Arms.size(); i++){
		double bValue = m_Arms[i].mean;
		if(bValue > maxBValue) {
			maxBValue = bValue;
			selectIndex = i;
		}
	}
	return m_Arms[selectIndex].m_originalIndex;
};

void UCBE::showLog(){
	cerr << "Index\tArm Pulls\tMean" << endl;
	for(int i = 0; i < m_Arms.size(); i++){
		cerr << m_Arms[i].m_originalIndex << "\t" << m_Arms[i].m_nPulls << "\t" << m_Arms[i].mean << endl;
	}
	cerr << endl;
};