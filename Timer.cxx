#include "Timer.h"

#include <iostream>
#include <sys/time.h>

using std::ostream;
using std::endl;

static double getMark()
{
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return tv.tv_sec + tv.tv_usec/1.0e6;
}

// ---------------------------------------------------------------------------
void Timer::StartEvent(const char *)
{
  this->Mark.push_back(getMark());
}

// ---------------------------------------------------------------------------
void Timer::EndEvent(const char *name)
{
  double start_t = this->Mark.back();
  double end_t = getMark();
  double delta_t = end_t - start_t;

  this->EventTime.push_back(delta_t);
  this->EventName.push_back(name);

  this->Mark.pop_back();
}

// ---------------------------------------------------------------------------
ostream &operator<<(ostream &os, const Timer &t)
{
  size_t n = t.EventTime.size();
  for (size_t i = 0; i < n; ++i)
    {
    os << "\"" << t.EventName[i] << "\", " << t.EventTime[i] << endl;
    }
  return os;
}
