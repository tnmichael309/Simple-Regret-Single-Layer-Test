#include "MultiArmBandits.h"

MultiArmBandits::MultiArmBandits(double* probs, int nArms){
	
	for(int i = 0; i < nArms; i++){
		RandomNumberGenerator brg(probs[i]);
		m_Arms.push_back(brg);
	}
	m_nArms = nArms;
};

MultiArmBandits::~MultiArmBandits(){

};

double MultiArmBandits::pullArm(int i){
	if(i < 0 || i >= m_nArms) {
		cerr << "Invalid arm pulls" << endl;
		return false;
	}

	return m_Arms[i].getSample();
};

int MultiArmBandits::getBestArm(){
	int bestIndex = 0;
	double bestProb = 0.0;
	for(int i = 0; i < m_nArms; i++){
		double prob = m_Arms[i].getProb();
		if(prob > bestProb){
			bestProb = prob;
			bestIndex = i;
		}
	}
	return bestIndex;
}

void MultiArmBandits::reset(){
	for(int i = 0; i < m_Arms.size(); i++){
		m_Arms[i].reset();
	}
}

void MultiArmBandits::setTrap(int i){
	double scoreDiff = 2*(m_Arms[0].getProb() - m_Arms[1].getProb());
	if(i == 0){
		m_Arms[0].setTrap(scoreDiff);
	}else if(i == 1){
		m_Arms[1].setTrap(-1.0*scoreDiff);
	}else {
		scoreDiff = (m_Arms[0].getProb() - m_Arms[1].getProb()) + (m_Arms[0].getProb() - m_Arms[i].getProb());
		m_Arms[i].setTrap(-1.0*scoreDiff);
	}
}
