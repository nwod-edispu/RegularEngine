#include "construct.h"

int Construct::parse()
{
	string expr="h?(a|b)*(c|d)+";
	cout << "input a regular expression:";	
	string tmp = insertExplicitConcatOperator(expr);
	cout << tmp << endl;;
	tmp = infixToSuffix(tmp);
	Nfa * nfaHead = constructNfa(tmp);
	string s;
	while (cin >> s)
	{
		bool res = matchExpression(s, nfaHead);
		if (res)cout << "true" << endl;
		else cout << "false" << endl;
	}
	
	return 0;
}

/*
if add concat operator or not
	*	(	)	|	ch	+	?
*	N	Y	N	N	Y	N	N
(	N	N	N	N	N	N	N
)	N	Y	N	N	Y	N	N
|	N	N	N	N	N	N	N
ch	N	Y	N	N	Y	N	N
+	N	Y	N	N	Y	N	N
?	N	Y	N	N	Y	N	N
*/
string Construct::insertExplicitConcatOperator(string expr)
{
	string output = "";
	int i = 0;
	for (; i < static_cast<int>(expr.size()); i++)
	{
		const char ch = expr[i];
		output += ch;
		if (ch == GROUP_LEFT_OPERATE || ch == UNION_OPERATOR)
			continue;
		if (i < int(expr.size()) - 1)
		{
			const char lookahead = expr[i + 1];
			if (isCharacter(lookahead) || lookahead == GROUP_LEFT_OPERATE)
			{
				output += CONCAT_OPERATOR;
			}
		}
	}
	return output;	
}

string Construct::infixToSuffix(string expr)
{
	char priorityMap[256];
	priorityMap['.'] = 2;
	priorityMap['*'] = 3;
	priorityMap['?'] = 3;
	priorityMap['+'] = 3;
	priorityMap['|'] = 1;
	string output = "";
	stack<char> chStack;
	int i = 0;
	for (; i < static_cast<int>(expr.size()); i++)
	{
		const char ch = expr[i];
		if (ch == GROUP_LEFT_OPERATE)
		{
			chStack.push(ch);
		}
		else if (ch == GROUP_RIGHT_OPERATE)
		{
			while (!chStack.empty() && chStack.top() != GROUP_LEFT_OPERATE)
			{
				output += chStack.top();
				chStack.pop();
			}
			chStack.pop();
		}
		else if (isCharacter(ch))
		{
			output += ch;
		}
		else
		{
			while (!chStack.empty() && priorityMap[ch] <= priorityMap[chStack.top()])
			{
				output += chStack.top();
				chStack.pop();
			}
			chStack.push(ch);
		}
	}
	while (!chStack.empty())
	{
		output += chStack.top();
		chStack.pop();
	}
	return output;	
}

Nfa* Construct::constructNfa(string expr)
{
	stack<Nfa*>nfaStack;
	int i;
	for (i = 0; i < static_cast<int>(expr.size()); i++)
	{
		if (expr[i] == CONCAT_OPERATOR)
		{
			Nfa* nfaptr1 = nfaStack.top();
			nfaStack.pop();
			Nfa* nfaptr2 = nfaStack.top();
			nfaStack.pop();
			Nfa* nfaptr = concatNfa(nfaptr2, nfaptr1);;
			nfaStack.push(nfaptr);
		}
		else if (expr[i] == UNION_OPERATOR)
		{
			Nfa* nfaptr1 = nfaStack.top();
			nfaStack.pop();
			Nfa* nfaptr2 = nfaStack.top();
			nfaStack.pop();
			Nfa* nfaptr = unionNfa(nfaptr1, nfaptr2);
			nfaStack.push(nfaptr);
		}
		else if (expr[i] == CLOSURE_OPERATOR)
		{
			Nfa* nfaptr1 = nfaStack.top();
			nfaStack.pop();
			Nfa* nfaptr = closureNfa(nfaptr1);
			nfaStack.push(nfaptr);
		}
		else if (expr[i] == ONE_OR_MORE_OPERATOR)
		{
			Nfa* nfaptr1 = nfaStack.top();
			nfaStack.pop();
			Nfa* nfaptr = oneOrMoreNfa(nfaptr1);
			nfaStack.push(nfaptr);
		}
		else if (expr[i] == ZERO_OR_MORE_OPERATOR)
		{
			Nfa* nfaptr1 = nfaStack.top();
			nfaStack.pop();
			Nfa* nfaptr = zeroOrMoreNfa(nfaptr1);
			nfaStack.push(nfaptr);
		}
		else
		{
			Nfa* nfaptr = createNormalNfa(expr[i]);
			nfaStack.push(nfaptr);
		}
	}
	return nfaStack.top();
}

Nfa * Construct::concatNfa(Nfa * nfaptr1, Nfa * nfaptr2)
{	
	nfaptr1->getEndState()->addEpsilonEdge(nfaptr2->getStartState());
	nfaptr1->getEndState()->setEnd(false);
	return new Nfa(nfaptr1->getStartState(), nfaptr2->getEndState());
}

Nfa * Construct::unionNfa(Nfa * nfaptr1, Nfa * nfaptr2)
{
	State* startState = new State(false);
	State* endState = new State(true);
	startState->addEpsilonEdge(nfaptr1->getStartState());
	startState->addEpsilonEdge(nfaptr2->getStartState());
	nfaptr1->getEndState()->addEpsilonEdge(endState);
	nfaptr1->getEndState()->setEnd(false);
	nfaptr2->getEndState()->addEpsilonEdge(endState);
	nfaptr2->getEndState()->setEnd(false);
	return new Nfa(startState, endState);
}

Nfa * Construct::closureNfa(Nfa * nfaptr1)
{
	State* startState = new State(false);
	State* endState = new State(true);
	State* endptr = nfaptr1->getEndState();
	State* startptr = nfaptr1->getStartState();
	startState->addEpsilonEdge(endState);
	startState->addEpsilonEdge(startptr);
	endptr->addEpsilonEdge(endState);
	endptr->setEnd(false);
	endptr->addEpsilonEdge(startptr);
	return new Nfa(startState, endState);
}

Nfa * Construct::oneOrMoreNfa(Nfa * nfaptr1)
{
	State* startState = new State(false);
	State* endState = new State(true);
	State* endptr = nfaptr1->getEndState();	
	State* startptr = nfaptr1->getStartState();
	startState->addEpsilonEdge(startptr);
	endptr->addEpsilonEdge(endState);
	endptr->setEnd(false);
	endptr->addEpsilonEdge(startptr);
	return new Nfa(startState, endState);	
	return nullptr;
}

Nfa * Construct::zeroOrMoreNfa(Nfa * nfaptr1)
{
	State* startState = new State(false);
	State* endState = new State(true);
	State* endptr = nfaptr1->getEndState();
	State* startptr = nfaptr1->getStartState();
	startState->addEpsilonEdge(endState);
	startState->addEpsilonEdge(startptr);
	endptr->addEpsilonEdge(endState);
	endptr->setEnd(false);
	return new Nfa(startState, endState);
}

Nfa * Construct::createNormalNfa(char ch)
{
	State* startState = new State(false);
	State* endState = new State(true);
	startState->addTransitionEdge(ch, endState);
	return new Nfa(startState, endState);
}

bool Construct::matchExpression(string expr, Nfa * nfaHead)
{
	State* firstState = nfaHead->getStartState();
	vector<State*>::iterator it;
	vector<State*>curStates;
	firstState->getCurStates(curStates);
	int i;
	for (i = 0; i < static_cast<int>(expr.size()); i++)
	{
		char ch = expr[i];
		vector<State*>nextStates;		
		for (it = curStates.begin(); it != curStates.end(); it++)
		{
			State* nextState = (*it)->goTransitionEdge(ch);			
			if (nextState != nullptr)
			{
				nextState->getCurStates(nextStates);
			}
		}
		curStates = nextStates;
	}
	for (it = curStates.begin(); it != curStates.end(); it++)
	{
		if ((*it)->getEnd() == true)return true;
	}
	return false;
}

bool Construct::isCharacter(const char ch)
{	
	return ch != '*'&&ch != '+'&&ch != '.'&&ch != '?'&&ch != '|'&&ch != '('&&ch != ')';
}
