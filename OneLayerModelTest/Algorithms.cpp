#include "Algorithms.h"

bool armInfo::armInfoCompare(const armInfo& arm1, const armInfo& arm2){
	return arm1.mean > arm2.mean;
};

Algorithms::Algorithms(int expNum, int numBudget, double* probs, int nArms):m_MABs(probs, nArms),m_nBudgets(numBudget),m_nExp(expNum){
	for(int i = 0; i < nArms; i++) {
		armInfo ai;

		ai.variance = 0.0;
		ai.mean = 0;
		ai.m_nPulls = 0;
		ai.m_originalIndex = i;

		m_Arms.push_back(ai);
	}

#if USE_TRAP == 1
	m_MABs.setTrap(TRAP_INDEX);
#endif

};

Algorithms::~Algorithms(){

};

double Algorithms::pullArm(int i){
	return (double)m_MABs.pullArm(i);
};

void Algorithms::resetArmInfos(){

	// initialize with a vague heuristic value
	for(int i = 0; i < m_Arms.size(); i++) {
		m_Arms[i].variance = 10.0;
		m_Arms[i].mean = pullArm(i);
		m_Arms[i].m_nPulls = 1;
		m_Arms[i].m_originalIndex = i;
	}
	m_MABs.reset();
};

void Algorithms::runExperiments(){

	double correctNum = 0.0;
	double changeNum = 0.0;
	
	for(int i = 0; i < m_nExp; i++){
		resetArmInfos();
		reset();

		for(int j = 0; j < m_nBudgets; j++){
			int selectArm = select();
			double result = pullArm(selectArm);
			update(result, selectArm);
		}
		//int x = m_MABs.getBestArm();
		if(recommend() == m_MABs.getBestArm()) {
			correctNum += 1.0;
			/*showLog();
			cin.get();*/
			/*cerr << "\nRecommend:" << recommend() << endl;
			cerr << "\nBest Arm:" << m_MABs.getBestArm() << endl;
			showLog();
			cin.get();*/
		}/*else{
			showLog();
			cin.get();
		}*/
		/*else {
			cerr << "\nRecommend:" << recommend() << endl;
			cerr << "\nBest Arm:" << m_MABs.getBestArm() << endl;
			showLog();
		}*/

		for(int j = 0 ; j < m_Arms.size(); j++){
			if(m_Arms[j].m_originalIndex == TRAP_INDEX) {
				if(m_Arms[j].m_nPulls > CHANGE_PROB_BOUNDARY) changeNum++;
				break;
			}
		}

		if(m_nExp>=10 && i%(m_nExp/10)==0) cerr << i/(m_nExp/10)*10 << "% ...\t";
// 		cerr << "\nRecommend:" << recommend() << endl;
// 		cerr << "\nBest Arm:" << m_MABs.getBestArm() << endl;
// 		showLog();
	}
	cerr << "100% finished" << endl;
	cerr << "Results of " << m_nExp << " experiments with " << m_nBudgets << " simulations:\n";
	double errorProbability = (m_nExp-correctNum)/m_nExp;
	cerr << "Error Probability = " << errorProbability << "\t( " << 1.96*sqrt(errorProbability*(1.0-errorProbability)/m_nExp)<< " )" << endl;

	cerr << "Change Rate = " << changeNum/m_nExp << endl;
};
