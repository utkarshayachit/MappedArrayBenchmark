#ifndef Timer_h
#define Timer_h

#include <vector>
#include <string>
#include <iosfwd>

class Timer
{
public:
    void StartEvent(const char *name=nullptr);
    void EndEvent(const char *name);

private:
    std::vector<double> Mark;
    std::vector<std::string> EventName;
    std::vector<double> EventTime;
private:
    friend std::ostream &operator<<(std::ostream &os, const Timer &t);
};

std::ostream &operator<<(std::ostream &os, const Timer &t);

#endif
