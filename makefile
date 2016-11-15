SrcDir=src 
ObjDir=src

Src=$(foreach dir, $(SrcDir), $(wildcard $(dir)/*.cpp)) main.cpp
FileSrc=$(notdir $(Src))

Obj=$(patsubst %.cpp, $(ObjDir)/%.obj, $(FileSrc))


Inc=$(foreach dir, $(SrcDir), -I $(dir)/ ) 
Lib=-L/usr/lib64 -lpthread -lz

CC=g++



whatshttp=./bin/whatshttp




all: $(whatshttp) 
	@echo make all ok! now you can install it 



CCFlags= -g -w -O0 $(Inc) -std=c++11
$(whatshttp):$(Obj) 
	@echo make lobby
	$(CC) -g -w  -o $@  $^ $(Lib) 

	
$(ObjDir)/%.obj:$(ObjDir)/%.cpp
	@echo make src
	$(CC) $(CCFlags) -c $< -o $@

$(ObjDir)/main.obj:main.cpp
	@echo make src
	$(CC) $(CCFlags) -c $< -o $@

	

clean:
	rm -f $(SrcDir)/*.obj
	rm -f $(whatshttp) 
	@echo clean ok! 

cleanobj:
	rm -f $(ObjDir)/*.obj
	@echo clean ok!

