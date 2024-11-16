#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "client_service.h" // Bao gồm file header của client_service

// Hàm kiểm tra gửi dữ liệu
void test_sendData() {
    int fd_pipe_to_service[2];
    if (pipe(fd_pipe_to_service) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int number1 = 10;
    int number2 = 20;

    // Gọi hàm sendData
    sendData(fd_pipe_to_service[1], number1, number2);
    
    // Đọc dữ liệu từ ống (pipe) để kiểm tra
    int received1, received2;
    read(fd_pipe_to_service[0], &received1, sizeof(received1));
    read(fd_pipe_to_service[0], &received2, sizeof(received2));

    if (received1 == number1 && received2 == number2) {
        printf("Test sendData: PASSED\n");
    } else {
        printf("Test sendData: FAILED\n");
    }

    close(fd_pipe_to_service[0]);
    close(fd_pipe_to_service[1]);
}

// Hàm kiểm tra nhận kết quả
void test_receiveResult() {
    int fd_pipe_from_service[2];
    if (pipe(fd_pipe_from_service) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int result = 30; // Kết quả giả định
    write(fd_pipe_from_service[1], &result, sizeof(result));

    char prefix[] = "Result: ";
    // Gọi hàm receiveResult
    receiveResult(fd_pipe_from_service[0], prefix);

    // Đọc kết quả từ stdout (có thể cần điều chỉnh để kiểm tra chính xác)
    // Thực hiện kiểm tra xem kết quả có được in ra đúng hay không

    close(fd_pipe_from_service[0]);
    close(fd_pipe_from_service[1]);
}

int main() {
    printf("Running tests for client_service...\n");

    // Gọi các hàm kiểm tra
    test_sendData();
    test_receiveResult();

    return EXIT_SUCCESS;
}