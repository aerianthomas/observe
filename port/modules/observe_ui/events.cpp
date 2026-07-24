#include "events.h"

#include "frame.h"
#include "modules/observe_utils/time_utils.h"

namespace observe
{

  EventTimer::~EventTimer()
  {
    if (isRunning())
      stopTimer();
  }

  void EventTimer::startTimer(int ms)
  {
    OBSERVE_ASSERT(ms > 0);

    if (ms > 0)
    {
      if (!isRunning())
        EventManager::instance().addTimer(this);

      last_run_time_ = time::milliseconds();
      ms_ = ms;
    }
  }

  void EventTimer::stopTimer()
  {
    if (isRunning())
    {
      EventManager::instance().removeTimer(this);
      ms_ = 0;
    }
  }

  inline bool EventTimer::checkTimer(long long current_time)
  {
    OBSERVE_ASSERT(isRunning());

    if (current_time - last_run_time_ >= ms_)
    {
      last_run_time_ = current_time;
      notifyTimerCallback();
      return true;
    }

    return false;
  }

  void EventManager::addTimer(EventTimer *timer)
  {
    timers_.push_back(timer);
  }

  void EventManager::removeTimer(const EventTimer *timer)
  {
    auto it = std::find(timers_.begin(), timers_.end(), timer);
    if (it != timers_.end())
      timers_.erase(it);
  }

  void EventManager::addCallback(std::function<void()> callback)
  {
    callbacks_.push_back(std::move(callback));
  }

  void EventManager::checkEventTimers()
  {
    long long current_time = time::milliseconds();
    std::vector<EventTimer *> timers = timers_;
    std::vector<std::function<void()>> callbacks = std::move(callbacks_);

    for (auto timer : timers)
      timer->checkTimer(current_time);

    for (auto &callback : callbacks)
      callback();
  }

  MouseEvent MouseEvent::relativeTo(const Frame *new_frame) const
  {
    MouseEvent copy = *this;
    copy.position = copy.window_position - new_frame->positionInWindow();
    copy.event_frame = new_frame;
    return copy;
  }
}
