////////////////////////////////////////////////////////////////////////////
//
// Copyright 2019 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef ASYNC_OPEN_TASK_HPP
#define ASYNC_OPEN_TASK_HPP

#include <realm/util/checked_mutex.hpp>
#include <realm/util/functional.hpp>

#include <memory>
#include <vector>

namespace realm {
class Realm;
class SyncSession;
class ThreadSafeReference;
namespace _impl {
class RealmCoordinator;
}

// Class used to wrap the intent of opening a new Realm or fully synchronize it before returning it to the user
// Timeouts are not handled by this class but must be handled by each binding.
class AsyncOpenTask : public std::enable_shared_from_this<AsyncOpenTask> {
public:
    AsyncOpenTask(std::shared_ptr<_impl::RealmCoordinator> coordinator, std::shared_ptr<realm::SyncSession> session);
    // Starts downloading the Realm. The callback will be triggered either when the download completes
    // or an error is encountered.
    //
    // If multiple AsyncOpenTasks all attempt to download the same Realm and one of them is canceled,
    // the other tasks will receive a "Cancelled" exception.
    void start(util::UniqueFunction<void(ThreadSafeReference, std::exception_ptr)> callback) REQUIRES(!m_mutex);

    // Cancels the download and stops the session. No further functions should be called on this class.
    void cancel() REQUIRES(!m_mutex);

    uint64_t register_download_progress_notifier(
        std::function<void(uint64_t transferred_bytes, uint64_t transferrable_bytes)>&& callback) REQUIRES(!m_mutex);
    void unregister_download_progress_notifier(uint64_t token) REQUIRES(!m_mutex);

private:
    std::shared_ptr<_impl::RealmCoordinator> m_coordinator GUARDED_BY(m_mutex);
    std::shared_ptr<SyncSession> m_session GUARDED_BY(m_mutex);
    std::vector<uint64_t> m_registered_callbacks GUARDED_BY(m_mutex);
    mutable util::CheckedMutex m_mutex;
};

} // namespace realm

#endif // // ASYNC_OPEN_TASK_HPP
