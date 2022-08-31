#include <iostream>
#include <atomic>
#include <functional>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <typeindex>
#include <map>

using Work = std::function<void(std::size_t)>;

struct Worker
{

	Worker()
	{
		static std::atomic<size_t> counter(0);
		_id = counter++; 
		is_stop.store(false);
		worker_thread=std::thread(&Worker::dispatch_loop,this);
	}
	~Worker(){
		stop();
		work_queue_notifier.notify_one();
		worker_thread.join();
	}

	void dispatch(Work&& w)
	{
		work_queue.emplace(w);	
		work_queue_notifier.notify_one();
	}

    template<typename Func, typename... Args>
    void dispatch(Func&& work, Args&&... args)
    {
		work_queue.emplace(std::bind(std::forward<Func>(work), std::forward<Args>(args)...));
		work_queue_notifier.notify_one();
    }

	std::size_t get_id()const {return _id;} 
	void stop(){is_stop.store(true);}
private:
	void dispatch_loop(){
		while (!is_stop) {
                Work w = nullptr;
                {
                    std::unique_lock<std::mutex> lock(work_queue_mutex);
                    work_queue_notifier.wait(lock, [this]() {
                        return !work_queue.empty() || is_stop;
                    });
                    if (!is_stop) {

                        w = work_queue.front();
                        work_queue.pop();
                    }
                }
                if (!is_stop && w != nullptr) {
                    w(_id);
                }
            }
	}
	std::queue<Work> work_queue;
	std::size_t _id;
	std::atomic<bool> is_stop;
	std::mutex work_queue_mutex;
    std::condition_variable work_queue_notifier;
	std::thread worker_thread;
};


struct SchedulePolicy
{
	using WorkerIdList=std::vector<std::size_t>;
	SchedulePolicy()=default;
	void set_worker_id_list(const WorkerIdList& worker_ids)
	{
		_worker_ids=worker_ids;
	}
	virtual std::size_t get_next_id()=0;
	WorkerIdList _worker_ids;
};

struct DefaultSchedulerPolicy: public SchedulePolicy 
{
	std::size_t get_next_id() override{
		static int next=0;
		return next++ % _worker_ids.size();
	}
};

struct BoundOnePolicy : public SchedulePolicy
{
	BoundOnePolicy(std::size_t wid):_wid(wid){}
	std::size_t get_next_id() override{	
		return _wid;
	}	
	private:
		std::size_t _wid;
};

struct WorkerGroup{
	WorkerGroup(SchedulePolicy& policy,std::size_t worker_size=4):
		number_of_workers(worker_size),
		_policy(policy)
	{
		for (size_t i = 0; i < number_of_workers; i++)
		{
			worker_list.push_back(std::make_unique<Worker>());
		}
		std::vector<std::size_t> wids;
		for (auto& w:worker_list)
		{
			wids.push_back(w->get_id());
		}
		_policy.set_worker_id_list(wids);
		
	}
	~WorkerGroup(){
	}

	void dispatch(Work&& work)
	{
		auto w_id=_policy.get_next_id();
		for (auto& worker:worker_list)
		{
			if (worker->get_id() == w_id)
			{
				worker->dispatch(std::forward<Work>(work));
			}
		}
	}

	template<typename Func, typename... Args>
    void dispatch_with_args(Func&& work, Args&&... args)
    {
		auto w_id=_policy.get_next_id();
		for (auto& worker:worker_list)
		{
			if (worker->get_id() == w_id)
			{
				worker->dispatch(std::forward<Func>(work), std::forward<Args>(args)...);
			}
		}
		
    }

	private:
		std::size_t number_of_workers;
		std::vector<std::unique_ptr<Worker>> worker_list;
		SchedulePolicy& _policy;
};

std::mutex print_mutex;
void taskA(std::size_t id)
{
	std::unique_lock<std::mutex> lock(print_mutex);
	std::cout<<"I am taskA dispatched by Worker "<<id<<std::endl;
}


struct Task{
	Task(){
		static std::atomic<size_t> counter(0);
		_id = counter++; 
	}
	std::size_t get_id(){return _id;}
	private:
		std::size_t _id;
};

struct TaskA:Task
{
	void handler()
	{
		std::cout<<"TaskA foo"<<std::endl;
	}
};


using handle = std::function<void(Task&)>;

struct Channel
{
	template<typename type_of_task>
	void register_task(handle h)
	{
		const auto type_idx = std::type_index(typeid(type_of_task));
		handlerRegister.emplace(type_idx,h);
	}
	template<typename type_of_task>
	void push(type_of_task a){
		const auto type_idx = std::type_index(typeid(type_of_task));
		handlerRegister[type_idx](a);
	} 
	private:
    	std::map < std::type_index, handle> handlerRegister;
};

struct TaskListener{
	TaskListener(Channel& ch)
	{
		ch.register_task<TaskA>(std::bind(&TaskListener::handle_taskA,this,std::placeholders::_1));
	}
	void handle_taskA(Task& task)
	{
		std::cout<<"handle_taskA"<<std::endl;
		TaskA&    aa = (TaskA&)(task);
		aa.handler();
	}

};

int main(){

	Channel ch;
	TaskListener listener(ch);
	ch.push(TaskA());

	// DefaultSchedulerPolicy policy;
	// BoundOnePolicy bpolicy(1);

	// WorkerGroup group(bpolicy,4);
	// group.dispatch(taskA);
	// group.dispatch(taskA);
	// group.dispatch(taskA);
	// group.dispatch(taskA);
	// group.dispatch(taskA);
	// group.dispatch(taskA);

	// std::this_thread::sleep_for(std::chrono::seconds(2));

	return 0;
}

