#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_SIZE 1024

int main(int argc, char *argv[])
{
	int fd, i;
	size_t rb;
	char buffer[MAX_SIZE];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file-name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		if (errno == EAGAIN)
			fprintf(stdout, "open would block: %s\n", strerror(errno));
		else
			fprintf(stderr, "open: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	do {
		rb = read(fd, buffer, MAX_SIZE);
		if (rb < 0) {
			if (errno == EAGAIN)
				fprintf(stdout, "read: Normally I'd block, but you told me not to: %s\n", strerror(errno));
			else
				fprintf(stderr, "open: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (rb > 0) {
			for (i = 0; i < rb; i++)
				putchar(buffer[i]);
		}
	} while (rb > 0);

	exit(EXIT_SUCCESS);
}
