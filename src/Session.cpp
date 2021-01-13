
#include <fstream>
#include "../include/Session.h"
#include "../include/User.h"
#include "../include/json.hpp"
#include "../include/Watchable.h"
#include <iostream>

using namespace std;

//Constructor
Session::Session(const std::string &configFilePath)
        : content(), actionsLog(), userMap(), activeUser(), input(), terminate(false) {

    //create default user
    User *defaultUser = new LengthRecommenderUser("default");
    userMap = {{"default", defaultUser}};
    activeUser = defaultUser;

    //read the json file and update content
    using json = nlohmann::json;
    std::ifstream file(configFilePath);
    json j;
    file >> j;

    //movies
    long counter(1);
    for (unsigned int i = 0; i < j["movies"].size(); i++) {
        Watchable *movieToAdd = new Movie(counter, j["movies"][i]["name"], j["movies"][i]["length"],
                                          j["movies"][i]["tags"]);
        content.push_back(movieToAdd);
        counter++;
    }
    //tv series
    for (unsigned int k = 0; k < j["tv_series"].size(); ++k) {
        int numOfSeasons(j["tv_series"][k]["seasons"].size());
        for (int i = 0; i < numOfSeasons; ++i) {
            int numOfEpisodes(j["tv_series"][k]["seasons"][i]);
            for (int l = 0; l < numOfEpisodes; ++l) {
                Watchable *episodeToAdd = new Episode(counter, j["tv_series"][k]["name"],
                                                      j["tv_series"][k]["episode_length"], i + 1, l + 1,
                                                      j["tv_series"][k]["tags"]);
                if ((i == numOfSeasons-1) & (l == numOfEpisodes-1)) {
                    //there is no more episodes in this series, so in this case "the next episode id" is -1
                    episodeToAdd->setNextEpisodeId(-1);
                }
                else {
                    episodeToAdd->setNextEpisodeId(counter+1);
                }
                counter++;
                content.push_back(episodeToAdd);
            }
        }
    }
}

void Session::copy(const Session &other) {
    //copy content
    for (Watchable *w : other.content) {
        Watchable *newW = w->clone();
        content.push_back(newW);
    }
    //copy actionsLog
    for (BaseAction *b : other.actionsLog) {
        BaseAction *newB = b->clone();
        actionsLog.push_back(newB);
    }
    //copy userMap & update user's history
    for (pair<string, User*> x : other.userMap) {
        string userName = x.second->getName();
        User* user = x.second->initializeUser(userName);
        for (Watchable* c: x.second->get_history()){
            for (Watchable* w : content){
                if (w->toString() == c->toString()){
                    user->addToHistory(w);
                }
            }
        }
        pair<string, User*> newX = {userName, user};
        userMap.insert(newX);
    }
    //update activeUser
    std::string otherUserName = other.activeUser->getName();
    for (pair<string, User *> x : userMap) {
        if (x.first == otherUserName) {
            activeUser = x.second;
        }
    }

    input = other.getInput();
    terminate = false;
}

void Session::clear() {
    //delete content pointers
    for (unsigned int i = 0; i < content.size(); ++i) {
        delete (content[i]);
        content[i] = nullptr;
    }
    content.clear();

    //delete actionsLog pointers
    for (unsigned int j = 0; j < actionsLog.size(); ++j) {
        delete (actionsLog[j]);
        actionsLog[j] = nullptr;
    }
    actionsLog.clear();

    //delete userMap pointers
    for (pair<std::string, User*> x : userMap) {
        delete x.second;
        x.second = nullptr;
    }
    userMap.clear();
    //delete activeUser pointer
    activeUser = nullptr;
}

void Session::steal(Session &other) {
    //copy content
    content.swap(other.content);

    //copy actionsLog
    actionsLog.swap(other.actionsLog);

    //copy userMap
    userMap.swap(other.userMap);

    //update activeUser
    activeUser = userMap[other.getActiveUser()->getName()];

    other.activeUser= nullptr;
    input = other.getInput();
    terminate = false;
}

//Copy Constructor
Session::Session(const Session &other) : content(), actionsLog(), userMap(), activeUser(), input(), terminate(false) {
    copy(other);
}

//Copy Assignment
Session &Session::operator=(const Session &other) {
    if (this != &other) {
        clear();
        copy(other);
    }
    return *this;
}

//Move Constructor
Session::Session(Session &&other) : content(), actionsLog(), userMap(), activeUser(), input(), terminate(false) {
    steal(other);
}

//Move Assignment
Session &Session::operator=(Session &&other) {
    if (this != &other) {
        clear();
        steal(other);
    }
    return *this;
}

//Destructor
Session::~Session() {
    clear();
}

std::unordered_map<std::string, User *> Session::getUserMap() const {
    return userMap;
}

std::string Session::getInput() const {
    return input;
}

std::vector<BaseAction *> Session::getActionsLog() const {
    return actionsLog;
}

void Session::addToActionsLog(BaseAction *act) {
    actionsLog.push_back(act);
}

void Session::addToUserMap(std::pair<std::string, User *> user) {
    userMap.insert(user);
}

void Session::eraseFromUserMap(std::string userName) {
    userMap.erase(userName);
    userName.clear();
}

User *Session::getActiveUser() const {
    return activeUser;
}

void Session::setInput(std::string newInput) {
    input = newInput;
}

void Session::setTerminate(bool ifExit) {
    terminate = ifExit;
}

void Session::setActiveUser(User *otherUser) {
    activeUser = otherUser;
}

std::vector<Watchable*> Session::getContent() const {
    return content;
}

void Session::start() {
    cout << "SPLFLIX is now on!" << endl;
    setTerminate(false);
    while (terminate == false) {
        string commandUser;
        getline(cin, commandUser);
        BaseAction *newAction;
        string theAction(commandUser.substr(0, commandUser.find(" ")));
        input = commandUser.substr(commandUser.find(" ") + 1);
        if (theAction == "createuser") {
            newAction = new CreateUser();
            newAction->act(*this);
        } else if (theAction == "changeuser") {
            newAction = new ChangeActiveUser();
            newAction->act(*this);
        } else if (theAction == "deleteuser") {
            newAction = new DeleteUser();
            newAction->act(*this);
        } else if (theAction == "dupuser") {
            newAction = new DuplicateUser();
            newAction->act(*this);
        } else if (theAction == "content") {
            newAction = new PrintContentList();
            newAction->act(*this);
        } else if (theAction == "watchhist") {
            newAction = new PrintWatchHistory();
            newAction->act(*this);
        } else if (theAction == "watch") {
            newAction = new Watch();
            newAction->act(*this);
        } else if (theAction == "log") {
            newAction = new PrintActionsLog();
            newAction->act(*this);
        } else if (theAction == "exit") {
            newAction = new Exit();
            newAction->act(*this);
        } else{
            cout << "this action isn't valid in SPLFLIX" << endl;
        }
    }
}

