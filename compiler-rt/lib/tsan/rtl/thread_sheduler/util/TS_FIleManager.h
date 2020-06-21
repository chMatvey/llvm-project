//
// Created by matvey on 6/18/20.
//

#ifndef SANITAZER_TS_FILEMANAGER_H
#define SANITAZER_TS_FILEMANAGER_H


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>

#define BLOCK_SIZE 128

namespace __tsan::ts::util {


    class TS_FIleManager {
    public:
        static const char *readFile(const char *path) {
            int fd = -1;
            ssize_t bytes_read = -1;
            int i = 0;

            fd = open(path, O_RDONLY);
            if (fd == -1) {
                return nullptr;
            }

            struct stat fileStat{};
            fstat(fd, &fileStat);
            long fileSize = fileStat.st_size + 1;
            char *buff = (char *) malloc(fileSize);
            long index = 0;
            long bitCount = BLOCK_SIZE;

            while (read(fd, buff + index, bitCount) > 0) {
                index += BLOCK_SIZE;
                if (index + bitCount > fileSize) {
                    bitCount = fileSize - index;
                }
            }

            close(fd);
            return buff;
        }

    };
}


#endif //SANITAZER_TS_FILEMANAGER_H
