#include <iostream>
#include <functional>
#include <memory>

#include "task_listener.hpp"

struct UploadTask : task_base
{
	UploadTask(std::string url, std::string filename) : _url(url), _filename(filename) {}
	void upload_task()
	{
		std::cout << "Upload " << _filename << " to " << _url << std::endl;
	}

private:
	std::string _url, _filename;
};

struct DownloadTask : task_base
{
	DownloadTask(std::string url, std::string filename) : _url(url), _filename(filename) {}
	void download_task()
	{
		std::cout << "Downloding file to " << _filename << " from " << _url << std::endl;
	}

private:
	std::string _url, _filename;
};

struct MyDeviceTaskListener : TaskListenerBase
{
	MyDeviceTaskListener(TaskChannel &ch) : TaskListenerBase(ch)
	{
		ch.register_task<UploadTask>(&MyDeviceTaskListener::handle_upload_task, this);
		ch.register_task<DownloadTask>(&MyDeviceTaskListener::handle_download_task, this);
	}
	void handle_upload_task(std::unique_ptr<UploadTask> &upInst)
	{
		upInst->upload_task();
	}
	void handle_download_task(std::unique_ptr<DownloadTask> &downInst)
	{
		downInst->download_task();
	}
};

int main()
{

	TaskChannel ch;
	MyDeviceTaskListener listener(ch);

	listener.get_channel().push(std::make_unique<UploadTask>("my_server_url:/upload/image/", "firmware.bin"));
	listener.get_channel().push(std::make_unique<DownloadTask>("my_server_url:/download/image/", "firmware.bin"));

	return 0;
}