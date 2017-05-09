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

all: indexbuilder searchclient PutCGI PutHTML
#all: PutHTML
indexbuilder.o: indexbuilder.cpp 
	$(CC) -c $(CFLAGS) indexbuilder.cpp

indexbuilder: indexbuilder.o
	$(CC) $(CFLAGS) indexbuilder.o -o indexbuilder -L/usr/local/lib -lcgicc

searchclient.o: searchclient.cpp 
	$(CC) -c searchclient.cpp 

searchclient: searchclient.o 
	$(CC) searchclient.o -o searchclient -L/usr/local/lib -lcgicc 

PutCGI: indexbuilder
	chmod 757 indexbuilder
	cp indexbuilder /usr/lib/cgi-bin/$(USER)_indexbuilder.cgi 

	#echo "Current contents of your cgi-bin directory: "
	#ls -l /usr/lib/cgi-bin/

PutHTML:
	cp web_BookSearch.html /var/www/html/class/softdev/$(USER)
	cp css_BookSearch.css /var/www/html/class/softdev/$(USER)
	cp js_BookSearch.js /var/www/html/class/softdev/$(USER)

	#echo "Current contents of your HTML directory: "
	#ls -l /var/www/html/class/softdev/$(USER)

clean:
	rm -f *.o indexbuilder searchclient book_catalog.txt
	rm -r index
	mkdir index
