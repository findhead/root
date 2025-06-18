#include "head.h"
const int check_true = 1;
class dotask
{
	public:
		void (* function)(void * arg);
		void * arg;
}
class  thread_pool
{
	private:
		pthread_mutex_t lock;
		pthread_cond_t cond;
		int tail;
		int quesize;
		int shutdown;
		int next;
		dotask * queue;
		int threadsize;
		int stated;
		int head ; 
		int f_thread;
		int queuefree;
		static thread_pool * pool = new thread_pool;
		pthread_t * threads;
	private:
	       thread_pool()
	       {
		       quesize = 10;
		       head = 0;
		       threadsize = 4;
		       f_thread = 4;
		       queuefree = 0;
		       this->threads = new pthread_t[f_thread];
		       pthread_mutex_init(&(this->lock),NULL);
		       pthread_cond_init(&(this->cond) ,NULL);
		       for(int i = 0 ; i < queuesize; ++i)
		       {
			       if(pthread_create(&(pool->thread[i]) , NULL ,thread_work , NULL ) != 0 )
			       {
				      	pthread_destroy(pool->thread[i]);
					cout<<"init thread[i] was wrong!"<<endl;
					exit(1);
			       }

			       pool->stated++;
		       }

	       }
	       void thread_work()
	       {
		     dotask task;
		     while(check_true)
		     {
			     pthread_mutex_lock(&(this->lock));
			     while((this->count) == 0 && (!this->shutdwown))
			     {
				     pthread_cond_wait(&(this->cond) , &(this->lock));
			     }
			     if(this->shutdown && this->count == 0)
			     {
				     break;
			     }
			     task.function = this->queue[this->head].function;
			     task.arg = this->queue[this->head].arg;
			     pool->head = (pool->head +1 ) % pool->quesize;
			     pool->count--;
			     pthread_mutex_unlock(&(this->lock));

			     (*(task.function))(task.arg);
		     }
		     pthread->strated--;
		     pthread_mutex_unlock(&(this->lock));
		     pthread_exit(NULL);
		     return NULL;


	       }
	       int threadpool_add(void (*function)(void *) , void * arg)
	       {
		     pthread_mutex_lock(&(this->lock));
		     next=( this->wail + 1)%this->quesize;
		     this->queue[this->tail].function = function;
		     this->queue[this->tail].arg = arg;
		     this->tail = next;
		     pool->count++;

		       
	       }
               ~thread_pool()
	       {
		       

	       }	       
}


int main()
{

	return 0;
}
