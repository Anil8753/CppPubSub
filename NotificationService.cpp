#include "NotificationService.h"

NotificationService* NotificationService::s_pNotificationService;

INotificationService* NotificationService::GetNotificationService()
{
    if (s_pNotificationService == nullptr)
    {
        s_pNotificationService = new NotificationService();
    }

    return s_pNotificationService;
}

NotificationService::NotificationService()
{
    m_bShutdown = false;
    m_FlushNotificationsThread = std::thread(&NotificationService::NotificationLoop, this);
}

NotificationService::~NotificationService()
{
    m_bShutdown = true;
    m_cvPauseFlushThread.notify_one();
    m_FlushNotificationsThread.join();

    for (auto it = m_mapSignal.begin(); it != m_mapSignal.end(); it++)
    {
        delete it->second;
    }
    m_vecQueuedNotifications.clear();
    s_pNotificationService = nullptr;
}

SubscriptionToken NotificationService::Subscribe(const string& channel, const function<NotificationHandler>& fnNotificationHandler)
{
    SIGNAL* pSignal = m_mapSignal[channel];

    if (nullptr == pSignal)
    {
        pSignal = new SIGNAL();
        m_mapSignal[channel] = pSignal;
    }

    return pSignal->connect(fnNotificationHandler);
}

void NotificationService::Unsubscribe(SubscriptionToken& subscriptionToken)
{
    if (subscriptionToken.connected())
    {
        subscriptionToken.disconnect();
    }
}

void NotificationService::Publish(const string& channel, const NotificationData& notificationData)
{
    SIGNAL* pSignal = m_mapSignal[channel];

    if (nullptr != pSignal)
    {
        QueueNotification(pSignal, notificationData);
    }
}

void NotificationService::NotificationLoop()
{
    std::unique_lock<std::mutex> loop_lock(m_mutexNotificationLoop);

    while (true)
    {
        if (0 == m_vecQueuedNotifications.size())
        {
            m_cvPauseFlushThread.wait(loop_lock);
        }

        if (m_bShutdown)
            break;

        FlushQueuedNotifications();
    }
}

// Notify listeners of all recent Notifications
void NotificationService::FlushQueuedNotifications()
{
    QueuedNotificationVector vecQueuedNotificationVector;

    // Open a protected scope to modify the Notifications vector
    {
        std::unique_lock<std::mutex> lock(m_mutexQueuedNotificationVector);

        // Copy the Notifications vector to our local list and clear it at the same time
        swap(vecQueuedNotificationVector, m_vecQueuedNotifications);
    }

    // Since we're looping over the copy we just made, new Notifications won't affect us.
    for (const SignalAndNotificationPair & signalAndPair : vecQueuedNotificationVector)
    {
        if (m_bShutdown)
            break;

        SIGNAL* pSignal = signalAndPair.first;

        if (pSignal)
            (*pSignal)(signalAndPair.second);
    }
}

// Queue an Notification with the Notification service
void NotificationService::QueueNotification(SIGNAL* pSignal, const NotificationData& notificationData)
{
    // One thread at a time.
    std::unique_lock<std::mutex> lock(m_mutexQueuedNotificationVector);

    if (m_vecQueuedNotifications.size() < MAX_NOTIFICATIONS_BUFFER)
    {
        m_vecQueuedNotifications.push_back(SignalAndNotificationPair(pSignal, notificationData));
        m_cvPauseFlushThread.notify_one();
    }
}
