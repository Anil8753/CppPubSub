#ifndef __NOTIFICATION_SERVICE_H__
#define __NOTIFICATION_SERVICE_H__

#include "INotificationService.h"

#include <thread>
#include <mutex>
#include <condition_variable>

#define MAX_NOTIFICATIONS_BUFFER 10240

typedef boost::signals2::signal<NotificationHandler> SIGNAL;
typedef pair<SIGNAL*, const NotificationData> SignalAndNotificationPair;
typedef vector<SignalAndNotificationPair> QueuedNotificationVector;

class WXEXPORT NotificationService : public INotificationService
{
public:
    static INotificationService* GetNotificationService();
    ~NotificationService();

    void Publish(const string& channel, const NotificationData& notificationData);
    SubscriptionToken Subscribe(const string& channel, const function<NotificationHandler>& fnNotificationHandler);
    void Unsubscribe(SubscriptionToken& subscriptionToken);

private:
    NotificationService();
    void NotificationLoop();
    void FlushQueuedNotifications();
    void QueueNotification(SIGNAL* pSignal, const NotificationData& notificationData);

private:
    mutex m_mutexQueuedNotificationVector;
    mutex m_mutexNotificationLoop;
    condition_variable m_cvPauseFlushThread;
    bool m_bShutdown;
    thread m_FlushNotificationsThread;

    // Queue of Notifications
    QueuedNotificationVector m_vecQueuedNotifications;
    map<string, SIGNAL*> m_mapSignal;

    static NotificationService* s_pNotificationService;
};

#endif /*__NOTIFICATION_SERVICE_H__*/
