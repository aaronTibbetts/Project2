#ifndef __PARSER__H__
#define __PARSER__H__

#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include "lexer.h"

struct rule{
    std::string LHS;
    std::vector<std::string> RHS;
};

extern std::vector<std::string> orderVector;
extern std::vector<std::string> seenOrderVector;
extern std::vector<std::string> seenOrderVectorTerm;

std::unordered_map<std::string, std::vector<std::vector<std::string>>> startParser();

void printTerminals(std::unordered_map<std::string, std::vector<std::vector<std::string>>>  grammar);

void printRules(std::unordered_map<std::string, std::vector<std::vector<std::string>>>  grammar);

std::set<std::string> nullable(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar);

void printNullable(std::set<std::string> nullable);

std::unordered_map<std::string, std::vector<std::string>> firstSet(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar);

void printFirstSet(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar,std::unordered_map<std::string, std::vector<std::string>> firstSets);

void printFollowSet(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar);

bool contains(std::vector<std::string> vec, std::string s);

void leftFactor(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar);

void elemRecursion(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar);

std::set<std::string> getNullable(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar);

std::string findMatch(std::vector<std::vector<std::string>> productions, int indexToCheck);

std::vector<std::string> findLongestPrefix(std::vector<std::vector<std::string>> productions);

std::string getMatch(std::vector<std::vector<std::string>> productions, int indexToCheckOne, int indexToCheckTwo);

void sortMultimapValues(std::multimap<std::string, std::vector<std::vector<std::string>>>& multimap,  std::string key);

void sortMultimapValues(std::multimap<std::string, std::vector<std::string>>& multimap, std::string key);

#endif
