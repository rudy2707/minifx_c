CC=gcc
CLIBS=-std=c99 -Wall
OBJS=main.o
OUT=main


all: $(OUT)

$(OUT): $(OBJS)
	$(CC) $(CLIBS) -o $@ $^ $(LIBS)
#$@ name of the entry ($(OUT)) $^ each dependecie listed in $(OBJS)

# create the .o file with their .c homologue
%.o: %.c
	gcc $(CLIBS) -c $< $(LIBS)

#$< first depedencie listed (%.c)

#dependencies
 
# delete all temporary files (.o)
clean:
		rm -rf *.o
 
# delete all non-source files
mrproper: clean
		rm -rf $(OUT)
