#include "../include/Watchable.h"
#include "../include/Session.h"
#include "../include/User.h"

using namespace std;

//Constructor
Watchable::Watchable(long id, int length, const std::vector<std::string>& tags): id(id), length(length), tags(tags) {}

//Destructor
Watchable::~Watchable(){}


long Watchable::getId() const{
	return id;
}

int Watchable::getLength() const{
	return length;
}

std::vector<std::string> Watchable::getTags() const{
	return tags;
}

Movie::Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags): Watchable(id, length, tags), name(name){}

std::string Movie::toString() const{
	return name;
}

Watchable* Movie::getNextWatchable(Session& sess) const{
    return nullptr;
}

Episode::Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags) : Watchable(id, length, tags), seriesName(seriesName), season(season), episode(episode), nextEpisodeId(id+1){}

std::string Episode::toString() const{
	return seriesName + " S" + std::to_string(season) + "E" + std::to_string(episode);
}
Watchable* Episode::getNextWatchable(Session& sess) const{
    if(nextEpisodeId == -1){
        return nullptr;
    }
    return sess.getContent()[nextEpisodeId-1];
}

Movie* Movie::clone(){
    return new Movie(*this);
}

Episode* Episode::clone(){
    return new Episode(*this);
}

void Watchable::setNextEpisodeId(long theNextEpisode){}

void Episode::setNextEpisodeId(long theNextEpisode){
    nextEpisodeId = theNextEpisode;
}