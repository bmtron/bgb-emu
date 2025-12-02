#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "cpu/cpu.h"
#include "memory/memorymap.h"

int loadTestRom(MemMap* mmap);
int readCartHeader(MemMap* mmap);

int main(int argc, char** argv) {
        MemMap* mmap = malloc(sizeof(MemMap));
        Cpu* cpu = malloc(sizeof(Cpu));
        printf("Size: %ld\n", sizeof(*mmap));
        printf("ROM Size: %d\n", ROM_SIZE);

        SDL_Renderer* renderer = NULL;
        SDL_Window* window = NULL;

        initSDL(&window, &renderer);
        int loadResult = loadTestRom(mmap);
        if (loadResult != 1) {
                exit(1);
        }

        int readres = readCartHeader(mmap);

        while (1) {
                prepareScene(renderer);

                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                        switch (event.type) {
                                case SDL_QUIT:
                                        exit(0);
                                        break;
                                default:
                                        break;
                        }
                }
                presentScene(renderer);
                SDL_Delay(16);
        }
        cleanup(window, renderer);
        free(mmap);
        free(cpu);

        return 0;
}
int readCartHeader(MemMap* mmap) {
        int cartHeaderSize = CART_HEADER_END - CART_HEADER_START;
        printf("HeaderSize: %d\n", cartHeaderSize);
        char buf[cartHeaderSize];
        memcpy(buf, mmap->rom00 + CART_HEADER_START, cartHeaderSize);

        for (int i = 0; i < cartHeaderSize; i++) {
                printf("%c", buf[i]);
        }
        printf("\n");

        return 0;
}
int loadTestRom(MemMap* mmap) {
        int gbfile = open("/home/bmtron/projects/c-misc/bgb-emu/PokemonBlue.gb",
                          O_RDWR, S_IRWXU);
        if (gbfile == -1) {
                printf("Error opening file\n");
                perror("open");
                return gbfile;
        }

        int readSuccess = read(gbfile, mmap->rom00, ROM_SIZE);
        printf("Read bytes: %d\n", readSuccess);
        if (readSuccess < 0) {
                printf("Error reading file\n");
                perror("read");
                return readSuccess;
        }
        close(gbfile);

        return 1;
}
