#pragma once

#include <concurrent_queue.h>

namespace scwin
{
	public enum class PubSubMessageIds
	{
		All,
		ApplicationInfoStatusMessage,
		ApplicationErrorStatusMessage,
		SearchMessage,

		// Products
		ProductNewCommitted,
		ProductEditCommitted,
		PricePlanSelected,
	};

	public ref class PubSubMessage sealed
	{
	public:
		PubSubMessage();
		PubSubMessage(Object^ sender, PubSubMessageIds messageId, Object^ data);
		property Object^ Sender;
		property PubSubMessageIds MessageId;
		property Object^ Data;
	};

	public delegate void ReceivePubSubMessageCallback(Platform::Object^ sender, PubSubMessageIds messageId, Platform::Object^ data);

	ref class PubSubService sealed
	{
		static PubSubService^ instance ;
		typedef concurrent_unordered_multimap < PubSubMessageIds, std::pair<Platform::Object^, ReceivePubSubMessageCallback^>> Subscribers;
		typedef concurrent_queue<PubSubMessage^> Queue;
		Subscribers subs;
		ThreadPoolTimer^ timer;
		Queue msgQueue;
		std::atomic_bool turnonFlag = true;
		std::atomic_bool busy = false;

	private:
		PubSubService();
		~PubSubService();

		void TimerCallback(ThreadPoolTimer^ source);
		void TimerDestroyedCallback(ThreadPoolTimer^ source);

		void DeliverMessages();

	public:
		static property PubSubService^ Instance
		{
			PubSubService^ get();
		}

		void Subscribe(Platform::Object^ receiver, ReceivePubSubMessageCallback^ callback, const Platform::Array<PubSubMessageIds>^ receivedMessage = nullptr);

		// Remove subscriber. if messageids parameter is null, the receiver is unsubscribed from receiving all messages
		void Unsubscribe(Platform::Object^ receiver, const Platform::Array<PubSubMessageIds>^ receivedMessage = nullptr);

		void Publish(Platform::Object^ sender, PubSubMessageIds messageId, Platform::Object^ data);

		void PublishError(Platform::Object^ sender, Platform::Object^ data);

		void Publish(PubSubMessage^ messsage);

	};
}

