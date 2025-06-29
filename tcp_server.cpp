/**
 * @file tcp_server.cpp
 * @brief A simple TCP server implementation using BSD sockets for Linux.
 *
 * This server listens for incoming connections on a specified port,
 * receives data from a client, sends a response, and then closes the connection.
 */

#include <iostream>   // Required for input/output operations (e.g., std::cout, std::cerr)
#include <string>     // Required for string manipulation (e.g., std::string)
#include <cstring>    // Required for C-style string manipulation (e.g., std::strerror, std::memset)
#include <sys/socket.h> // Required for socket programming functions (e.g., socket, bind, listen, accept, send, recv)
#include <arpa/inet.h>  // Required for internet address manipulation (e.g., inet_ntoa, htons)
#include <unistd.h>   // Required for POSIX operating system API (e.g., close)

// Define constants for the server
#define PORT 8080             /**< @brief The port number the server will listen on. */
#define BUFFER_SIZE 1024      /**< @brief The size of the buffer used for sending and receiving data. */
#define MAX_PENDING_CONNECTIONS 5 /**< @brief The maximum number of pending connections in the queue. */

/**
 * @brief The main function of the TCP server.
 *
 * This function initializes the socket, binds it to an address, listens for connections,
 * accepts a client, receives and sends data, and handles any errors that occur.
 *
 * @return 0 if the server runs successfully, 1 if an error occurs.
 */
int main() {
    int server_socket;                /**< @brief File descriptor for the server listening socket. */
    int client_socket;                /**< @brief File descriptor for the accepted client socket. */
    struct sockaddr_in server_address; /**< @brief Structure to hold the server's address information. */
    struct sockaddr_in client_address; /**< @brief Structure to hold the client's address information. */
    socklen_t client_address_len = sizeof(client_address); /**< @brief Length of the client address structure. */
    char buffer[BUFFER_SIZE] = {0};   /**< @brief Buffer to store data sent to and received from the client. */
    const char* response_message = "Hello from TCP server!"; /**< @brief The message to send back to the client. */

    // 1. Create socket
    // AF_INET: IPv4 Internet protocols
    // SOCK_STREAM: Provides sequenced, reliable, two-way, connection-based byte streams (TCP)
    // 0: Protocol (IP)
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error: Could not create socket. " << std::strerror(errno) << std::endl; /**< @brief Error message if socket creation fails. */
        return 1;
    }
    std::cout << "Server socket created successfully." << std::endl;

    // Optional: Set socket options to reuse address (helps with quick restarts)
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Warning: setsockopt SO_REUSEADDR failed. " << std::strerror(errno) << std::endl; /**< @brief Warning message if SO_REUSEADDR fails. */
    }

    // 2. Prepare the server address structure
    std::memset(&server_address, 0, sizeof(server_address)); // Clear the structure
    server_address.sin_family = AF_INET;                     // Set address family to IPv4
    server_address.sin_addr.s_addr = INADDR_ANY;             // Listen on any available network interface
    server_address.sin_port = htons(PORT);                   // Convert port number to network byte order

    // 3. Bind the socket to the specified IP and port
    // Cast sockaddr_in to sockaddr* as bind expects a generic socket address structure
    if (bind(server_socket, (const struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error: Bind failed. " << std::strerror(errno) << std::endl; /**< @brief Error message if socket binding fails. */
        close(server_socket); // Close the server socket before exiting
        return 1;
    }
    std::cout << "Socket bound to port " << PORT << std::endl;

    // 4. Listen for incoming connections
    // MAX_PENDING_CONNECTIONS defines the maximum length of the queue of pending connections
    if (listen(server_socket, MAX_PENDING_CONNECTIONS) == -1) {
        std::cerr << "Error: Listen failed. " << std::strerror(errno) << std::endl; /**< @brief Error message if listen fails. */
        close(server_socket); // Close the server socket before exiting
        return 1;
    }
    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    // 5. Accept a client connection
    // This call blocks until a client connects
    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
    if (client_socket == -1) {
        std::cerr << "Error: Accept failed. " << std::strerror(errno) << std::endl; /**< @brief Error message if accepting client connection fails. */
        close(server_socket); // Close the server socket before exiting
        return 1;
    }
    std::cout << "Accepted connection from " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << std::endl; /**< @brief Displays client IP and port. */

    // 6. Receive data from the client
    // Receive data into the buffer
    ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received == -1) {
        std::cerr << "Error: Receive failed. " << std::strerror(errno) << std::endl; /**< @brief Error message if receiving data fails. */
    } else if (bytes_received == 0) {
        std::cout << "Client disconnected." << std::endl; /**< @brief Message if client disconnects gracefully. */
    } else {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        std::cout << "Message received from client: " << buffer << " (" << bytes_received << " bytes)" << std::endl;

        // 7. Send response back to the client
        // Send the response message to the connected client
        ssize_t bytes_sent = send(client_socket, response_message, std::strlen(response_message), 0);
        if (bytes_sent == -1) {
            std::cerr << "Error: Send failed. " << std::strerror(errno) << std::endl; /**< @brief Error message if sending data fails. */
        } else {
            std::cout << "Response sent to client: " << response_message << " (" << bytes_sent << " bytes)" << std::endl;
        }
    }

    // 8. Close the client socket
    close(client_socket); /**< @brief Close the client socket to release resources. */
    std::cout << "Client socket closed." << std::endl;

    // 9. Close the server listening socket
    close(server_socket); /**< @brief Close the server listening socket to release resources. */
    std::cout << "Server socket closed." << std::endl;

    return 0; // Indicate successful execution
}
