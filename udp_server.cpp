/**
 * @file udp_server.cpp
 * @brief A simple UDP server implementation using BSD sockets for Linux.
 *
 * This server listens for incoming UDP packets on a specified port,
 * receives data from a client, sends a response back to the client,
 * and continues to listen for more packets.
 */

#include <iostream>   // Required for input/output operations (e.g., std::cout, std::cerr)
#include <string>     // Required for string manipulation (e.g., std::string)
#include <cstring>    // Required for C-style string manipulation (e.g., std::strerror, std::memset)
#include <sys/socket.h> // Required for socket programming functions (e.g., socket, bind, sendto, recvfrom)
#include <arpa/inet.h>  // Required for internet address manipulation (e.g., inet_ntoa, htons)
#include <unistd.h>   // Required for POSIX operating system API (e.g., close)

// Define constants for the server
#define PORT 8080             /**< @brief The port number the server will listen on. */
#define BUFFER_SIZE 1024      /**< @brief The size of the buffer used for sending and receiving data. */

/**
 * @brief The main function of the UDP server.
 *
 * This function initializes the socket, binds it to an address, listens for packets,
 * receives data from a client, sends a response, and handles any errors that occur.
 *
 * @return 0 if the server runs successfully, 1 if an error occurs.
 */
int main() {
    int server_socket;                /**< @brief File descriptor for the server socket. */
    struct sockaddr_in server_address; /**< @brief Structure to hold the server's address information. */
    struct sockaddr_in client_address; /**< @brief Structure to hold the client's address information. */
    socklen_t client_address_len = sizeof(client_address); /**< @brief Length of the client address structure. */
    char buffer[BUFFER_SIZE] = {0};   /**< @brief Buffer to store data sent to and received from the client. */
    const char* response_message = "Hello from UDP server!"; /**< @brief The message to send back to the client. */

    // 1. Create socket
    // AF_INET: IPv4 Internet protocols
    // SOCK_DGRAM: Provides unreliable, connectionless datagrams (UDP)
    // 0: Protocol (IP)
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
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

    std::cout << "UDP Server listening on port " << PORT << "..." << std::endl;

    // Loop indefinitely to receive and respond to packets
    while (true) {
        // 4. Receive data from a client
        // recvfrom is used for connectionless sockets (UDP)
        ssize_t bytes_received = recvfrom(server_socket, buffer, BUFFER_SIZE - 1, 0,
                                          (struct sockaddr*)&client_address, &client_address_len);
        if (bytes_received == -1) {
            std::cerr << "Error: Recvfrom failed. " << std::strerror(errno) << std::endl; /**< @brief Error message if receiving data fails. */
            continue; // Continue to next iteration to keep server running
        } else if (bytes_received == 0) {
            std::cout << "Received empty datagram." << std::endl; /**< @brief Message if an empty datagram is received. */
            continue;
        } else {
            buffer[bytes_received] = '\0'; // Null-terminate the received data
            std::cout << "Received from " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port)
                      << ": " << buffer << " (" << bytes_received << " bytes)" << std::endl; /**< @brief Displays client IP, port, and received message. */

            // 5. Send response back to the client
            // sendto is used for connectionless sockets (UDP)
            ssize_t bytes_sent = sendto(server_socket, response_message, std::strlen(response_message), 0,
                                        (const struct sockaddr*)&client_address, client_address_len);
            if (bytes_sent == -1) {
                std::cerr << "Error: Sendto failed. " << std::strerror(errno) << std::endl; /**< @brief Error message if sending data fails. */
            } else {
                std::cout << "Response sent to " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port)
                          << ": " << response_message << " (" << bytes_sent << " bytes)" << std::endl; /**< @brief Displays client IP, port, and sent response. */
            }
        }
    }

    // 6. Close the socket (This part will not be reached in the infinite loop)
    close(server_socket); /**< @brief Close the server socket to release resources. */
    std::cout << "Server socket closed." << std::endl;

    return 0; // Indicate successful execution
}
