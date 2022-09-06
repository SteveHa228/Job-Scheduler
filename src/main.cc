#include <sys/resource.h>
#include <sys/time.h>

#include <cerrno>
#include <iostream>
#include <stdexcept>
#include <map>
#include <functional>

using Callback = std::function<void()>;

struct Job{
    int64_t startTime;
    int64_t endTime;
    int64_t intervalTime;
    Callback callbackFunction;
    int64_t lastTriggered;
    Job(int64_t start, int64_t interval, int64_t end, Callback callback){
        startTime = start;
        endTime = end;
        intervalTime = interval;
        callbackFunction = callback;
        lastTriggered = start;
    }
};

class Scheduler{
    public:
        void Schedule(int64_t start, Callback callback);
        void Schedule(int64_t start, int64_t interval, int64_t end, Callback callback);
        void UpdateTime(int64_t microseconds);
    private:
        std::multimap<int64_t, Job> Jobs;
        int64_t currentTime = 0;
};

void f()
{
    printf("f()\n");
}

void g()
{
    printf("g()\n");
}

void Scheduler::Schedule(int64_t start, int64_t interval, int64_t end, Callback callback){
    //Jobs[interval] = Job(start, interval, end, callback);
    Jobs.insert(std::pair<int64_t, Job>(interval, Job(start, interval, end, callback)));
}

void Scheduler::Schedule(int64_t start, Callback callback){
    Jobs.insert(std::pair<int64_t, Job>(0, Job(start, 0, 0, callback)));
}

void Scheduler::UpdateTime(int64_t microseconds){
    //check from previous time to current time
    //get the first difference non-zero interval
    int64_t duration = 1;
    for(auto const& job : Jobs){
        if(job.first != 0){
            duration = job.first;
            break;
        }
    }
    for(int t = currentTime; t <= microseconds;)
    {
        std::multimap<int64_t, Job>::iterator it;
        // rbegin() returns to the last value of map
        for (it = Jobs.begin(); it != Jobs.end();) {
            if(t > it->second.startTime && t <= it->second.endTime && t >= it->second.lastTriggered )
            {
                it->second.callbackFunction();
                it->second.lastTriggered += it->second.intervalTime;
                ++it;
            }
            else if(t >= it->second.startTime && it->first == 0){
                it->second.callbackFunction();
                //erase after first trigger
                Jobs.erase(it++);
            }
            else if (t > it->second.endTime && it->first != 0)
            {
                //job expired
                Jobs.erase(it++);
            }
            else{
                ++it;
            }
        }
        if(t >= microseconds )
            break;
        t += duration;
        if(t > microseconds)
            t = microseconds;
    }

    //
    currentTime = microseconds;
  
}

int main(void){
    Scheduler jobScheduler;
    jobScheduler.Schedule(20, 20 , 200 , f);
    jobScheduler.UpdateTime(10);
    jobScheduler.UpdateTime(30);
    jobScheduler.Schedule(50, g);
    jobScheduler.UpdateTime(100);
    jobScheduler.Schedule(120, g);
    jobScheduler.Schedule(170, f);
    jobScheduler.UpdateTime(200);
    return 0;
}