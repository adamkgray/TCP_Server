# TCP Server

A minimal TCP server and client wrapper written in C

## Server

The server works like this:
    * Open socket
    * Bind socket to local interface
    * Accept new connection
    * Read from connection
    * Write to connection
    * Close connection
    * Accept new connection ...

#### Sockets

To open a socket, we call `socket()`, which returns a file descriptor that can be used to interface with the socket. A socket is a generalized interprocess communication channel. Like a pipe, a socket is represented as a file descriptor. Unlike pipes sockets support communication between unrelated processes, and even between processes running on different machines that communicate over a network.

* `socket(int domain, int type, int protocol)`
    - `domain` is always set to `AF_INET`
    - `type` is always set to `SOCK_STREAM`, which a constant indicating the type of socket (TCP), as opposed to `SOCK_DGRAM` (UDP)
    - `protocol` specifies a particular protocol to be used with the socket. Normally only a single protocol exists to support a particular socket type within a given protocol family, in which case protocol can be specified as 0

#### Binding

When a process wants to receive new incoming packets or connections, it should bind a socket to a local interface address using `bind()`. This assigns a name to an unnamed socket. When a socket is created with `socket()` it exists in a name space (address family) but has no name assigned. `bind()` requests that address be assigned to the socket.

* `bind(int socket, const struct sockaddr *address, socklen_t address_len)`
    - `socket` is the file descriptor value returned by calling `socket()`
    - `address` is a pointer to a sockaddr struct
    - `address_len` is the `sizeof` the `address`;

But what about these `sockaddr` structs?

All pointers to socket address structures are often cast to pointers to `struct sockaddr` before use in various functions and system calls:

```
struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
};
```


But before it's cast, we we're really making are these (for IPv4):

```
struct sockaddr_in {
	sa_family_t    sin_family; /* address family, which is always AF_INET */
	in_port_t      sin_port;   /* port in network byte order */
	struct in_addr sin_addr;   /* internet address */
};
```

Aaaaaand that last member, `sin_addr` is another struct that looks like this:

```
struct in_addr {
	uint32_t       s_addr;     /* address in network byte order */
};
```

In practice, you only need to do this:
    * Create a new `struct sockaddr_in`
    * Set `sin_family` to `AF_INET`
    * Set `sin_port` to `htons(port)` (This is where you pass in the port to listen on)
    * Set `sin_addr.s_addr` to `htonl(INADDR_ANY)`

`INADDR_ANY` is used when you don't need to bind a socket to a specific IP. When you use this value as the address when calling bind() , the socket accepts connections to all the IPs of the machine.

Yet what are these `htonl()` and `htons()` functions?

According to the man pages, these routines convert 16 and 32 bit quantities between 'network byte order' and 'host byte order' (network byte order is big endian, or most significant byte first). This, however, doesn't mean much to me at this point in my life, so I am content trusting that it just works and moving on.




## TCP Client

