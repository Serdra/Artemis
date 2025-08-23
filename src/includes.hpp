#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <mutex>
#include <thread>
#include <pthread.h>
#include <cstring>

#include "chess/chess.hpp"
#include "chess/mapping.hpp"

#include "xorshift.hpp"

#include "PUCT.hpp"
#include "PUCT.cpp"

#include "NNUE.hpp"
#include "NNUE.cpp"

#include "Search.hpp"
#include "Search.cpp"

#include "Writer.hpp"

#include "Generation.hpp"
#include "Generation.cpp"

#include "UCI.cpp"