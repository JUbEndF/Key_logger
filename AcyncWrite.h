#include <aio.h>
#include <cstdio>
#include <string>
#include <list>

class AsyncWrite {

private:
    std::string FileName;

    struct aiocb createIoRequest(int fd,
                                 off_t offset,
                                 volatile void * content,
                                 size_t length){
        // create and initialize the aiocb structure.
        // If we don't init to 0, we have undefined behavior.
        // E.g. through sigevent op.aio_sigevent there could be
        //      a callback function being set, that the program
        //      tries to call - which will then fail.
        struct aiocb ret = {0};
        {
            ret.aio_fildes = fd;
            ret.aio_offset = offset;
            ret.aio_buf = content;
            ret.aio_nbytes = length;
        }
        return ret;
    }



public:
    AsyncWrite(std::string fileName)
    {
        FileName = fileName;
    }

    void write(std::list<std::string> obj)
    {
        std::string ret = "";
        for (auto i = obj.begin(); i != obj.end(); i++) {
            ret += (*i) + "\n";
        }
        write(ret);
    }

    int write(std::string user_string)
    {
        FILE * file = fopen(FileName.c_str(),"a+");
        int fd = fileno(file);
        {
            //fseek(file, 0L, SEEK_END);
            struct aiocb op  = createIoRequest(fd, 0, (volatile void *) user_string.c_str(), user_string.size());

            // schedule write
            // for valgrind mem leak output see comments from answer in
            //   https://stackoverflow.com/questions/4248720/aio-h-aio-read-and-write-memory-leak
            auto ret = aio_write(&op);
            printf("aio_write 1: %d\n",ret);

            // wait until everything is done
            {
                const struct aiocb * aiolist[1];
                aiolist[0] = &op;

                int ret = aio_suspend(aiolist,1,NULL);
                printf("aio_suspend: %d\n",ret);

                // report possible errors
                {
                    ret = aio_error(&op);
                    printf("errno 1: %d\n",ret);
                }
            }
        }
        fclose(file);
        return 0;
    }
};
