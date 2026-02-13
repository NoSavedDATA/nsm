#pragma once

#include <ctype.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>


class ITokenizer {
  public:
    virtual ~ITokenizer() = default;
  
    virtual int GetNextToken() = 0;
    // virtual Value *codegen(Value *scope_struct) = 0;
    // virtual bool GetNeedGCSafePoint();
};


class LibTokenizer : public ITokenizer {
    public:
    LibTokenizer();

    int GetNextToken() override;
};
