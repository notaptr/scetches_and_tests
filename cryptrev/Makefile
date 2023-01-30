all:
	@echo "\n"
	@echo "------------ Using ------------"
	@echo "Build for linux:                 make linux"
	@echo "Build for Windows with mingw:    make win"
	@echo "\n"
linux:
	g++ -O2 -g0 -s -pthread -lpthread -o hash main.cpp
	@echo "\n"
	@echo "ok! for use: ./hash [number]"
	@echo "default [number] is 130"
	@echo "for good load on fast systems, 1000-5000 is more reasonable"
	@echo "\n"
win:
	x86_64-w64-mingw32-c++-posix -O2 -g0 -s -o hash.exe -pthread -lpthread main.cpp
	@echo "\n"
	@echo "ok! for use: hash.exe [number]"
	@echo "default [number] is 130"
	@echo "for good load on fast systems, 1000-5000 is more reasonable"
	@echo "\n"
	
