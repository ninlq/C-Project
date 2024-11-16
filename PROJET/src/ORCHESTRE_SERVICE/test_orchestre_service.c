#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "orchestre_service.h"
#include "../UTILS/myassert.h"

// Hàm kiểm tra khởi tạo dịch vụ orchestral
void test_init_orchestre_service() {
    int semaphore_key = 1234; // Chọn một khóa semaphore
    int service_num = 0; // Số dịch vụ

    // Gọi hàm khởi tạo
    init_orchestre_service(service_num, semaphore_key);

    // Kiểm tra xem semaphore đã được tạo thành công không
    int semaphore_id = semget(semaphore_key, 1, 0666);
    myassert(semaphore_id != -1, "Semaphore was not created successfully");
    
    // Giải phóng semaphore
    semctl(semaphore_id, 0, IPC_RMID);
}

// Hàm kiểm tra gửi dữ liệu đến dịch vụ
void test_send_data_to_service() {
    int fd_pipe[2];
    if (pipe(fd_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    const char *data = "Test data";
    send_data_to_service(fd_pipe[1], data);

    // Đọc dữ liệu từ pipe để kiểm tra
    char buffer[256];
    read(fd_pipe[0], buffer, sizeof(buffer));

    myassert(strcmp(buffer, data) == 0, "Data sent and received do not match");

    close(fd_pipe[0]);
    close(fd_pipe[1]);
}

// Hàm kiểm tra nhận dữ liệu từ dịch vụ
void test_receive_data_from_service() {
    int fd_pipe[2];
    if (pipe(fd_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    const char *data = "Received data";
    write(fd_pipe[1], data, strlen(data) + 1); // Ghi dữ liệu vào pipe

    char buffer[256];
    receive_data_from_service(fd_pipe[0], buffer, sizeof(buffer));

    myassert(strcmp(buffer, data) == 0, "Data received does not match");

    close(fd_pipe[0]);
    close(fd_pipe[1]);
}

// Hàm kiểm tra thông báo hoàn thành cho orchestral
void test_notify_orchestre_of_completion() {
    int semaphore_key = 1234; // Chọn một khóa semaphore
    int semaphore_id = semget(semaphore_key, 1, IPC_CREAT | 0666);
    myassert(semaphore_id != -1, "Semaphore was not created successfully");

    // Gọi hàm thông báo hoàn thành
    notify_orchestre_of_completion(semaphore_id);

    // Kiểm tra giá trị của semaphore
    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = 0; // Kiểm tra giá trị của semaphore
    sem_op.sem_flg = IPC_NOWAIT;
    
    int result = semop(semaphore_id, &sem_op, 1);
    myassert(result != -1, "Semaphore operation failed");

    // Giải phóng semaphore
    semctl(semaphore_id, 0, IPC_RMID);
}

int main() {
    printf("Running tests for orchestre_service...\n");

    test_init_orchestre_service();
    test_send_data_to_service();
    test_receive_data_from_service();
    test_notify_orchestre_of_completion();

    printf("All tests passed!\n");

    return EXIT_SUCCESS;
}