#ifndef ACTION_H_
#define ACTION_H_

#include <string>
#include <iostream>
#include <vector>

class Session;

enum ActionStatus{
    PENDING, COMPLETED, ERROR
};

class BaseAction{
public:
    BaseAction();
    virtual ~BaseAction() = default;
    virtual void act(Session& sess) = 0;
    virtual std::string toString() const = 0;
    std::string toStringStatus() const;
    std::string printTags(std::vector<std::string>);
    virtual BaseAction* clone() = 0;

protected:
    void complete();
    void error(const std::string& errorMsg);
    std::string getErrorMsg() const;

private:
    std::string errorMsg;
    ActionStatus status;
};

class CreateUser  : public BaseAction {
public:
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual CreateUser* clone();
};

class ChangeActiveUser : public BaseAction {
public:
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual ChangeActiveUser* clone();
};

class DeleteUser : public BaseAction {
public:
    virtual void act(Session & sess);
    virtual std::string toString() const;
    virtual DeleteUser* clone();
};

class DuplicateUser : public BaseAction {
public:
    virtual void act(Session & sess);
    virtual std::string toString() const;
    virtual DuplicateUser* clone();
};

class PrintContentList : public BaseAction {
public:
    virtual void act (Session& sess);
    virtual std::string toString() const;
    virtual PrintContentList* clone();
};

class PrintWatchHistory : public BaseAction {
public:
    virtual void act (Session& sess);
    virtual std::string toString() const;
    virtual PrintWatchHistory* clone();
};

class Watch : public BaseAction {
public:
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual Watch* clone();
};

class PrintActionsLog : public BaseAction {
public:
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual PrintActionsLog* clone();
};

class Exit : public BaseAction {
public:
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual Exit* clone();
};

#endif