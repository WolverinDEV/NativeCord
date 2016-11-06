//
// Created by wolverindev on 06.11.16.
//

#ifndef NATIVECORD_TERMINAL_H
#define NATIVECORD_TERMINAL_H

#include <pthread.h>
#include <zconf.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <sstream>
#include <csignal>

using namespace std;
class Terminal {
    public:
        static Terminal* instance;

        ~Terminal(){
            stopReader();
        }

        void static resetTerminal(){
            tcsetattr(0, TCSANOW, &orig_termios);
        }

        void static setupTerminal(){
            struct termios new_termios;

            /* take two copies - one for now, one for later */
            tcgetattr(0, &orig_termios);
            memcpy(&new_termios, &orig_termios, sizeof(new_termios));

            /* register cleanup handler, and set the new terminal mode */
            atexit(resetTerminal);
            cfmakeraw(&new_termios);
            tcsetattr(0, TCSANOW, &new_termios);
        }

        void startReader(){
            if(readerThread == nullptr){
                readerThread = new pthread_t;
                pthread_create(readerThread, nullptr, &readerTask, this);
            }
        }

        void stopReader(){
            if(readerThread != nullptr){
                pthread_cancel(*readerThread);
                pthread_join(*readerThread, nullptr);
                delete readerThread;
                readerThread = nullptr;
            }
        }

        void printMessage(string message, bool updateLine = true){
            pthread_mutex_lock(&mutex);
            cout << "\r" << message;
            int i = message.size();
            while(i < promt.size()+currentLine.size()){
                i++;
                cout << " ";
            }
            cout << endl;

            if(updateLine)
                resetLine(false);
            pthread_mutex_unlock(&mutex);
        }

        void resetLine(bool mutex = true){
            if(mutex)
                pthread_mutex_lock(&(this->mutex));
            stringstream ss;
            ss << "\r" << promt << string(&(currentLine[0]), currentLine.size());
            int size = ss.str().length();
            //ss << "\033["+to_string(size)+"D";
            //for(int i = ss.str().length();i<this->lastLineSize;i++)
            //    ss << " ";
            this->lastLineSize = size;
            cout << ss.str();
            cout.flush();
            if(mutex)
                pthread_mutex_unlock(&(this->mutex));
        }

    protected:
        virtual void handleLine(string message){
            printMessage("Having line: "+message, false);
            if(message.compare("stop") == 0 || message.compare("end") == 0 || message.compare("exit") == 0){
                printMessage("§cStopping nativecord");
            }
        }
    private:
        static struct termios orig_termios;

        static void readerTask(void* terminalVPtr){
            //int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
            //fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

            Terminal* terminal = (Terminal*) terminalVPtr;
            char current;
            while (1) {
                while(terminal->kbhit());
                current = (char) terminal->readNextCharacter();
                //std::cin.read(&current, sizeof(char));
                switch (current){
                    case 13: //Return character
                        terminal->handleLine(string(&(terminal->currentLine[0]), terminal->currentLine.size()));
                        terminal->currentLine.clear();
                        terminal->resetLine();
                        break;
                    case 11:
                        if(terminal->readCharacter() == 3){
                            raise(SIGABRT);
                        }
                        break;
                    default:
                        terminal->printMessage("having character: "+to_string(current));
                        terminal->currentLine.push_back(current);
                        terminal->resetLine();
                        break;
                }
            }
        }

        int readCharacter(){
            int readed = readNextCharacter();
            if(readed > 0)
                return readed;
            while (kbhit()){
                usleep(1000);
            }
            return readNextCharacter();
        }

        int kbhit()
        {
            struct timeval tv = { 0L, 0L };
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(0, &fds);
            return select(1, &fds, NULL, NULL, &tv);
        }

        int readNextCharacter()
        {
            int r;
            unsigned char c;
            if ((r = read(0, &c, sizeof(c))) < 0) {
                return r;
            } else {
                return c;
            }
        }
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        string promt = "";
        vector<char> currentLine;
        int lastLineSize = 0;
        pthread_t* readerThread = nullptr;
};


#endif //NATIVECORD_TERMINAL_H
