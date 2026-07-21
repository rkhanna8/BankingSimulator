cmake_minimum_required(VERSION 4.2)
project(BankingSimulator)

set(CMAKE_CXX_STANDARD 20)

add_executable(BankingSimulator main.cpp
        Account.cpp
        Account.h
        Transaction.cpp
        Transaction.h
        AccountManager.cpp
        AccountManager.h
        Utils.cpp
        Utils.h)
