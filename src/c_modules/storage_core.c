/*
 * File: src/c_modules/storage_core.c
 * Description: POSIX implementation for Flash storage.
 */

#include "storage_core.h"
#include <fcntl.h>      // open, O_RDWR, O_CREAT
#include <unistd.h>     // write, read, close, unlink
#include <sys/stat.h>   // struct stat
#include <errno.h>      // errno

#define FILENAME "state.bin"
#define MAGIC_KEY 0xCAFEBABE

bool storage_save_state(const AppState* state) {
    // O_TRUNC: Clears previous content
    // O_CREAT: Creates file if missing
    // S_IRWXU: Read/Write/Execute permissions for User
    int fd = open(FILENAME, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
    
    if (fd < 0) {
        return false;
    }

    // Write the full struct as a binary block
    int bytes_written = write(fd, state, sizeof(AppState));
    close(fd);
    
    // Return true only if the full struct was written successfully
    return (bytes_written == sizeof(AppState));
}

bool storage_load_state(AppState* state) {
    int fd = open(FILENAME, O_RDONLY);
    
    if (fd < 0) {
        return false; // File does not exist
    }

    int bytes_read = read(fd, state, sizeof(AppState));
    close(fd);

    if (bytes_read != sizeof(AppState)) {
        return false; // Corrupt file or size mismatch
    }

    if (state->magic_number != MAGIC_KEY) {
        return false; // Invalid data signature
    }

    return true;
}

bool storage_reset_data(void) {
    // unlink removes the inode from the filesystem
    int result = unlink(FILENAME);
    
    return (result == 0 || errno == ENOENT);
}

bool storage_file_exists(void) {
    struct stat st;
    return (stat(FILENAME, &st) == 0);
}