// Note: Link with -lrt

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <string>
#include <iostream>

volatile sig_atomic_t active = 1;

void sigint_handler(int sig)
{
    active = 0;
}

int main(int argc, char **argv)
{
    int oflags = O_RDWR | O_CREAT;
    off_t mem_length = sizeof(int);
    std::string name = "/testing_hello";

    int fd = shm_open(name.c_str(), oflags, S_IRUSR | S_IWUSR );

    ftruncate(fd, mem_length);

    std::cout << "Shared Mem Descriptor: fd=" << fd << std::endl;

    if ( fd < 0 )
    {
        perror("shm_open");
        exit(1);
    }

    void* ptr = mmap(NULL, mem_length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if ( ptr == MAP_FAILED )
    {
        perror("mmap");
        exit(1);
    }

    std::cout << "Shared Memory Address: " << ptr
              << " [0 .. " << mem_length-1 << " ]" << std::endl;
    std::cout << "Shared Memory Path: /dev/shm" << name << std::endl;

    signal(SIGINT, sigint_handler);

    int* data;
    data = static_cast<int*>(ptr);
    int itr = 0;
    while ( active )
    {
        *data = itr;
        std::cout << "Memory contains: " << itr << std::endl;
        itr++;
        sleep(1);
    }

    close(fd);
    munmap(ptr, mem_length);
    shm_unlink(name.c_str());
    std::cout << std::endl << "Closed shared memory" << std::endl;
    exit(0);
}
