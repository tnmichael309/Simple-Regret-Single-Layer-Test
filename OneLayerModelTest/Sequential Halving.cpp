#include "Sequential Halving.h"


SequentialHalving::SequentialHalving(int expNum, int numBudget, double* probs, int nArms):Algorithms(expNum, numBudget, probs, nArms){};

SequentialHalving::~SequentialHalving(){};

void SequentialHalving::reset(){
	m_totalRounds = (int)ceil(log((double)m_Arms.size())/log(2.0));
	m_currentRounds = 0;
	m_bugetForEachRound = (int)floor((double)m_nBudgets/(double)m_totalRounds);
	m_selectNums = 0;
	m_selectRange = m_Arms.size();
};

int SequentialHalving::select(){

	if(m_bugetForEachRound == 0) {
		m_bugetForEachRound = (int)floor((double)m_nBudgets/(double)m_totalRounds);
		m_currentRounds++;
		m_selectNums = 0;

		// sort by win rate
		sort(m_Arms.begin(),m_Arms.begin()+m_selectRange, armInfo::armInfoCompare);

		// shrink the selected range in half
		m_selectRange = ceil(((double)m_selectRange)/2.0);
	}

	int selectIndex = m_selectNums%m_selectRange;

	m_bugetForEachRound--;
	m_selectNums++;

	return m_Arms[selectIndex].m_originalIndex;
};

void SequentialHalving::update(double result, int armNum){
	double reward = result;
	for(int i = 0 ; i < m_selectRange; i++){
		if(m_Arms[i].m_originalIndex == armNum) {
			m_Arms[i].mean = result; //(m_Arms[i].mean*m_Arms[i].m_nPulls + 1.0*reward)/(m_Arms[i].m_nPulls+1);
			m_Arms[i].m_nPulls++;
			break;
		}
	}
};

int SequentialHalving::recommend(){
	if(m_Arms[0].mean > m_Arms[1].mean) return m_Arms[0].m_originalIndex;
	else return m_Arms[1].m_originalIndex;
};

void SequentialHalving::showLog(){
	cerr << "Index\tArm Pulls\tMean" << endl;
	for(int i = 0; i < m_Arms.size(); i++){
		cerr << m_Arms[i].m_originalIndex << "\t" << m_Arms[i].m_nPulls << "\t" << m_Arms[i].mean << endl;
	}
	cerr << endl;
};