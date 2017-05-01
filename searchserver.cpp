/***************************************************************************
* searchserver.cpp - Project Guttenburg Search Engine
*
* copyright : (C) 2017 by Christian Solorio
*
* This program is a command line interface for the project guttenburg search
* engine we will be building. It makes a map of all of the words in all of
*
*
***************************************************************************/

//test

#include <sys/types.h>
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
//#include "fifo.h"

using namespace std;

/*Global Variables*/
string filepath = "/home/skon/books/";


/*Function Declarations*/
vector<string> BuildBookArray(vector<string> bookInfo);
void PrintVec(vector<string> vec);
string FindTitle(string filename, vector<string> bookInfo);

//For navigating the directory
void ProcessDirectory(string directory,string word);
void ProcessFile(string file,string word);
void ProcessEntity(struct dirent* entity,string word);
bool hasEnding (string const &fullString, string const &ending);


int main() {
	vector<string> bookInfo;

	PrintVec(BuildBookArray(filepath,bookInfo));
	
}

/*Function Definitions*/

//Function that will build and then return the array with a book's number, title, and filepath
/*
vector<string> BuildBookArray(vector<string> bookInfo) {
	
	string filename = filepath +  "/1/0/0/0/10001/10001/10001.txt";
	
	bookInfo.push_back(FindTitle(filename, bookInfo));
	
	return(bookInfo);
}*/

//Function that will find the title of a book and return that string
string FindTitle(string filename, vector<string> bookInfo) {
	string line = "";
	ifstream infile(filename.c_str());
	
	while (line.find("Title:") == string::npos) {
		getline(infile,line);
	}
	
	line.erase(0,7);//Erases "Title: " from the string
	
	return(line);
}

//Prints each element of a vector, mainly used for testing purposes
void PrintVec(vector<string> vec) {
	vector<string>::iterator it;
	
	for(it = vec.begin(); it < vec.end(); it++) {
		cout << *it << endl;
	}
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
void ProcessDirectory(string directory, string word)
{
  string dirToOpen = path + directory;
  DIR *dir;
  dir = opendir(dirToOpen.c_str());

  //set the new path for the content of the directory
  path = dirToOpen + "/";

  //  cout << "Process directory: " << dirToOpen.c_str() << endl;

  if(NULL == dir)
    {
      cout << "could not open directory: " << dirToOpen.c_str() <<\
	endl;
      return;
    }
  struct dirent *entity;
  entity = readdir(dir);

  while(entity != NULL)
    {
      ProcessEntity(entity,word);
      entity = readdir(dir);
    }

  //we finished with the directory so remove it from the path
  path.resize(path.length() - 1 - directory.length());
  closedir(dir);
}

// Process a given entry in a directory
void ProcessEntity(struct dirent* entity, string word)
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
      ProcessDirectory(string(entity->d_name),word);
      return;
    }

  if(entity->d_type == DT_REG)
    {//regular file
      //logFile.open("/tmp/skon.log",ios::out | ios::app);
      //logFile << "File: " << path << entity->d_name << endl;
      //logFile.close();  

      ProcessFile(string(entity->d_name), word);
      return;
    }

  //there are some other types
  //read here http://linux.die.net/man/3/readdir
  cout << "Not a file or directory: " << entity->d_name << endl;
}

// Process a given file
// Only look at it if it has a ".txt" extension
// Searches the file fot the given word, counting the matches
void ProcessFile(string file, string word)
{
  string fileType = ".txt";
  if (hasEnding(file,fileType)) {
    fileCount++;
    if (word.length()>0) {
      int matches = stringMatchCount(file,word);
      if (matches > 0) {
	fileMatchCount++;
	matchCount += matches;

	cout << "<h4>Matches: " << matches << "</h4>";
      }
    }
  }
  // Get the current time
  gettimeofday(&current, 0);
  
  int delta = current.tv_sec - start.tv_sec;
  if (delta >= max_Time) {
    throw (delta);
  }
  //if you want to do something with the file add your code here
}

// get the next "word" from line, removing it from line,
// and returning the word
string getNext(string & line) {
  string next;
  //cout << "$" << line.length();
  size_t start = line.find_first_not_of(delimiters);
  if (start != string::npos) {
    //cout << ":" << start;
    size_t end = line.find_first_of(delimiters,start);
    if (end != string::npos) {
      //cout << "#" << end;
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




