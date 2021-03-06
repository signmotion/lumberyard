/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
// Original file Copyright Crytek GMBH or its affiliates, used under license.

#ifndef CRYINCLUDE_EDITOR_BACKGROUNDSCHEDULEMANAGER_H
#define CRYINCLUDE_EDITOR_BACKGROUNDSCHEDULEMANAGER_H
#pragma once

#include "Include/IBackgroundScheduleManager.h"

namespace BackgroundScheduleManager
{
    class CScheduleItem
        : public IBackgroundScheduleItem
    {
    private:
        std::string m_name;
        volatile int m_refCount;

        EScheduleItemState m_state;

        typedef std::vector<IBackgroundScheduleItemWork*> TWorkItems;
        TWorkItems m_workItems;
        TWorkItems m_addedWorkItems;
        TWorkItems m_processedWorkItems;

    public:
        CScheduleItem(const char* szName);
        virtual ~CScheduleItem();

        // IBackgroundScheduleItem interface
        virtual const char* GetDescription() const;
        virtual EScheduleItemState GetState() const;
        virtual const float GetProgress() const;
        virtual const uint32 GetNumWorkItems() const;
        virtual IBackgroundScheduleItemWork* GetWorkItem(const uint32 index) const;
        virtual void AddWorkItem(IBackgroundScheduleItemWork* pWork);
        virtual void AddRef();
        virtual void Release();

        // Update schedule item
        EScheduleWorkItemStatus Update();

        // Request to stop work in this item
        void RequestStop();
    };

    class CSchedule
        : public IBackgroundSchedule
    {
    private:
        std::string m_name;
        volatile int m_refCount;
        bool m_bCanceled;

        EScheduleState m_state;

        typedef std::vector<CScheduleItem*> TItems;
        TItems m_items;

        uint32 m_currentItem;

    public:
        CSchedule(const char* szName);
        virtual ~CSchedule();

        // IBackgroundSchedule interface
        virtual const char* GetDescription() const;
        virtual float GetProgress() const;
        virtual IBackgroundScheduleItem* GetProcessedItem() const;
        virtual const uint32 GetNumItems() const;
        virtual IBackgroundScheduleItem* GetItem(const uint32 index) const;
        virtual EScheduleState GetState() const;
        virtual void Cancel();
        virtual bool IsCanceled() const;
        virtual void AddItem(IBackgroundScheduleItem* pItem);
        virtual void AddRef();
        virtual void Release();

        // Update schedule item
        EScheduleWorkItemStatus Update();
    };

    class CScheduleManager
        : public IBackgroundScheduleManager
        , public IEditorNotifyListener
    {
    private:
        typedef std::vector<CSchedule*> TSchedules;
        TSchedules m_schedules;

    public:
        CScheduleManager();
        virtual ~CScheduleManager();

        // IBackgroundScheduleManager interface
        virtual IBackgroundSchedule* CreateSchedule(const char* szName);
        virtual IBackgroundScheduleItem* CreateScheduleItem(const char* szName);
        virtual void SubmitSchedule(IBackgroundSchedule* pSchedule);
        virtual const uint32 GetNumSchedules() const;
        virtual IBackgroundSchedule* GetSchedule(const uint32 index) const;
        virtual void Update();

        // IEditorNotifyListener interface implementation
        virtual void OnEditorNotifyEvent(EEditorNotifyEvent ev) override;
    };
}
#endif // CRYINCLUDE_EDITOR_BACKGROUNDSCHEDULEMANAGER_H
