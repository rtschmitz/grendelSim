/*
 * grUserTrackInformation.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */


#include "grUserTrackInformation.hh"

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grUserTrackInformation::grUserTrackInformation()
  :status(active),reflections(0),intReflections(0),forcedraw(false), trackID(-1)
{
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grUserTrackInformation::~grUserTrackInformation()
{
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void grUserTrackInformation::AddTrackStatusFlag(int s)
{
  if(s&active) //track is now active
    status&=~inactive; //remove any flags indicating it is inactive
  else if(s&inactive) //track is now inactive
    status&=~active; //remove any flags indicating it is active
  status|=s; //add new flags
}
