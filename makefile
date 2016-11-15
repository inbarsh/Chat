FLAGS = -c -g -Wall -Werror -pedantic
OBJC = logger_pub.o HashMap.o DLlist.o ADTErr.o server_user.o server_rooms.o ChatCreator.o MultiCast_Addresses.o client_actions.o client_chating.o

build: client server chatWritingWindow chatReadingWindow

chatWritingWindow: $(OBJC) chatWritingWindow.o
	gcc -o chatWritingWindow $(OBJC) chatWritingWindow.o
	
chatReadingWindow: $(OBJC) chatReadingWindow.o
	gcc -o chatReadingWindow $(OBJC) chatReadingWindow.o
	
client: $(OBJC) client.o
	gcc -o client $(OBJC) client.o

server: $(OBJC) server.o
	gcc -o server $(OBJC) server.o
	
-include depends

%.o : %.c
	gcc $(FLAGS)  $<
	gcc $(FLAGS) -MM  $< >>depends

clean:
	rm -f *.o
	rm -f depends
	rm -f *.out
	rm -f *.txt

