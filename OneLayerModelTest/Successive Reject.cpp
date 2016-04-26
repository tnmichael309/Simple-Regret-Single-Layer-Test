#include "Successive Reject.h"


SuccessiveReject::SuccessiveReject(int expNum, int numBudget, double* probs, int nArms):Algorithms(expNum, numBudget, probs, nArms),m_selectRange(nArms){};
SuccessiveReject::~SuccessiveReject(){};

void SuccessiveReject::reset(){
	m_selectNums = 0;
	m_selectRange = m_Arms.size();
	m_currentRound = 1;
	m_bugetForEachRound = ceil(1.0/avgLogK(m_Arms.size())*(m_nBudgets-m_Arms.size())/(double)(m_Arms.size()+1-m_currentRound));
};

int SuccessiveReject::select(){
	if(m_bugetForEachRound == 0) {
		m_selectNums = 0;
		m_currentRound++;
		m_bugetForEachRound = ceil(1.0/avgLogK(m_Arms.size())*(m_nBudgets-m_Arms.size())/(double)(m_Arms.size()+1-m_currentRound));

		// sort by win rate
		sort(m_Arms.begin(),m_Arms.begin()+m_selectRange, armInfo::armInfoCompare);
		m_selectRange--;
	}

	int selectIndex = m_selectNums%m_selectRange;

	m_bugetForEachRound--;
	m_selectNums++;

	return m_Arms[selectIndex].m_originalIndex;
};

void SuccessiveReject::update(double result, int armNum){
	double reward = result;
	for(int i = 0 ; i < m_selectRange; i++){
		if(m_Arms[i].m_originalIndex == armNum) {
			m_Arms[i].mean = result;//(m_Arms[i].mean*m_Arms[i].m_nPulls + 1.0*reward)/(m_Arms[i].m_nPulls+1);
			m_Arms[i].m_nPulls++;
			break;
		}
	}
};

int SuccessiveReject::recommend(){
	return m_Arms[0].m_originalIndex;
};

void SuccessiveReject::showLog(){
	cerr << "Index\tArm Pulls\tMean" << endl;
	for(int i = 0; i < m_Arms.size(); i++){
		cerr << m_Arms[i].m_originalIndex << "\t" << m_Arms[i].m_nPulls << "\t" << m_Arms[i].mean << endl;
	}
	cerr << endl;
};

double SuccessiveReject::avgLogK(int k){
	double result = 0.5;
	for(int i = 2; i <= k; i++){
		result += 1.0/(double)i;
	}
	return result;
};