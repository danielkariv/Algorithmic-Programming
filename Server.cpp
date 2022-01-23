
#include "Server.h"

string socketIO::read(){
    char c=0;
    size_t i=0;
    string s="";
    recv(clientID,&c,sizeof(char),0);
    while(c!='\n'){
        s+=c;
        recv(clientID,&c,sizeof(char),0);
    }
    return s;
}
void socketIO::write(string text){
    const char* txt=text.c_str();
    send(clientID,txt,strlen(txt),0);
}

void socketIO::write(float f){
    // load float to a stream and then write the string given from it.
    ostringstream ss;
    ss <<f;
    string s(ss.str());
    write(s);
}

void socketIO::read(float* f){
    *f = atof(read().c_str());
    //recv(clientID,f,sizeof(float),0);
    // it will be already in the string line
}


Server::Server(int port)throw (const char*) {
    stopped=false;
    serverfd=socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd < 0)
        throw "socket failed";

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if(::bind(serverfd, (struct sockaddr*)&server, sizeof(server)) < 0)
        throw "bind failure";

    if(listen(serverfd, 3) < 0)
        throw "listen failure";
}

void sigHandler(int sigNum){
    cout<<"Client connection has been timeout signal. error:" << sigNum <<endl;
}

void Server::start(ClientHandler& ch)throw(const char*){
    t=new thread([&ch,this](){
        signal(SIGALRM,sigHandler);
        while(!stopped){
            socklen_t clientSize=sizeof(client);
            alarm(5); // set time-out for hanging clients.
            // accept call is Blocking till we get new client. Can be killed by close(serverfd).
            int aClient = accept(serverfd, (struct sockaddr*)&client, &clientSize); // race condition.
            // aClient negative value is error value.
            if(aClient>0){
                ch.handle(aClient);
                close(aClient);
            }
            alarm(0);
        }
        close(serverfd);
    });
}

void Server::stop(){
    stopped=true;
    close(serverfd); // close socket so if we stuck in accept call, it ends the blocking,  and finish running.
    t->join(); // do not delete this!
}

Server::~Server() {
    // TODO Auto-generated destructor stub
}

