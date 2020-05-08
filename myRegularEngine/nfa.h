#pragma once
#include<map>
#include<vector>
#include<string>
#include<stack>
using namespace std;

class State
{
	bool isEnd;
	vector<State*> epsilonEdge;
	map<char, State*> transitionEdge;
public:
	State(bool isEnd);
	void addEpsilonEdge(State *state);
	void addTransitionEdge(char ch, State *state);
	void setEnd(bool isEnd);
	bool getEnd();
	void getCurStates(vector<State*> &nextStates);
	State* goTransitionEdge(char ch);
};

class Nfa
{
	State *startState;
	State *endState;
public:	
	Nfa(State *startState,State *endState);
	State* getStartState();
	State* getEndState();
};

