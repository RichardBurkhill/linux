/**
 * @file tcp_client.cpp
 * @brief A simple TCP client implementation using BSD sockets for Linux.
 *
 * This client connects to a specified server IP and port, sends a message,
 * receives a response, and then closes the connection.
 */

#include <iostream>   // Required for input/output operations (e.g., std::cout, std::cerr)
#include <string>     // Required for string manipulation (e.g., std::string)
#include <cstring>    // Required for C-style string manipulation (e.g., std::strerror, std::memset)
#include <sys/socket.h> // Required for socket programming functions (e.g., socket, connect, send, recv)
#include <arpa/inet.h>  // Required for internet address manipulation (e.g., inet_pton)
#include <unistd.h>   // Required for POSIX operating system API (e.g., close)

// Define constants for the server
#define SERVER_IP "127.0.0.1" /**< @brief The IP address of the server to connect to. */
#define PORT 8080             /**< @brief The port number of the server to connect to. */
#define BUFFER_SIZE 1024      /**< @brief The size of the buffer used for sending and receiving data. */

/**
 * @brief The main function of the TCP client.
 *
 * This function initializes the socket, connects to the server, sends a message,
 * receives a response, and handles any errors that occur during these operations.
 *
 * @return 0 if the client runs successfully, 1 if an error occurs.
 */
int main() {
    int client_socket;                /**< @brief File descriptor for the client socket. */
    struct sockaddr_in server_address; /**< @brief Structure to hold the server's address information. */
    char buffer[BUFFER_SIZE] = {0};   /**< @brief Buffer to store data sent to and received from the server. */
    const char* message = "Hello from TCP client!"; /**< @brief The message to send to the server. */

    // 1. Create socket
    // AF_INET: IPv4 Internet protocols
    // SOCK_STREAM: Provides sequenced, reliable, two-way, connection-based byte streams (TCP)
    // 0: Protocol (IP)
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error: Could not create socket. " << std::strerror(errno) << std::endl; /**< @brief Error message if socket creation fails. */
        return 1;
    }
    std::cout << "Socket created successfully." << std::endl;

    // 2. Prepare the server address structure
    std::memset(&server_address, 0, sizeof(server_address)); // Clear the structure
    server_address.sin_family = AF_INET;                     // Set address family to IPv4
    server_address.sin_port = htons(PORT);                   // Convert port number to network byte order

    // Convert IP address from string to binary form
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        std::cerr << "Error: Invalid address/ Address not supported. " << std::strerror(errno) << std::endl; /**< @brief Error message if IP address conversion fails. */
        close(client_socket); // Close the socket before exiting
        return 1;
    }

    // 3. Connect to the server
    // Cast sockaddr_in to sockaddr* as connect expects a generic socket address structure
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error: Connection failed. " << std::strerror(errno) << std::endl; /**< @brief Error message if connection to server fails. */
        close(client_socket); // Close the socket before exiting
        return 1;
    }
    std::cout << "Connected to server " << SERVER_IP << ":" << PORT << std::endl;

    // 4. Send data to the server
    // Send the message to the connected socket
    ssize_t bytes_sent = send(client_socket, message, std::strlen(message), 0);
    if (bytes_sent == -1) {
        std::cerr << "Error: Send failed. " << std::strerror(errno) << std::endl; /**< @brief Error message if sending data fails. */
        close(client_socket); // Close the socket before exiting
        return 1;
    }
    std::cout << "Message sent: " << message << " (" << bytes_sent << " bytes)" << std::endl;

    // 5. Receive data from the server
    // Receive data into the buffer
    ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received == -1) {
        std::cerr << "Error: Receive failed. " << std::strerror(errno) << std::endl; /**< @brief Error message if receiving data fails. */
        close(client_socket); // Close the socket before exiting
        return 1;
    } else if (bytes_received == 0) {
        std::cout << "Server closed the connection." << std::endl; /**< @brief Message if server closes connection gracefully. */
    } else {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        std::cout << "Message received: " << buffer << " (" << bytes_received << " bytes)" << std::endl;
    }

    // 6. Close the socket
    close(client_socket); /**< @brief Close the client socket to release resources. */
    std::cout << "Socket closed." << std::endl;

    return 0; // Indicate successful execution
}
