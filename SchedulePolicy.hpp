#pragma once

#include <vector>

struct SchedulePolicy
{
    using WorkerIdList = std::vector<std::size_t>;
    SchedulePolicy() = default;
    void set_worker_id_list(const WorkerIdList &worker_ids)
    {
        _worker_ids = worker_ids;
    }
    virtual std::size_t get_next_id() = 0;
    WorkerIdList _worker_ids;
};

struct DefaultSchedulerPolicy : public SchedulePolicy
{
    std::size_t get_next_id() override
    {
        static int next = 0;
        return next++ % _worker_ids.size();
    }
};

struct BoundOnePolicy : public SchedulePolicy
{
    BoundOnePolicy(std::size_t wid) : _wid(wid) {}
    std::size_t get_next_id() override
    {
        return _wid;
    }

private:
    std::size_t _wid;
};