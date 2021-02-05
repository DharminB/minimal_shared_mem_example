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
	int oflags = O_RDONLY;
    std::string name = "/testing_hello";

    int fd = shm_open(name.c_str(), oflags, S_IRUSR );

    std::cout << "Shared Mem Descriptor: fd=" << fd << std::endl;

    if ( fd < 0 )
    {
        perror("shm_open");
        exit(1);
    }

	struct stat sb;

	fstat(fd, &sb);
	off_t mem_length = sb.st_size ;

	void* ptr = mmap(NULL, mem_length, PROT_READ, MAP_SHARED, fd, 0);

    if ( ptr == MAP_FAILED )
    {
        perror("mmap");
        exit(1);
    }

    std::cout << "Shared Memory Address: " << ptr
              << " [0 .. " << mem_length-1 << " ]" << std::endl;
    std::cout << "Shared Memory Path: /dev/shm" << name << std::endl;

    signal(SIGINT, sigint_handler);

    u_char* data;
    data = static_cast<u_char*>(ptr);

	// hexdump max first 100 bytes
    int buf_size = ( mem_length < 100 ) ? mem_length : 100;
    std::cout << "Printing " << buf_size << " Bytes" << std::endl;
    while ( active )
    {
        for ( int i = 0; i < buf_size; i++ )
        {
            fprintf(stdout, "%02X ", data[i]);
        }
        std::cout << std::endl;
        sleep(1);
    }

	close(fd);
    munmap(ptr, mem_length);
	exit(0);
}
