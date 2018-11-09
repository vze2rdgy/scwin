#include "pch.h"
#include "PubSubService.h"

namespace scwin
{
	const long long TIMERDELAYPERIOD = 5000000; // 1/2 second.

	PubSubService^ PubSubService::instance = nullptr;

	PubSubService::PubSubService()
	{
		TimeSpan delay;
		delay.Duration = TIMERDELAYPERIOD;
		timer = ThreadPoolTimer::CreatePeriodicTimer(
			ref new TimerElapsedHandler(this, &PubSubService::TimerCallback), 
			delay, ref new TimerDestroyedHandler(this, &PubSubService::TimerDestroyedCallback));
	}

	PubSubService::~PubSubService()
	{
		if (timer)
		{
			timer->Cancel();
			delete timer;
		}
	}

	PubSubService^ PubSubService::Instance::get()
	{
		if (!instance)
		{
			GUARDEDGLOCK;
			if (!instance)
			{
				instance = ref new PubSubService();
			}
		}
		return instance;
	}

	void PubSubService::TimerCallback(ThreadPoolTimer^ source)
	{
		DeliverMessages();
	}

	void PubSubService::TimerDestroyedCallback(ThreadPoolTimer ^ source)
	{
		// clear up all messages in tehq ueue
		DeliverMessages();
		subs.clear();
	}

	void PubSubService::DeliverMessages()
	{
		// TODO: improve later to be more efficient
		while (turnonFlag && !msgQueue.empty())
		{
			busy = true;
			PubSubMessage^ msg;
			if (msgQueue.try_pop(msg))
			{
				auto bounds = subs.equal_range(PubSubMessageIds::All);
				for (auto it = bounds.first; it != bounds.second; ++it)
				{
					auto pr = it->second;
					pr.second(pr.first, msg->MessageId, msg->Data);
				}
				auto id = msg->MessageId;
				bounds = subs.equal_range(id);
				for (auto it = bounds.first; it != bounds.second; ++it)
				{
					auto pr = it->second;
					pr.second(pr.first, id, msg->Data);
				}
			}
			busy = false;
			// if flag is turned off, exit and continue at next timer call
			if (!turnonFlag)
				break;
		}
	}

	void PubSubService::Subscribe(Platform::Object ^ receiver, ReceivePubSubMessageCallback ^ callback, const Platform::Array<PubSubMessageIds>^ receivedMessage)
	{
		turnonFlag = false;
		ScopeComplete<std::atomic_bool> c(turnonFlag, [](std::atomic_bool& b) { b = true; });
		while (busy)
		{
			Sleep(50);
		}
		if (receivedMessage == nullptr)
		{
			// TODO: check existance of same record in the map.
			subs.insert(std::make_pair(PubSubMessageIds::All, std::make_pair(receiver, callback)));
			return;
		}
		for (auto id : receivedMessage)
		{
			// TODO: check existance of same record in the map.
			subs.insert(std::make_pair(id, std::make_pair(receiver, callback)));
		}

		OutputDebugString(((std::wstring)L"Added " + receiver->GetType()->FullName->Data() + L" to pubsubservice\n").c_str());

	}

	void PubSubService::Unsubscribe(Platform::Object ^ receiver, const Platform::Array<PubSubMessageIds>^ receivedMessage)
	{
		turnonFlag = false;
		ScopeComplete<std::atomic_bool> c(turnonFlag, [](std::atomic_bool& b) { b = true; });
		while (busy)
		{
			Sleep(50);
		}
		if (receivedMessage == nullptr)
		{
			// remove all messages of this subscriber
			std::vector<Subscribers::iterator> founditems;
			for(auto it = subs.begin(); it != subs.end(); ++it)
			{
				if (it->second.first == receiver)
				{
					founditems.emplace_back(it);
				}
			}
			// now remove them
			for (auto item : founditems)
			{
				subs.unsafe_erase(item);
			}
		}
		OutputDebugString(((std::wstring)L"Removed " + receiver->GetType()->FullName->Data() + L" to pubsubservice\n").c_str());
	}

	void PubSubService::Publish(Platform::Object ^ sender, PubSubMessageIds messageId, Platform::Object ^ data)
	{
		PubSubMessage^ msg = ref new PubSubMessage(sender, messageId, data);
		Publish(msg);
	}

	void PubSubService::PublishError(Platform::Object ^ sender, Platform::Object ^ data)
	{
		Publish(sender, PubSubMessageIds::ApplicationErrorStatusMessage, data);
	}

	void PubSubService::Publish(PubSubMessage ^ message)
	{
		// add to queue
		msgQueue.push(message);
	}

	///////////////////////////////// PubSubMessage class ////////////////////////////////////////

	PubSubMessage::PubSubMessage()
	{
	}

	PubSubMessage::PubSubMessage(Object ^ sender, PubSubMessageIds messageId, Object ^ data)
	{
		this->Sender = sender;
		this->MessageId = messageId;
		this->Data = data;
	}

}
