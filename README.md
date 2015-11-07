# CppPubSub
boost based Publisher/Subscriber framework in C++
One can subscribe for a chanel and another can publish the notification on the same chanel.

Note: It has dependency on boost. Please download boost library and set the boost path in project settings (CppPubSub.vcxproj).

Full sample code is available in Source.cpp
Usage:

        // you should create a singleton object of NotificationService class, make it accessible throughout your application. 
        INotificationService* pNotificationService = new NotificationService();

        // Subscribe for the event.
        function<NotificationHandler> fnNotificationHandler = bind(&SubscriberClass::NotificationHandlerFunction, this, std::placeholders::_1);
        subscriptionToken = pNotificationService->Subscribe("TEST_CHANEL", fnNotificationHandler);
        
        // Publish event
        NotificationData _data;
        _data["data1"] = "Hello";
        _data["data2"] = "World";
        pNotificationService->Publish("TEST_CHANEL", _data);
        
        // Unsubscribe event.
        pNotificationService->Unsubscribe(subscriptionToken);
        
