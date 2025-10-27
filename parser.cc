#include <iostream>
#include "parser.h"
#include "lexer.h"

std::vector<std::string> orderVector; 
std::vector<std::string> seenOrderVector;
std::vector<std::string> seenOrderVectorTerm;
std::vector<std::pair<std::string ,std::vector<std::string>>> order; 

std::unordered_map<std::string, std::vector<std::vector<std::string>>> startParser(){
    LexicalAnalyzer lexer = LexicalAnalyzer();
    Token currentToken; 
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammarRules;
    std::unordered_set<std::string> seenTerminals;
    std::unordered_set<std::string> seenNonTerminals;
    bool hashSeen = false;
    currentToken = lexer.GetToken();

    while(currentToken.token_type!= END_OF_FILE){
        if(currentToken.token_type == ERROR){
            std::cout << "SYNTAX ERROR !!!!!!!!!!!!!!" << "\n";
            exit(1);
        }
        
        if(currentToken.token_type == HASH){
            hashSeen = true;
            currentToken = lexer.GetToken();
            
            if(currentToken.token_type != END_OF_FILE){
                std::cout << "SYNTAX ERROR !!!!!!!!!!!!!!" << "\n";
                exit(1); 
            }
            break;
        }
        
        if(currentToken.token_type != ID){
            std::cout << "SYNTAX ERROR !!!!!!!!!!!!!!" << "\n";
            exit(1);
        }
        
        std::string lhs = currentToken.lexeme;

        if(seenNonTerminals.find(lhs) == seenNonTerminals.end()){
            seenNonTerminals.insert(lhs);
            seenOrderVector.push_back(lhs);
        }

        currentToken = lexer.GetToken();
        if(currentToken.token_type != ARROW){
            std::cout << "SYNTAX ERROR !!!!!!!!!!!!!!" << "\n";
            exit(1);
        }
        currentToken = lexer.GetToken();

        if(currentToken.token_type == STAR){
            std::vector<std::string> emptyRHS;
            grammarRules[lhs].push_back(emptyRHS);
            order.push_back({lhs,emptyRHS});
            currentToken = lexer.GetToken();
            continue;
        }

        if(currentToken.token_type == OR){
            std::vector<std::string> emptyRHS;
            grammarRules[lhs].push_back(emptyRHS);
            order.push_back({lhs,emptyRHS});
            currentToken = lexer.GetToken();
            if(currentToken.token_type == STAR){
                currentToken = lexer.GetToken();
                continue;
            }
        }

        std::vector<std::string> rhs;
        while(true){
            if(currentToken.token_type == ID){
                rhs.push_back(currentToken.lexeme);
                currentToken = lexer.GetToken();
            } else if(currentToken.token_type == OR){
                grammarRules[lhs].push_back(rhs);
                order.push_back({lhs,rhs});
                rhs.clear();
                currentToken = lexer.GetToken();

                if(currentToken.token_type == STAR){
                    std::vector<std::string> emptyRHS;
                    grammarRules[lhs].push_back(emptyRHS);
                    order.push_back({lhs,emptyRHS});
                    break;
                }
                while(currentToken.token_type == OR){
                    std::vector<std::string> emptyRHS;
                    grammarRules[lhs].push_back(emptyRHS);
                    order.push_back({lhs,emptyRHS});
                    currentToken = lexer.GetToken();

                    if(currentToken.token_type == STAR){
                        std::vector<std::string> emptyRHS2;
                        grammarRules[lhs].push_back(emptyRHS2);
                        order.push_back({lhs,emptyRHS2});
                        break;
                    }
                }
                if(currentToken.token_type == STAR){
                    break;
                }

            } else if (currentToken.token_type == STAR){
                if(rhs.empty()){
                    std::cout << "SYNTAX ERROR !!!!!!!!!!!!!!" << "\n";
                    exit(1);
                }
                grammarRules[lhs].push_back(rhs);
                order.push_back({lhs,rhs});
                break;
            } else if (currentToken.token_type == ARROW){
                std::cout << "SYNTAX ERROR !!!!!!!!!!!!!!" << "\n";
                exit(1);
            } else {
                std::cout << "SYNTAX ERROR !!!!!!!!!!!!!!" << "\n";
                exit(1);
            }
        }
        currentToken = lexer.GetToken();
    }
    if(!hashSeen){
        std::cout << "SYNTAX ERROR !!!!!!!!!!!!!!" << "\n";
        exit(1);
    }

    if(grammarRules.empty()){
        std::cout << "SYNTAX ERROR !!!!!!!!!!!!!!" << "\n";
        exit(1);
    }

    for(const auto& rule : order){
        std::string lhs = rule.first;
        std::vector<std::string> rhs = rule.second;

        for(const auto& symbol : rhs){
            if(seenNonTerminals.find(symbol) == seenNonTerminals.end()){
                if(seenTerminals.find(symbol) == seenTerminals.end()){
                    seenTerminals.insert(symbol);
                    seenOrderVectorTerm.push_back(symbol);
                }
            }
        }
    }

    return grammarRules;
}

void printTerminals(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std::set <std::string> seen;
    for(const auto& e : order){
        for(const auto& terminal : e.second){
            if(!contains(seenOrderVector, terminal) && seen.find(terminal)== seen.end()){
                std::cout << terminal << " ";
                seen.insert(terminal);
            }
        }
    }
}

void printRules(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std:: set<std::string> seen;
    for(const auto& e : order){
        if(seen.find(e.first) == seen.end()){
            std::cout << e.first << " ";
            seen.insert(e.first);
        }
        for(const auto& rhs : e.second){
            if(seen.find(rhs) == seen.end() && !contains(seenOrderVectorTerm, rhs)){
                std::cout << rhs << " ";
                seen.insert(rhs);
            }
        }
    }
}

std::set<std::string> nullable(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std::set <std::string> nullable;
    bool changed = true;
    while(changed){
        changed = false; 
        for(const auto& element: order){
            std::string lhs = element.first;
            std::vector<std::string> rhs = element.second;
            if(rhs.empty() && nullable.find(lhs) == nullable.end()){
                    nullable.insert(lhs);
                    changed = true;
            }
            bool allNullable = true;
            for(const auto& symbol : rhs){
                if(!contains(seenOrderVector, symbol) || nullable.find(symbol) == nullable.end()){
                    allNullable = false;
                    break;
                }
            }

            if(allNullable && nullable.find(lhs) == nullable.end()){
                nullable.insert(lhs);
                changed = true;
            }
        }
    }
    return nullable;
}

void printNullable(std::set<std::string> nullable){
    std::cout<<"Nullable = { ";
    std::set<std::string> printed;
    int count = 0;
    for(const auto& element : order){
        if(nullable.find(element.first) != nullable.end() && printed.find(element.first) == printed.end()){
            if(count > 0){
                std::cout<<" , ";
            }
            printed.insert(element.first);
            std::cout<< element.first;
            count++;
        }
        for(const auto& term : element.second){
            if(nullable.find(term) != nullable.end() && printed.find(term) == printed.end()){
                if(count > 0){
                    std::cout<<" , ";
                }
                count++;
                printed.insert(term);
                std::cout<< term;
            }
        }
    }
    std::cout<< " }";
}

std::unordered_map<std::string, std::vector<std::string>> firstSet(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std::set<std::string> nullableSet = getNullable(grammar);
    std::unordered_map<std::string, std::vector<std::string>> firstSets;
    bool changed = true;

    //apply rule 1
    for(const auto& element : order){
        if(!element.second.empty() && contains(seenOrderVectorTerm, element.second[0])){
            if(firstSets[element.first].empty()){
                firstSets[element.first].push_back(element.second[0]);
            } else {
                if(!contains(firstSets[element.first], element.second[0])){
                    firstSets[element.first].push_back(element.second[0]);
                }
            }
            
        }
    }

    while(changed){
        changed = false;
        for(const auto& element : order){
            for(const auto& symbol : element.second){
                if(contains(seenOrderVector, symbol)){ //get first set of current symbol
                    if(nullableSet.find(symbol) == nullableSet.end()){
                        if(firstSets[element.first].empty()){
                            if(!firstSets[symbol].empty()){
                                for(const auto& fs: firstSets[symbol]){
                                    firstSets[element.first].push_back(fs);
                                    changed = true;
                                }
                            }
                        } else{
                            if(!firstSets[symbol].empty()){
                                for(const auto& fs : firstSets[symbol]){
                                    if(!contains(firstSets[element.first], fs)){
                                        firstSets[element.first].push_back(fs);
                                        changed = true;
                                    }
                                }
                            }
                        }
                        break;
                    } else{
                        if(firstSets[element.first].empty()){
                            if(!firstSets[symbol].empty()){
                                for(const auto& fs : firstSets[symbol]){
                                    firstSets[element.first].push_back(fs);
                                }
                                changed = true;
                            }
                        } else{
                            if(!firstSets[symbol].empty()){
                                for(const auto& fs : firstSets[symbol]){
                                    if(!contains(firstSets[element.first], fs)){
                                        firstSets[element.first].push_back(fs);
                                        changed = true;
                                    } 
                                }
                            }
                        }
                        continue;
                    }
                } else {
                    if(firstSets[element.first].empty()){
                        firstSets[element.first].push_back(symbol);
                        changed = true;
                    } else{
                        if(!contains(firstSets[element.first], symbol)){
                            firstSets[element.first].push_back(symbol);
                            changed = true;
                        }
                    }
                    break;
                }
            }
        }
    }

    
    return firstSets;
}

void printFirstSet(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar,std::unordered_map<std::string, std::vector<std::string>> firstSets){
    std::set<std::string> printed;
    for(const auto& e : order){
        if(printed.find(e.first) == printed.end()){
            std:: cout << "FIRST(" << e.first << ") = { ";
            printed.insert(e.first);
            int count = 0;
            for(const auto& term : seenOrderVectorTerm){
                if(contains(firstSets[e.first],term)){
                    if(count > 0){
                        std:: cout << ", ";
                    }
                    std:: cout << term;
                    count++;
                }
            }
            std::cout << " } \n";
        }
        for(const auto& symbol : e.second){
            int count = 0;
            if(printed.find(symbol) == printed.end() && contains(seenOrderVector, symbol)){
                printed.insert(symbol);
                std:: cout << "FIRST(" << symbol << ") = { ";
                for(const auto& term : seenOrderVectorTerm){
                    if(contains(firstSets[symbol], term)){
                        if(count > 0){
                            std::cout << ", ";
                        }
                        count++;
                        std::cout << term;
                    }
                }
                std::cout << " } \n";
            }
        }
    }
    
}


void printFollowSet(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std::unordered_map<std::string, std::set<std::string>> followSets;
    std::unordered_map<std::string, std::vector<std::string>> firstSets = firstSet(grammar); 
    std::set<std::string> nullable = getNullable(grammar);

    followSets[order[0].first].insert("$"); //rule 1 

    //apply rule 4 & 5
    for(const auto& element : order){
        for(int i = 0; i < element.second.size(); i++){
            std::string currentSymbol = element.second[i];
            if(contains(seenOrderVector,currentSymbol)){ // if it is a terminal we care and wanna start building the followSets 
                if(i+1 < element.second.size()){ //checking if the next symbol is the last one in the vector 
                    std::string nextSymbol = element.second[i+1]; //get the next symbol
                    if(contains(seenOrderVector, nextSymbol)){ //if it is a nonTerminal we need to get the first set 
                        if(nullable.find(nextSymbol) != nullable.end()){ //if the nextSymbol is nullable, att first set and then move to the next symbol till not nullable
                            for(const auto& symbol : firstSets[nextSymbol]){
                                followSets[currentSymbol].insert(symbol);
                            }
                            for(int j = i+1; j < element.second.size(); j++){
                                std::string s = element.second[j];
                                if(nullable.find(s) == nullable.end()){
                                    if(contains(seenOrderVectorTerm, s)){
                                        followSets[currentSymbol].insert(s);
                                    } else{
                                        for(const auto& sym : firstSets[s]){
                                            followSets[currentSymbol].insert(sym);
                                        }
                                    }
                                    break;
                                }
                            }
                        } else{ //not nullable
                            for(const auto& symbol : firstSets[nextSymbol]){
                                followSets[currentSymbol].insert(symbol);
                            }
                        }
                    } else { //if it is a terminal we just add it to the followSets
                        followSets[currentSymbol].insert(nextSymbol);
                    }
                }
            }
        }
    }

    bool changed = true;
    while(changed){
        changed = false;
        for(const auto& element : order){
            for(int i = 0; i < element.second.size(); i++){
                std::string currentSymbol = element.second[i];
                if(i+1 < element.second.size()){
                    std::string nextSymbol = element.second[i+1];
                    if(nullable.find(nextSymbol) != nullable.end()){
                        bool allNullable = true;
                        for(int j = i+1; j < element.second.size(); j++){
                            if(nullable.find(element.second[j]) == nullable.end()){
                                allNullable = false;
                                break;
                            }
                        }
                        if(allNullable){
                            for(const auto& s: followSets[element.first]){
                                if(followSets[currentSymbol].insert(s).second == true){
                                    changed = true;
                                }
                            }
                        }
                    }
                }  else{
                    for(const auto& symbol : followSets[element.first]){
                        if(followSets[currentSymbol].insert(symbol).second == true){
                            changed = true;
                        }
                    }
                    
                }
            }
        }
    }

    seenOrderVectorTerm.insert(seenOrderVectorTerm.begin(), "$");
    std::set<std::string> printed;
    for(const auto& e : order){
        if(printed.find(e.first) == printed.end()){
            std:: cout << "FOLLOW(" << e.first << ") = { ";
            printed.insert(e.first);
            int count = 0;
            for(const auto& term : seenOrderVectorTerm){
                if(followSets[e.first].find(term) != followSets[e.first].end()){
                    if(count > 0){
                        std:: cout << ", ";
                    }
                    std:: cout << term;
                    count++;
                }
            }
            std::cout << " } \n";
        }
        for(const auto& symbol : e.second){
            int count = 0;
            if(printed.find(symbol) == printed.end() && contains(seenOrderVector, symbol)){
                printed.insert(symbol);
                std:: cout << "FOLLOW(" << symbol << ") = { ";
                for(const auto& term : seenOrderVectorTerm){
                    if(followSets[symbol].find(term) != followSets[symbol].end()){
                        if(count > 0){
                            std::cout << ", ";
                        }
                        count++;
                        std::cout << term;
                    }
                }
                std::cout << " } \n";
            }
        }
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

std::set<std::string> getNullable(std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar){
    std::set <std::string> nullable;
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
