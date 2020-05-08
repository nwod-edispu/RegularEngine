#pragma once
#include"nfa.h"
#include"character.h"
#include<iostream>
class Construct
{
public:
	int parse();
private:
	string insertExplicitConcatOperator(string expr);
	string infixToSuffix(string expr);
	Nfa* constructNfa(string expr);
	Nfa* createNormalNfa(char ch);
	Nfa* concatNfa(Nfa* nfaptr1, Nfa* nfaptr2);
	Nfa* unionNfa(Nfa* nfaptr1, Nfa* nfaptr2);
	Nfa* closureNfa(Nfa* nfaptr1);
	Nfa* oneOrMoreNfa(Nfa* nfaptr1);
	Nfa* zeroOrMoreNfa(Nfa* nfaptr1);
	bool matchExpression(string expr,Nfa *nfaHead);
	bool isCharacter(const char ch);
};