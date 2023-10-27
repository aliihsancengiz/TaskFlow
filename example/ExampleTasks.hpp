#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include "Task/TaskListener.hpp"
#include "Task/ThreadPoolExecutor.hpp"

std::mutex m;

struct UploadTask : taskBase
{
	UploadTask(std::string url, std::string filename) : _url(url), _filename(filename)
	{
	}
	void upload_task()
	{
		std::lock_guard<std::mutex> g(m);
		std::cout << "Upload " << _filename << " to " << _url << std::endl;
	}

private:
	std::string _url, _filename;
};

struct DownloadTask : taskBase
{
	DownloadTask(std::string url, std::string filename) : _url(url), _filename(filename)
	{
	}
	void download_task()
	{
		std::lock_guard<std::mutex> g(m);
		std::cout << "Downloding file to " << _filename << " from " << _url << std::endl;
	}

private:
	std::string _url, _filename;
};

struct DeviceFwUploadTaskListener : TaskListenerBase
{
	DeviceFwUploadTaskListener(TaskChannel& ch) : TaskListenerBase(ch)
	{
		ch.registerTask<UploadTask>(&DeviceFwUploadTaskListener::handle_upload_task, this);
		ch.registerTask<DownloadTask>(&DeviceFwUploadTaskListener::handle_download_task, this);
	}
	void handle_upload_task(std::shared_ptr<UploadTask>& upInst)
	{
		upInst->upload_task();
	}
	void handle_download_task(std::shared_ptr<DownloadTask>& downInst)
	{
		downInst->download_task();
	}
};

struct OpenedEvent : taskBase
{
	void open_event_task()
	{
		std::lock_guard<std::mutex> g(m);
		std::cout << "on Opened Event" << std::endl;
	}
};

struct ClosedEvent : taskBase
{
	void close_event_task()
	{
		std::lock_guard<std::mutex> g(m);
		std::cout << "on Closed Event" << std::endl;
	}
};

struct DataReceivedEvent : taskBase
{
	void data_rcv_event_task()
	{
		std::lock_guard<std::mutex> g(m);
		std::cout << "on DataReceived Event" << std::endl;
	}
};

struct DataTransmittedEvent : taskBase
{
	void data_transmitted_event_task()
	{
		std::lock_guard<std::mutex> g(m);
		std::cout << "on DataTransmitted Event" << std::endl;
	}
};
