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

	m_TrapIndex = 7;
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
	
	// can only done once
	m_MABs.setTrap(m_TrapIndex);
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
			if(m_Arms[j].m_originalIndex == m_TrapIndex) {
				if(m_Arms[j].m_nPulls > 125) changeNum++;
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