//
// Created by wolverindev on 28.08.16.
//

#ifndef CBUNGEE_CONFIGURATION_H
#define CBUNGEE_CONFIGURATION_H

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <string.h>
#include <fstream>
#include <iostream>

using namespace std;

class Configuration {
    public:
        static Configuration *instance;

        Configuration(string &filename) : fileName(filename) {

        }

        ~Configuration(){
            config = NULL;
            errors.clear();
        }

        void loadConfig();
        void saveConfig();

        bool isValid() {
            return errors.empty();
        }

        vector<string> getErrors() {
            return errors;
        }

        YAML::Node config;
    private:
        string &fileName;
        vector<string> errors;
};


#endif //CBUNGEE_CONFIGURATION_H
