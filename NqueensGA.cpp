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

#define POPULATION 30			// 개체군의 크기
#define TOUR_N 20				// 토너먼트 선택의 n
#define CROSS_PROB 0.8			// 교배 확률
#define MUT_PROB 0.01			// 돌연변이 확률

// 36진수 <-> 10진수 변환에 이용
string nAryString = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
int boardSize, maxFit;	// 입력 받은 체스판의 길이, 최대 적응도

struct entity {
	string genome;		// 유전자형
	int fit = 0;		// 적응도
};
typedef vector<entity*> generation_type;
generation_type generation;

// 개체 생성
entity* createNode() {
	entity* newNode = new entity;
	return newNode;
}

// 0 ~ 1 사이 난수 생성
double randDouble() {
	return rand() / (RAND_MAX + 1.0);
}

// 랜덤 유전자 인덱스 생성
int randGenomeInd() {
	return rand() % boardSize;
}

// 36진수 -> 10진수 변환
int convertDecimal(char nary) {
	int i;
	for (i = 0; i < boardSize; i++)
		if (nary == nAryString[i])
			break;
	return i + 1;
}

// 솔루션 검증
bool isSolution(entity* test) {
	if (test->fit == maxFit)			// 검증할 개체의 적응도와
		return true;					// 최대 적응도가 같다면 솔루션 발견
	return false;
}

// 적응도 계산
int calFitness(string g) {
	int fitness = maxFit;							// 최대 fitness에서 깍는 식
	for (int i = 0; i < boardSize; i++)				// i행과
		for (int j = i + 1; j < boardSize; j++) {	// i의 바로 아래행 j 비교
			int col_i = convertDecimal(g[i]);		// i와 j행의 열 값(퀸의 위치)을
			int col_j = convertDecimal(g[j]);		// 10진수로 변환

			if ((i - col_i == j - col_j) || (i + col_i == j + col_j))
				fitness--;							// i행과 j행의 동일한 대각선 상에
		}											// 위치하면 적응도 -1

	return fitness;									// 계산한 적응도 반환
}

// Rank Scaling (적응도 가공)
void scaling(generation_type gen) {

	// bubble sort
	// 개체를 적응도 기준 내림차순 정렬
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

	for (int i = 0; i < POPULATION; i++)	// 높은 적응도는 빠른 순위값 부여
		generation[i]->fit = i;				// 앞에서부터 0, 1, 2... 차례로 부여
}

// Hexatrigesimal Encording (36진수 인코딩), 1세대 생성
void getFirstGeneration() {
	string genomeStr = "";					// 유전자형을 담을 string
	entity* newEntity;						// 개체 선언
	for (int i = 0; i < boardSize; i++) {
		ostringstream ostr;
		ostr << nAryString[i];
		genomeStr += ostr.str();			// 설정한 체스판 길이만큼 유전자형 생성
	}										// "123456.."

	// 랜덤 개체를 생성하여 1세대 인구 집단에 추가
	for (int i = 0; i < POPULATION; i++) {
		random_shuffle(genomeStr.begin(), genomeStr.end());	// 유전자형을 무작위로 섞기
		newEntity = createNode();							// 새로운 개체 생성
		newEntity->genome = genomeStr;						// 개체의 유전자 설정
		newEntity->fit = calFitness(genomeStr);				// 적응도 설정
		generation.push_back(newEntity);					// 인구 집단에 추가
	}
	scaling(generation);									// 1세대 개체의 적응도 가공
}

// Tournament Selection (선택 연산자)
entity* selection(int n) {
	int bestRank = POPULATION;						// 스케일링한 후의 적응도, 즉 순위를 이용
	int bestIdx = 0, tryIdx = 0;					// 순위값이 작으면 적응도가 높다는 의미

	for (int i = 0; i < n; i++) {					// n번 반복
		tryIdx = rand() % POPULATION;				// 랜덤 인구 인덱스 생성

		if (generation[tryIdx]->fit < bestRank) {	// 랜덤 개체의 순위가 최적 순위보다 앞서면,
			bestRank = generation[tryIdx]->fit;		// 해당 순위를 최적으로 설정
			bestIdx = tryIdx;						// 해당 index를 최적으로 설정
		}
	}
	return generation[bestIdx];						// 가장 앞선 순위를 가진 개체 반환
}

// Position-based Crossover (교배 연산자)
entity* crossover(entity* dad, entity* mom) {
	entity* child = createNode();					// 자식 개체 생성

	int p1 = randGenomeInd();
	int p2 = randGenomeInd();
	int p3 = randGenomeInd();
	while (p1 == p2 || p2 == p3 || p1 == p3) {
		p2 = randGenomeInd();						// 서로 다른 3개의
		p3 = randGenomeInd();						// 랜덤 유전자 인덱스 생성
	}

	queue<char> momOrder;							// mom 유전자의 순서를 저장하는 큐
	child->genome = dad->genome;					// child 유전자는 우선 dad 것을 기반

	for (int i = 0; i < boardSize; i++)
		if (mom->genome[i] != dad->genome[p1] && mom->genome[i] != dad->genome[p2]
			&& mom->genome[i] != dad->genome[p3])	// mom 유전자 중에서
			momOrder.push(mom->genome[i]);			// dad 쪽에서 선택된 3개의 유전자는 제외,
													// 나머지 유전자를 순서대로 저장
	for (int i = 0; i < boardSize; i++) {
		if (i != p1 && i != p2 && i != p3) {		// 선택했던 3개의 index를 제외한 나머지 index에
			child->genome[i] = momOrder.front();	// mom 유전자를 순서대로 덮어 씌움
			momOrder.pop();							// 덮어 씌운 유전자형은 큐에서 제거
		}
	}
	return child;									// 자식 개체 반환
}

// Insertion mutation (변이 연산자)
entity* mutate(entity* ent) {
	int from = 0, to = 0;
	while (from >= to) {
		from = randGenomeInd();					// 서로 다르며, from < to인
		to = randGenomeInd();					// 유전자 인덱스 2개 생성
	}

	char fromG = ent->genome[from];				// from 위치의 유전자 기억

	for (int i = from; i < to; i++)				// from 바로 다음 ~ to까지
		ent->genome[i] = ent->genome[i + 1];	// 한 칸씩 좌로 이동

	ent->genome[to] = fromG;					// to 위치에 from 위치에 있던 유전자 삽입

	return ent;									// 개체 반환
}

// 유전 알고리즘으로 솔루션 탐색
entity* geneticAlgorithm() {
	entity* dad, * mom, * baby = NULL;			// 개체 생성
	bool found = false;
	while (found == false) {					// 솔루션을 찾을 때까지 반복
		generation_type new_generation;			// 새로운 세대 생성
		for (int i = 0; i < POPULATION; i++) {	// 정해진 개체 수 만큼 loop

			dad = selection(TOUR_N);			// 아빠 개체 selection
			mom = selection(TOUR_N);			// 엄마 개체 selection

			if (CROSS_PROB > randDouble())		// crossover 확률에 들면,
				baby = crossover(dad, mom);		// 진행

			else								// 아닌 경우,
				baby = dad;						// 아빠 개체를 자식으로 넘겨
												// 다음 세대로 전달

			if (MUT_PROB > randDouble())		// mutation 확률에 들면,
				baby = mutate(baby);			// 시행

			baby->fit = calFitness(baby->genome); // 적응도 재계산

			if (isSolution(baby)) {				// 현재 자식 솔루션인지 검증
				found = true;					// 맞다면,
				return baby;					// 자식 반환
			}

			new_generation.push_back(baby);		// 자식을 다음 세대에 포함
		}
		generation = new_generation;			// 현재 세대 반영
		scaling(generation);
	}
	return baby;
}

// 체스판 출력 함수
void printBoard(entity* sol) {
	cout << "\n[ 체스판 ]";
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
	srand(unsigned(time(NULL)));									// 매번 새로운 난수를 얻기 위해
	cout << "[ 12161756 윤성호 - Genetic Algorithm ]" << endl;
	cout << "**입력 가능한 체스판 길이 범위는 4 ~ 35입니다." << endl;
	cout << "**각 행에서 Queen의 위치가 순서대로 출력됩니다." << endl << endl;
	cout << "체스판 길이(n) : ";
	cin >> boardSize;												// 체스판 길이 입력
	maxFit = boardSize * boardSize;									// Fitness의 최대값

	high_resolution_clock::time_point start, end;					// 시간 측정을 위한 변수 선언

	start = high_resolution_clock::now();							// 측정 시작

	getFirstGeneration();											// 1세대 인구 집단 생성
	entity* sol = geneticAlgorithm();								// 유전 알고리즘으로 solution 탐색								
	cout << "GA solution    : " << sol->genome << endl;				// soltuion(유전자형) 출력

	end = high_resolution_clock::now();								// 측정 끝

	milliseconds gap = duration_cast<milliseconds>(end - start);	// 소요시간 계산
	cout << "소요 시간      : " << gap.count() << " ms" << endl;	// 소요시간 출력

	printBoard(sol);												// 체스판 출력

	return 0;
}