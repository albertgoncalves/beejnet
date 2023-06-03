#include "prelude.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <sys/wait.h>

// NOTE: Usage `$ telnet $(hostname) 3490`.

#define PORT           "3490"
#define LISTEN_BACKLOG 10
#define MESSAGE        "Hello, world!\n"

typedef struct sockaddr         SockAddr;
typedef struct sockaddr_in      SockAddrIn4;
typedef struct sockaddr_in6     SockAddrIn6;
typedef struct addrinfo         AddrInfo;
typedef struct sigaction        SigAction;
typedef struct sockaddr_storage SockAddrStorage;
typedef socklen_t               SockLen;

static void sigchild_handler(i32 _) {
    (void)_;
    const i32 saved_errno = errno;
    while (0 < waitpid(-1, NULL, WNOHANG)) {
    }
    errno = saved_errno;
}

static void* get_address(SockAddr* address) {
    if (address->sa_family == AF_INET) {
        return &((SockAddrIn4*)address)->sin_addr;
    }
    EXIT_IF(address->sa_family != AF_INET6);
    return &((SockAddrIn6*)address)->sin6_addr;
}

i32 main(void) {
    i32 socket_fd = -1;
    {
        AddrInfo* server_info;
        {
            AddrInfo hints = {0};
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_flags = AI_PASSIVE;
            EXIT_IF(getaddrinfo(NULL, PORT, &hints, &server_info));
        }

        for (AddrInfo* current = server_info; current;
             current = current->ai_next) {
            socket_fd = socket(current->ai_family,
                               current->ai_socktype,
                               current->ai_protocol);
            if (socket_fd == -1) {
                continue;
            }
            {
                const i32 yes = 1;
                EXIT_IF(setsockopt(socket_fd,
                                   SOL_SOCKET,
                                   SO_REUSEADDR,
                                   &yes,
                                   sizeof(i32)) == -1);
            }
            if (bind(socket_fd, current->ai_addr, current->ai_addrlen) == -1) {
                close(socket_fd);
                continue;
            }
            break;
        }
        freeaddrinfo(server_info);
    }

    EXIT_IF(socket_fd == -1);
    EXIT_IF(listen(socket_fd, LISTEN_BACKLOG) == -1);

    {
        SigAction action = {0};
        action.sa_handler = sigchild_handler;
        sigemptyset(&action.sa_mask);
        action.sa_flags = SA_RESTART;
        EXIT_IF(sigaction(SIGCHLD, &action, NULL) == -1);
    }

    for (;;) {
        SockAddrStorage client_address;
        SockLen         sin_size = sizeof client_address;
        const i32       new_fd =
            accept(socket_fd, (SockAddr*)&client_address, &sin_size);
        if (new_fd == -1) {
            continue;
        }
        {
            char buffer[INET6_ADDRSTRLEN];
            inet_ntop(client_address.ss_family,
                      get_address((SockAddr*)&client_address),
                      buffer,
                      sizeof buffer);
            printf(" > `%s`\n", buffer);
        }
        if (!fork()) {
            close(socket_fd);
            if (send(new_fd, MESSAGE, (sizeof MESSAGE) - 1, 0) == -1) {
                // ...
            }
            close(new_fd);
            return OK;
        }
        close(new_fd);
    }

    return OK;
}
