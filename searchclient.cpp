#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <iostream>
#include <cstring>
#include <limits>
#include <dirent.h> // Needed for directory search
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

// Time
#include <ctime>
#include <unistd.h>
#include <sys/time.h>

using namespace std;

/*Global Vars*/
string bookInfo[3];//Contains book num, path, and title
string delimiters = " ,.;:\'\"()[]*!&^#${}@?";//Delimiters
int matches = 0;//Number of matches found

//For finding elapsed time
clock_t begin;
clock_t end;

/*Functions*/
int stoi(string str);//Convert string to int
void SearchIndex(string searchTerm);//Searches index for a word
void ParseLine(const string &line,const char* delim, string wordLoc[2], int wordLocInt[2]);//Parses line of book id, pos
void RetrieveBookInfo(int wordLocInt[2]);//Gets a book's info once id is found
std::fstream& GotoLine(std::fstream& file, int &num);//Goes to a certain line in a file
string FindLine(string bookPath, int pos);//Finds a line in a book


int main() {
	string searchTerm;
	/*Cgicc cgi;//Ajax object
	form_iterator termIt = cgi.getElement("term");
	string term = **searchTerm;
	
	// output required message for AJAX
	cout << "Content-Type: text/plain\n\n";
	*/
	
	cout << "Enter a search term: " << endl;
	cin >> searchTerm; 
	
	begin = clock();
	SearchIndex(searchTerm);
	
}

void SearchIndex(string searchTerm) {
	
	int pos = 0;//Pos init to 0
	double delta;//Time elapsed
	int wordLocInt[2];//Integer value of book id and word pos
	string wordLoc[2];//String value from file of book id and word pos
	string line;//Contains line
	
	searchTerm = "index/" + searchTerm + ".index";//Turns search term into "index/word.index"

	
	ifstream infile(searchTerm.c_str());//Brings index file into stream
	
		if(!infile.fail()) {//If the file opens, ...
		
			while(!infile.eof()) {//While not at end of file, ...
				getline(infile,line);//Gets line
				
				if(!line.empty()) {//If the line isn't empty, 
					
					ParseLine(line,",",wordLoc,wordLocInt);//Gets the book id, path, and title
					
					RetrieveBookInfo(wordLocInt);//Gets a book's id, path, and title. Stores it in bookInfo array
					
					cout << "Title:" << bookInfo[2] << "\n" << "Path:" << bookInfo[1] << "\n"  << FindLine(bookInfo[1],wordLocInt[1]) << "\n" <<endl;
					matches++;
				} else	{}
				
				pos = infile.tellg();//Sets pos to start of next line
			}
			
			end = clock();//Get current time
			
			delta = double(end-begin)/ CLOCKS_PER_SEC;//Finds elapsed time in s
			cout << matches << " matches found in " << 	delta << "s" << endl;//Prints # matches in elapsed time
			
		} else {
			cout << "Word not found" << endl;//If index doesn't exist, no words can be matched
		}

}

//Code snippet by Yang Song from http://ysonggit.github.io/coding/2014/12/16/split-a-string-using-c.html
//Modified by Christian Solorio
void ParseLine(const string &line,const char* delim, string wordLoc[2], int wordLocInt[2]) {

    // to avoid modifying original string
    // first duplicate the original string and return a char pointer then free the memory
    char * dup = strdup(line.c_str());
    char * token = strtok(dup, delim);
    int i = 0;
    while(token != NULL && i<2){
        wordLoc[i] = string(token);
        i++;
        // the call is treated as a subsequent calls to strtok:
        // the function continues from where it left in previous invocation
        token = strtok(NULL, delim);
    }
    free(dup);
    
    wordLocInt[0] = stoi(wordLoc[0]);
    wordLocInt[1] = stoi(wordLoc[1]);
}

void RetrieveBookInfo(int wordLocInt[2]) {
	
	int pos;
	string line;//Stores the line that contains book number, path, and title
	fill_n(bookInfo,3,0);
	
	fstream infile("book_catalog.txt");//Opens the book
	
	GotoLine(infile,wordLocInt[0]);
	
	getline(infile,line);
	
	const char * delim = ",";
	
	char * dup = strdup(line.c_str());
    char * token = strtok(dup, delim);
    int i = 0;
    while(token != NULL && i<4){
        bookInfo[i] = string(token);
        i++;
        // the call is treated as a subsequent calls to strtok:
        // the function continues from where it left in previous invocation
        token = strtok(NULL, delim);
    }
    free(dup);
    
    //cout << "Book path is " << bookInfo[1] << endl;
}

//Finds the line of a given position of a word
string FindLine(string bookPath, int pos) {

	string wordLine;
	ifstream infile(bookPath.c_str());
	
	if(infile.good()) {
		infile.seekg(pos,infile.beg);
		getline(infile, wordLine);//Goes to pos in file, saves line as wordLine
		return(wordLine);
	}else { 
		return("error reading line");
	}
}

//Converts a string of numbers to an int
int stoi(string str) {
	int num;
	stringstream sstream;
    
    sstream.str(str);
    sstream >> num;
    
    sstream.clear();
    
    return(num);
}

std::fstream& GotoLine(std::fstream& file, int &num) {

    file.seekg(std::ios::beg);
    
    for(int i=0; i < num; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');//Ignores lines until num reached
    }
    
    return file;
}
