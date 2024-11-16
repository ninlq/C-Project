#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "orchestre_service.h"

#define PIPE_READ 0
#define PIPE_WRITE 1
#define SEMAPHORE_KEY 1234

// Hàm chính để kiểm tra các chức năng
int main() {
    // Tạo một anonymous pipe
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Khởi tạo dịch vụ
    init_orchestre_service(1, SEMAPHORE_KEY);

    // Gửi dữ liệu đến dịch vụ
    const char *data_to_send = "Hello, service!";
    send_data_to_service(fd[PIPE_WRITE], data_to_send);
    printf("Dữ liệu đã gửi: %s\n", data_to_send);

    // Nhận dữ liệu từ dịch vụ
    char buffer[256];
    receive_data_from_service(fd[PIPE_READ], buffer, sizeof(buffer));
    printf("Dữ liệu nhận được: %s\n", buffer);

    // Thông báo cho orchestration rằng dịch vụ đã hoàn thành
    notify_orchestre_of_completion(SEMAPHORE_KEY);
    printf("Đã thông báo cho orchestration về việc hoàn thành.\n");

    // Đóng các file descriptor
    close(fd[PIPE_READ]);
    close(fd[PIPE_WRITE]);

    return 0;
}