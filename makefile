CC = clang++
CFLAGS = -Wall -std=c++17

main.o: main.cpp renderer.o svo.o
	$(CC) $(CFLAGS) main.cpp -o main.o

renderer.o: svo.o
	$(CC) $(CFLAGS) -c VoxelEngine/Renderer.hpp -o renderer.o

svo.o: ray.o
	$(CC) $(CFLAGS) -c utils/SVO.hpp ray.o -o svo.o

ray.o: vec3.o
	$(CC) $(CFLAGS) -c VoxelEngine/Ray.hpp -o ray.o

vec3.o:
	$(CC) $(CFLAGS) -c utils/Vec3.hpp -o vec3.o

clean:
	rm *.o
	rm -R *.o.dSYM
	rm -R *.dSYM