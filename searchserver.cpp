/***************************************************************************
* searchserver.cpp - Project Guttenburg Search Engine
*
* copyright : (C) 2017 by Christian Solorio, Aidan Murphy, Jessie Alperin
*
* This program is a command line interface for the project guttenburg search
* engine we will be building. It makes a map of all of the words in all of
*
*
***************************************************************************/

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
#include <dirent.h> // Needed for directory search
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

// Time
#include <ctime>
#include <unistd.h>
#include <sys/time.h>

using namespace std;

/*Data Structure*/
struct bookInfo { 
	int bookNum;
	string bookPath;
	string title;
};
struct wordLoc {
	short pos;
	string wordPath;
};


/*Global Variables*/
string path = "/home/skon/books";
string delimiters = " ?,.;:'\"()[]*!&^#$";

vector<bookInfo> books;
vector<wordLoc> wordLocation;
map<string, vector<int> > index;//The map of words/references

int bookNumber = 0;
int bookLimit = 2;

/*Function Declarations*/
int CreateBookMap(map<string, vector<int> > &indexMap, string filename);//Creates the index map

void AddBookInfo(string file);
void PrintVec(vector<bookInfo> vec);
void WriteVec(vector<bookInfo> vec);
void ReadVec(vector<bookInfo> vec);

string FindTitle(string filename);
string getNext(string & line);

//For navigating the directory
void ProcessDirectory(string directory);
void ProcessFile(string file);
void ProcessEntity(struct dirent* entity);
bool hasEnding (string const &fullString, string const &ending);

int main() {
	
	string directory = "";
	
	try {
		ProcessDirectory(directory);
		//WriteVec(books);
		//PrintVec(books);
		//ReadVec(books);
		
		cout << "Finished run!" << endl;
		
	} catch (int e) {
		cout << "Error: Call Helpline\n" << endl;
	}
	
}

/*FUNCTION DEFINITIONS*/

//Function that will find the title of a book and return that string
string FindTitle(string filename) {

	string line = "";
	int lineNum = 0;
	int lineMax = 150;
	
	ifstream infile(filename.c_str());
	
	while (line.find("Title:") == string::npos) {
	
		getline(infile,line);
		
		if(line.find("Title:") != string::npos) {
		
			line.erase(0,7);
			return(line);
		}
		
		if(lineNum > lineMax) {
			return("Title Not Found");
		}		
		lineNum++;
	}
	
}

//Prints each element of a vector, mainly used for testing purposes
void PrintVec(vector<bookInfo> vec) {
	
	for(int it = 0; it < vec.size(); it++) {
		cout << vec[it].bookNum << "," << vec[it].bookPath << "," << vec[it].title << endl;
	}
}

//Writes the book information  vector to a file. 
//Can write in binary (I think?) or as regular text
void WriteVec(vector<bookInfo> vec) {
	
	/* CODE TO WRITE IN BINARY*/
	/*string fileName = "book_info.txt";
	ofstream myfile(fileName.c_str(),ios::out | ios::binary);
	if(myfile.is_open()) {
	
		for(int it = 0; it < vec.size(); it++) {
			myfile.write((char*)&vec[it].bookNum,sizeof(int));
			
			//<< vec[it].bookNum << "," << vec[it].bookPath << "," << vec[it].title << endl;
		}
		
		myfile.close();	
	} else {
		cout << "Unable to open file";
	}*/
	
	
	/*CODE TO WRITE TO FILE NOT IN BINARY*/
	ofstream myfile;
	myfile.open("book_info.txt");
	for(int it = 0; it < vec.size(); it++) {
		myfile << vec[it].bookNum << "," << vec[it].bookPath << "," << vec[it].title << "\n";
	}
	myfile.close();
	
}

//Can read the book information in binary (I don't think this actually works yet)
void ReadVec(vector<bookInfo> vec) {
	
	/*int bookID;
	
	string fileName = "book_info.txt";
	ifstream myfile(fileName.c_str(), ios::in | ios::binary);
	
	if(myfile.is_open()) {
		while(!myfile.eof()) {
			myfile.read((char*)bookID,sizeof(int));
			cout << bookID << endl;
		}
		myfile.close();
	}else {
		cout << "Unable to read file";
	}*/
}


// Check if a string has a given ending
bool hasEnding (string const &fullString, string const &ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare (fullString.length() - ending.\
				     length(), ending.length(), ending));
  } else {
    return false;
  }
}

// Process a given directory
// This routine in mutually recursive with process entry
void ProcessDirectory(string directory)
{
  string dirToOpen = path + directory;
  DIR *dir;
  dir = opendir(dirToOpen.c_str());

  //set the new path for the content of the directory
  path = dirToOpen + "/";

  //  cout << "Process directory: " << dirToOpen.c_str() << endl;

  if(NULL == dir)
    {
      cout << "could not open directory: " << dirToOpen.c_str() << endl;
      return;
    }
    
  struct dirent *entity;
  entity = readdir(dir);

  while(entity != NULL)
    {
      ProcessEntity(entity);
      entity = readdir(dir);
    }

  //we finished with the directory so remove it from the path
  path.resize(path.length() - 1 - directory.length());
  closedir(dir);
}

// Process a given entry in a directory
void ProcessEntity(struct dirent* entity)
{
  //find entity type
  if(entity->d_type == DT_DIR)
    {//it's an directory
     //don't process the  '..' and the '.' directories
      if(entity->d_name[0] == '.')
	{
	  return;
	}

      //it's an directory so process it
      ProcessDirectory(string(entity->d_name));
      return;
    }

  if(entity->d_type == DT_REG)
    {//regular file
      //logFile.open("/tmp/skon.log",ios::out | ios::app);
      //logFile << "File: " << path << entity->d_name << endl;
      //logFile.close();  

      ProcessFile(string(entity->d_name));
      return;
    }

  //there are some other types
  //read here http://linux.die.net/man/3/readdir
  cout << "Not a file or directory: " << entity->d_name << endl;
}

// Process a given file
// Only look at it if it has a ".txt" extension
// Searches the file fot the given word, counting the matches
void ProcessFile(string file)
{
  string fileType = ".txt";
  if (hasEnding(file,fileType)) {
  
  	if (bookNumber < bookLimit) {
  	
	AddBookInfo(file.c_str());//Adds book information to vector
	
	if(CreateBookMap(index, file.c_str()) != 1) {
		cout << "Error adding" << file << "to index" << endl;
	}//Adds a book's words to the map
	
	} else {}
  }
  
}

//Function that will build and then return the array with a book's number, title, and filepath
void AddBookInfo(string file) {
	
	//Appends a bookInfo structure to the vector
	books.push_back(bookInfo());
	
	//Searches for a books title, then adds it to vector
	books[bookNumber].title = FindTitle(path + file);
	//Adds the current book number to vector
	books[bookNumber].bookNum = bookNumber;
	//Adds the path of the book to the vector
	books[bookNumber].bookPath = path + file;
	
	//Adds 1 to bookNumber so that when another book is added, it doesn't overwrite the previous books
	bookNumber++;
	
}

int CreateBookMap(map<string, vector<int> > &indexMap, string filename) {//Function that makes the map. 
	
	int pos = 0;//Starts at beginning of file
	int lineCount;//Used to limit lines in file
	int wordCount;//Number of words in a line
	
	int lineLimit = 110000;//Limits number of lines read in a file
	int wordLimit = 100; //Limits the number of words read in a line
	string word, line;//Creates strings for word and line
	
	ifstream infile(path + filename.c_str());
	
	if(infile.good()) {
		
		lineCount = 0;
		
		while(!infile.eof() && lineCount < lineLimit) {
			
			getline(infile,line);
			//cout << line << endl;
			
			/*Loop that goes through each word in one line*/
			while(line.length() > 0 && wordCount < wordLimit) {
				
				word = getNext(line);
				transform(word.begin(),word.end(),word.begin(),::tolower);
				
				//Check if the word is greater than 3 chars
				if(word.length() > 2) {
				
					ofstream myfile;
					string indexFile = "index/" + word + ".index";
				
					myfile.open(indexFile.c_str(),fstream::in | fstream::out | fstream::app);
						myfile << pos << endl;
					myfile.close();
					
					/*
					cout << "Word:" << word << endl;
					cout << "Line length is " << line.length() << endl;
					cout << "Line Count is " << lineCount << endl;
					cout << "Word Count is " << wordCount << endl;
					*/
					
					wordCount++;
					pos = infile.tellg();
				} else {
					pos = infile.tellg();
				}
			}
			
			wordCount = 0;
			lineCount++;
		}
		
		infile.close();
		cout << "File Closed: " << filename.c_str() << endl;
		return(1);
	
	/*
	if(infile.good()) {//If the file opens, ...
		while (!infile.fail()) {//While NOT at the end of the file, ...
			
			getline(infile, line);//Gets the next line of the code
			
			stringstream lineStream(line);//Create a string stream of the line
			
			while (lineStream >> word) { //Looks at the string of the line, finds the words
				indexMap[word].push_back(pos); //Adds the word and its position to the vector
				
				/*WRITES MAP TO FILE
				
				ofstream myfile;
				string indexFile = "index/" + word + ".index";
				
				myfile.open(indexFile.c_str());
				
					myfile << pos << "\n";
					
				myfile.close();
				
			}
			
			pos = infile.tellg();//Sets the position to the start of the next line, loop continues
		}
		return(1);//If the file does open, returns 1
	} else {
		return(0);//If the file fails to open, returns 0
	}
	*/
	
	} else {
		infile.close();
		return(0);
	}

}

string getNext(string & line) {
  string next;
  size_t start = line.find_first_not_of(delimiters);
  
  if (start != string::npos) {

    size_t end = line.find_first_of(delimiters,start);
    if (end != string::npos) {

      // word with delimiters on both sides
      next = line.substr(start,end-start);
      line.erase(0,end+1);
    } else {
      // word with delimiter only at start
      next = line.substr(start);
      line = "";
    }
  } else {
    // no delimiters found at all
    next = line;
    line = "";
  }
  return next;
}
