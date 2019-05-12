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

#ifndef LIBREPCB_SIGNALSLOT_H
#define LIBREPCB_SIGNALSLOT_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include <functional>
#include <set>
#include <vector>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

template <typename Tsender, typename... Args>
class Slot;

/*******************************************************************************
 *  Class Signal
 ******************************************************************************/

/**
 * @brief The Signal class is used to emit signals on non-QObject derived
 *        classes
 *
 * The classes ::librepcb::Signal and ::librepcb::Slot are similar to Qt's
 * signal/slot concept. The main difference is that senders and receivers of
 * signals do not need to be derived from QObject, thus our own signal/slot
 * mechanism is better suited for low-level classes.
 *
 * This gives the advantage of avoiding QObject overhead, but has several
 * drawbacks:
 *
 *   - No thread-safety
 *   - Always synchronous, no queued connections are possible
 *   - No endless loop detection
 *
 * @see ::librepcb::Slot
 *
 * @tparam Tsender  Type of the sender object
 * @tparam Args     Arguments passed from ::librepcb::Signal::notify() to the
 *                  callbacks
 */
template <typename Tsender, typename... Args>
class Signal {
  friend class Slot<Tsender, Args...>;

public:
  // Constructors / Destructor
  Signal()                    = delete;
  Signal(const Signal& other) = delete;

  /**
   * @brief Constructor
   *
   * @param sender  Reference to the sender object of the signal
   */
  explicit Signal(const Tsender& sender) noexcept : mSender(sender) {}

  /**
   * @brief Destructor
   *
   * Automatically disconnects from all slots.
   */
  ~Signal() noexcept {
    for (auto slot : mSlots) {
      slot->mSignals.erase(this);
    }
  }

  /**
   * @brief Attach a slot
   *
   * @param slot  Reference to the slot to attach
   */
  void attach(Slot<Tsender, Args...>& slot) noexcept {
    slot.mSignals.insert(this);
    mSlots.insert(&slot);
  }

  /**
   * @brief Detach a slot
   *
   * @param slot  Reference to the slot to detach
   */
  void detach(Slot<Tsender, Args...>& slot) noexcept {
    slot.mSignals.erase(this);
    mSlots.erase(&slot);
  }

  /**
   * @brief Notify all attached slots
   *
   * @param args  Arguments passed to the slots
   */
  void notify(Args... args) noexcept {
    for (auto slot : mSlots) {
      slot->mCallback(mSender, args...);
    }
  }

  // Operator Overloadings
  Signal& operator=(Signal const& other) = delete;

private:
  const Tsender& mSender;                    ///< Reference to the sender object
  std::set<Slot<Tsender, Args...>*> mSlots;  ///< All attached slots
};

/*******************************************************************************
 *  Class Slot
 ******************************************************************************/

/**
 * @brief The Slot class is used to receive signals from non-QObject derived
 *        classes
 *
 * Instances of this class allow to connect ::librepcb::Signal objects to
 * callback functions. Instead of connecting signals directly to callbacks,
 * this indirection allows to automatically disconnect connections if either
 * the sender or the receiver object is destroyed. This avoids potential
 * segfaults due to dereferencing dangling pointers.
 *
 * A slot can be connected to multiple signals if they have the same signature.
 *
 * @tparam Tsender  Type of the sender object
 * @tparam Args     Arguments passed from ::librepcb::Signal::notify() to the
 *                  callbacks
 *
 * @see ::librepcb::Signal
 */
template <typename Tsender, typename... Args>
class Slot {
  friend class Signal<Tsender, Args...>;

public:
  // Constructors / Destructor
  Slot()                  = delete;
  Slot(const Slot& other) = delete;

  /**
   * @brief Constructor
   *
   * @param callback  The callback to be called if the signal is emitted
   */
  explicit Slot(
      std::function<void(const Tsender&, Args...) noexcept>& callback) noexcept
    : mCallback(callback) {}

  /**
   * @brief Constructor
   *
   * @param obj   The object to be called if the signal is emitted
   * @param func  The member function to be called if the signal is emitted
   */
  template <typename T>
  explicit Slot(T& obj,
                void (T::*func)(const Tsender&, Args...) noexcept) noexcept
    : mCallback([=, &obj](const Tsender& s, Args... args) {
        (obj.*func)(s, args...);
      }) {}

  /**
   * @brief Destructor
   *
   * Automatically disconnects from all signals.
   */
  ~Slot() noexcept {
    for (auto signal : mSignals) {
      signal->mSlots.erase(this);
    }
  }

  // Operator Overloadings
  Slot& operator=(Slot const& other) = delete;

private:
  std::set<Signal<Tsender, Args...>*> mSignals;  ///< All signals attached to

  // The registered callback function
  std::function<void(const Tsender&, Args...) noexcept> mCallback;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb

#endif  // LIBREPCB_SIGNALSLOT_H
