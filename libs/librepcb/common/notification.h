/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * https://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREPCB_NOTIFICATION_H
#define LIBREPCB_NOTIFICATION_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <functional>
#include <map>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Class Notification
 ******************************************************************************/

/**
 * @brief The Notification class
 */
template <typename... Args>
class Notification {
public:
  Notification() noexcept : mId(0) {}
  Notification(const Notification& other) = delete;

  int connect(const std::function<void(Args...)>& slot) const noexcept {
    mObservers.insert(std::make_pair(++mId, slot));
    return mId;
  }

  template <typename T>
  int connect(T& obj, void (T::*func)(Args...)) const noexcept {
    return connect([=, &obj](Args... args) { (obj.*func)(args...); });
  }

  template <typename T, typename... FuncArgs, typename... FwArgs>
  int connect(T& obj, void (T::*func)(FuncArgs...), FwArgs... args) const
      noexcept {
    return connect([=, &obj](Args...) { (obj.*func)(args...); });
  }

  template <typename... OtherArgs, typename... FwArgs>
  int connect(Notification<OtherArgs...>& obj, FwArgs... args) const noexcept {
    return connect([=, &obj](Args...) { obj.notify(args...); });
  }

  void disconnect(int id) const noexcept { mObservers.erase(id); }

  void disconnectAll() const noexcept { mObservers.clear(); }

  void notify(Args... p) const noexcept {
    for (auto it : mObservers) {
      it.second(p...);
    }
  }

  Notification& operator=(Notification const& other) = delete;

private:
  mutable std::map<int, std::function<void(Args...)>> mObservers;
  mutable int                                         mId;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb

#endif  // LIBREPCB_NOTIFICATION_H
