#include "windows.h"
#include "NotificationService.h"

#define TEST_CHANEL "TestChanel"

typedef boost::shared_ptr<INotificationService> INotificationServicePtr;

class SubscriberClass
{
private:
    INotificationServicePtr m_pNotificationService;
    SubscriptionToken       m_SubscriptionToken;

public:

    SubscriberClass(INotificationServicePtr pNotificationService)
        :m_pNotificationService(pNotificationService)
    {
        SubscribeForNotiications();
    }

    ~SubscriberClass()
    {
        m_pNotificationService->Unsubscribe(m_SubscriptionToken);
    }

    void SubscribeForNotiications()
    {
        function<NotificationHandler> fnNotificationHandler = bind(&SubscriberClass::NotificationHandlerFunction, this, std::placeholders::_1);
        m_SubscriptionToken = m_pNotificationService->Subscribe(TEST_CHANEL, fnNotificationHandler);
    }

    void NotificationHandlerFunction(const NotificationData& data)
    {
        NotificationData _data = data;

        cout << _data["data1"] << endl;
        cout << _data["data2"] << endl;
        cout << "------------" << endl;
    }
};

class PublisherClass
{
public:
    PublisherClass(INotificationServicePtr pNotificationService)
        :m_pNotificationService(pNotificationService)
    {
        PublishNotifications();
    }

    ~PublisherClass()
    {
    }

private:
    void PublishNotifications()
    {
        INotificationServicePtr pNotificationService = m_pNotificationService;

        thread publishThread = std::thread{ [pNotificationService]() {

            while (true)
            {
                NotificationData _data;
                _data["data1"] = "Hello";
                _data["data2"] = "World";

                pNotificationService->Publish(TEST_CHANEL, _data);

                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        } };

        publishThread.detach();
    }

    INotificationServicePtr m_pNotificationService;
};

int main()
{
    INotificationServicePtr     pNotificationService(NotificationService::GetNotificationService());
    SubscriberClass             objSubscriberClass(pNotificationService);
    PublisherClass              objPublisherClass(pNotificationService);

    getchar();

    return 0;
}