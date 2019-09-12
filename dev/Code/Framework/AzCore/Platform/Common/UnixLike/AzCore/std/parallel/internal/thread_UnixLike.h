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
#ifndef AZSTD_THREAD_LINUX_H
#define AZSTD_THREAD_LINUX_H

#include <unistd.h>
#include <sched.h>

namespace AZStd
{
    namespace Internal
    {
        /**
         * Create and run thread
         */
        pthread_t create_thread(const thread_desc* desc, thread_info* ti);
    }

    //////////////////////////////////////////////////////////////////////////
    // thread
    template <class F>
    inline thread::thread(F&& f, const thread_desc* desc)
    {
        Internal::thread_info* ti = Internal::create_thread_info(AZStd::forward<F>(f));
#if defined(AZ_PLATFORM_APPLE)
        ti->m_name = desc ? desc->m_name : nullptr;
#endif
        m_thread = Internal::create_thread(desc, ti);
    }

    inline bool thread::joinable() const
    {
        if (pthread_equal(m_thread, native_thread_invalid_id))
        {
            return false;
        }
        return !pthread_equal(m_thread, this_thread::get_id().m_id);
    }
    inline thread::id thread::get_id() const
    {
        return thread::id(m_thread);
    }
    thread::native_handle_type
    inline thread::native_handle()
    {
        return m_thread;
    }
    //////////////////////////////////////////////////////////////////////////

    namespace this_thread
    {
        AZ_FORCE_INLINE thread::id get_id()
        {
            return thread::id(pthread_self());
        }
        AZ_FORCE_INLINE void yield()
        {
            sched_yield();
        }
        AZ_FORCE_INLINE void pause(int numLoops)
        {
            for (int i = 0; i < numLoops; ++i)
            {
                sched_yield();
            }
        }
        //template <class Clock, class Duration>
        //AZ_FORCE_INLINE void sleep_until(const chrono::time_point<Clock, Duration>& abs_time)
        //{
        //  chrono::milliseconds now = chrono::system_clock::now().time_since_epoch();
        //  AZ_Assert(now<abs_time,"Absolute time must be in the future!");
        //  chrono::milliseconds toSleep = abs_time - now;
        //  ::Sleep((DWORD)toSleep.count());
        //}
        template <class Rep, class Period>
        AZ_FORCE_INLINE void sleep_for(const chrono::duration<Rep, Period>& rel_time)
        {
            chrono::microseconds toSleep = rel_time;
            //Note: nanosleep is reccomended over usleep
            usleep(static_cast<useconds_t>(toSleep.count()));
        }
    }
}

#endif // AZSTD_THREAD_WINDOWS_H
#pragma once