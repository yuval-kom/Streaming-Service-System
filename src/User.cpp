#include <algorithm>
#include "../include/User.h"
#include "../include/Session.h"
#include "../include/Watchable.h"
#include <cmath>
using namespace std;

//Constructor
User::User(const std::string& name): history(), name(name){}

//Destructor
User::~User() {
   clear();
}

void User::clear(){
    history.clear();
}

void User::steal(const User &other){
    name = other.getName();
    for (Watchable* w : other.get_history()) {
        history.push_back(w);
    }
}

//Copy Constructor
User::User(const User &other): history(), name(other.getName()){
    steal(other); //steal and don't copy because both are pointing to the same content list
}

//Copy Assignment
User& User::operator=(const User& other){
    if (this != &other){
        steal(other);
    }
    return *this;
}

//Move Constructor
User::User(User&& other): history(), name(other.getName()){
    steal(other);

}
//Move Assignment
User& User::operator= (User&& other){
    if (this != &other){
        steal(other);
    }
    return *this;
}

std::string User::getName() const{
	return name;
}

std::vector<Watchable*> User::get_history() const {
    return history;
}

void User::addToHistory(Watchable* watch){
    history.push_back(watch);
}

LengthRecommenderUser::LengthRecommenderUser(const std::string& name): User(name), sum(0) {}

std::vector<Watchable*> User::notSeenContent(Session &s){
    //returns the content that not seen by the active user
    std::vector<Watchable*> content = s.getContent();
    std::vector<Watchable*> watchHistory = s.getActiveUser()->get_history();
    std::vector<Watchable*> notSeenContent;
    std::vector<Watchable*>::iterator it;
    for (unsigned int i = 0; i < content.size(); i++) {
        it = std::find(watchHistory.begin(), watchHistory.end(), content[i]);
        if (it == watchHistory.end()) {
            //content[i] doesn't exist in the user watch history
            notSeenContent.push_back(content[i]);
        }
    }
    return notSeenContent;
}

Watchable* LengthRecommenderUser::getRecommendation(Session& s){
    //calculate the average time
    double average = sum/history.size();
    //looking for content with the length that closest to the average time, that the user DIDN'T saw
    std::vector<Watchable*> notSeenContent(User::notSeenContent(s));
    double minGap(HUGE_VAL);
    Watchable* theRecommendContent(nullptr);
    for (unsigned int i=0; i < notSeenContent.size(); i++) {
        int gap = notSeenContent[i]->getLength();
        if (abs(average - gap) < minGap) {
            minGap = abs(average - gap);
            theRecommendContent = notSeenContent[i];
        }
    }
    return theRecommendContent;
 }

RerunRecommenderUser::RerunRecommenderUser(const std::string& name): User(name), counter(0) {}

Watchable* RerunRecommenderUser::getRecommendation(Session& s){
	counter++;
	int historySize = s.getActiveUser()->get_history().size();
	return s.getActiveUser()->get_history()[(counter-1) % historySize];
}

GenreRecommenderUser::GenreRecommenderUser(const std::string& name):User(name){}

Watchable* GenreRecommenderUser::getRecommendation(Session& s){
    std::vector<std::pair<std::string, int>> tagsAndAppearance;
    std::vector<Watchable*> history = s.getActiveUser()->get_history();

    //for every content in the history watch: checks if her tags exist and update her appearance number
    for (unsigned int  i = 0;  i < history.size(); ++i) {
        std::vector<std::string> tags = history[i]->getTags();
        for (unsigned int j = 0; j < tags.size(); ++j) {
            std::string tag = tags[j];
            if (tagsAndAppearance.size() == 0) {
                tagsAndAppearance.push_back(pair<string, int>(tags[j], 1));
            }
            else {
                for (unsigned int k = 0; k < tagsAndAppearance.size(); ++k) {
                    if (tagsAndAppearance[k].first == tag) {
                        //the tag is in the list, so we just increase his counter
                        tagsAndAppearance[k].second++;
                    } else if (k == tagsAndAppearance.size() - 1) {
                        //the tag is not in the list, so we add him with counter = 1
                        tagsAndAppearance.push_back(pair<string, int>(tag, 1));
                        break;
                    }
                }
            }
        }
    }

    //finish to update tagsAndAppearance
    //find the popular tags
    std::vector<std::string> popularTags;
    //find the biggest counter
    int biggestCounter(1);
    for(pair<string,int> p : tagsAndAppearance){
        if(p.second > biggestCounter){
            biggestCounter = p.second;
        }
    }

    for(int tagsCounter = biggestCounter; tagsCounter > 0; tagsCounter--){
        popularTags = {};
        for (unsigned int m = 0; m < tagsAndAppearance.size(); ++m) {
            if (tagsAndAppearance[m].second == tagsCounter){
                popularTags.push_back(tagsAndAppearance[m].first);
            }
        }
        //sort the popularTags list in lexicographic order
        std::string temp;
        for (unsigned int n = 0; n < popularTags.size(); ++n) {
            for (unsigned int i = n + 1; i < popularTags.size(); ++i) {
                if (popularTags[n] > popularTags[i]) {
                    temp = popularTags[n];
                    popularTags[n] = popularTags[i];
                    popularTags[i] = temp;
                }
            }
        }

        //return content that the user didn't saw with one of the popular tags, if exist
        std::vector<Watchable*> notSeenContent = s.getActiveUser()->notSeenContent(s);
        for (unsigned int j = 0; j < popularTags.size(); ++j) {
            string thePopularTag(popularTags[j]);
            for (int k = 0; k < (int)notSeenContent.size(); ++k) {
                std::vector<string> tags(notSeenContent[k]->getTags());
                for (int i = 0; i < (int)tags.size(); ++i) {
                    if (tags[i] == thePopularTag) {
                        return notSeenContent[k];
                    }
                }
            }
        }

    }
    return nullptr;
}

LengthRecommenderUser* LengthRecommenderUser::initializeUser(std::string& name) const{
   return new LengthRecommenderUser(name);
}

RerunRecommenderUser* RerunRecommenderUser::initializeUser(std::string& name) const{
    return new RerunRecommenderUser(name);
}

GenreRecommenderUser* GenreRecommenderUser::initializeUser(std::string& name) const{
    return new GenreRecommenderUser(name);
}

void User::setSum(int contentLength){}

void LengthRecommenderUser::setSum(int contentLength){
    sum = sum + contentLength;
}
