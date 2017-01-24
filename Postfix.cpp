//
//  Assignment 3
//
//  Created by Edgar Ruiz 
//  Bronco ID: 009634885
//  Copyright (c) 2015 Edgar Ruiz. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <map>
#include <fstream>
#include <windows.h>
#include <tchar.h> 
#include "stdafx.h"
#include <strsafe.h>

#pragma comment(lib, "User32.lib")

using namespace std;

class StatementEvaluator {
public:
	// Public Functions
	void Evaluate(string &expr, map<string, double> &mapVars){
		string assignmentVarName;
		vector<string> fileNames;
		LookupFiles("C:\\*", fileNames);
		SetStatement(expr);
		// Parse the expression
		//   Tokenize the expression into operands and operators
		TokenizeStatement(expr);
		if (_exprTokens[0] == "OUT"){
			// Process OUT Statement
			string varName = _exprTokens[1];
			double varValue = mapVars[varName];
			cout << varValue << endl;
			return;
		}

		else if (_exprTokens[0] == "CREATE") {
			string myFile = _exprTokens[1];
			fList.push_back(myFile);
			ofstream fileName;
			fileName.open(myFile);
			string str;
			while (getline(cin,str) &&  str != "SAVE"){
				fileName << str << endl;
			}
			fileName.close();
			return;
		}

		else if (_exprTokens[0] == "RUN"){
			string myFile = _exprTokens[1];
			ifstream fileName;
			fileName.open(myFile);
			string line;
			while (getline(fileName, line)){
				cout << line << endl;
				Evaluate(line, mapVars);
			}
			fileName.close();
			return;
		}

		else if (_exprTokens[0] == "DIR"){
			for (int i = 0; i < fList.size(); i++){
				cout << fList[i] << " ";
			}
			cout << endl;
			return;
		}

		else if (_exprTokens[0] == "VARLIST"){
			for (auto it = mapVars.cbegin(); it != mapVars.cend(); ++it){
				cout << it->first << ":" << it->second << endl;
			}
			return;
		}

		else{
			// Initialize Variable
			assignmentVarName = _exprTokens[0];
			string assignOper = _exprTokens[_exprTokens.size() - 1];
			if (isalpha(assignmentVarName[0]) && (assignOper == "=")){
				mapVars[assignmentVarName] = 0;
			}
			else{
				cout << "Could not assign value to variable(Either missing variable or '=')" << endl;
				return;
			}
		}
	
		vector<double> evalStack;
		// Define Evaluation Stack
		// Loop through the tokens and evaluate the operators
		for (int i = 1; i < _exprTokens.size(); i++){
			string token = _exprTokens[i];
			if (isdigit(token[0])){
				// Put it on Eval Stack
				evalStack.push_back(atof(token.c_str()));
			}

			else if (isalpha(token[0])){
				// Its a variable. Get the value of the Variable
				// Put it on the Eval Stack
				evalStack.push_back(mapVars[token]);
			}

			else{
				// Its an operator
				// Pop the values from the stack and push the result 
				// back on the stack
				if (token == "+"){
					double val1 = evalStack[evalStack.size() - 1]; evalStack.pop_back();
					double val2 = evalStack[evalStack.size() - 1]; evalStack.pop_back();
					evalStack.push_back(val1 + val2);
				}

				else if (token == "-"){
					double val1 = evalStack[evalStack.size() - 1]; evalStack.pop_back();
					double val2 = evalStack[evalStack.size() - 1]; evalStack.pop_back();
					evalStack.push_back(val2 - val1);
				}

				else if (token == "*"){
					double val1 = evalStack[evalStack.size() - 1]; evalStack.pop_back();
					double val2 = evalStack[evalStack.size() - 1]; evalStack.pop_back();
					evalStack.push_back(val1 * val2);
				}

				else if (token == "/"){
					double val1 = evalStack[evalStack.size() - 1]; evalStack.pop_back();
					double val2 = evalStack[evalStack.size() - 1]; evalStack.pop_back();
					evalStack.push_back(val2 / val1);
				}

				else if (token == "="){
					double val1 = evalStack[evalStack.size() - 1]; evalStack.pop_back();
					mapVars[assignmentVarName] = val1;
				}

				else{
					cout << "Invalid operator" << endl;
					return;
				}

			}

		}

	}

	double Evaluate() { }

	string GetStatement() { return _exprString; }

	void SetStatement(string &expr) { _exprString = expr; }

private:
	// Private Functions
	void TokenizeStatement(string expr){
		_exprTokens.clear();
		string a[100];
		char* context = NULL;
		char *pch = strtok_s((char *)expr.c_str(), " ", &context);

		while (pch != NULL){
			_exprTokens.push_back(pch);
			//cout << pch << endl;
			pch = strtok_s(NULL, " ", &context);
		}
	}

	void LookupFiles(string dirPath, vector<string> &fileNames){
		WIN32_FIND_DATA ffd;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWORD dwError = 0;

		// Clear out Filenames Vector
		fileNames.clear();

		wstring dirPathWildCard(dirPath.begin(), dirPath.end());
		hFind = FindFirstFile(dirPathWildCard.c_str(), &ffd);

		if (INVALID_HANDLE_VALUE == hFind){
			cout << "Invalid Directory: " << dirPathWildCard.c_str() << endl;
			return;
		}
		// List all the files in the directory with some info about them.

		do{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
				// Ignore Subdirectories
			}

			else{
				wstring fileName1 = ffd.cFileName;
				string fileName2(fileName1.begin(), fileName1.end());
				fileNames.push_back(fileName2);
			}
		} while (FindNextFile(hFind, &ffd) != 0);

		dwError = GetLastError();

		if (dwError != ERROR_NO_MORE_FILES){
			cout << "Unable to read filenames" << endl;
		}
		FindClose(hFind);
	}

	// Private Data Members
	string _exprString;
	vector<string> _exprTokens;
	vector<string> fList;
protected:
};



int _tmain(int argc, _TCHAR* argv[]){
	StatementEvaluator stmtEval;
	map<string, double> mapVars;

	while (true){
		// Get Input Statement from the User
		string stmt;
		getline(cin, stmt);

		if (stmt == "exit")
			break;

		stmtEval.Evaluate(stmt, mapVars);
	}
	return 0;
}


