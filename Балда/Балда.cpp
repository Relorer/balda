#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <cstring>
#include <ctime>
#define MAX 1000
#define LEN 30

enum state { empty, letter, possible, passed };

struct findInfoWord {
	char letter;
	int x, y;
	int length = 0;
	char str[LEN];
};

void fillFindInfoWord(findInfoWord& info, const char* str, int x, int y, char letter) {
	info.letter = letter;
	info.x = x;
	info.y = y;
	info.length = strlen(str);
	strcpy(info.str, str);
}

char** dictionary;
int sizeDictionary;
int maxLengthWord = 0;

void foundMaxLengthWord() {
	for (int i = 0; i < sizeDictionary; i++) {
		int len = strlen(dictionary[i]);
		if (len > maxLengthWord) maxLengthWord = len;
	}
}

void deleteDictionary() {
	for (int i = 0; i < sizeDictionary; i++) {
		delete[] dictionary[i];
	}
	delete[] dictionary;
}

bool checkSorting() {
	for (int i = 0; i < sizeDictionary - 1; i++) {
		if (strcmp(dictionary[i], dictionary[i + 1]) > 0) return false;
	}
	return true;
}

void sort() {
	for (int i = 0; i < sizeDictionary - 1; i++) {
		for (int j = 0; j < sizeDictionary - i - 1; j++) {
			if (strcmp(dictionary[j], dictionary[j + 1]) > 0) {
				char* temp;
				temp = dictionary[j];
				dictionary[j] = dictionary[j + 1];
				dictionary[j + 1] = temp;
			}
		}
	}
}

void rewriteFileDictionary(const char* str) {
	FILE* dictionaryFile = fopen(str, "w");
	for (int i = 0; i < sizeDictionary; i++) {
		fprintf(dictionaryFile, "%s\n", dictionary[i]);
	}
	fclose(dictionaryFile);
}

void addWordToDictionary(const char* str) {
	int len = strlen(str);
	if (len > 25) false;
	if (len > maxLengthWord) maxLengthWord = len;
	char** temp = new char* [sizeDictionary + 1];
	bool check = true;
	for (int i = 0, j = 0; i < sizeDictionary + 1; i++) {
		temp[i] = new char[LEN];
		if (check && strcmp(str, dictionary[j]) < 0) {
			strcpy(temp[i], str);
			check = false;
		}
		else strcpy(temp[i], dictionary[j++]);
	}

	deleteDictionary();
	sizeDictionary++;
	dictionary = temp;
	temp = nullptr;
}

int rememberDictionary(const char* str) {
	FILE* dictionaryFile = fopen(str, "r");
	char buff[LEN];
	sizeDictionary = 0;
	while (fgets(buff, 30, dictionaryFile)) sizeDictionary++;

	fseek(dictionaryFile, 0, 0);

	dictionary = new char* [sizeDictionary];
	for (int i = 0; i < sizeDictionary; i++) {
		dictionary[i] = new char[LEN];
		fgets(dictionary[i], 30, dictionaryFile);
		dictionary[i][strlen(dictionary[i]) - 1] = '\0';
	}

	fclose(dictionaryFile);
	if (!checkSorting()) sort();
	foundMaxLengthWord();
	return sizeDictionary;
}

bool isWord(const char* str)
{
	int begin = 0;
	int end = sizeDictionary - 1;
	while (begin <= end) {
		int middle = (begin + end) / 2;
		int result = strcmp(dictionary[middle], str);
		if (result == 0) return true;
		if (result < 0) {
			begin = middle + 1;
		}
		else if (result > 0) {
			end = middle - 1;
		}
	}
	return false;
}

char wordsOnTheField[MAX * LEN] = " \0";
bool haveWordUsed(const char* str) 
{
	char interim[LEN];
	strcpy(interim + sizeof(char), str);
	interim[0] = ' ';
	interim[strlen(interim) + 1] = '\0';
	interim[strlen(interim)] = ' ';
	if (strstr(wordsOnTheField, interim)) return true;
	return false;
}

void addWordsInEnteredWords(const char* str) {
	snprintf(wordsOnTheField + sizeof(char) * strlen(wordsOnTheField), sizeof(wordsOnTheField), "%s ", str);
}

void generateRandomWord(char** field, state * **stateField, int size)
{
	int curlenght = 0;
	int index = 0;
	while (curlenght != size) {
		index = rand() % sizeDictionary + 1;
		curlenght = strlen(dictionary[index]);
	}
	addWordsInEnteredWords(dictionary[index]);
	for (int i = 0; i < size; i++) {
		field[size / 2][i] = dictionary[index][i];
		stateField[0][size / 2][i] = letter;
		stateField[0][size / 2 + 1][i] = possible;
		stateField[0][size / 2 - 1][i] = possible;
	}
}

int numlen(int num) {
	int res = 0;
	do {
		num /= 10;
		res++;
	} while (num > 0);
	return res;
}

void printStateField(state * **stateField, int size)
{
	printf("\n");
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			printf("%d ", stateField[0][i][j]);
		}
		printf("\t");
		for (int j = 0; j < size; j++) {
			printf("%d ", stateField[1][i][j]);
		}
		printf("\n");
	}
}

void printFullField(char** field, state * **stateField, COORD * *coordField, int size, HANDLE hConsole) {
	SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (stateField[0][i][j] == empty) {
				SetConsoleCursorPosition(hConsole, coordField[i][j]);
				printf("%c ", field[i][j]);
			}
		}
	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (stateField[0][i][j] == possible) {
				SetConsoleCursorPosition(hConsole, coordField[i][j]);
				printf("%c ", field[i][j]);
			}
		}
	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (stateField[0][i][j] == letter) {
				SetConsoleCursorPosition(hConsole, coordField[i][j]);
				printf("%c ", field[i][j]);
			}
		}
	}
}

void printField(char** field, state * **stateField, COORD * *coordField, int size, HANDLE hConsole) //выводит поле в консоль
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (stateField[0][i][j] == possible) {
				SetConsoleCursorPosition(hConsole, coordField[i][j]);
				printf("%c ", field[i][j]);
			}
		}
	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (stateField[0][i][j] == letter) {
				SetConsoleCursorPosition(hConsole, coordField[i][j]);
				printf("%c ", field[i][j]);
			}
		}
	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (stateField[1][i][j] == letter) {
				SetConsoleCursorPosition(hConsole, coordField[i][j]);
				printf("%c ", field[i][j]);
			}
			else if (stateField[1][i][j] == passed) {
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED);
				SetConsoleCursorPosition(hConsole, coordField[i][j]);
				printf("%c ", field[i][j]);
				SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			}
		}
	}
	Sleep(3000);
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (stateField[1][i][j] == letter || stateField[1][i][j] == passed) {
				SetConsoleCursorPosition(hConsole, coordField[i][j]);
				printf("%c ", field[i][j]);
			}
		}
	}
}

void emphasisOnLetters(char** field, state * **stateField, COORD * *coordField, int size, HANDLE hConsole) {
	SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (stateField[0][i][j] == empty || stateField[0][i][j] == possible) {
				SetConsoleCursorPosition(hConsole, coordField[i][j]);
				printf("%c ", field[i][j]);
			}
		}
	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (stateField[0][i][j] == letter) {
				SetConsoleCursorPosition(hConsole, coordField[i][j]);
				printf("%c ", field[i][j]);
			}
		}
	}
}

void cellSelect(char** field, COORD * *coordField, int size, int x, int y, HANDLE hConsole) {
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
	SetConsoleCursorPosition(hConsole, coordField[y][x]);
	printf("%c ", field[y][x]);
	SetConsoleCursorPosition(hConsole, coordField[y][x]);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void cellActive(char** field, COORD * *coordField, int size, int x, int y, HANDLE hConsole) {
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	SetConsoleCursorPosition(hConsole, coordField[y][x]);
	printf("%c ", field[y][x]);
	SetConsoleCursorPosition(hConsole, coordField[y][x]);
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
}

void cellDisable(char** field, state * **stateField, COORD * *coordField, int size, int x, int y, HANDLE hConsole) {
	if (stateField[0][y][x] == empty) SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
	else if (stateField[0][y][x] == possible) SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	else if (stateField[0][y][x] == letter) SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
	SetConsoleCursorPosition(hConsole, coordField[y][x]);
	printf("%c", field[y][x]);
	SetConsoleCursorPosition(hConsole, coordField[y][x]);
}

void displayMessageInCenter(int size, int maxX, const char* str, HANDLE hConsole, int offset = 0) {
	COORD coord;
	coord.X = (maxX - strlen(str)) / 2;
	coord.Y = size + 10 + offset;
	SetConsoleCursorPosition(hConsole, coord);
	printf("%s", str);
}

void clearMessageInCenter(int size, int maxX, HANDLE hConsole, int offset = 0) {
	COORD coord;
	coord.X = 15;
	coord.Y = size + 10 + offset;
	SetConsoleCursorPosition(hConsole, coord);
	for (int i = 0; i < maxX - 30; i++)
		printf(" ");
}

void printScore(const char* str1, int score1, const char* str2, int score2, int maxX, HANDLE hConsole, bool phase) {
	if (phase) SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
	else SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	COORD coord1;
	coord1.X = 1;
	coord1.Y = 1;
	SetConsoleCursorPosition(hConsole, coord1);
	for (int i = 0; i < maxX; i++)
		printf(" ");
	SetConsoleCursorPosition(hConsole, coord1);
	printf("%s: %d", str1, score1);
	if (!phase) SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
	else SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	COORD coord2;
	coord2.X = maxX - strlen(str2) - numlen(score2) - 3;
	coord2.Y = 1;
	SetConsoleCursorPosition(hConsole, coord2);
	printf("%s: %d", str2, score2);
}

void printFoundWordPlayer1(const char* word, int maxX, HANDLE hConsole, bool clear = false) {
	static int offset = 1;
	if (clear) {
		offset = 1;
		return;
	}
	COORD coord;
	coord.X = 1;
	coord.Y = 2 + offset;
	SetConsoleCursorPosition(hConsole, coord);
	printf("%s", word);
	offset++;
}

void printFoundWordPlayer2(const char* word, int maxX, HANDLE hConsole, bool clear = false) {
	static int offset = 1;
	if (clear) {
		offset = 1;
		return;
	}
	COORD coord;
	coord.X = maxX - strlen(word) - 1;
	coord.Y = 2 + offset;
	SetConsoleCursorPosition(hConsole, coord);
	printf("%s", word);
	offset++;
}


void _foundRightPartWords(char** field, state * **stateField, int size, findInfoWord & word,
int x, int y, const int centralx, const int centraly, char wordBuff[LEN * 2], int& rightDepth, const int& leftDepth) {
	rightDepth++;
	if (rightDepth > LEN - 2 || leftDepth + rightDepth + 1 > maxLengthWord) {
		rightDepth--;
		return;
	}
	wordBuff[LEN + rightDepth] = field[y][x];
	wordBuff[LEN + rightDepth + 1] = '\0';
	stateField[0][y][x] = passed;

	if (leftDepth + rightDepth + 1 > word.length) {
		for (int i = 0; i < 32; i++) {
			wordBuff[LEN] = 'А' + i;
			if (isWord(wordBuff + sizeof(char) * (LEN - leftDepth)) && !haveWordUsed(wordBuff + sizeof(char) * (LEN - leftDepth))) {
				fillFindInfoWord(word, wordBuff + sizeof(char) * (LEN - leftDepth), centralx, centraly, wordBuff[LEN]);
				for (int i = 0; i < size; i++) {
					for (int j = 0; j < size; j++) {
						if (stateField[0][i][j] == passed) stateField[1][i][j] = letter;
						else  stateField[1][i][j] = empty;
					}
				}
				stateField[1][centraly][centralx] = passed;
			}
		}
	}

	if (y + 1 < size && stateField[0][y + 1][x] == letter) _foundRightPartWords(field, stateField, size, word, x, y + 1, centralx, centraly, wordBuff, rightDepth, leftDepth);
	if (y - 1 >= 0 && stateField[0][y - 1][x] == letter) _foundRightPartWords(field, stateField, size, word, x, y - 1, centralx, centraly, wordBuff, rightDepth, leftDepth);
	if (x + 1 < size && stateField[0][y][x + 1] == letter) _foundRightPartWords(field, stateField, size, word, x + 1, y, centralx, centraly, wordBuff, rightDepth, leftDepth);
	if (x - 1 >= 0 && stateField[0][y][x - 1] == letter) _foundRightPartWords(field, stateField, size, word, x - 1, y, centralx, centraly, wordBuff, rightDepth, leftDepth);

	stateField[0][y][x] = letter;

	wordBuff[LEN + rightDepth] = '\0';
	rightDepth--;
}

void _foundLeftPartWords(char** field, state * **stateField, int size, findInfoWord & word,
	int x, int y, const int centralx, const int centraly, char wordBuff[LEN * 2], int& leftDepth) {
	leftDepth++;
	if (leftDepth > maxLengthWord) {
		leftDepth--;
		return;
	}
	wordBuff[LEN - leftDepth] = field[y][x];
	stateField[0][y][x] = passed;

	if (leftDepth + 1 > word.length) {
		for (int i = 0; i < 32; i++) {
			wordBuff[LEN] = 'А' + i;
			wordBuff[LEN + 1] = '\0';
			if (isWord(wordBuff + sizeof(char) * (LEN - leftDepth)) && !haveWordUsed(wordBuff + sizeof(char) * (LEN - leftDepth))) {
				fillFindInfoWord(word, wordBuff + sizeof(char) * (LEN - leftDepth), centralx, centraly, wordBuff[LEN]);
				for (int i = 0; i < size; i++) {
					for (int j = 0; j < size; j++) {
						if (stateField[0][i][j] == passed) stateField[1][i][j] = letter;
						else  stateField[1][i][j] = empty;
					}
				}
				stateField[1][centraly][centralx] = passed;
			}
		}
	}

	int rightDepth = 0;
	if (centraly + 1 < size && stateField[0][centraly + 1][centralx] == letter) _foundRightPartWords(field, stateField, size, word, centralx, centraly + 1, centralx, centraly, wordBuff, rightDepth, leftDepth);
	if (centraly - 1 >= 0 && stateField[0][centraly - 1][centralx] == letter) _foundRightPartWords(field, stateField, size, word, centralx, centraly - 1, centralx, centraly, wordBuff, rightDepth, leftDepth);
	if (centralx + 1 < size && stateField[0][centraly][centralx + 1] == letter) _foundRightPartWords(field, stateField, size, word, centralx + 1, centraly, centralx, centraly, wordBuff, rightDepth, leftDepth);
	if (centralx - 1 >= 0 && stateField[0][centraly][centralx - 1] == letter) _foundRightPartWords(field, stateField, size, word, centralx - 1, centraly, centralx, centraly, wordBuff, rightDepth, leftDepth);


	if (y + 1 < size && stateField[0][y + 1][x] == letter) _foundLeftPartWords(field, stateField, size, word, x, y + 1, centralx, centraly, wordBuff, leftDepth);
	if (y - 1 >= 0 && stateField[0][y - 1][x] == letter) _foundLeftPartWords(field, stateField, size, word, x, y - 1, centralx, centraly, wordBuff, leftDepth);
	if (x + 1 < size && stateField[0][y][x + 1] == letter) _foundLeftPartWords(field, stateField, size, word, x + 1, y, centralx, centraly, wordBuff, leftDepth);
	if (x - 1 >= 0 && stateField[0][y][x - 1] == letter) _foundLeftPartWords(field, stateField, size, word, x - 1, y, centralx, centraly, wordBuff, leftDepth);
	stateField[0][y][x] = letter;
	leftDepth--;
}

void foundWords(char** field, state * **stateField, int size, findInfoWord & word, int x, int y) {

	char wordBuff[LEN * 2];

	int leftDepth = 0;
	int rightDepth = 0;
	if (y + 1 < size && stateField[0][y + 1][x] == letter) _foundLeftPartWords(field, stateField, size, word, x, y + 1, x, y, wordBuff, leftDepth);
	if (y + 1 < size && stateField[0][y + 1][x] == letter) _foundRightPartWords(field, stateField, size, word, x, y + 1, x, y, wordBuff, rightDepth, leftDepth);
	if (y - 1 >= 0 && stateField[0][y - 1][x] == letter) _foundLeftPartWords(field, stateField, size, word, x, y - 1, x, y, wordBuff, leftDepth);
	if (y - 1 >= 0 && stateField[0][y - 1][x] == letter) _foundRightPartWords(field, stateField, size, word, x, y - 1, x, y, wordBuff, rightDepth, leftDepth);
	if (x + 1 < size && stateField[0][y][x + 1] == letter) _foundLeftPartWords(field, stateField, size, word, x + 1, y, x, y, wordBuff, leftDepth);
	if (x + 1 < size && stateField[0][y][x + 1] == letter) _foundRightPartWords(field, stateField, size, word, x + 1, y, x, y, wordBuff, rightDepth, leftDepth);
	if (x - 1 >= 0 && stateField[0][y][x - 1] == letter) _foundLeftPartWords(field, stateField, size, word, x - 1, y, x, y, wordBuff, leftDepth);
	if (x - 1 >= 0 && stateField[0][y][x - 1] == letter) _foundRightPartWords(field, stateField, size, word, x - 1, y, x, y, wordBuff, rightDepth, leftDepth);


}

void foundLongestWords(char** field, state * **stateField, int size, findInfoWord & word)
{
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			if (stateField[0][y][x] == possible)
				foundWords(field, stateField, size, word, x, y);
		}
	}
}

void putWord(char** field, state * **stateField, int size, int x, int y, char let) {
	field[y][x] = let;
	stateField[0][y][x] = letter;
	if (y + 1 < size && stateField[0][y + 1][x] == empty) stateField[0][y + 1][x] = possible;
	if (y - 1 >= 0 && stateField[0][y - 1][x] == empty) stateField[0][y - 1][x] = possible;
	if (x + 1 < size && stateField[0][y][x + 1] == empty) stateField[0][y][x + 1] = possible;
	if (x - 1 >= 0 && stateField[0][y][x - 1] == empty) stateField[0][y][x - 1] = possible;
}

bool descend(char** field, state * **stateField, int size, int& score, char exword[LEN]) {
	findInfoWord word;
	foundLongestWords(field, stateField, size, word);
	if (word.length == 0) return false;
	addWordsInEnteredWords(word.str);
	putWord(field, stateField, size, word.x, word.y, word.letter);
	score += word.length;
	strcpy(exword, word.str);
	return true;
}

int turnPlayer(char** field, state * **stateField, COORD * *coordField, int size, int maxX, HANDLE hConsole, int& score, char exword[LEN]) {
	while (true) {
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				if (stateField[1][i][j] == passed)
					stateField[1][i][j] = empty;
			}
		}
		printFullField(field, stateField, coordField, size, hConsole);
		COORD posСarriage;
		posСarriage.X = size / 2;
		posСarriage.Y = size / 2;
		cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
		unsigned char ch;

		displayMessageInCenter(size, maxX, "выберите клетку и нажмите enter", hConsole);
		SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
		displayMessageInCenter(size, maxX, "нажмите esc для пропуска хода", hConsole, 1);
		displayMessageInCenter(size, maxX, "нажмите backspace, чтобы сдаться", hConsole, 3);


		//select cell
		do
		{
			ch = _getch();
			if (ch == 224)
			{
				ch = _getch();
				switch (ch)
				{
				case 72: // /\ 
					cellDisable(field, stateField, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					posСarriage.Y--;
					if (posСarriage.Y < 0) posСarriage.Y = size - 1;
					cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					break;
				case 80: // \/ 
					cellDisable(field, stateField, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					posСarriage.Y++;
					if (posСarriage.Y >= size) posСarriage.Y = 0;
					cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					break;
				case 75: // <- 
					cellDisable(field, stateField, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					posСarriage.X--;
					if (posСarriage.X < 0) posСarriage.X = size - 1;
					cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					break;
				case 77: // -> 
					cellDisable(field, stateField, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					posСarriage.X++;
					if (posСarriage.X >= size) posСarriage.X = 0;
					cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					break;
				}
			}
		} while (ch != 27 && ch != 8 && (ch != 13 || stateField[0][posСarriage.Y][posСarriage.X] != possible));

		if (ch == 27) {
			clearMessageInCenter(size, maxX, hConsole);
			clearMessageInCenter(size, maxX, hConsole, 1);
			clearMessageInCenter(size, maxX, hConsole, 3);
			return 1;
		}

		if (ch == 8) {
			clearMessageInCenter(size, maxX, hConsole);
			clearMessageInCenter(size, maxX, hConsole, 1);
			clearMessageInCenter(size, maxX, hConsole, 3);
			return 2;
		}

		cellSelect(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
		clearMessageInCenter(size, maxX, hConsole);
		clearMessageInCenter(size, maxX, hConsole, 1);
		clearMessageInCenter(size, maxX, hConsole, 3);

		displayMessageInCenter(size, maxX, "введите букву и нажмите enter", hConsole);
		SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
		displayMessageInCenter(size, maxX, "нажмите esc для отмены выбора", hConsole, 1);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
		char let = '\0';
		do
		{
			ch = _getch();
			if (ch >= 224 /*a*/ && ch <= 255 /*я*/) ch -= 32;
			if (ch >= 192 /*А*/ && ch <= 223 /*Я*/) {
				SetConsoleCursorPosition(hConsole, coordField[posСarriage.Y][posСarriage.X]);
				printf("%c", ch);
				let = ch;
				SetConsoleCursorPosition(hConsole, coordField[posСarriage.Y][posСarriage.X]);
			}
		} while ((ch != 13 || !let) && ch != 27);

		if (ch == 27) {
			clearMessageInCenter(size, maxX, hConsole, 1);
			continue;
		}

		char word[LEN]{};
		clearMessageInCenter(size, maxX, hConsole);
		int numLetter = 1;
		stateField[0][posСarriage.Y][posСarriage.X] = letter;
		field[posСarriage.Y][posСarriage.X] = let;
		COORD inserted = posСarriage;

		emphasisOnLetters(field, stateField, coordField, size, hConsole);
		displayMessageInCenter(size, maxX, "выделите первую букву слова и нажмите enter", hConsole);
		SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
		displayMessageInCenter(size, maxX, "нажмите esc для отмены выбора", hConsole, 1);

		cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
		do
		{
			if (inserted.X != posСarriage.X || inserted.Y != posСarriage.Y)
				cellSelect(field, coordField, size, inserted.X, inserted.Y, hConsole);
			ch = _getch();
			if (ch == 224)
			{
				ch = _getch();
				switch (ch)
				{
				case 72: // /\ 
					posСarriage.Y--;
					if (posСarriage.Y < 0 || stateField[0][posСarriage.Y][posСarriage.X] != letter) {
						posСarriage.Y++;
						break;
					}
					cellDisable(field, stateField, coordField, size, posСarriage.X, posСarriage.Y + 1, hConsole);
					cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					break;
				case 80: // \/ 
					posСarriage.Y++;
					if (posСarriage.Y >= size || stateField[0][posСarriage.Y][posСarriage.X] != letter) {
						posСarriage.Y--;
						break;
					}
					cellDisable(field, stateField, coordField, size, posСarriage.X, posСarriage.Y - 1, hConsole);
					cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					break;
				case 75: // <- 
					posСarriage.X--;
					if (posСarriage.X < 0 || stateField[0][posСarriage.Y][posСarriage.X] != letter) {
						posСarriage.X++;
						break;
					}
					cellDisable(field, stateField, coordField, size, posСarriage.X + 1, posСarriage.Y, hConsole);
					cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					break;
				case 77: // -> 
					posСarriage.X++;
					if (posСarriage.X >= size || stateField[0][posСarriage.Y][posСarriage.X] != letter) {
						posСarriage.X--;
						break;
					}
					cellDisable(field, stateField, coordField, size, posСarriage.X - 1, posСarriage.Y, hConsole);
					cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					break;
				}
			}
		} while (ch != 13 && ch != 27);

		if (ch == 27) {
			stateField[0][inserted.Y][inserted.X] = possible;
			field[inserted.Y][inserted.X] = '+';
			clearMessageInCenter(size, maxX, hConsole);
			clearMessageInCenter(size, maxX, hConsole, 1);
			continue;
		}

		stateField[1][posСarriage.Y][posСarriage.X] = passed;

		word[0] = field[posСarriage.Y][posСarriage.X];

		clearMessageInCenter(size, maxX, hConsole);
		clearMessageInCenter(size, maxX, hConsole, 1);

		displayMessageInCenter(size, maxX, "выделите слово и нажмите enter", hConsole);
		SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
		displayMessageInCenter(size, maxX, "нажмите esc для отмены выбора", hConsole, 1);

		bool check = false;
		if (inserted.X == posСarriage.X && inserted.Y == posСarriage.Y) check = true;

		do
		{
			ch = _getch();
			if (ch == 224)
			{
				ch = _getch();
				switch (ch)
				{
				case 72: // /\ 
					posСarriage.Y--;
					if (posСarriage.Y < 0 || stateField[0][posСarriage.Y][posСarriage.X] != letter || stateField[1][posСarriage.Y][posСarriage.X] == passed) {
						posСarriage.Y++;
						break;
					}
					stateField[1][posСarriage.Y][posСarriage.X] = passed;
					word[numLetter++] = field[posСarriage.Y][posСarriage.X];
					cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					break;
				case 80: // \/ 
					posСarriage.Y++;
					if (posСarriage.Y >= size || stateField[0][posСarriage.Y][posСarriage.X] != letter || stateField[1][posСarriage.Y][posСarriage.X] == passed) {
						posСarriage.Y--;
						break;
					}
					stateField[1][posСarriage.Y][posСarriage.X] = passed;
					word[numLetter++] = field[posСarriage.Y][posСarriage.X];
					cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					break;
				case 75: // <- 
					posСarriage.X--;
					if (posСarriage.X < 0 || stateField[0][posСarriage.Y][posСarriage.X] != letter || stateField[1][posСarriage.Y][posСarriage.X] == passed) {
						posСarriage.X++;
						break;
					}
					stateField[1][posСarriage.Y][posСarriage.X] = passed;
					word[numLetter++] = field[posСarriage.Y][posСarriage.X];
					cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					break;
				case 77: // -> 
					posСarriage.X++;
					if (posСarriage.X >= size || stateField[0][posСarriage.Y][posСarriage.X] != letter || stateField[1][posСarriage.Y][posСarriage.X] == passed) {
						posСarriage.X--;
						break;
					}
					stateField[1][posСarriage.Y][posСarriage.X] = passed;
					word[numLetter++] = field[posСarriage.Y][posСarriage.X];
					cellActive(field, coordField, size, posСarriage.X, posСarriage.Y, hConsole);
					break;
				}
			}
			if (inserted.X == posСarriage.X && inserted.Y == posСarriage.Y) check = true;
		} while (ch != 13 && ch != 27);

		if (ch == 27) {
			for (int i = 0; i < size; i++) {
				for (int j = 0; j < size; j++) {
					if (stateField[1][i][j] == passed)
						stateField[1][i][j] = empty;
				}
			}
			stateField[0][inserted.Y][inserted.X] = possible;
			field[inserted.Y][inserted.X] = '+';
			clearMessageInCenter(size, maxX, hConsole);
			clearMessageInCenter(size, maxX, hConsole, 1);
			continue;
		}

		if (!check) {
			displayMessageInCenter(size, maxX, "Выделенное слово не содержит вставленную букву", hConsole);
			SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
			displayMessageInCenter(size, maxX, "Нажмите enter для повторного выбора слова", hConsole, 1);
			do {
				ch = _getch();
			} while (ch != 13);
			for (int i = 0; i < size; i++) {
				for (int j = 0; j < size; j++) {
					if (stateField[1][i][j] == passed)
						stateField[1][i][j] = empty;
				}
			}
			stateField[0][inserted.Y][inserted.X] = possible;
			field[inserted.Y][inserted.X] = '+';
			clearMessageInCenter(size, maxX, hConsole);
			clearMessageInCenter(size, maxX, hConsole, 1);
			continue;
		}

		clearMessageInCenter(size, maxX, hConsole);
		clearMessageInCenter(size, maxX, hConsole, 1);

		if (isWord(word) && !haveWordUsed(word)) {
			addWordToDictionary(word);
			addWordsInEnteredWords(word);
			putWord(field, stateField, size, inserted.X, inserted.Y, let);
			cellDisable(field, stateField, coordField, size, inserted.X, inserted.Y, hConsole);
			score += strlen(word);
			strcpy(exword, word);
			return 0;
		}
		else if (isWord(word)) {
			displayMessageInCenter(size, maxX, "Выделенное слово уже было", hConsole);
			SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
			displayMessageInCenter(size, maxX, "Нажмите enter для повторного выбора слова", hConsole, 1);
			do {
				ch = _getch();
			} while (ch != 13);
			clearMessageInCenter(size, maxX, hConsole);
			clearMessageInCenter(size, maxX, hConsole, 1);
			for (int i = 0; i < size; i++) {
				for (int j = 0; j < size; j++) {
					if (stateField[1][i][j] == passed)
						stateField[1][i][j] = empty;
				}
			}
			stateField[0][inserted.Y][inserted.X] = possible;
			field[inserted.Y][inserted.X] = '+';
			continue;
		}
		else {
			displayMessageInCenter(size, maxX, "Выделенное слово отсутствует в словаре", hConsole);
			int res = 0;
			SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			displayMessageInCenter(size, maxX, "Добавить слово в словарь", hConsole, 2);
			SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
			displayMessageInCenter(size, maxX, "Выбрать другое слово", hConsole, 3);
			do
			{
				ch = _getch();
				if (ch == 224)
				{
					ch = _getch();
					switch (ch)
					{
					case 72: // /\ 
						SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
						displayMessageInCenter(size, maxX, "Добавить слово в словарь", hConsole, 2);
						SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
						displayMessageInCenter(size, maxX, "Выбрать другое слово", hConsole, 3);
						res = 0;
						break;
					case 80: // \/ 
						SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
						displayMessageInCenter(size, maxX, "Добавить слово в словарь", hConsole, 2);
						SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
						displayMessageInCenter(size, maxX, "Выбрать другое слово", hConsole, 3);
						res = 1;
						break;
					}
				}
			} while (ch != 13);
			clearMessageInCenter(size, maxX, hConsole);
			clearMessageInCenter(size, maxX, hConsole, 2);
			clearMessageInCenter(size, maxX, hConsole, 3);
			if (res) {
				for (int i = 0; i < size; i++) {
					for (int j = 0; j < size; j++) {
						if (stateField[1][i][j] == passed)
							stateField[1][i][j] = empty;
					}
				}
				stateField[0][inserted.Y][inserted.X] = possible;
				field[inserted.Y][inserted.X] = '+';
				clearMessageInCenter(size, maxX, hConsole, 1);
				continue;
			}
			else {
				addWordToDictionary(word);
				addWordsInEnteredWords(word);
				putWord(field, stateField, size, inserted.X, inserted.Y, let);
				cellDisable(field, stateField, coordField, size, inserted.X, inserted.Y, hConsole);
				score += strlen(word);
				strcpy(exword, word);
				return 0;
			}
		}

		break;
	}
}

bool _checkRightPartWord(char** field, state * **stateField, int size,
	int x, int y, const int centralx, const int centraly, char wordBuff[LEN * 2], int& rightDepth, const int& leftDepth) {
	rightDepth++;
	if (rightDepth > LEN - 2 || leftDepth + rightDepth + 1 > maxLengthWord) {
		rightDepth--;
		return false;
	}
	wordBuff[LEN + rightDepth] = field[y][x];
	wordBuff[LEN + rightDepth + 1] = '\0';
	stateField[0][y][x] = passed;

	for (int i = 0; i < 32; i++) {
		wordBuff[LEN] = 'А' + i;
		if (isWord(wordBuff + sizeof(char) * (LEN - leftDepth)) && !haveWordUsed(wordBuff + sizeof(char) * (LEN - leftDepth))) {
			stateField[0][y][x] = letter;
			wordBuff[LEN + rightDepth] = '\0';
			rightDepth--;
			return true;
		}
	}

	if (y + 1 < size && stateField[0][y + 1][x] == letter)
		if (_checkRightPartWord(field, stateField, size, x, y + 1, centralx, centraly, wordBuff, rightDepth, leftDepth)) {
			stateField[0][y][x] = letter;
			wordBuff[LEN + rightDepth] = '\0';
			rightDepth--;
			return true;
		}
	if (y - 1 >= 0 && stateField[0][y - 1][x] == letter)
		if (_checkRightPartWord(field, stateField, size, x, y - 1, centralx, centraly, wordBuff, rightDepth, leftDepth)) {
			stateField[0][y][x] = letter;
			wordBuff[LEN + rightDepth] = '\0';
			rightDepth--;
			return true;
		}
	if (x + 1 < size && stateField[0][y][x + 1] == letter)
		if (_checkRightPartWord(field, stateField, size, x + 1, y, centralx, centraly, wordBuff, rightDepth, leftDepth)) {
			stateField[0][y][x] = letter;
			wordBuff[LEN + rightDepth] = '\0';
			rightDepth--;
			return true;
		}
	if (x - 1 >= 0 && stateField[0][y][x - 1] == letter)
		if (_checkRightPartWord(field, stateField, size, x - 1, y, centralx, centraly, wordBuff, rightDepth, leftDepth)) {
			stateField[0][y][x] = letter;
			wordBuff[LEN + rightDepth] = '\0';
			rightDepth--;
			return true;
		}

	stateField[0][y][x] = letter;

	wordBuff[LEN + rightDepth] = '\0';
	rightDepth--;
	return false;
}

bool _checkLeftPartWord(char** field, state * **stateField, int size,
	int x, int y, const int centralx, const int centraly, char wordBuff[LEN * 2], int& leftDepth) {
	leftDepth++;
	if (leftDepth > maxLengthWord) {
		leftDepth--;
		return false;
	}
	wordBuff[LEN - leftDepth] = field[y][x];
	stateField[0][y][x] = passed;

	for (int i = 0; i < 32; i++) {
		wordBuff[LEN] = 'А' + i;
		wordBuff[LEN + 1] = '\0';
		if (isWord(wordBuff + sizeof(char) * (LEN - leftDepth)) && !haveWordUsed(wordBuff + sizeof(char) * (LEN - leftDepth))) {
			stateField[0][y][x] = letter;
			leftDepth--;
			return true;
		}
	}

	int rightDepth = 0;
	if (centraly + 1 < size && stateField[0][centraly + 1][centralx] == letter)
		if (_checkRightPartWord(field, stateField, size, centralx, centraly + 1, centralx, centraly, wordBuff, rightDepth, leftDepth)) {
			stateField[0][y][x] = letter;
			leftDepth--;
			return true;
		}
	if (centraly - 1 >= 0 && stateField[0][centraly - 1][centralx] == letter)
		if (_checkRightPartWord(field, stateField, size, centralx, centraly - 1, centralx, centraly, wordBuff, rightDepth, leftDepth)) {
			stateField[0][y][x] = letter;
			leftDepth--;
			return true;
		}
	if (centralx + 1 < size && stateField[0][centraly][centralx + 1] == letter)
		if (_checkRightPartWord(field, stateField, size, centralx + 1, centraly, centralx, centraly, wordBuff, rightDepth, leftDepth)) {
			stateField[0][y][x] = letter;
			leftDepth--;
			return true;
		}
	if (centralx - 1 >= 0 && stateField[0][centraly][centralx - 1] == letter)
		if (_checkRightPartWord(field, stateField, size, centralx - 1, centraly, centralx, centraly, wordBuff, rightDepth, leftDepth)) {
			stateField[0][y][x] = letter;
			leftDepth--;
			return true;
		}


	if (y + 1 < size && stateField[0][y + 1][x] == letter)
		if (_checkLeftPartWord(field, stateField, size, x, y + 1, centralx, centraly, wordBuff, leftDepth)) {
			stateField[0][y][x] = letter;
			leftDepth--;
			return true;
		}
	if (y - 1 >= 0 && stateField[0][y - 1][x] == letter)
		if (_checkLeftPartWord(field, stateField, size, x, y - 1, centralx, centraly, wordBuff, leftDepth)) {
			stateField[0][y][x] = letter;
			leftDepth--;
			return true;
		}
	if (x + 1 < size && stateField[0][y][x + 1] == letter)
		if (_checkLeftPartWord(field, stateField, size, x + 1, y, centralx, centraly, wordBuff, leftDepth)) {
			stateField[0][y][x] = letter;
			leftDepth--;
			return true;
		}
	if (x - 1 >= 0 && stateField[0][y][x - 1] == letter)
		if (_checkLeftPartWord(field, stateField, size, x - 1, y, centralx, centraly, wordBuff, leftDepth)) {
			stateField[0][y][x] = letter;
			leftDepth--;
			return true;
		}
	stateField[0][y][x] = letter;
	leftDepth--;
	return false;
}

bool checkWord(char** field, state * **stateField, int size, int x, int y) {
	char wordBuff[LEN * 2];
	int leftDepth = 0;
	int rightDepth = 0;

	if (y + 1 < size && stateField[0][y + 1][x] == letter)
		if (_checkLeftPartWord(field, stateField, size, x, y + 1, x, y, wordBuff, leftDepth)) return true;

	if (y + 1 < size && stateField[0][y + 1][x] == letter)
		if (_checkRightPartWord(field, stateField, size, x, y + 1, x, y, wordBuff, rightDepth, leftDepth)) return true;

	if (y - 1 >= 0 && stateField[0][y - 1][x] == letter)
		if (_checkLeftPartWord(field, stateField, size, x, y - 1, x, y, wordBuff, leftDepth)) return true;

	if (y - 1 >= 0 && stateField[0][y - 1][x] == letter)
		if (_checkRightPartWord(field, stateField, size, x, y - 1, x, y, wordBuff, rightDepth, leftDepth)) return true;

	if (x + 1 < size && stateField[0][y][x + 1] == letter)
		if (_checkLeftPartWord(field, stateField, size, x + 1, y, x, y, wordBuff, leftDepth)) return true;

	if (x + 1 < size && stateField[0][y][x + 1] == letter)
		if (_checkRightPartWord(field, stateField, size, x + 1, y, x, y, wordBuff, rightDepth, leftDepth)) return true;

	if (x - 1 >= 0 && stateField[0][y][x - 1] == letter)
		if (_checkLeftPartWord(field, stateField, size, x - 1, y, x, y, wordBuff, leftDepth)) return true;

	if (x - 1 >= 0 && stateField[0][y][x - 1] == letter)
		if (_checkRightPartWord(field, stateField, size, x - 1, y, x, y, wordBuff, rightDepth, leftDepth)) return true;

	return false;
}

bool checkForWords(char** field, state * **stateField, int size)
{
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			if (stateField[0][y][x] == possible)
				if (checkWord(field, stateField, size, x, y)) return true;
		}
	}
	return false;
}

void playOnePlayer(HANDLE hConsole, PCONSOLE_SCREEN_BUFFER_INFO pwi) {
	system("cls");
	int sizeField = 0;
	do {
		printf("Введите размер поля: ");
		scanf("%d", &sizeField);
	} while (sizeField < 3 || sizeField > 11 || sizeField % 2 != 1);
	system("cls");
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	char** field = new char* [sizeField];
	for (int i = 0; i < sizeField; i++) {
		field[i] = new char[sizeField];
		for (int j = 0; j < sizeField; j++)
			field[i][j] = '+';
	}
	state*** stateField = new state * *[2];
	for (int i = 0; i < 2; i++) {
		stateField[i] = new state * [sizeField];
		for (int j = 0; j < sizeField; j++) {
			stateField[i][j] = new state[sizeField];
			for (int k = 0; k < sizeField; k++)
				stateField[i][j][k] = empty;
		}
	}
	generateRandomWord(field, stateField, sizeField);

	int xmax = pwi->dwSize.X;
	COORD** coordField = new COORD * [sizeField];
	for (int i = 0; i < sizeField; i++) {
		coordField[i] = new COORD[sizeField];
		for (int j = 0; j < sizeField; j++) {
			coordField[i][j].X = (xmax - sizeField * 2) / 2 + 2 * j;
			coordField[i][j].Y = i + 5;
		}
	}
	printFullField(field, stateField, coordField, sizeField, hConsole);

	int scorePlayer = 0, scorePC = 0;

	int firstTurn = rand() % 2;
	char word[LEN];
	if (firstTurn) {
		printScore("Игрок", scorePlayer, "Пк", scorePC, xmax, hConsole, 0);
		descend(field, stateField, sizeField, scorePC, word);
		printField(field, stateField, coordField, sizeField, hConsole);
		printScore("Игрок", scorePlayer, "Пк", scorePC, xmax, hConsole, 1);
		printFoundWordPlayer2(word, xmax, hConsole);
	}

	int res;
	while (checkForWords(field, stateField, sizeField)) {
		printScore("Игрок", scorePlayer, "Пк", scorePC, xmax, hConsole, 1);
		res = turnPlayer(field, stateField, coordField, sizeField, xmax, hConsole, scorePlayer, word);
		if (res == 2) break;
		else if (res == 0) printFoundWordPlayer1(word, xmax, hConsole);

		printScore("Игрок", scorePlayer, "Пк", scorePC, xmax, hConsole, 0);

		if (!descend(field, stateField, sizeField, scorePC, word)) break;
		printField(field, stateField, coordField, sizeField, hConsole);
		printFoundWordPlayer2(word, xmax, hConsole);
	}

	if (res == 2 || scorePlayer < scorePC) {
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		displayMessageInCenter(sizeField, xmax, "Вы проиграли", hConsole);
	}
	else if (scorePC == scorePlayer) {
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
		displayMessageInCenter(sizeField, xmax, "Ничья", hConsole);
	}
	else {
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
		displayMessageInCenter(sizeField, xmax, "Вы победили", hConsole);
	}

	SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);


	printFoundWordPlayer1(word, xmax, hConsole, true);
	printFoundWordPlayer2(word, xmax, hConsole, true);
	wordsOnTheField[0] = ' ';
	wordsOnTheField[1] = '\0';

	displayMessageInCenter(sizeField, xmax, "Нажмите enter для выхода", hConsole, 2);
	char ch;
	do {
		ch = _getch();
	} while (ch != 13);

	for (int i = 0; i < 2; i++) {//ну и очищаем память от масивов
		for (int j = 0; j < sizeField; j++) delete[] stateField[i][j];
		delete[] stateField[i];
	}
	delete[] stateField;
	for (int i = 0; i < sizeField; i++) delete[] field[i];
	delete[] field;
}

void playTwoPlayer(HANDLE hConsole, PCONSOLE_SCREEN_BUFFER_INFO pwi) {
	system("cls");
	int sizeField = 0;
	do {
		printf("Введите размер поля: ");
		scanf("%d", &sizeField);
	} while (sizeField < 3 || sizeField > 11 || sizeField % 2 != 1);
	system("cls");
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	char** field = new char* [sizeField];
	for (int i = 0; i < sizeField; i++) {
		field[i] = new char[sizeField];
		for (int j = 0; j < sizeField; j++)
			field[i][j] = '+';
	}
	state*** stateField = new state * *[2];
	for (int i = 0; i < 2; i++) {
		stateField[i] = new state * [sizeField];
		for (int j = 0; j < sizeField; j++) {
			stateField[i][j] = new state[sizeField];
			for (int k = 0; k < sizeField; k++)
				stateField[i][j][k] = empty;
		}
	}
	generateRandomWord(field, stateField, sizeField);

	int xmax = pwi->dwSize.X;
	COORD** coordField = new COORD * [sizeField];
	for (int i = 0; i < sizeField; i++) {
		coordField[i] = new COORD[sizeField];
		for (int j = 0; j < sizeField; j++) {
			coordField[i][j].X = (xmax - sizeField * 2) / 2 + 2 * j;
			coordField[i][j].Y = i + 5;
		}
	}
	printFullField(field, stateField, coordField, sizeField, hConsole);

	int scorePlayer1 = 0, scorePlayer2 = 0;

	int firstTurn = rand() % 2;

	int res1 = 0, res2 = 0;

	char word[LEN];
	if (firstTurn) {
		printScore("Игрок 1", scorePlayer1, "Игрок 2", scorePlayer2, xmax, hConsole, 1);
		res1 = turnPlayer(field, stateField, coordField, sizeField, xmax, hConsole, scorePlayer1, word);
		printScore("Игрок 1", scorePlayer1, "Игрок 2", scorePlayer2, xmax, hConsole, 0);
		printFoundWordPlayer1(word, xmax, hConsole);
	}

	while (checkForWords(field, stateField, sizeField) && res1 != 2) {
		printScore("Игрок 1", scorePlayer1, "Игрок 2", scorePlayer2, xmax, hConsole, 0);
		res2 = turnPlayer(field, stateField, coordField, sizeField, xmax, hConsole, scorePlayer2, word);
		if (res2 == 2) break;
		else if (res2 == 0) printFoundWordPlayer2(word, xmax, hConsole);
		printScore("Игрок 1", scorePlayer1, "Игрок 2", scorePlayer2, xmax, hConsole, 1);

		res1 = turnPlayer(field, stateField, coordField, sizeField, xmax, hConsole, scorePlayer1, word);
		if (res1 == 2) break;
		else if (res1 == 0) printFoundWordPlayer1(word, xmax, hConsole);
	}

	if (res2 == 2 || scorePlayer2 < scorePlayer1) {
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
		displayMessageInCenter(sizeField, xmax, "Выиграл игрок 1", hConsole);
	}
	else if (res1 == 2 || scorePlayer2 > scorePlayer1) {
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
		displayMessageInCenter(sizeField, xmax, "Выиграл игрок 2", hConsole);
	}
	else {
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
		displayMessageInCenter(sizeField, xmax, "Ничья", hConsole);
	}

	SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);


	printFoundWordPlayer1(word, xmax, hConsole, true);
	printFoundWordPlayer2(word, xmax, hConsole, true);
	wordsOnTheField[0] = ' ';
	wordsOnTheField[1] = '\0';

	displayMessageInCenter(sizeField, xmax, "Нажмите enter для выхода", hConsole, 2);
	char ch;
	do {
		ch = _getch();
	} while (ch != 13);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < sizeField; j++) delete[] stateField[i][j];
		delete[] stateField[i];
	}
	delete[] stateField;
	for (int i = 0; i < sizeField; i++) delete[] field[i];
	delete[] field;
}

int main() {
	srand(time(0));
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	HWND hcon = GetConsoleWindow();

	CONSOLE_FONT_INFO cfi;
	GetCurrentConsoleFont(hConsole, false, &cfi);
	COORD fs = GetConsoleFontSize(hConsole, cfi.nFont);
	PCONSOLE_FONT_INFOEX ccf = new CONSOLE_FONT_INFOEX;
	(*ccf).dwFontSize.X = 12;
	(*ccf).dwFontSize.Y = 20;
	(*ccf).nFont = 11;
	(*ccf).cbSize = sizeof(CONSOLE_FONT_INFOEX);
	ccf->FontWeight = 400;
	lstrcpyW((*ccf).FaceName, L"Lucida Console");
	(*ccf).FontFamily = FF_DONTCARE;
	bool b = SetCurrentConsoleFontEx(hConsole, false, ccf);
	fs = GetConsoleFontSize(hConsole, cfi.nFont);

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	PCONSOLE_SCREEN_BUFFER_INFO pwi = new CONSOLE_SCREEN_BUFFER_INFO;
	PWINDOWINFO pgwi = new WINDOWINFO;

	
	char whereDictionary[] = "Slovar.txt";
	sizeDictionary = rememberDictionary(whereDictionary); //заполняем массив словаря


	const int n = 3;
	COORD positionMenuItem[n];
	int punkt = 0;
	char names[n][25] = { "Игра: один игрок", "Игра: два игрока", "Выход" };
	do
	{
		int xmax, ymax;

		GetConsoleScreenBufferInfo(hConsole, pwi);
		GetWindowInfo(hcon, pgwi);
		xmax = pwi->dwSize.X;
		ymax = pwi->dwSize.Y;

		int y0 = 7;
		for (int i = 0; i < n; i++)
		{
			positionMenuItem[i].X = (xmax - strlen(names[i])) / 2;
			positionMenuItem[i].Y = y0 + i;
		}
		system("cls");
		char title[] = "Балда";
		COORD pos;
		pos.X = (xmax - strlen(title)) / 2;
		pos.Y = n;
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
		SetConsoleCursorPosition(hConsole, pos);
		puts(title);
		for (int i = 0; i < n; i++)
		{
			SetConsoleCursorPosition(hConsole, positionMenuItem[i]);
			puts(names[i]);
		}
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		SetConsoleCursorPosition(hConsole, positionMenuItem[punkt]);
		puts(names[punkt]);
		unsigned char ch;
		do
		{
			ch = _getch();
			if (ch == 224)
			{
				ch = _getch();
				switch (ch)
				{
				case 72:
					SetConsoleCursorPosition(hConsole, positionMenuItem[punkt]);
					SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
					puts(names[punkt]);
					punkt--;
					if (punkt < 0) punkt = n - 1;
					SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					SetConsoleCursorPosition(hConsole, positionMenuItem[punkt]);
					puts(names[punkt]); break;
				case 80:
					SetConsoleCursorPosition(hConsole, positionMenuItem[punkt]);
					SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
					puts(names[punkt]);
					punkt++;
					if (punkt > n - 1) punkt = 0;
					SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					SetConsoleCursorPosition(hConsole, positionMenuItem[punkt]);
					puts(names[punkt]); break;
				}
			}
		} while (ch != 13);	// enter
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
		switch (punkt)
		{
		case 0:
			playOnePlayer(hConsole, pwi);
			break;
		case 1:
			playTwoPlayer(hConsole, pwi);
			break;
		}
	} while (punkt != n - 1);

	rewriteFileDictionary(whereDictionary);
	deleteDictionary();
}