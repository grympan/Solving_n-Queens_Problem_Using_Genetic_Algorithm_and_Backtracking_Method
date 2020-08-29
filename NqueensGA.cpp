#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <map>
#include <list>
#include <queue>
#include <chrono>
using namespace std;
using namespace chrono;

#define POPULATION 30			// ��ü���� ũ��
#define TOUR_N 20				// ��ʸ�Ʈ ������ n
#define CROSS_PROB 0.8			// ���� Ȯ��
#define MUT_PROB 0.01			// �������� Ȯ��

// 36���� <-> 10���� ��ȯ�� �̿�
string nAryString = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
int boardSize, maxFit;	// �Է� ���� ü������ ����, �ִ� ������

struct entity {
	string genome;		// ��������
	int fit = 0;		// ������
};
typedef vector<entity*> generation_type;
generation_type generation;

// ��ü ����
entity* createNode() {
	entity* newNode = new entity;
	return newNode;
}

// 0 ~ 1 ���� ���� ����
double randDouble() {
	return rand() / (RAND_MAX + 1.0);
}

// ���� ������ �ε��� ����
int randGenomeInd() {
	return rand() % boardSize;
}

// 36���� -> 10���� ��ȯ
int convertDecimal(char nary) {
	int i;
	for (i = 0; i < boardSize; i++)
		if (nary == nAryString[i])
			break;
	return i + 1;
}

// �ַ�� ����
bool isSolution(entity* test) {
	if (test->fit == maxFit)			// ������ ��ü�� ��������
		return true;					// �ִ� �������� ���ٸ� �ַ�� �߰�
	return false;
}

// ������ ���
int calFitness(string g) {
	int fitness = maxFit;							// �ִ� fitness���� ��� ��
	for (int i = 0; i < boardSize; i++)				// i���
		for (int j = i + 1; j < boardSize; j++) {	// i�� �ٷ� �Ʒ��� j ��
			int col_i = convertDecimal(g[i]);		// i�� j���� �� ��(���� ��ġ)��
			int col_j = convertDecimal(g[j]);		// 10������ ��ȯ

			if ((i - col_i == j - col_j) || (i + col_i == j + col_j))
				fitness--;							// i��� j���� ������ �밢�� ��
		}											// ��ġ�ϸ� ������ -1

	return fitness;									// ����� ������ ��ȯ
}

// Rank Scaling (������ ����)
void scaling(generation_type gen) {

	// bubble sort
	// ��ü�� ������ ���� �������� ����
	int right = POPULATION - 1;
	int temp = 0;
	while (right > 0) {
		int k = 0;
		for (int i = 0; i < right; i++)
			if (generation[i + 1]->fit > generation[i]->fit) {
				swap(generation[i + 1], generation[i]);
				k = i;
			}
		right = k;
	}

	for (int i = 0; i < POPULATION; i++)	// ���� �������� ���� ������ �ο�
		generation[i]->fit = i;				// �տ������� 0, 1, 2... ���ʷ� �ο�
}

// Hexatrigesimal Encording (36���� ���ڵ�), 1���� ����
void getFirstGeneration() {
	string genomeStr = "";					// ���������� ���� string
	entity* newEntity;						// ��ü ����
	for (int i = 0; i < boardSize; i++) {
		ostringstream ostr;
		ostr << nAryString[i];
		genomeStr += ostr.str();			// ������ ü���� ���̸�ŭ �������� ����
	}										// "123456.."

	// ���� ��ü�� �����Ͽ� 1���� �α� ���ܿ� �߰�
	for (int i = 0; i < POPULATION; i++) {
		random_shuffle(genomeStr.begin(), genomeStr.end());	// ���������� �������� ����
		newEntity = createNode();							// ���ο� ��ü ����
		newEntity->genome = genomeStr;						// ��ü�� ������ ����
		newEntity->fit = calFitness(genomeStr);				// ������ ����
		generation.push_back(newEntity);					// �α� ���ܿ� �߰�
	}
	scaling(generation);									// 1���� ��ü�� ������ ����
}

// Tournament Selection (���� ������)
entity* selection(int n) {
	int bestRank = POPULATION;						// �����ϸ��� ���� ������, �� ������ �̿�
	int bestIdx = 0, tryIdx = 0;					// �������� ������ �������� ���ٴ� �ǹ�

	for (int i = 0; i < n; i++) {					// n�� �ݺ�
		tryIdx = rand() % POPULATION;				// ���� �α� �ε��� ����

		if (generation[tryIdx]->fit < bestRank) {	// ���� ��ü�� ������ ���� �������� �ռ���,
			bestRank = generation[tryIdx]->fit;		// �ش� ������ �������� ����
			bestIdx = tryIdx;						// �ش� index�� �������� ����
		}
	}
	return generation[bestIdx];						// ���� �ռ� ������ ���� ��ü ��ȯ
}

// Position-based Crossover (���� ������)
entity* crossover(entity* dad, entity* mom) {
	entity* child = createNode();					// �ڽ� ��ü ����

	int p1 = randGenomeInd();
	int p2 = randGenomeInd();
	int p3 = randGenomeInd();
	while (p1 == p2 || p2 == p3 || p1 == p3) {
		p2 = randGenomeInd();						// ���� �ٸ� 3����
		p3 = randGenomeInd();						// ���� ������ �ε��� ����
	}

	queue<char> momOrder;							// mom �������� ������ �����ϴ� ť
	child->genome = dad->genome;					// child �����ڴ� �켱 dad ���� ���

	for (int i = 0; i < boardSize; i++)
		if (mom->genome[i] != dad->genome[p1] && mom->genome[i] != dad->genome[p2]
			&& mom->genome[i] != dad->genome[p3])	// mom ������ �߿���
			momOrder.push(mom->genome[i]);			// dad �ʿ��� ���õ� 3���� �����ڴ� ����,
													// ������ �����ڸ� ������� ����
	for (int i = 0; i < boardSize; i++) {
		if (i != p1 && i != p2 && i != p3) {		// �����ߴ� 3���� index�� ������ ������ index��
			child->genome[i] = momOrder.front();	// mom �����ڸ� ������� ���� ����
			momOrder.pop();							// ���� ���� ���������� ť���� ����
		}
	}
	return child;									// �ڽ� ��ü ��ȯ
}

// Insertion mutation (���� ������)
entity* mutate(entity* ent) {
	int from = 0, to = 0;
	while (from >= to) {
		from = randGenomeInd();					// ���� �ٸ���, from < to��
		to = randGenomeInd();					// ������ �ε��� 2�� ����
	}

	char fromG = ent->genome[from];				// from ��ġ�� ������ ���

	for (int i = from; i < to; i++)				// from �ٷ� ���� ~ to����
		ent->genome[i] = ent->genome[i + 1];	// �� ĭ�� �·� �̵�

	ent->genome[to] = fromG;					// to ��ġ�� from ��ġ�� �ִ� ������ ����

	return ent;									// ��ü ��ȯ
}

// ���� �˰������� �ַ�� Ž��
entity* geneticAlgorithm() {
	entity* dad, * mom, * baby = NULL;			// ��ü ����
	bool found = false;
	while (found == false) {					// �ַ���� ã�� ������ �ݺ�
		generation_type new_generation;			// ���ο� ���� ����
		for (int i = 0; i < POPULATION; i++) {	// ������ ��ü �� ��ŭ loop

			dad = selection(TOUR_N);			// �ƺ� ��ü selection
			mom = selection(TOUR_N);			// ���� ��ü selection

			if (CROSS_PROB > randDouble())		// crossover Ȯ���� ���,
				baby = crossover(dad, mom);		// ����

			else								// �ƴ� ���,
				baby = dad;						// �ƺ� ��ü�� �ڽ����� �Ѱ�
												// ���� ����� ����

			if (MUT_PROB > randDouble())		// mutation Ȯ���� ���,
				baby = mutate(baby);			// ����

			baby->fit = calFitness(baby->genome); // ������ ����

			if (isSolution(baby)) {				// ���� �ڽ� �ַ������ ����
				found = true;					// �´ٸ�,
				return baby;					// �ڽ� ��ȯ
			}

			new_generation.push_back(baby);		// �ڽ��� ���� ���뿡 ����
		}
		generation = new_generation;			// ���� ���� �ݿ�
		scaling(generation);
	}
	return baby;
}

// ü���� ��� �Լ�
void printBoard(entity* sol) {
	cout << "\n[ ü���� ]";
	cout << "\n  ";
	for (int i = 0; i < boardSize; i++) {
		cout << " " << nAryString[i];
	}
	cout << endl;

	for (int i = 0; i < boardSize; i++) {
		int column = convertDecimal(sol->genome[i]);
		cout << nAryString[i] << " |";
		for (int j = 1; j <= boardSize; j++) {
			if (j == column)
				cout << "Q|";
			else
				cout << "_|";
		}
		cout << endl;
	}
}

int main() {
	srand(unsigned(time(NULL)));									// �Ź� ���ο� ������ ��� ����
	cout << "[ 12161756 ����ȣ - Genetic Algorithm ]" << endl;
	cout << "**�Է� ������ ü���� ���� ������ 4 ~ 35�Դϴ�." << endl;
	cout << "**�� �࿡�� Queen�� ��ġ�� ������� ��µ˴ϴ�." << endl << endl;
	cout << "ü���� ����(n) : ";
	cin >> boardSize;												// ü���� ���� �Է�
	maxFit = boardSize * boardSize;									// Fitness�� �ִ밪

	high_resolution_clock::time_point start, end;					// �ð� ������ ���� ���� ����

	start = high_resolution_clock::now();							// ���� ����

	getFirstGeneration();											// 1���� �α� ���� ����
	entity* sol = geneticAlgorithm();								// ���� �˰������� solution Ž��								
	cout << "GA solution    : " << sol->genome << endl;				// soltuion(��������) ���

	end = high_resolution_clock::now();								// ���� ��

	milliseconds gap = duration_cast<milliseconds>(end - start);	// �ҿ�ð� ���
	cout << "�ҿ� �ð�      : " << gap.count() << " ms" << endl;	// �ҿ�ð� ���

	printBoard(sol);												// ü���� ���

	return 0;
}