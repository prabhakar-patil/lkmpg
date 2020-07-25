#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "chardev.h"

#define BUF_LEN 1024

int main(int argc, char *argv[])
{
	int fd, rb, wb, ret, n;
	char buffer[BUF_LEN];

	fd = open(DEVICE_NAME, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "%s: open: %s: %s\n", argv[0], DEVICE_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}

/*	memset(buffer, 0, BUF_LEN);
	rb = read(fd, buffer, BUF_LEN);
	if (rb < 0) {
		fprintf(stderr, "%s: read: %s: %s\n", argv[0], DEVICE_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "%d bytes read from driver\n", rb);
	fprintf(stdout, "%s\n", buffer);
*/

	/* call ioctls */
	memset(buffer, 0, BUF_LEN);
	ret = ioctl(fd, IOCTL_GET_MSG, buffer);
	if (ret < 0) {
		fprintf(stderr, "%s: ioctl: %s: %s\n", argv[0], DEVICE_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "message from driver: %s\n", buffer);

	memset(buffer, 0, BUF_LEN);
	sprintf(buffer, "India is my country\n");
	ret = ioctl(fd, IOCTL_SET_MSG, buffer);
	if (ret < 0) {
		fprintf(stderr, "%s: ioctl: %s: %s\n", argv[0], DEVICE_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}

	memset(buffer, 0, BUF_LEN);
	n = 0;
	ret = ioctl(fd, IOCTL_GET_NTH_BYTE, 0);
	if (ret < 0) {
		fprintf(stderr, "%s: ioctl: %s: %s\n", argv[0], DEVICE_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("%d'th character from driver message: %c\n", n, ret);

	close(fd);

	exit(EXIT_SUCCESS);
}
