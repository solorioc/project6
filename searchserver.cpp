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
//#include "fifo.h"
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


/*Global Variables*/
string path = "/home/skon/books";
vector<bookInfo> books;
int bookNumber = 0;


/*Function Declarations*/
void PrintVec(vector<bookInfo> vec);
string FindTitle(string filename);

//For navigating the directory
void ProcessDirectory(string directory);
void ProcessFile(string file);
void ProcessEntity(struct dirent* entity);
bool hasEnding (string const &fullString, string const &ending);

int main() {
	
	string directory = "";
	
	try {
		ProcessDirectory(directory);
		PrintVec(books);

	} catch (int e) {
		cout << "Shit's fucked, dawg \n" << endl;
	}
	
}

/*Function Definitions*/

//Function that will build and then return the array with a book's number, title, and filepath

//Function that will find the title of a book and return that string
string FindTitle(string filename) {

	string line = "";
	int lineNum = 0;
	int lineMax = 50;
	
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
  
	books.push_back(bookInfo());
		
	books[bookNumber].title = FindTitle(path + file.c_str());
	books[bookNumber].bookNum = bookNumber;
	books[bookNumber].bookPath = path + file.c_str();
	
  	bookNumber++;
  }
  
}
