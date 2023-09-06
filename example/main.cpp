#include "ExampleTasks.hpp"

struct MyDeviceTaskListener : TaskListenerBase
{
	MyDeviceTaskListener(TaskChannel& ch) : TaskListenerBase(ch)
	{
		ch.registerTask<UploadTask>(&MyDeviceTaskListener::handle_upload_task, this);
		ch.registerTask<DownloadTask>(&MyDeviceTaskListener::handle_download_task, this);
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

struct DeviceEventListener : TaskListenerBase
{
	DeviceEventListener(TaskChannel& ch) : TaskListenerBase(ch)
	{
		ch.registerTask<OpenedEvent>(&DeviceEventListener::handle_open, this);
		ch.registerTask<ClosedEvent>(&DeviceEventListener::handle_close, this);
		ch.registerTask<DataReceivedEvent>(&DeviceEventListener::handle_data_rcv, this);
		ch.registerTask<DataTransmittedEvent>(&DeviceEventListener::handle_data_transmit, this);
	}
	void handle_open(std::shared_ptr<OpenedEvent>& openInst)
	{
		openInst->open_event_task();
	}

	void handle_close(std::shared_ptr<ClosedEvent>& openInst)
	{
		openInst->close_event_task();
	}

	void handle_data_rcv(std::shared_ptr<DataReceivedEvent>& openInst)
	{
		openInst->data_rcv_event_task();
	}

	void handle_data_transmit(std::shared_ptr<DataTransmittedEvent>& openInst)
	{
		openInst->data_transmitted_event_task();
	}
};

int main()
{
	DefaultSchedulerPolicy policy;
	std::shared_ptr<ThreadPool> wg = std::make_shared<ThreadPool>(policy);
	TaskChannel ch(wg);
	DeviceFwUploadTaskListener listener(ch);
	DeviceEventListener evListener(ch);

	listener.get_channel().push(std::make_shared<UploadTask>("my_server_url:/upload/image/", "firmware.bin"));
	listener.get_channel().push(std::make_shared<DownloadTask>("my_server_url:/download/image/", "firmware.bin"));
	evListener.get_channel().push(std::make_shared<OpenedEvent>());
	evListener.get_channel().push(std::make_shared<ClosedEvent>());
	evListener.get_channel().push(std::make_shared<DataReceivedEvent>());
	evListener.get_channel().push(std::make_shared<DataTransmittedEvent>());

	return 0;
}