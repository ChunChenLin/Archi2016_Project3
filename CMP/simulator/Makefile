CC = g++ -O2 -w -std=c++11
SRCS = ./*.cpp
OBS = ./*.o

CMP: SRC
	$(CC) -o $@ $(OBS)

SRC: $(SRCS)
	$(CC) -c $(SRCS)

clean: $(OBS)
	rm -f $(OBS)  
