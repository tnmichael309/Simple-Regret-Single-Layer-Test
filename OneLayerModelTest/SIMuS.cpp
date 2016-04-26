#include "SIMuS.h"


StochasticModel::StochasticModel(int expNum, int numBudget, double* probs, int nArms):Algorithms(expNum, numBudget, probs, nArms){m_lastBestMove = 0;};
StochasticModel::~StochasticModel(){};

void StochasticModel::reset(){
	for(int i = 0; i < m_Arms.size(); i++) {
		m_Arms[i].variance = 10.0;
		m_Arms[i].mean = pullArm(i);
		m_Arms[i].m_nPulls = 1;
		m_Arms[i].m_originalIndex = i;
		m_Arms[i].meanOfMean = m_Arms[i].mean;
	}
};

int StochasticModel::select(){

	// sort by win rate
	sort(m_Arms.begin(),m_Arms.end(), armInfo::armInfoCompare);
	m_lastBestMove = m_Arms[0].m_originalIndex;

	vector<int> vMoveCount;
	vector<double> vVariance;

	static int counter = 1;
	counter ++;

	// normalize w.r.t sample nums
	for(int i = 0; i < m_Arms.size(); i++){
		vMoveCount.push_back(m_Arms[i].m_nPulls);
		vMoveCount[i] = vMoveCount[i] >= 1 ? vMoveCount[i] : 1.0;
		vVariance.push_back(m_Arms[i].variance / vMoveCount[i]);
	}

	double dMaxZValue = 0.0;
	double dMinZValue = 10000.0;
	for(int i = 0; i < m_Arms.size(); i++){

		if(i == 0) continue;

		double dSquareSumOfTwoDeviations = vVariance[0] + vVariance[i];
		double dDifferenceOfWinRate = m_Arms[0].mean-m_Arms[i].mean;
		double dZValue = dDifferenceOfWinRate/sqrt(dSquareSumOfTwoDeviations);

		if(dZValue > dMaxZValue) dMaxZValue = dZValue;
		if(dZValue < dMinZValue) dMinZValue = dZValue;
	}
	if(dMaxZValue == dMinZValue) {
		dMaxZValue = 1.0;
		dMinZValue = 0.0;
	}

	double dLargestGradient = -1.0;
	int iSelectChild = -1;
	double dGradientOfBestChild = 0.0;
	for(int i = 0; i < m_Arms.size(); i++){
		if(i == 0) continue;

		double dSquareSumOfTwoDeviations = vVariance[0] + vVariance[i];
		double dDifferenceOfWinRate = m_Arms[0].mean-m_Arms[i].mean;
		double dZValue = dDifferenceOfWinRate/sqrt(dSquareSumOfTwoDeviations);

		// scale the zValue to 0 ~ 1, where z value has better linearity
		//dZValue = (dZValue - dMinZValue)/(dMaxZValue-dMinZValue);

		int n1 = vMoveCount[0];
		int n2 = vMoveCount[i];
		double degreeOfFreedom = ((n1<=1) || (n2<=1)) ? 1 : pow(vVariance[0]+vVariance[i],2.0)/(pow(vVariance[0],2.0)/(n1-1)+pow(vVariance[i],2.0)/(n2-1));
		degreeOfFreedom = degreeOfFreedom <= 1 ? 1 : degreeOfFreedom;


		double dGradient = calculateStudentTPDF(dZValue,degreeOfFreedom)/calculateStudentTCDF(dZValue,degreeOfFreedom)/dSquareSumOfTwoDeviations*(-1*sqrt(dSquareSumOfTwoDeviations) - 1/sqrt(dSquareSumOfTwoDeviations)*(m_Arms[i].mean - m_Arms[i].meanOfMean)*dDifferenceOfWinRate/(vMoveCount[i]+1));

		dGradient = sqrt(vVariance[i])*fabs(dGradient);


		if(dGradient > dLargestGradient){
			iSelectChild = i;
			dLargestGradient = dGradient;
		}


		dGradientOfBestChild += calculateStudentTPDF(dZValue,degreeOfFreedom)/calculateStudentTCDF(dZValue,degreeOfFreedom)/dSquareSumOfTwoDeviations*(sqrt(dSquareSumOfTwoDeviations) - 1/sqrt(dSquareSumOfTwoDeviations)*(m_Arms[0].mean - m_Arms[0].meanOfMean)*dDifferenceOfWinRate/(vMoveCount[0]+1));
	}

	dGradientOfBestChild = sqrt(vVariance[0])*fabs(dGradientOfBestChild);
	if(dGradientOfBestChild > dLargestGradient){
		iSelectChild = 0;
		dLargestGradient = dGradientOfBestChild;
	}

	m_lastSelectedChild = iSelectChild;

	return m_Arms[iSelectChild].m_originalIndex;
}

void StochasticModel::update(double result, int armNum){

	for(int i = 0 ; i < m_Arms.size(); i++){
		if(m_Arms[i].m_originalIndex == armNum) {
			m_Arms[i].mean = result; //(m_Arms[i].mean*m_Arms[i].m_nPulls + 1.0*result)/(m_Arms[i].m_nPulls+1);
			m_Arms[i].meanOfMean = (m_Arms[i].meanOfMean*m_Arms[i].m_nPulls + m_Arms[i].mean)/(m_Arms[i].m_nPulls+1);
			if(m_Arms[i].m_nPulls >= 2) {
				m_Arms[i].variance = m_Arms[i].variance*(m_Arms[i].m_nPulls-1)/(double)m_Arms[i].m_nPulls + pow(m_Arms[i].mean-m_Arms[i].meanOfMean,2.0)/(m_Arms[i].m_nPulls+1.0);
			}

			m_Arms[i].m_nPulls++;
			break;
		}
	}

	double maxMean = 0.0;
	int newBestArm = 0;
	int newBestArmIndex = 0;
	for(int i = 0 ; i < m_Arms.size(); i++){
		if(i == 0 || m_Arms[i].mean > maxMean){
			maxMean = m_Arms[i].mean;
			newBestArm = i;
			newBestArmIndex = m_Arms[i].m_originalIndex;
		}
	}

#if isResetVariance == 1
	if(newBestArmIndex != m_lastBestMove){

#if isResetHalf == 0
		m_Arms[0].variance = 10.0;
		m_Arms[newBestArm].variance = 10.0;
#else
		if(m_lastSelectedChild == 0){
			m_Arms[0].variance = 10.0;
			m_Arms[newBestArm].variance = 10.0;
		}else{
			m_Arms[newBestArm].variance = 10.0;
		}
#endif

	}
#endif

};

int StochasticModel::recommend(){
	vector<int> vMoveCount;
	vector<double> vVariance;
	vector<double> vMeans;

	// normalize w.r.t sample nums
	for(int i = 0; i < m_Arms.size(); i++){
		vMeans.push_back(m_Arms[i].mean);
		vMoveCount.push_back(m_Arms[i].m_nPulls);
		vMoveCount[i] = vMoveCount[i] >= 1 ? vMoveCount[i] : 1.0;
		vVariance.push_back(m_Arms[i].variance / vMoveCount[i]);
	}

	double dMaxWinRate = 0.0;
	double dMaxBestProbability = 0.0;
	int selectedIndex = 0;
	for(int i = 0; i < m_Arms.size(); i++){
		double bestProbability = calculateStudentTBestProbability(i, vMeans, vVariance, vMoveCount);

		if(bestProbability > dMaxBestProbability){
			dMaxBestProbability = bestProbability;
			selectedIndex = i;
		}
	}

	return m_Arms[selectedIndex].m_originalIndex;
};

void StochasticModel::showLog(){
	cerr << "Index\tArm Pulls\tMean\tVariance\tBestProbability" << endl;

	vector<int> vMoveCount;
	vector<double> vVariance;
	vector<double> vMeans;

	// normalize w.r.t sample nums
	for(int i = 0; i < m_Arms.size(); i++){
		vMeans.push_back(m_Arms[i].mean);
		vMoveCount.push_back(m_Arms[i].m_nPulls);
		vMoveCount[i] = vMoveCount[i] >= 1 ? vMoveCount[i] : 1.0;
		vVariance.push_back(m_Arms[i].variance / vMoveCount[i]);
	}

	double dMaxWinRate = 0.0;
	double dMaxBestProbability = 0.0;
	int selectedIndex = 0;
	for(int i = 0; i < m_Arms.size(); i++){
		double bestProbability = calculateStudentTBestProbability(i, vMeans, vVariance, vMoveCount);
		cerr << m_Arms[i].m_originalIndex << "\t" << m_Arms[i].m_nPulls << "\t" << m_Arms[i].mean << "\t" << m_Arms[i].variance << "\t" << bestProbability << endl;
	}

	cerr << endl;
};

double StochasticModel::calculateStudentTPDF(double tValue, double degreeOfFreedom)
{
	students_t dist(degreeOfFreedom);

	return pdf(dist, fabs(tValue));
};

// separability
double StochasticModel::calculateStudentTCDF(double tValue, double degreeOfFreedom)
{
	students_t dist(degreeOfFreedom);

	return cdf(dist, fabs(tValue));

};

// win rate
double StochasticModel::calculateStudentTWinRate(double tValue, double degreeOfFreedom)
{
	students_t dist(degreeOfFreedom);

	return cdf(dist, tValue);

};

double StochasticModel::calculateStudentTBestProbability(int iIndex, vector<double> &vMeans, vector<double> &vDeviations, vector<int> &vMoveCounts)
{
	double dResult = 1.0;

	for(int i = 0; i < vMeans.size(); i++){
		if(i == iIndex) continue;
		double zValue = (vMeans[iIndex]-vMeans[i])/sqrt(vDeviations[iIndex]+vDeviations[i]);

		int n1 = vMoveCounts[iIndex];
		int n2 = vMoveCounts[i];
		double degreeOfFreedom = ((n1<=1) || (n2<=1)) ? 1 : pow(vDeviations[iIndex]+vDeviations[i],2.0)/(pow(vDeviations[iIndex],2.0)/(n1-1)+pow(vDeviations[i],2.0)/(n2-1));
		degreeOfFreedom = degreeOfFreedom <= 1 ? 1 : degreeOfFreedom;

		dResult *= calculateStudentTWinRate(zValue,degreeOfFreedom);
	}
	return dResult;
}
