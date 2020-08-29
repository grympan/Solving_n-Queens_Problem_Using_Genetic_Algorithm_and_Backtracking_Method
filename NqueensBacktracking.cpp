#include <iostream>
#include <string.h>
#include <chrono>
using namespace std;
using namespace chrono;

char board[35][35];		// ü����
int N;					// ü���� ����

// ü���� ��� �Լ�
void printBoard() {
	string nAryString = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	cout << "\n[ ü���� ]";		// 10���� -> 36���� ��ȯ�� �̿�
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

// queen�� �ĺ� ��ġ�� �����ϴ� �Լ�
bool chkQueen(int row, int col) {
	// ������ queen�� �����ϴ��� �˻�
	for (int i = 0; i < col; i++)
		if (board[row][i] == 'Q')
			return false;

	// ���� �밢������ queen�� �����ϴ��� �˻�
	for (int i = row, j = col; i >= 0 && j >= 0; i--, j--)
		if (board[i][j] == 'Q')
			return false;

	// �Ʒ��� �밢������ queen�� �����ϴ��� �˻�
	for (int i = row, j = col; j >= 0 && i < N; i++, j--)
		if (board[i][j] == 'Q')
			return false;

	return true;
}

// backTracking���� �ַ�� ã��
bool backTrackingSolution(int col) {
	if (col == N) {			// ������ ������ �˻縦 ����ߴٸ�,
		return true;		// true ����
	}

	bool result = false;
	for (int i = 0; i < N; i++) {
		if (chkQueen(i, col) == true) {		// �˻� ��ġ�� Queen�� �͵� ���� ���ٸ�,
			board[i][col] = 'Q';			// Queen ��ġ

			result = backTrackingSolution(col + 1);	// ���� �� �˻� ����
			if (result)						// �̹� �ش��� ��������,
				return true;				// Ž�� ����

			board[i][col] = '_';			// �ƴϸ� backtracking ����
		}
	}
	return false;
}

int main() {
	bool found = false;
	memset(board, '_', sizeof(board));
	high_resolution_clock::time_point start, end;					// �ð� ������ ���� ���� ����

	cout << "[ 12161756 ����ȣ - Backtracking ]" << endl;
	cout << "**�Է� ������ ü������ �ִ� ���̴� 35�Դϴ�." << endl;
	cout << "ü���� ����(n) : ";

	cin >> N;

	start = high_resolution_clock::now();							// ���� ����
	found = backTrackingSolution(0);
	end = high_resolution_clock::now();								// ���� ��

	milliseconds gap = duration_cast<milliseconds>(end - start);	// �ҿ�ð� ���
	cout << "�ҿ� �ð�      : " << gap.count() << " ms" << endl;	// �ҿ�ð� ���

	if (found == true)												// �ַ���� ã�Ҵٸ�,
		printBoard();												// ü���� ���
	else
		cout << "�ַ���� �������� �ʽ��ϴ�." << endl;				// ��ã������,
																	// �޽��� ���
	return 0;
}