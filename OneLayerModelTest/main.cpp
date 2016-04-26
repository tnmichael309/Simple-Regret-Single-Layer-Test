
#include "BernoulliRandomGenerator.h"
#include "MultiArmBandits.h"
#include "Algorithms.h"

void experiment();

void experiment(){

	int nArms;
	int nExperimentNum;
	int nBudgets;

	cout << "Enter # of arms: " << endl;
	cin >> nArms;
	cout << "Enter # of experiments: " << endl;
	cin >> nExperimentNum;
	cout << "Enter # of budgets: " << endl;
	cin >> nBudgets;

	double* testProb = new double[nArms];

	int caseNum = 0;
	cout << "Enter setting (1-6): " << endl;
	cin >> caseNum;

	testProb[0] = 0.5;
	if(caseNum == 1) {
		for(int i = 1; i < nArms; i++) {
			testProb[i] = 0.45;
		}
	} else if(caseNum == 2){
		for(int i = 1; i < ceil(sqrt((double)nArms))+1; i++) {
			testProb[i] = 0.5 - 1.0/(2.0*nArms);
		}
		for(int i = ceil(sqrt((double)nArms))+1; i < nArms; i++) {
			testProb[i] = 0.45;
		}
	} else if(caseNum == 3){
		for(int i = 1; i < 6; i++) {
			testProb[i] = 0.5 - 1.0/(5.0*nArms);
		}
		for(int i = 6; i < 2*ceil(sqrt((double)nArms)); i++) {
			testProb[i] = 0.49;
		}
		for(int i = 2*ceil(sqrt((double)nArms)); i < nArms; i++) {
			testProb[i] = 0.35;
		}
	} else if(caseNum == 4){
		testProb[1] = 0.5 - 1.0/(5.0*nArms);
		testProb[nArms-1] = 0.25;

		double diff = (testProb[1]-testProb[nArms-1])/(double (nArms - 2));

		for(int i = 2; i < nArms - 1; i++){
			testProb[i] = testProb[i-1]-diff;
		}
	} else if(caseNum == 5){
		testProb[1] = 0.5 - 1.0/(5.0*nArms);
		testProb[nArms-1] = 0.25;

		double multiplicant = pow(testProb[nArms-1]/testProb[1], 1/(double (nArms - 2)));

		for(int i = 2; i < nArms - 1; i++){
			testProb[i] = testProb[i-1]*multiplicant;
		}

		double* tempProb = new double[nArms];
		tempProb[0] = testProb[0];
		tempProb[1] = testProb[1];
		tempProb[nArms-1] = testProb[nArms-1];
		for(int i = 2; i < nArms - 1; i++){
			tempProb[i] = tempProb[i-1] - (testProb[nArms-i]-testProb[nArms+1-i]);
		}
		double* temp = testProb;
		testProb = tempProb;
		delete[] temp;

	} else if(caseNum == 6){
		testProb[1] = 0.5 - 1.0/(10.0*nArms);
		for(int i = 2; i < nArms ; i++){
			testProb[i] = 0.4;
		}
	} else {
		cout << "Invalid input case" << endl;
	}

	cout << "Tested Probability Sequence: " << endl;
	for(int i = 0; i < nArms; i++) {
		cout << testProb[i] << "\t";
	}
	cout << endl;

	cout << "\nStart running experiments for sequential halving" << endl;
	SequentialHalving sh(nExperimentNum,nBudgets,testProb,nArms);
	sh.runExperiments();

	cout << "\nStart running experiments for stochastic model" << endl;
	StochasticModel sm(nExperimentNum,nBudgets,testProb,nArms);
	sm.runExperiments();

	cout << "\nStart running experiments for UCB-E(1)" << endl;
	UCBE ucbe1(nExperimentNum,nBudgets,testProb,nArms,1);
	ucbe1.runExperiments();

	cout << "\nStart running experiments for UCB-E(2)" << endl;
	UCBE ucbe2(nExperimentNum,nBudgets,testProb,nArms,2);
	ucbe2.runExperiments();

	cout << "\nStart running experiments for UCB-E(4)" << endl;
	UCBE ucbe4(nExperimentNum,nBudgets,testProb,nArms,4);
	ucbe4.runExperiments();

	cout << "\nStart running experiments for UCB-E(8)" << endl;
	UCBE ucbe8(nExperimentNum,nBudgets,testProb,nArms,8);
	ucbe8.runExperiments();

	cout << "\nStart running experiments for Successive Reject" << endl;
	SuccessiveReject sr(nExperimentNum,nBudgets,testProb,nArms);
	sr.runExperiments();


	cout << "Experiment Settings: " << endl;
	cout << "# of arms: " << nArms << endl;
	cout << "# of budgets: " << nBudgets << endl;
	cout << "# of repeats: " << nExperimentNum << endl;
	cout << "setting: " << caseNum << endl;
};

int main(){

	// test1: bernoulli
	/*int nRolls = 100;
	int winCount = 0;
	RandomNumberGenerator brg(0.5);
	for(int i = 0; i < nRolls; i++) {
		double sample = brg.getSample();
		cerr << sample << endl;
		if(brg.getSample()>0) winCount++;
	}
	cout << winCount << "\t" << nRolls-winCount << endl;*/

	// test2: MultiArms
	/*double testProb[5] = {0.5, 0.4, 0.3, 0.2, 0.1};
	MultiArmBandits mab(testProb, 5);
	for(int arm = 0; arm < 5; arm++){
		cerr << "Pulling arm " << arm << " :\n";
		int winCount = 0;
		for(int i = 0; i < 1000; i++) {
			if(mab.pullArm(arm)) winCount++;
		}
		cerr << "Arm 1 win count(s):\t" << winCount << endl;
	}
	cerr << "Real best arm is:\t" << mab.getBestArm() << endl;*/

	// test3: Sequential Halving
	/*double testProb1[16] = {0.5, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45};
	SequentialHalving sh(10,2000,testProb1,16);
	sh.runExperiments();*/

	// test4: Stochastic Model
	/*double testProb2[16] = {0.5, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45};
	StochasticModel sm(2,2000,testProb2,16);
	sm.runExperiments();*/

	// test 5: UCB-E
	/*double testProb2[16] = {0.5, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45};
	UCBE ucbe(2,2000,testProb2,16,1);
	ucbe.runExperiments();*/

	// test 6: Successive Reject
	/*double testProb2[16] = {0.5, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45};
	SuccessiveReject sr(2,2000,testProb2,16);
	sr.runExperiments();*/

	experiment();

	while(1);
	return 0;
}