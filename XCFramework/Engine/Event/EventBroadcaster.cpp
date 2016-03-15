/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "EventBroadcaster.h"

EventBroadcaster* EventBroadcaster::m_psEventBroadcaster = nullptr;

EventBroadcaster* EventBroadcaster::GetInstance()
{
    if (m_psEventBroadcaster == nullptr)
    {
        m_psEventBroadcaster = new EventBroadcaster();
    }

    return m_psEventBroadcaster;
}

EventBroadcaster::~EventBroadcaster()
{
}

void EventBroadcaster::AddListener(IEventListener* listener)
{
    if (listener)
    {
        m_eventListeners.push_back(listener);
    }
}

void EventBroadcaster::RemoveListener(IEventListener* listener)
{
    if (listener)
    {
        for (std::vector<IEventListener*>::iterator it = m_eventListeners.begin();
            it != m_eventListeners.end(); it++)
        {
            if (*it == listener)
            {
                m_eventListeners.erase(it);
            }
        }
    }
}

void EventBroadcaster::BroadcastEvent(IEvent* evt)
{
    for (std::vector<IEventListener*>::iterator it = m_eventListeners.begin();
        it < m_eventListeners.end(); it++)
    {
        if (*it)
        {
            (*it)->OnEvent(evt);
        }
    }
}