#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "Action.h"

class User;
class Watchable;

class Session{
public:
    void start();

    //RUL OF 5
    Session(const std::string &configFilePath);
    Session(const Session &other);
    Session& operator=(const Session& other);
    Session(Session&& other);
    Session& operator=(Session&& other);
    ~Session();
    void copy(const Session &other);
    void clear();
    void steal(Session &other);

    //GETTERS & SETTERS
    std::vector<Watchable*> getContent() const;
    std::vector<BaseAction*> getActionsLog() const;
    std::unordered_map<std::string,User*> getUserMap() const;
    User* getActiveUser() const;
    std::string getInput() const;
    void addToActionsLog(BaseAction *act);
    void addToUserMap(std::pair<std::string, User*> user);
    void eraseFromUserMap(std::string userName);
    void setActiveUser(User* oterUser);
    void setTerminate(bool ifExit);
    void setInput(std::string newInput);


private:
    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<std::string,User*> userMap;
    User* activeUser;
    std::string input;
    bool terminate;
};

#endif
