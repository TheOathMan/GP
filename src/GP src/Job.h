#pragma once
#include "AppCore.h"

#ifdef ASYNC_METHOD_THREADING

#include <future>
enum class job_status // Job.h
{
	working,
	not_working,
	done_working
};

//----------------------- base job class ---------------------------
template<typename T>
struct _base_job {
	std::future<T> m_job;

	// get the status of this job.
	job_status get_status()
	{
		return m_job.valid() ? (m_job.wait_for(std::chrono::seconds(0)) == std::future_status::ready ?
			job_status::done_working : job_status::working) : job_status::not_working;
	}
};

//----------------------- value filter ---------------------------
template <typename T, class = void>
struct  type_methods : _base_job <T> {
	
	T Get_Value()
	{
		return  this->get_status() == job_status::done_working ? this->m_job.get() : NULL;
	}
};

template <typename T>
struct  type_methods<T,T> : _base_job<T> { /* void case */ };

//----------------------- main job class ---------------------------
template <typename T>
struct Job :  type_methods< typename std::remove_cv<T>::type ,void> {
	
	// is this job working right now ?
	bool is_working();
	
	// provide a method and provide it's parameters if available for a job
	template <typename ... Args >
	void give(Args&& ... a);

	// number of timess this thread used
	int used();

private:
	int m_used = 0;
};


template<typename T>
inline bool Job<T>::is_working()
{
	return this->get_status() == job_status::working ? true : false;
}


template<typename T>
inline int Job<T>::used()
{
	return m_used;
}

template<typename T> template<typename ...Args>
inline void Job<T>::give(Args && ...a)
{
	++m_used;
	this->m_job = std::move(std::async(std::launch::async, std::forward<Args>(a) ...));
	
}

#elif defined LOCALTHREAD_METHOD_THREADING


#include <functional>
#include <vector>
#include <thread>


struct _job_base{


    void on_thread_exit(const std::function<void()>& on_exit_fun)
    {
        thread_local struct ThreadExiter {
            std::vector<std::function<void()>> callbacks;
            ~ThreadExiter() {             
                for (auto &callback: callbacks) {
                    callback();
                }

            }
        } exiter;

        if(on_exit_fun) exiter.callbacks.emplace_back(on_exit_fun);
        exiter.callbacks.emplace_back( [&]{m_is_working = false;});

    }
	const bool is_working()const{return m_is_working;}
	const bool used()const{return m_used;}
	protected:
	bool m_is_working=false;
	int m_used = 0;
};

template <typename T,typename = void>
struct Ex_job_f : public _job_base {

    T call_value;

    void Start(std::function<T()>&& Job, std::function<void()>&& OnWorkFinish = nullptr ){
		this->m_is_working = true;
		this->m_used++;
		std::thread([=]() mutable { call_value = std::move(Job)();  on_thread_exit(OnWorkFinish); }).detach();
	}
};

template <typename T>
struct Ex_job_f<T,T> : public _job_base{

    void Start(std::function<void()>&& Job, std::function<void()>&& OnWorkFinish = nullptr ){
        this->m_is_working=true;
        this->m_used++;
        std::thread( [=] () mutable { std::move(Job)();  on_thread_exit(OnWorkFinish); }).detach();
    }
};


template<typename call_t>
struct Job : public Ex_job_f <call_t> {  };

#endif