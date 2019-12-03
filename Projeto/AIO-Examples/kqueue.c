#include <sys/event.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>   /* for strerror() */
#include <unistd.h>

#include <err.h>
#include <fcntl.h>

int kqueue_example_read(int argc, char *argv[]) {
    char buf[1024];
    int kq;
    struct kevent ev[1];
    struct timespec ts = {5, 0};
    ssize_t n;

    kq = kqueue();
    if (kq == -1)
        err(1, "kqueue");

    EV_SET(ev, STDIN_FILENO, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, ev, 1, NULL, 0, &ts) == -1)
        err(1, "setup kevent");

    for (;;) {
        switch (kevent(kq, NULL, 0, ev, 1, &ts)) {
            case 0:
                printf("timeout expired\n");
                break;
            case -1:
                err(1, "kevent");
                break;
            default:
                printf("data ready (%ld)\n", ev->data);
                n = read(STDIN_FILENO, buf, sizeof buf - 1);
                buf[n] = '\0';
                printf("(%zd) %s\n", n, buf);
        }
    }

    close(kq);
    return EXIT_SUCCESS;
}


/* function prototypes */
void diep(const char *s);

int kqueue_example_date(void) {
    struct kevent change;    /* event we want to monitor */
    struct kevent event;     /* event that was triggered */
    pid_t pid;
    int kq, nev;

    /* create a new kernel event queue */
    if ((kq = kqueue()) == -1)
        diep("kqueue()");

    /* initalise kevent structure */
    EV_SET(&change, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 5000, 0);

    /* loop forever */
    for (;;) {
        nev = kevent(kq, &change, 1, &event, 1, NULL);

        if (nev < 0)
            diep("kevent()");

        else if (nev > 0) {
            if (event.flags & EV_ERROR) {   /* report any error */
                fprintf(stderr, "EV_ERROR: %s\n", strerror(event.data));
                exit(EXIT_FAILURE);
            }

            if ((pid = fork()) < 0)         /* fork error */
                diep("fork()");

            else if (pid == 0)              /* child */
                if (execlp("date", "date", (char *) 0) < 0)
                    diep("execlp()");
        }
    }

    close(kq);
    return EXIT_SUCCESS;
}

void diep(const char *s) {
    perror(s);
    exit(EXIT_FAILURE);
}

