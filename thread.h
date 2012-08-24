 #include <stdio.h>
 #include <pthread.h>
class Thread
{
    private:
        pthread_t pid;
    private:
        static void * start_thread(void *arg);                                                       //静态成员函数
    public: 
        int start();
		void mythread_join();
        virtual void run() = 0;//
};

int Thread::start()
{
        if(pthread_create(&pid,NULL,start_thread,(void *)this) != 0)                 //创建一个线程(必须是全局函数)
        {       
            return -1;
        }
        return 0;
}

void* Thread::start_thread(void *arg) //静态成员函数只能访问静态变量或静态函数，通过传递this指针进行调用
{
    Thread *ptr = (Thread *)arg;
	ptr->run();
}

void Thread::mythread_join(){
	if(pid !=0) {
            pthread_join(pid,NULL);
            printf("子线程结束\n");
        } 	
}

