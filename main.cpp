#define DEV_PATH "/dev/input/event2"
#include <stdio.h>
#include <linux/input.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <string.h>
#include <string>
#include <fstream>
#define BUF (256)

int check(int result){
    if (result >= 0)    
        return result;
    
        
    perror(strerror(errno));
    exit(-1);
}

//Функция возвращает время в секундах
float GetComputerOperatingTime()
{
    char buf[BUF] = {'\0'};
    std::fstream fs("/proc/uptime", std::fstream::in);
    if(!fs.is_open())
        return 0;
    float a = -1,b;
    if ( !(fs.getline(buf, BUF).eof() )){
        if (sscanf(buf, "%f %f", &a, &b) != 2)
        {
            fs.close();
            return -1;
        }
    }
    fs.close();
    return a;
}


int main()
{
    
    int keys_fd; //дескриптор
    std::string user;
    std::cout << "Enter your name: ";
    getline(std::cin, user);
    std::cout << "Your name - " + user << std::endl;
    struct input_event t;  
    keys_fd = check(open(DEV_PATH, O_RDONLY));
    if (keys_fd <= 0)
    {
        printf("open/dev/input/js0 device error!\n");
        return 0;
    }
    while (true)
    {
        if (read(keys_fd, &t, sizeof(t)) == sizeof(t))
        {
            if (t.type == EV_KEY)
                if (t.value == 0 || t.value == 1)
                {
                    printf("key %d %s\n", t.code, (t.value) ? "Pressed" : "Released");
                }
        }
    }

    close(keys_fd);
    return 0;
}
