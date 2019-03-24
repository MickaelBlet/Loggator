#!/bin/sh

# CXX=g++-4.8 make re -j && CXX=g++-4.8 make release exe_test -j && \

CXX=g++-5 make re -j && CXX=g++-5 make release exe_test -j && \
CXX=g++-6 make re -j && CXX=g++-6 make release exe_test -j && \
CXX=g++-7 make re -j && CXX=g++-7 make release exe_test -j && \
CXX=g++-8 make re -j && CXX=g++-8 make release exe_test -j && \
CXX=clang++-3.9 make re -j && CXX=clang++-3.9 make release exe_test -j && \
CXX=clang++-4.0 make re -j && CXX=clang++-4.0 make release exe_test -j && \
CXX=clang++-5.0 make re -j && CXX=clang++-5.0 make release exe_test -j && \
CXX=clang++-6.0 make re -j && CXX=clang++-6.0 make release exe_test -j && \
CXX=clang++-7 make re -j && CXX=clang++-7 make release exe_test -j