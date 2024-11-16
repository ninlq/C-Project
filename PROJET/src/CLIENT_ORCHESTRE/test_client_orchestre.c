#include <stdio.h>
#include <stdlib.h>
#include "client_orchestre.h" // Bao gồm file header của client_orchestre

// Hàm kiểm tra khởi tạo giao tiếp
void test_init_communication() {
    if (init_communication_with_orchestre() == 0) {
        printf("Test init_communication_with_orchestre: PASSED\n");
    } else {
        printf("Test init_communication_with_orchestre: FAILED\n");
    }
}

// Hàm kiểm tra gửi yêu cầu
void test_send_request() {
    int result = send_request_to_orchestre("Test request");
    if (result == 0) {
        printf("Test send_request_to_orchestre: PASSED\n");
    } else {
        printf("Test send_request_to_orchestre: FAILED\n");
    }
}

// Hàm kiểm tra nhận phản hồi
void test_receive_response() {
    char response[256];
    if (receive_response_from_orchestre(response, sizeof(response)) == 0) {
        printf("Test receive_response_from_orchestre: PASSED\n");
    } else {
        printf("Test receive_response_from_orchestre: FAILED\n");
    }
}

int main() {
    printf("Running tests for client_orchestre...\n");

    // Gọi các hàm kiểm tra
    test_init_communication();
    test_send_request();
    test_receive_response();

    return EXIT_SUCCESS;
}