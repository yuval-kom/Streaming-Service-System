#include <sstream>
#include "../include/Action.h"
#include "../include/User.h"
#include "../include/Session.h"
#include "../include/Watchable.h"
using namespace std;

//Constructor
BaseAction::BaseAction(): errorMsg(""), status(PENDING) {}

std::string BaseAction::getErrorMsg() const{
    return errorMsg;
}

void BaseAction::complete(){
    status = COMPLETED;
}

std::string BaseAction::toStringStatus() const {
    if (status == PENDING){
        return  "PENDING";
    }
    else if (status == COMPLETED){
        return "COMPLETED";
    }
    else{
        return "";
    }
}

void BaseAction::error(const std::string& _errorMsg){
    status = ERROR;
	errorMsg = _errorMsg;
	cout << errorMsg << endl;
}

CreateUser* CreateUser::clone(){
    return new CreateUser(*this);
}

void CreateUser::act(Session& sess){
    string input = sess.getInput();
    string name = input.substr(0, input.find(" "));
    string recAlgo = input.substr(input.find(" ")+1);
    if ((recAlgo != "len") & (recAlgo != "rer") & (recAlgo != "gen")){
        error("ERROR: Algorithm type is not valid");
    }
    else {
        //check if already exist user with the same name
        std::unordered_map<std::string, User*>::const_iterator iter = sess.getUserMap().find(name);
        if (iter == sess.getUserMap().end()) {
            //doesn't exist
            User* newUser;
            if (recAlgo == "len"){
                newUser = new LengthRecommenderUser(name);
            }
            else if (recAlgo == "rer"){
                newUser = new RerunRecommenderUser(name);
            }
            else{
                newUser = new GenreRecommenderUser(name);
            }
            sess.addToUserMap({name, newUser});
            complete();
        } else {
            // found
            error("ERROR: the new user name is already taken");
        }
    }
    sess.addToActionsLog(this);
}

std::string CreateUser::toString()const {
	return "CreateUser " + this->toStringStatus() + this->getErrorMsg();
}

ChangeActiveUser* ChangeActiveUser::clone(){
    return new ChangeActiveUser(*this);
}

void ChangeActiveUser::act(Session& sess) {
    string userName = sess.getInput();
    //check if exist user with that name
    std::unordered_map<string,User*> userMap = sess.getUserMap();
    bool isExist(false);
    for (pair<string, User*> x : userMap){
        if (x.first == userName){
            //user found
            sess.setActiveUser(x.second);
            complete();
            isExist=true;
            break;
        }
    }
    if (!isExist){
        //user didn't found
        error("ERROR: User doesn't exist");
    }
    sess.addToActionsLog(this);
}

std::string ChangeActiveUser::toString() const{
	return "ChangeActiveUser " + this->toStringStatus() + this->getErrorMsg();
}

DeleteUser* DeleteUser::clone(){
    return new DeleteUser(*this);
}

void DeleteUser::act(Session &sess){
    string userName = sess.getInput();
    if (userName != sess.getActiveUser()->getName()) {
        bool isExist(false);
        //check if exist user with that name
        for (pair<string, User *> user : sess.getUserMap()) {
            if (user.first == userName) {
                //user found
                isExist = true;
                sess.eraseFromUserMap(userName);
                delete (user.second);
                complete();
                break;
            }
        }
        if (!isExist) {
            //user didn't found
            error("ERROR: User doesn't exist");
        }
    }
    else{
        //userToDelete is active user
        error("ERROR: It's impossible to delete the active user");
    }
    sess.addToActionsLog(this);
}

std::string DeleteUser::toString() const{
	return "DeleteUser " + this->toStringStatus() + this->getErrorMsg();
}

DuplicateUser* DuplicateUser::clone(){
    return new DuplicateUser(*this);
}

void DuplicateUser::act(Session & sess){
    string input = sess.getInput();
    string originalUserName = input.substr(0, input.find(" "));
    string newUserName = input.substr(input.find(" ")+1);
    std::unordered_map<string, User *> userMap = sess.getUserMap();
    bool isValid(true);
    bool isExist(false);
    User *myUser;

    for (auto s : userMap) {
        if (s.first == newUserName) {
            isValid = false;
            error("ERROR: the name of new user is taken");
            break;
        }
    }
    if (isValid) {
        for (auto s : userMap) {
            if (s.first == originalUserName) {
                myUser = s.second->initializeUser(newUserName);
                sess.addToUserMap({newUserName, myUser});
                for (unsigned int i = 0; i < s.second->get_history().size(); ++i) {
                    myUser->addToHistory(s.second->get_history()[i]);
                }
                isExist = true;
                break;
            }
        }
        if (!isExist) {
            error("ERROR: the original user doesn't exist");
        } else {
            complete();
        }
    }

    sess.addToActionsLog(this);
}

std::string DuplicateUser::toString() const{
	return "DuplicateUser " + this->toStringStatus() + this->getErrorMsg();
}

string BaseAction::printTags(std::vector<std::string> tags){
    string output = "[";
    for (unsigned int i = 0; i < tags.size(); ++i) {
        if(i == tags.size()-1){
            output = output + tags[i] + "]";
        }
        else {
            output = output + tags[i] + ", ";
        }
    }
    return  output;
}

PrintContentList* PrintContentList::clone(){
    return new PrintContentList(*this);
}

void PrintContentList::act (Session& sess){
	std::vector<Watchable*> content = sess.getContent();
	for (unsigned  int i=0; i < content.size(); i++){
		cout << i+1 << ". " << content[i]->toString() << " " << content[i]->getLength() << " minutes " << printTags(content[i]->getTags()) << endl;
	}
	complete();
    sess.addToActionsLog(this);
}
std::string PrintContentList::toString() const{
	return "PrintContentList " + this->toStringStatus() + this->getErrorMsg();
}

PrintWatchHistory* PrintWatchHistory::clone(){
    return new PrintWatchHistory(*this);
}

void PrintWatchHistory::act (Session& sess){
	User* activeUser = sess.getActiveUser();
	cout << "watch history for " << activeUser->getName() << endl;
	std::vector<Watchable*> watchHistory(sess.getActiveUser()->get_history());
	for(unsigned int i=0; i<watchHistory.size(); i++){
        std::string iToString = std::to_string(i+1);
		cout << iToString << ". " << watchHistory[i]->toString() << endl;
	}
	complete();
    sess.addToActionsLog(this);
}
std::string PrintWatchHistory::toString() const{
	return "PrintWatchHistory " + this->toStringStatus() + this->getErrorMsg();
}

Watch* Watch::clone(){
    return new Watch(*this);
}

void Watch::act(Session& sess){
	//read the content id
    string input = sess.getInput();
    string contentIdString = input.substr(0, input.find(" "));
    std::stringstream convert(contentIdString);
    unsigned int contentId;
    convert >> contentId;
    contentId--;
    sess.addToActionsLog(this);
    if(contentId > sess.getContent().size()){
        error("ERROR: number of content not valid");
    }
    else {
        //add it to the watch history of the active user
        sess.getActiveUser()->addToHistory(sess.getContent()[contentId]);
        sess.getActiveUser()->setSum(sess.getContent()[contentId]->getLength());
        cout << "Watching " << sess.getContent()[contentId]->toString() << endl;
        complete();
        Watchable *recommendContent;
        if (sess.getContent()[contentId]->getNextWatchable(sess) == nullptr){
            //the content that showed is a movie
            recommendContent = sess.getActiveUser()->getRecommendation(sess);
        }
        else{
            //the content that showed is an episode
            recommendContent = sess.getContent()[contentId]->getNextWatchable(sess);
        }
        // found a recommendation
        if (recommendContent != nullptr) {
            cout << "We recommend watching " << recommendContent->toString() << ", continue watching? [y/n]" << endl;
            //check what the user answered...
            string commandUser;
            getline(cin, commandUser);
            if (commandUser == "y") {
                std::string recommendContentId = std::to_string(recommendContent->getId());
                sess.setInput(recommendContentId);
                Watch *newWatch = new Watch();
                newWatch->act(sess);
            }
        }
    }
}

std::string Watch::toString() const{
	return "Watch " + this->toStringStatus() + this->getErrorMsg();
}

PrintActionsLog* PrintActionsLog::clone(){
    return new PrintActionsLog(*this);
}

void PrintActionsLog::act(Session& sess){
	vector<BaseAction*> actionsLog = sess.getActionsLog();
	for (unsigned int i=0; i < actionsLog.size(); i++){
		cout << actionsLog[i]->toString() << endl;
	}
    complete();
    sess.addToActionsLog(this);
}
std::string PrintActionsLog::toString() const{
	return "PrintActionsLog " + this->toStringStatus() + this->getErrorMsg();
}

Exit* Exit::clone(){
    return new Exit(*this);
}

void Exit::act(Session& sess){
	sess.setTerminate(true);
    complete();
    sess.addToActionsLog(this);
}
std::string Exit::toString() const{
	return "Exit " + this->toStringStatus() + this->getErrorMsg();
}
