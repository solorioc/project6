#MakeFile to build and deploy SearchBooks
# For CSC3004 Software Development

# Put your user name below:
USER= solorioc

CC= g++

#For Optimization
#CFLAGS= -O2
#For debugging
CFLAGS= -std=c++11

RM= /bin/rm -f

all: searchserver PutCGI PutHTML

searchserver.o: searchserver.cpp 
	$(CC) -c $(CFLAGS) searchserver.cpp

searchserver: searchserver.o
	$(CC) $(CFLAGS) searchserver.o -o searchserver -L/usr/local/lib -lcgicc

testclient.o: testclient.cpp 
	$(CC) -c testclient.cpp 

testclient: testclient.o 
	$(CC) testclient.o -o testclient 

PutCGI: searchserver
	chmod 757 searchserver
	cp searchserver /usr/lib/cgi-bin/$(USER)_searchserver.cgi 

	#echo "Current contents of your cgi-bin directory: "
	#ls -l /usr/lib/cgi-bin/

PutHTML:
	cp web_BookSearch.html /var/www/html/class/softdev/$(USER)
	cp css_BookSearch.css /var/www/html/class/softdev/$(USER)
	cp js_BookSearch.js /var/www/html/class/softdev/$(USER)

	#echo "Current contents of your HTML directory: "
	#ls -l /var/www/html/class/softdev/$(USER)

clean:
	rm -f *.o searchserver testclient
	rm -r index
	mkdir index
