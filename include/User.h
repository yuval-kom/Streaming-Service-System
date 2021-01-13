#ifndef USER_H_
#define USER_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
class Watchable;
class Session;

class User{
public:
    //RUL OF 5
    User(const std::string& name);
    User(const User &other);
    User& operator=(const User& other);
    User(User&& other);
    User& operator=(User&& other);
    virtual ~User();
    void clear();
    void steal(const User &other);

    //GETTERS & SETTERS
    virtual Watchable* getRecommendation(Session& s) = 0;
    std::string getName() const;
    std::vector<Watchable*> get_history() const;
    std::vector<Watchable*> notSeenContent(Session &s);
    void addToHistory(Watchable* watch);
    virtual void setSum(int contentLength);

    virtual User* initializeUser(std::string& name) const = 0;

protected:
    std::vector<Watchable*> history;

private:
    std::string name;
};


class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual LengthRecommenderUser* initializeUser(std::string& name) const;
    virtual void setSum(int contentLength);
private:
	int sum; //sum of the content's length in the user's watch history
};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual RerunRecommenderUser* initializeUser(std::string& name) const;
private:
	int counter; //count the number of contents that already recommend
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual GenreRecommenderUser* initializeUser(std::string& name) const;
private:
};
#endif
