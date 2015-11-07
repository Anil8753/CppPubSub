#ifndef __I_NOTIFICATION_SERVICE_H__
#define __I_NOTIFICATION_SERVICE_H__

#include <functional>
#include <boost/signals2.hpp>
#include <map>

using namespace std;

typedef map<string, string> NotificationData;
typedef void NotificationHandler(const NotificationData&);
typedef boost::signals2::connection SubscriptionToken;

class INotificationService
{
public:
    virtual void Publish(const string& channel, const NotificationData& notificationData) = 0;
    virtual SubscriptionToken Subscribe(const string& channel, const function<NotificationHandler>& fn) = 0;
    virtual void Unsubscribe(SubscriptionToken& subscriptionToken) = 0;
    virtual ~INotificationService() {};
};

#endif /*__I_NOTIFICATION_SERVICE_H__*/
