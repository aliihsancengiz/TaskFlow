#include <iostream>
#include <functional>
#include <memory>
#include "include/TaskListener.hpp"

struct UploadTask : taskBase
{
	UploadTask(std::string url, std::string filename) : _url(url), _filename(filename) {}
	void upload_task()
	{
		std::cout << "Upload " << _filename << " to " << _url << std::endl;
	}

private:
	std::string _url, _filename;
};

struct DownloadTask : taskBase
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
		ch.registerTask<UploadTask>(&MyDeviceTaskListener::handle_upload_task, this);
		ch.registerTask<DownloadTask>(&MyDeviceTaskListener::handle_download_task, this);
	}
	void handle_upload_task(std::shared_ptr<UploadTask> &upInst)
	{
		upInst->upload_task();
	}
	void handle_download_task(std::shared_ptr<DownloadTask> &downInst)
	{
		downInst->download_task();
	}
};

int main()
{
	DefaultSchedulerPolicy policy;
	WorkerGroup wg(policy);
	TaskChannel ch(wg);
	MyDeviceTaskListener listener(ch);

	listener.get_channel().push(std::make_shared<UploadTask>("my_server_url:/upload/image/", "firmware.bin"));
	listener.get_channel().push(std::make_shared<DownloadTask>("my_server_url:/download/image/", "firmware.bin"));

	return 0;
}