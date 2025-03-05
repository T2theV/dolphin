// Copyright 2022 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <deque>
#include <fstream>
#include <optional>
#include <shared_mutex>

#include "Common/CommonTypes.h"

class PerformanceTracker
{
public:
  PerformanceTracker(const std::optional<std::string> log_name = std::nullopt,
                     const std::optional<DT> sample_window_duration = std::nullopt);
  ~PerformanceTracker();

  PerformanceTracker(const PerformanceTracker&) = delete;
  PerformanceTracker& operator=(const PerformanceTracker&) = delete;
  PerformanceTracker(PerformanceTracker&&) = delete;
  PerformanceTracker& operator=(PerformanceTracker&&) = delete;

  // Functions for recording performance information
  void Reset();
  void Count();

  // Functions for reading performance information
  DT GetSampleWindow() const;

  double GetHzAvg() const;

  DT GetDtAvg() const;
  DT GetDtStd() const;

  DT GetLastRawDt() const;

  void ImPlotPlotLines(const char* label) const;

private:
  void LogRenderTimeToFile(DT val);
  void SetPaused(bool paused);

  void PushFront(DT value);
  void PopBack();

  bool m_paused = false;
  int m_on_state_changed_handle;

  // Name of log file and file stream
  std::optional<std::string> m_log_name;
  std::ofstream m_bench_file;

  // Last time Count() was called
  TimePoint m_last_time;

  // Amount of time to sample dt's over (defaults to config)
  const std::optional<DT> m_sample_window_duration;

  // Queue + Running Total used to calculate average dt
  DT m_dt_total = DT::zero();
  std::deque<DT> m_dt_queue;

  // Average rate/time throughout the window
  DT m_dt_avg = DT::zero();  // Uses Moving Average
  double m_hz_avg = 0.0;     // Uses Moving Average + Euler Average

  // Used to initialize this on demand instead of on every Count()
  mutable std::optional<DT> m_dt_std = std::nullopt;

  // Used to enable thread safety with the performance tracker
  mutable std::shared_mutex m_mutex;
};
