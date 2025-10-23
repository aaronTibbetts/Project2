/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"
#include "parser.h"

using namespace std;

// read grammar
void ReadGrammar(std::unordered_map<std::string, std::vector<std::vector<std::string>>>& grammar) {
  grammar =startParser();
}

/* 
 * Task 1: 
 * Printing the terminals, then nonterminals of grammar in appearing order
 * output is one line, and all names are space delineated
*/
void Task1(){


}

/*
 * Task 2:
 * Print out nullable set of the grammar in specified format.
*/
void Task2()
{
}

// Task 3: FIRST sets
void Task3()
{
}

// Task 4: FOLLOW sets
void Task4()
{
}

// Task 5: left factoring
void Task5()
{
}

// Task 6: eliminate left recursion
void Task6()
{
}
    
int main (int argc, char* argv[])
{
    int task;
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar;
    std::unordered_map<std::string, std::vector<std::string>> firstSets;
    std::vector<string> order; 
    std::vector<string> termOrder;
    std::set<string> nullableSet;
    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }
    
    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);
    
    ReadGrammar(grammar);  // Reads the input grammar from standard input
                    // and represent it internally in data structures
                    // ad described in project 2 presentation file

    switch (task) {
        case 1: Task1();
            printTerminals(grammar);
            printRules(grammar);
            break;

        case 2: Task2();
            nullableSet = nullable(grammar);
            printNullable(nullableSet);
            break;

        case 3: Task3();
        firstSets= printFirstSet(grammar);
        order = getVector(grammar);
        termOrder =getVectorTerm(grammar);

        for(const auto& entry: order){
                std::cout<<"FIRST(" << entry <<") = { ";
                int count = 0;
                for(int it = 0; it < termOrder.size(); it++){
                    if(contains(firstSets[entry], termOrder[it])){
                        if(firstSets[entry].size() == 1){
                            std::cout<< termOrder[it];
                        }else{
                            if(count == firstSets[entry].size()-1){
                                std::cout<<termOrder[it];
                            }else{
                                    std::cout<<termOrder[it]<<", ";
                                    count++;
                                }
                                
                            }
                        } 
                        
                    }
                    std::cout<<" }\n";

                }
            break;

        case 4: Task4();
            printFollowSet(grammar);
            break;

        case 5: Task5();
            leftFactor(grammar);
            break;
        
        case 6: Task6();
            elemRecursion(grammar);
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
            
    return 0;
}
