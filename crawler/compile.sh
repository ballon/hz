#!/bin/bash

g++ -Wall -std=c++11  -pthread  crawler.cpp -o crawler -lboost_regex -lcurl
