#pragma once

#include "Worker.hpp"
#include "SchedulePolicy.hpp"

struct WorkerGroup
{
    WorkerGroup(SchedulePolicy &policy, std::size_t worker_size = 4) : number_of_workers(worker_size),
                                                                       _policy(policy)
    {
        for (size_t i = 0; i < number_of_workers; i++)
        {
            worker_list.push_back(std::make_unique<Worker>());
        }
        std::vector<std::size_t> wids;
        for (auto &w : worker_list)
        {
            wids.push_back(w->get_id());
        }
        _policy.set_worker_id_list(wids);
    }
    ~WorkerGroup()
    {
    }

    void dispatch(Work &&work)
    {
        auto w_id = _policy.get_next_id();
        for (auto &worker : worker_list)
        {
            if (worker->get_id() == w_id)
            {
                worker->dispatch(std::forward<Work>(work));
            }
        }
    }

    template <typename Func, typename... Args>
    void dispatch_with_args(Func &&work, Args &&...args)
    {
        auto w_id = _policy.get_next_id();
        for (auto &worker : worker_list)
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
    SchedulePolicy &_policy;
};
