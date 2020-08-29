#include <iostream>
#include <string.h>
#include <chrono>
using namespace std;
using namespace chrono;

char board[35][35];		// 체스판
int N;					// 체스판 길이

// 체스판 출력 함수
void printBoard() {
	string nAryString = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	cout << "\n[ 체스판 ]";		// 10진수 -> 36진수 변환에 이용
	cout << "\n  ";
	for (int i = 0; i < N; i++)
		cout << " " << nAryString[i];
	cout << endl;

	for (int i = 0; i < N; i++) {
		cout << nAryString[i] << " |";
		for (int j = 0; j < N; j++)
			cout << board[i][j] << "|";
		cout << endl;
	}
	cout << endl;
}

// queen의 후보 위치를 검증하는 함수
bool chkQueen(int row, int col) {
	// 직선상에 queen이 존재하는지 검사
	for (int i = 0; i < col; i++)
		if (board[row][i] == 'Q')
			return false;

	// 위쪽 대각선으로 queen이 존재하는지 검사
	for (int i = row, j = col; i >= 0 && j >= 0; i--, j--)
		if (board[i][j] == 'Q')
			return false;

	// 아래쪽 대각선으로 queen이 존재하는지 검사
	for (int i = row, j = col; j >= 0 && i < N; i++, j--)
		if (board[i][j] == 'Q')
			return false;

	return true;
}

// backTracking으로 솔루션 찾기
bool backTrackingSolution(int col) {
	if (col == N) {			// 마지막 열까지 검사를 통과했다면,
		return true;		// true 리턴
	}

	bool result = false;
	for (int i = 0; i < N; i++) {
		if (chkQueen(i, col) == true) {		// 검사 위치에 Queen이 와도 문제 없다면,
			board[i][col] = 'Q';			// Queen 배치

			result = backTrackingSolution(col + 1);	// 다음 행 검사 시작
			if (result)						// 이미 해답이 나왔으면,
				return true;				// 탐색 종료

			board[i][col] = '_';			// 아니면 backtracking 진행
		}
	}
	return false;
}

int main() {
	bool found = false;
	memset(board, '_', sizeof(board));
	high_resolution_clock::time_point start, end;					// 시간 측정을 위한 변수 선언

	cout << "[ 12161756 윤성호 - Backtracking ]" << endl;
	cout << "**입력 가능한 체스판의 최대 길이는 35입니다." << endl;
	cout << "체스판 길이(n) : ";

	cin >> N;

	start = high_resolution_clock::now();							// 측정 시작
	found = backTrackingSolution(0);
	end = high_resolution_clock::now();								// 측정 끝

	milliseconds gap = duration_cast<milliseconds>(end - start);	// 소요시간 계산
	cout << "소요 시간      : " << gap.count() << " ms" << endl;	// 소요시간 출력

	if (found == true)												// 솔루션을 찾았다면,
		printBoard();												// 체스판 출력
	else
		cout << "솔루션이 존재하지 않습니다." << endl;				// 못찾았으면,
																	// 메시지 출력
	return 0;
}