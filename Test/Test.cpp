#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <string>
#include <array>
#include <vector>

using namespace std;

vector<vector<string>> palindromes;
vector<string> wordList, revList;

//проверка слова на симметричность
bool isPal(string s);

//убрать лишние пробелы в словах и после слов
string remove_spaces(const string& source);

//слияние левого и правого векторов
vector<string> addpal(vector<string> left, vector<string> right);

//построчное чтение из файла и занесение их в вектор
void read();

//поиск максимального палиндрома из всех найденных
vector<vector<string>> findmax(vector<vector<string>> p);

//запись в файл одного из найденных максимальных палиндромов
void write(vector<vector<string>> pol);

//рекурсивная функция поиска палиндромов
void findpal(string oldw, string cw, bool side,
	vector<string> work, vector<string> work_rev,
	vector<string> left, vector<string> right);

void matchcase(vector<string> work, vector<string> work_rev,
	vector<string> left, vector<string> right,
	bool side, int j, string cw, string oldw);

int main()
{
	vector<string> leftList, rightList;		
	vector<vector<string>> maxpals;
	string cw, oldw;						//cw - текущее слово(current word) из списка оригинальных слов, 
											//oldw - старое слово(old word), которое удаляем и возвращаем после выполнения функции
	bool side = true;						//true - запись влево, false - запись вправо
	setlocale(LC_ALL, "Russian");
	read();
	for (int i = 0; i < wordList.size(); i++) { //цикл по исходным словам
		oldw = wordList[i];
		wordList.erase(wordList.begin() + i); //удаляем использованное слово
		revList.erase(revList.begin() + i);
		leftList.push_back(oldw);
		findpal(oldw,oldw, side, wordList, revList, leftList, rightList);
		leftList.clear();
		wordList.insert(wordList.begin() + i, oldw);
		reverse(oldw.begin(), oldw.end());
		revList.insert(revList.begin() + i, oldw);
	}
	//вывод на консоль всех найденных палиндромов
	/*for (int i = 0; i < palindromes.size(); i++) {
		for (int j = 0; j < palindromes[i].size(); j++) {
			cout << palindromes[i][j]<<" ";
		}
		cout << endl;
	}*/
	maxpals = findmax(palindromes);
	write(maxpals);
    return 0;
}

void matchcase(vector<string> work, vector<string> work_rev,
	vector<string> left, vector<string> right,
	bool side, int j, string cw, string oldw) {
	work.erase(work.begin() + j); //удаляем использованное слово
	work_rev.erase(work_rev.begin() + j);
	if (cw == "") {						//случай, когда остаток пуст
		for (int l = 0; l < work.size(); ++l) {
			string oldd = work[l];
			work.erase(work.begin() + l); //удаляем использованное слово
			work_rev.erase(work_rev.begin() + l);
			left.push_back(oldd);
			findpal(oldw,oldd, side, work, work_rev, left, right);
			left.pop_back();
			work.insert(work.begin() + l, oldd);
			reverse(oldd.begin(), oldd.end());
			work_rev.insert(work_rev.begin() + l, oldd);
		}
	}
	else {
		findpal(oldw,cw, side, work, work_rev, left, right);
	}
	work.insert(work.begin() + j, oldw);
	reverse(oldw.begin(), oldw.end());
	work_rev.insert(work_rev.begin() + j, oldw);
}

void findpal(string oldw ,string cw, bool side,
	vector<string> work, vector<string> work_rev,
	vector<string> left, vector<string> right) {
	bool match = false;		//нашлось ли совпадение с cw
	string oldcw;			//старое значение cw для восстановления после рекурсии
							//случай, когда cw находится с левой стороны
	if (side) {
		for (int j = 0; j < work_rev.size(); j++) { //цикл по перевернутым словам
			if (cw.length() <= work_rev[j].length()) {
				for (int k = 0; k < cw.length(); ++k) { //цикл по буквам текущего слова
					if (cw[k] == work_rev[j][k]) {//побуквенно сравниваем j-е слово очередной k-й буквы
						if (k + 1 == cw.length()) { //если исходное слово меньше очередного из списка - оно нам подходит
							match = true;
							oldcw = cw;
							cw = "";
							for (int m = k + 1; m < work_rev[j].size(); m++) {
								cw += work_rev[j][m];
							}
							reverse(cw.begin(), cw.end()); //перевернули остаток 
							right.push_back(work[j]);
							if (cw.length() <= 1) {
								palindromes.push_back(addpal(left, right)); //в случае если остаток 0 или 1 записываем в палиндромы
								if (cw.length() == 0) {
									side = 1; //если нет остатка - запись снова слева
								}
								else side = 0;
							}
							else side = 0;	
						}
					}
					else break;			//если буква не совпала - переходим к следующему
				}
			}
			else {
				for (int k = 0; k < work_rev[j].length(); ++k) {
					if (cw[k] == work_rev[j][k]) {//побуквенно сравниваем j-е слово очередной k-й буквы
						if (k + 1 == work_rev[j].length()) { //если текущее слово входит в одно из перевернутых остальных - подходит
							match = true;
							oldcw = cw;
							cw = cw.substr(k + 1, cw.length() - k - 1);
							right.push_back(work[j]);
							if (cw.length() >= 1) {
								if (isPal(cw))
								{
									palindromes.push_back(addpal(left, right));
								}
							}
						}
					}
					else break;
				}
			}
			if (match) {
				string oldw = work[j];
				matchcase(work, work_rev, left, right, side, j, cw, oldw);
				right.pop_back();
				cw = oldcw;
			}
			match = false;
		}
		//проверяем является ли остаток словом и палиндромом
		if (cw==oldw &&isPal(cw))
		{
			palindromes.push_back(addpal(left, right));
		}
	}
	else { //если side = 0, т.е. слово справа
		reverse(cw.begin(), cw.end());
		for (int j = 0; j < work.size(); j++) { //цикл по обычным словам
			if (cw.length() <= work[j].length()) {
				for (int k = 0; k < cw.length(); ++k) { //цикл по буквам текущего слова
					if (cw[k] == work[j][k]) {//побуквенно сравниваем j-е слово очередной k-й буквы
						if (k + 1 == cw.length()) { //если исходной слово меньше очередного из списка - оно там подходит
							match = true;
							oldcw = cw;
							cw = "";
							for (int m = k + 1; m < work[j].size(); m++) {
								cw += work[j][m];
							}
							left.push_back(work[j]);
							side = 1;
							if (cw.length() <= 1) {
								palindromes.push_back(addpal(left, right)); //в случае если остаток 0 или 1 записываем в палиндромы
							}
							else
							{
								if (isPal(cw))
								{
									palindromes.push_back(addpal(left, right));
								}
							}
							
						}
					}
					else break;
				}
			}
			else {
				for (int k = 0; k < work[j].length(); ++k) { //цикл по буквам текущего слова
					if (cw[k] == work[j][k]) {//побуквенно сравниваем j-е слово очередной k-й буквы
						if (k + 1 == work[j].length()) { //если текущее слово содержит очередное из списка - оно подходит
							match = true;
							oldcw = cw;
							cw = cw.substr(k + 1, cw.length() - k - 1);
							reverse(cw.begin(), cw.end()); //перевернули остаток 
							left.push_back(work[j]);
							if (cw.length() == 1) {
								palindromes.push_back(addpal(left, right)); //в случае если 1 записываем в палиндромы
							}
						}
					}
					else break;
				}
			}
			if (match) {
				string oldw = work[j];
				matchcase(work, work_rev, left, right, side, j, cw, oldw);
				cw = oldcw;
			}
		}
	}
}

string remove_spaces(const string& source)
{
	string s = source;
	s.erase(remove(s.begin(), s.end(), ' '), s.end());
	return (s);
}

vector<string> addpal(vector<string> left, vector<string> right) {
	vector<string> pal;
	for (int i = 0; i < left.size(); i++) {
		pal.push_back(left[i]);
	}
	for (int i = right.size() - 1; i >= 0; i--) {
		pal.push_back(right[i]);
	}

	return pal;
}

void read() {
	string s;
	ifstream input_file("input.txt");
	while (true) {
		if (!input_file.eof()) {
			getline(input_file, s);
			s = remove_spaces(s);
			transform(s.begin(), s.end(), s.begin(), ::tolower);	//уравнять регистр всех слов
			wordList.push_back(s);
			reverse(s.begin(), s.end());
			revList.push_back(s);

		}
		else break;
	}
}

vector<vector<string>> findmax(vector<vector<string>> p) {
	vector<int> count;
	vector<vector<string>> result;
	for (int i = 0; i < p.size(); i++) {
		count.push_back(0);
		for (int j = 0; j < p[i].size(); j++) {
			count[i] += 1;
		}
	}
	int max = count[0]; // максимальная длина
	for (int i = 0; i < count.size(); i++) {
		if (count[i] > max) max = count[i];
	}
	for (int i = 0; i < p.size(); i++) {
		if (p[i].size() == max) result.push_back(p[i]);
	}
	return result;
}

void write(vector<vector<string>> pol) {
	ofstream fout;
	fout.open("output.txt");
	for (int i = 0; i < pol[0].size(); i++) {
		fout << pol[0][i] << endl;
	}
	fout.close();
}

bool isPal(string s) {
	for (int i = 0; i < s.length() / 2; ++i) {
		if (s[i] != s[s.length() - 1 - i])
			return false;
	}
	return true;
}