#pragma once
#include <vector>

class Rule;
class LSystem
{
public:
	LSystem(std::string ax);
	~LSystem();

	bool AddRule(Rule* newRule);
	void Generate();

	std::string getAxiom() { return axiom; }
	std::string getSentence() { return sentence; }
	std::vector<Rule*> getRules() { return rules; }

private:
	std::string axiom;
	std::string sentence;
	std::vector<Rule*> rules;
};