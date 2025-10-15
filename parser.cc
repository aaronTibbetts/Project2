#include <iostream>
#include "parser.h"
#include "lexer.h"

std::vector<std::string> orderVector; 
std::vector<std::string> seenOrderVector;
std::vector<std::string> seenOrderVectorTerm;

std::unordered_map<std::string, std::vector<std::vector<std::string>>> startParser(){
    LexicalAnalyzer lexer = LexicalAnalyzer();
    Token currentToken; 
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammarRules;
    while((currentToken = lexer.GetToken()).token_type!= END_OF_FILE){
        rule newRule;
        if(lexer.peek(1).token_type == ARROW){
            int i = 2;
            newRule.LHS = currentToken.lexeme;
            orderVector.push_back(currentToken.lexeme);
            std::vector<std::string> rhs;
            while(lexer.peek(i).token_type!= STAR){
            if(lexer.peek(i).token_type == ARROW){
                //do nothin   
            } else if (lexer.peek(i).token_type == ID){ 
               rhs.push_back(lexer.peek(i).lexeme);
            } else if(lexer.peek(i).token_type == OR ){
                grammarRules[newRule.LHS].push_back(rhs);
                rhs.clear();
            }
                i++;
            }

            grammarRules[newRule.LHS].push_back(rhs);
        }
        
    }
   
    return grammarRules;
}

void printTerminals(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std:: vector <std::string> terminals;
    std:: set<std::string> seen;
    for (const auto& element : orderVector) {
        const auto v = grammar[element]; 
        for (const auto& rhs : v) {
            for (const auto& symbol : rhs) {
                if (grammar.find(symbol) == grammar.end() &&seen.find(symbol)==seen.end()) {
                    terminals.push_back(symbol);
                    seen.insert(symbol);
                }
            }
        }
    }
    for(const auto& e : terminals){
        std::cout << e << " ";
    }
}

void printRules(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std:: vector <std::string> nonTerminals;
    std:: set<std::string> seen;
    for (int i = 0; i < orderVector.size(); i++) {
        const auto v = grammar[orderVector[i]];
        if(i == 0 || seen.find(orderVector[i]) == seen.end()){
            nonTerminals.push_back(orderVector[i]);
            seen.insert(orderVector[i]);
        }
        for(const auto& rhs : v){
            for(const auto& symbol : rhs){
                if(grammar.find(symbol) != grammar.end() && seen.find(symbol) == seen.end()){
                    nonTerminals.push_back(symbol);
                    seen.insert(symbol);
                }
            }
        }
    }
       

    for(const auto& e : nonTerminals){
        std::cout << e << " ";
    }
}

void printNullable(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std::set <std::string> nullable;
    seenOrderVector = getVector(grammar);
    bool changed = true;
    while(changed){
        changed = false; 
        for(auto& element: grammar){
            int count = 0;
            for(const auto& rhs: element.second){
                if(rhs.size() == 0 &&nullable.insert(element.first).second){
                    changed = true;
                } else {
                    for(const auto& s: rhs){
                        if(nullable.find(s)!= nullable.end()){
                            count++;
                        }
                    }
                    if(count == rhs.size()&&nullable.insert(element.first).second){
                        changed = true; 
                    }
                } 
            }
        }
    }

    std::cout<<"Nullable = { ";
    for(int i = 0; i < seenOrderVector.size(); i++){
        if(nullable.count(seenOrderVector[i]) && contains(seenOrderVector,seenOrderVector[i])){
            if(nullable.size() == 1){
                std::cout<<seenOrderVector[i]<< " ";
            } else {
                if(i >= nullable.size()){
                    std::cout<<seenOrderVector[i]<< " ";
                } else {
                    std::cout<<seenOrderVector[i]<< " , ";
                }
            }
        }
    }
    std::cout<< " }";
    
}

std::unordered_map<std::string, std::vector<std::string>> printFirstSet(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std::unordered_map<std::string, std::vector<std::string>> firstSets;
    bool changed = true;
    std::vector<std::string> seenOrderVector = getVector(grammar);


    for(const auto& element : seenOrderVector){
         firstSets[element] = { };
    }

    while(changed){
        changed = false;
        for(const auto& element: grammar){
            std::string nonTerminal = element.first;
            for(const auto& production: element.second){
                bool isNullable = true;
                for(int i = 0; i < production.size(); i++){
                    std::string symbol = production[i];
                    if(grammar.find(symbol) == grammar.end()){
                       if(!contains(firstSets[nonTerminal], symbol)){
                            firstSets[nonTerminal].push_back(symbol);
                            changed = true;
                        }
                        isNullable = false;
                        break;
                    }else{
                        std::vector<std::string> firstOfSymbol = firstSets[symbol];
                        for(const auto& terminal : firstOfSymbol){
                            if(terminal!= " " &&!contains(firstSets[nonTerminal], terminal)){
                                firstSets[nonTerminal].push_back(terminal);
                                changed = true;
                            }
                        }
                    if(std::find(firstOfSymbol.begin(), firstOfSymbol.end(), " ") == firstOfSymbol.end()){
                        isNullable = false;
                        break;
                        }
                    }
                }
                if(isNullable && !contains(firstSets[nonTerminal], " ")){
                        firstSets[nonTerminal].push_back(" ");
                        changed = true;
                    }
                }
            }
      }
     return firstSets;
}



void printFollowSet(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std::unordered_map<std::string, std::vector<std::string>> followSets;
    std::unordered_map<std::string, std::vector<std::string>> firstSets = printFirstSet(grammar); 
    std::vector<std::string> seenOrderVector = getVector(grammar);
    std::vector<std::string> seenOrderVectorTerm = getVectorTerm(grammar);
    std::set<std::string> nullable = getNullable(grammar);
    seenOrderVectorTerm.insert(seenOrderVectorTerm.begin(), "$");

    for(const auto& element : seenOrderVector){
        followSets[element] = {};
    }
    
    followSets[seenOrderVector[0]].push_back("$"); //rule  1

    for(const auto& element: grammar){ //rule 4 & 5
        for(const auto& vec : element.second){
            int k = vec.size();
            for(int i =0; i < k-1; i++){
                if(contains(seenOrderVector,vec[i])){
                    std::string j = vec[i+1];
                    for(const auto& symbol :  firstSets[j]){
                        if(!symbol.empty()){
                            followSets[vec[i]].push_back(symbol);
                            if(nullable.find(j) == nullable.end())
                            break;
                        }
                    }
                }
            }
        }
    }


    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& element : grammar) {
            std::string nonTerminal = element.first;
            for(const auto& production : element.second){
                for(int i = 0; i < production.size(); i++){
                    std::string symbol  = production[i];
                    if(grammar.find(symbol) != grammar.end()){
                        if(i<production.size()-1){
                            std::string nextSymbol = production[i+1];
                            if(grammar.find(nextSymbol) != grammar.end()){ //if non terminal 
                                if(nullable.find(nextSymbol) != nullable.end()){
                                    std::string next = nextSymbol;
                                    int current = i+1;
                                    while(current < production.size()){
                                        std::string next = production[current];
                                        if(grammar.find(next) == grammar.end()){
                                            if(!contains(followSets[symbol],next)){
                                                followSets[symbol].push_back(next);
                                                changed = true;
                                                
                                            }
                                            break;
                                        } else if(nullable.find(next) == nullable.end()){
                                            break;
                                        }else{
                                            current++;
                                        }
                                    }
                                    if(current == production.size()){
                                        for(const auto& term: followSets[nonTerminal]){
                                            if(!contains(followSets[symbol],term) && !term.empty()){
                                                followSets[symbol].push_back(term);
                                                changed = true;
                                            }
                                        }
                                    }
                                } else { 
                                    for(const auto& term: firstSets[nextSymbol]){
                                        if(!term.empty() && !contains(followSets[symbol],term)){
                                            followSets[symbol].push_back(term);
                                            changed = true;
                                        }
                                    }
                                }
                            } else {
                                if(!contains(followSets[symbol],nextSymbol) && !nextSymbol.empty()){
                                    followSets[symbol].push_back(nextSymbol);
                                    changed = true;
                                }
                            } 
                        }
                        if(i == production.size()-1){ 
                            if(nullable.find(symbol)!=nullable.end()){
                                if(i-1 > -1){
                                std::string prev = production[i-1];
                                    for(const auto& term: followSets[nonTerminal]){
                                        if(!contains(followSets[prev],term)&& !term.empty()){
                                            followSets[prev].push_back(term);
                                            changed = true;
                                        }
                                    }
                                }
                            }
                            for(const auto& term: followSets[nonTerminal]){
                                if(!contains(followSets[symbol],term) &&  !term.empty()){
                                    followSets[symbol].push_back(term);
                                    changed = true;
                                }   
                            }   
                        }
                    }
                }
            }
        } 
    }
   

    for(const auto& element :followSets){
        for(const auto& v: element.second){
            if(contains(element.second, "$")){
                if(v == element.second.front()){
                    if(v != "$"){
                        auto tmp = element.second;
                        auto it = std::find(tmp.begin(), tmp.end(), "$");
                        std::rotate(tmp.begin(), it , it+1);
                        followSets[element.first] = tmp; 
                    }
                }
            }
        }
    }

    for(const auto& element : followSets){
        if(contains(followSets[element.first], " ")){
            if(contains(element.second, " ")){
                auto tmp = element.second;
                auto i = std::remove(tmp.begin(), tmp.end(), " ");
                tmp.erase(i, tmp.end());
                followSets[element.first]= tmp;
            }
        }
    }

    for(const auto& element: seenOrderVectorTerm){
        if(element.empty()){
           std::cout<<"has space";
        }
    }

    for(const auto& entry: seenOrderVector){
        std::cout<<"FOLLOW(" << entry <<") = { ";
        bool first = true;
        for(const auto& term: seenOrderVectorTerm){
            if(contains(followSets[entry], term)){
                if(!first){
                    std::cout<<", ";
                }
                std::cout << term;
                first = false;
            }
        }
        std::cout<<" }\n";
    }
       
}
    
void leftFactor(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std::multimap<std::string, std::vector<std::vector<std::string>>> newGrammar;
    for(const auto& element : grammar){
        if(element.second.size() >1){
            std::vector<std::string> prefix = findLongestPrefix(element.second);
            if(prefix.size() > 0){
                std::string newNonTerminal = element.first + "1";
                for(auto& i :element.second){
                    if(i.empty()){
                        newGrammar.insert({element.first, {i}});
                        continue;
                    }
                    int count = 0;
                    for(auto& j : prefix){
                        if(contains(i,j)){
                            count++;
                        }
                    }
                    if(count == prefix.size()){
                        std::vector<std::string> newRHS;
                        for(int k = prefix.size(); k < i.size(); k++){
                            newRHS.push_back(i[k]);
                        }
                        newGrammar.insert({newNonTerminal, {newRHS}});
                    }
                }
                sortMultimapValues(newGrammar, newNonTerminal);

                prefix.push_back(newNonTerminal);
                newGrammar.insert({element.first, {prefix}});
            } else {
                std::vector<std::vector<std::string>> tmp = element.second;
                std::sort(tmp.begin(), tmp.end(),[](const std:: vector<std::string>&a, const std::vector<std::string>&b){
                    if(a.empty()) return true;
                    if(b.empty()) return false;
                    if(a[0].empty()) return true;
                    if(b[0].empty()) return false;
                    return a[0] < b[0];
                });
                
                for(const auto& rhs : tmp){
                    newGrammar.insert({element.first,{rhs}});
                }
            }
        }else if(element.second.size() == 1){
            newGrammar.insert({element.first,element.second});
        }
    }

    for(const auto& e : newGrammar){
        std::cout << e.first << " -> ";
        for(const auto& q: e.second){
            for(const auto& w : q){
                std::cout << w << " ";
            }
        }
        std::cout<<"#\n";
    }
}

void elemRecursion(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std::multimap<std::string, std::vector<std::string>> newGrammar;
    std::vector<std::string> seenOrderVector = getVector(grammar); 
   

    for(const auto& nonTerminal: seenOrderVector){
         bool ruleDeleted = false;
         bool added = false;
         bool needsReview = false;
        for(auto& production : grammar[nonTerminal]){
            std::vector<std::string> newRHS;
            if(isupper(production[0][0]) && production[0] < nonTerminal){
                while((production[0] <= nonTerminal && !added)){
                    std::string firstElement = production[0];
                    if(grammar[firstElement][0][0] == firstElement){
                        //need to delete this production
                        std::vector<std::string> oldRHS = grammar[firstElement][0];
                        grammar[nonTerminal].erase(std::remove(grammar[nonTerminal].begin(), grammar[nonTerminal].end(), production), grammar[nonTerminal].end());
                        grammar.erase(firstElement);
                        oldRHS.push_back(firstElement + "1");
                        oldRHS.erase(oldRHS.begin());
                        newGrammar.insert({firstElement + "1", {{" "}}});
                        newGrammar.insert({firstElement + "1", {oldRHS}});
                        seenOrderVector.erase(std::remove(seenOrderVector.begin(), seenOrderVector.end(), firstElement), seenOrderVector.end());
                        ruleDeleted = true;
                        break;
                    } else {
                        if(grammar[firstElement].size() > 1){
                            std::vector<std::string> reviewProdcution;
                            for(const auto& rhs : grammar[firstElement]){
                                std::vector<std::string> temp = production;
                                temp.erase(temp.begin());
                                temp.insert(temp.begin(), rhs.begin(), rhs.end());
                                //need to check if there is still left recursion
                                newGrammar.insert({nonTerminal, temp});
                                added = true;
                            }
                            break;
                            
                        } else {
                            production.erase(production.begin());
                            production.insert(production.begin(), grammar[firstElement][0].begin(), grammar[firstElement][0].end());
                        }
                    }
                }
                if(!ruleDeleted && !added){
                    newGrammar.insert({nonTerminal, production});
                }
                
            } else {
                newGrammar.insert({nonTerminal, production});
            }
        }
       sortMultimapValues(newGrammar, nonTerminal);
    }
   
    

    for(const auto& e : newGrammar){
        std::cout << e.first << " -> ";
        for(const auto& q: e.second){
            for(const auto& w : q){
                std::cout << w << " ";
            }
        }
        std::cout<<"#\n";
    }
       
}

std::vector<std::string>getVector(std::unordered_map<std::string, std::vector<std::vector<std::string>>>  grammar){
    std:: vector <std::string> nonTerminals;
    std:: set<std::string> seen;
    for (int i = 0; i < orderVector.size(); i++) {
        const auto v = grammar[orderVector[i]];
        if(i == 0 || seen.find(orderVector[i]) == seen.end()){
            nonTerminals.push_back(orderVector[i]);
            seen.insert(orderVector[i]);
        }
        for(const auto& rhs : v){
            for(const auto& symbol : rhs){
                if(grammar.find(symbol) != grammar.end() && seen.find(symbol) == seen.end()){
                    nonTerminals.push_back(symbol);
                    seen.insert(symbol);
                }
            }
        }
    }
    seenOrderVector = nonTerminals;
    
    return seenOrderVector;
}

std::vector<std::string> getVectorTerm(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std:: vector <std::string> terminals;
    std:: set<std::string> seen;
    for (const auto& element : orderVector) {
        const auto v = grammar[element]; 
        for (const auto& rhs : v) {
            for (const auto& symbol : rhs) {
                if (grammar.find(symbol) == grammar.end() &&seen.find(symbol)==seen.end()) {
                    terminals.push_back(symbol);
                    seen.insert(symbol);
                }
            }
        }
    }
    seenOrderVectorTerm = terminals;
    return seenOrderVectorTerm;
}

std::set<std::string> getNullable(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std::set <std::string> nullable;
    seenOrderVector = getVector(grammar);
    bool changed = true;
    while(changed){
        changed = false; 
        for(auto& element: grammar){
            int count = 0;
            for(const auto& rhs: element.second){
                if(rhs.size() == 0 &&nullable.insert(element.first).second){
                    changed = true;
                } else {
                    for(const auto& s: rhs){
                        if(nullable.find(s)!= nullable.end()){
                            count++;
                        }
                    }
                    if(count == rhs.size()&&nullable.insert(element.first).second){
                        changed = true; 
                    }
                } 
            }
        }
    }
    return nullable;
}

bool contains(std::vector<std::string> vec, std::string s) {
    return std::find(vec.begin(), vec.end(), s) != vec.end();
}

std::vector<std::string> findLongestPrefix(std::vector<std::vector<std::string>> productions){ //not fully working to find longest 
    std::set<int> indexes;
    std::vector<std::string> prefix;
    std::string tmp;
    for(int i = 0; i < productions.size(); i++){
        tmp = findMatch(productions, i);
        if(tmp.empty()){
            break;
        } else{
            prefix.push_back(tmp);
        }
    }
    return prefix;
}

std::string findMatch(std::vector<std::vector<std::string>> productions, int indexToCheck){
    std::string match;
    for(int i=0; i < productions.size(); i++){
        if(!productions[i].empty()){
            if(i+1 < productions.size() && !productions[i+1].empty()){ 
                if(productions[i][indexToCheck] == productions[i+1][indexToCheck]){
                    match = getMatch(productions, i, indexToCheck);
                }
            } else{
                if(productions[i+1].empty()){
                    break;
                }
            }
        }
    }
    return match;
}

std::string getMatch(std::vector<std::vector<std::string>> productions, int indexToCheckOne, int indexToCheckTwo){
    return productions[indexToCheckOne][indexToCheckTwo];
}

void sortMultimapValues(std::multimap<std::string, std::vector<std::vector<std::string>>>& multimap,  std::string key) {
    // Extract all values associated with the given key
    std::vector<std::vector<std::string>> values;
    auto range = multimap.equal_range(key);
    for (auto it = range.first; it != range.second; ++it) {
        for(const auto& vec: it->second){
            values.push_back(vec);
        }
    }

    // Sort the extracted values lexicographically
    std::sort(values.begin(), values.end(), [](const std::vector<std::string>& a, const std::vector<std::string>& b) {
        if (a.empty()) return true;
        if (b.empty()) return false;
        return a < b; // Lexicographical comparison
    });

    // Remove the old entries for the key
    multimap.erase(range.first, range.second);

    // Reinsert the sorted values into the multimap
    for (const auto& value : values) {
        multimap.insert({key, {value}});

    }
}

void sortMultimapValues(std::multimap<std::string, std::vector<std::string>>& multimap, std::string key){
        // Extract all values associated with the given key
        std::vector<std::vector<std::string>> values;
        auto range = multimap.equal_range(key);
        for (auto it = range.first; it != range.second; ++it) {
            values.push_back(it->second); // Push the vector of strings into the values vector
        }
    
        // Sort the extracted values lexicographically
        std::sort(values.begin(), values.end(), [](const std::vector<std::string>& a, const std::vector<std::string>& b) {
            if (a.empty()) return true;  // Empty vector comes first
            if (b.empty()) return false; // Non-empty vector comes after empty vector
            return a < b;                // Lexicographical comparison
        });
    
        // Remove the old entries for the key
        multimap.erase(range.first, range.second);
    
        // Reinsert the sorted values into the multimap
        for (const auto& value : values) {
            multimap.insert({key, value});
        }
    }
