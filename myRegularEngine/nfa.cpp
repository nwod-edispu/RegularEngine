#include"nfa.h"

State::State(bool isEnd = false)
{
	this->isEnd = isEnd;
}

void State::addEpsilonEdge(State * state)
{
	epsilonEdge.push_back(state);	
}

void State::addTransitionEdge(char ch,State * state)
{
	this->transitionEdge[ch] = state;
}

void State::setEnd(bool isEnd)
{
	this->isEnd = isEnd;
}

bool State::getEnd()
{
	return isEnd;
}

void State::getCurStates(vector<State*>& nextStates)
{
	//the first edge may be epsilon or not 
	nextStates.push_back(this);
	//to avoid continuous epsilon edge
	stack<State*>current;	
	current.push(this);
	vector<State*>::iterator it;
	while (!current.empty())
	{
		State* tmp = current.top();
		current.pop();
		for (it = tmp->epsilonEdge.begin(); it != tmp->epsilonEdge.end(); it++)
		{
			//TODO need further modify
			//maybe come into a circle?
			nextStates.push_back(*it);
			current.push(*it);
		}
	}
	return;
}

State * State::goTransitionEdge(char ch)
{
	map<char, State*>::iterator it = transitionEdge.find(ch);
	if (it == transitionEdge.end())return nullptr;
	else return it->second;
}

Nfa::Nfa(State *startState = nullptr, State *endState = nullptr)
{
	this->startState = startState;
	this->endState = endState;
}

State * Nfa::getStartState()
{
	return this->startState;
}

State * Nfa::getEndState()
{
	return this->endState;
}
