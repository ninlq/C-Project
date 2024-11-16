#include <stdio.h>
#include "client_orchestre.h"

int main() {
    if (init_communication_with_orchestre()) {
        printf("Communication initialized successfully.\n");
    } else {
        printf("Failed to initialize communication.\n");
    }

    // Gửi yêu cầu thử nghiệm
    if (send_request_to_orchestre(0, "test parameters")) {
        printf("Request sent successfully.\n");
    } else {
        printf("Failed to send request.\n");
    }

    // Nhận phản hồi thử nghiệm
    char response[256];
    if (receive_response_from_orchestre(response, sizeof(response))) {
        printf("Response received: %s\n", response);
    } else {
        printf("Failed to receive response.\n");
    }

    // Đóng giao tiếp
    close_communication_with_orchestre();
    printf("Communication closed.\n");

    return 0;
}