/*
 * File: ChronoDotDataSource.h
 * Description:
 *   ChronoDot (i.e. DS3231) time source.
 * Author: Lauri Nurmi <lanurmi@iki.fi>
 * License: New BSD License
 */

#include "ITimeSource.h"

class ChronoDotTimeSource : public ITimeSource
{
public:
  virtual void now(uint32_t *secs, uint32_t *fract);
  virtual uint32_t timeRecv(uint32_t *secs, uint32_t *fract) const;

  void enableInterrupts();
private:
  uint32_t secondsOfRecv_;
  uint32_t fractionalSecondsOfRecv_;
};


