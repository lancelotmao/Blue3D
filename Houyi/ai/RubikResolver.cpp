//
//  RubikResolver.cpp
//  Houyi
//
//  Created by Lance Mao on 3/3/13.
//  Copyright (c) 2013 Lance Mao. All rights reserved.
//

#include "RubikResolver.h"
#include "HouyiLog.h"
#include <sstream>
#include <set>
#include <map>
#include <string>

namespace Houyi
{
	const char RubikResolver::GOAL[20][6] = {
		{'n', 'n', 'o', 'y', 'b', 'n'},         //0
		{'n', 'n', 'n', 'y', 'b', 'n'},         //1
		{'n', 'n', 'n', 'y', 'b', 'r'},         //2
		{'n', 'n', 'o', 'y', 'n', 'n'},         //3
		{'n', 'n', 'n', 'y', 'n', 'r'},         //4
		{'n', 'w', 'o', 'y', 'n', 'n'},         //5
		{'n', 'w', 'n', 'y', 'n', 'n'},         //6
		{'n', 'w', 'n', 'y', 'n', 'r'},         //7

		{'n', 'n', 'o', 'n', 'b', 'n'},         //8
		{'n', 'n', 'n', 'n', 'b', 'r'},         //9
		{'n', 'w', 'o', 'n', 'n', 'n'},         //10
		{'n', 'w', 'n', 'n', 'n', 'r'},         //11

		{'g', 'n', 'o', 'n', 'b', 'n'},         //12
		{'g', 'n', 'n', 'n', 'b', 'n'},         //13
		{'g', 'n', 'n', 'n', 'b', 'r'},         //14
		{'g', 'n', 'o', 'n', 'n', 'n'},         //15
		{'g', 'n', 'n', 'n', 'n', 'r'},         //16
		{'g', 'w', 'o', 'n', 'n', 'n'},         //17
		{'g', 'w', 'n', 'n', 'n', 'n'},         //18
		{'g', 'w', 'n', 'n', 'n', 'r'},         //19
	};

    static const float ROTATE_WEIGH = 100;
	const char RubikResolver::FACE_COLOR[6] = {'g', 'w', 'o', 'y', 'b', 'r'};
	const int RubikResolver::CORNER_IDX[8] = {0, 2, 5, 7, 12, 14, 17, 19};
	const int RubikResolver::EDGE_IDX[12] = {1, 3, 4, 6, 8, 9, 10, 11, 13, 15, 16, 18};

	const char RubikResolver::STAGE0_MOVES[4][24][24] = {
        // GW
        {
            {1,  'n', 'n', 'n', 'g', 'w', 'n', 4, YL, YL, WR, WR},
            {3,  'n', 'n', 'w', 'g', 'n', 'n', 3, YL, WR, WR},
            {4,  'n', 'n', 'n', 'g', 'n', 'w', 3, YR, WR, WR},
            {6,  'n', 'w', 'n', 'g', 'n', 'n', 2, WR, WR},
            {8,  'n', 'n', 'g', 'n', 'w', 'n', 3, OL, OL, WL},
            {9,  'n', 'n', 'n', 'n', 'w', 'g', 3, RR, RR, WR},
            {10, 'n', 'w', 'g', 'n', 'n', 'n', 1, WL},
            {11, 'n', 'w', 'n', 'n', 'n', 'g', 1, WR},
            {13, 'g', 'n', 'n', 'n', 'w', 'n', 2, GL, GL},
            {15, 'g', 'n', 'w', 'n', 'n', 'n', 1, GR},
            {16, 'g', 'n', 'n', 'n', 'n', 'w', 1, GL},
            
            {1,  'n', 'n', 'n', 'w', 'g', 'n', 3, YL, OR, WL},
            {3,  'n', 'n', 'g', 'w', 'n', 'n', 2, OR, WL},
            {4,  'n', 'n', 'n', 'w', 'n', 'g', 2, RL, WR},
            {6,  'n', 'g', 'n', 'w', 'n', 'n', 3, YR, OR, WL},
            {8,  'n', 'n', 'w', 'n', 'g', 'n', 4, OR, YL, WR, WR},
            {9,  'n', 'n', 'n', 'n', 'g', 'w', 4, RL, YR, WR, WR},
            {10, 'n', 'g', 'w', 'n', 'n', 'n', 2, OR, GR},
            {11, 'n', 'g', 'n', 'n', 'n', 'w', 2, RL, GL},
            {13, 'w', 'n', 'n', 'n', 'g', 'n', 3, BL, OL, GR},
            {15, 'w', 'n', 'g', 'n', 'n', 'n', 2, OL, WL},
            {16, 'w', 'n', 'n', 'n', 'n', 'g', 2, RR, WR},
            
            {18, 'w', 'g', 'n', 'n', 'n', 'n', 3, GL, OL, WL},
            {18, 'g', 'w', 'n', 'n', 'n', 'n', 0},
        },
        
        // GO
        {
            {1,  'n', 'n', 'n', 'o', 'g', 'n', 2, BR, OL},
            {3,  'n', 'n', 'o', 'g', 'n', 'n', 2, OL, OL},
            {4,  'n', 'n', 'n', 'g', 'n', 'o', 4, RR, BL, BL, OL},
            {6,  'n', 'o', 'n', 'g', 'n', 'n', 3, YR, OL, OL},
            {8,  'n', 'n', 'o', 'n', 'g', 'n', 1, OL},
            {9,  'n', 'n', 'n', 'n', 'g', 'o', 3, BR, BR, OL},
            {10, 'n', 'g', 'o', 'n', 'n', 'n', 1, OR},
            {11, 'n', 'g', 'n', 'n', 'n', 'o', 5, WR, WR, OR, WL, WL},
            {13, 'g', 'n', 'n', 'n', 'o', 'n', 5, BR, BR, YL, OR, OR},
            {16, 'g', 'n', 'n', 'n', 'n', 'o', 4, RL, BR, BR, OL},
            {18, 'g', 'o', 'n', 'n', 'n', 'n', 1, GL},
            
            {1,  'n', 'n', 'n', 'g', 'o', 'n', 3, YL, OL, OL},
            {3,  'n', 'n', 'g', 'o', 'n', 'n', 3, YR, BR, OL},
            {4,  'n', 'n', 'n', 'o', 'n', 'g', 4, YR, WL, OR, WR},
            {6,  'n', 'g', 'n', 'o', 'n', 'n', 4, YL, YL, BR, OL},
            {8,  'n', 'n', 'g', 'n', 'o', 'n', 4, BL, YL, OL, OL},
            {9,  'n', 'n', 'n', 'n', 'o', 'g', 4, BR, YL, OL, OL},
            {10, 'n', 'o', 'g', 'n', 'n', 'n', 4, OL, YR, BR, OL},
            {11, 'n', 'o', 'n', 'n', 'n', 'g', 5, WL, YR, WR, OR, OR},
            {13, 'o', 'n', 'n', 'n', 'g', 'n', 2, BL, OL},
            {16, 'o', 'n', 'n', 'n', 'n', 'g', 5, RL, BR, YL, OR, OR},
            {18, 'o', 'g', 'n', 'n', 'n', 'n', 2, WR, OR},
            
            {15, 'o', 'n', 'g', 'n', 'n', 'n', 5, OR, BL, YL, OR, OR},
            {15, 'g', 'n', 'o', 'n', 'n', 'n', 0},
        },
        
        // GB
        {
            {1,  'n', 'n', 'n', 'g', 'b', 'n', 2, BR, BR},
            {3,  'n', 'n', 'b', 'g', 'n', 'n', 3, YR, BR, BR},
            {4,  'n', 'n', 'n', 'b', 'n', 'g', 2, RR, BL},
            {6,  'n', 'g', 'n', 'b', 'n', 'n', 3, YL, RR, BL},
            {8,  'n', 'n', 'g', 'n', 'b', 'n', 1, BR},
            {9,  'n', 'n', 'n', 'n', 'b', 'g', 1, BL},
            {10, 'n', 'b', 'g', 'n', 'n', 'n', 6, WR, YL, YL, WL, BR, BR},
            {11, 'n', 'b', 'n', 'n', 'n', 'g', 6, WL, YL, YL, WR, BR, BR},
            {15, 'g', 'n', 'b', 'n', 'n', 'n', 1, GL},
            {16, 'b', 'n', 'n', 'n', 'n', 'g', 2, RL, BL},
            {18, 'g', 'b', 'n', 'n', 'n', 'n', 2, GR, GL},
            
            {1,  'n', 'n', 'n', 'b', 'g', 'n', 3, YR, RR, BL},
            {3,  'n', 'n', 'g', 'b', 'n', 'n', 4, YL, YL, RR, BL},
            {4,  'n', 'n', 'n', 'g', 'n', 'b', 3, YL, BR, BR},
            {6,  'n', 'b', 'n', 'g', 'n', 'n', 4, YL, YL, BL, BL},
            {8,  'n', 'n', 'b', 'n', 'g', 'n', 4, BL, YR, RR, BL},
            {9,  'n', 'n', 'n', 'n', 'g', 'b', 4, RL, YL, BR, BR},
            {10, 'n', 'g', 'b', 'n', 'n', 'n', 5, OL, YR, OR, BR, BR},
            {11, 'n', 'g', 'n', 'n', 'n', 'b', 4, RR, YL, BR, BR},
            {15, 'b', 'n', 'g', 'n', 'n', 'n', 2, OR, BR},
            {16, 'g', 'n', 'n', 'n', 'n', 'b', 5, RR, RR, YL, BR, BR},
            {18, 'b', 'g', 'n', 'n', 'n', 'n', 3, WR, OR, GL},
            
            {13, 'b', 'n', 'n', 'n', 'g', 'n', 5, BR, RL, YL, BR, BR},
            {13, 'g', 'n', 'n', 'n', 'b', 'n', 0},
        },
        
        // GR
        {
            {1,  'n', 'n', 'n', 'g', 'r', 'n', 3, YR, RL, RL},
            {3,  'n', 'n', 'r', 'g', 'n', 'n', 4, YL, YL, RL, RL},
            {4,  'n', 'n', 'n', 'g', 'n', 'r', 2, RL, RL},
            {6,  'n', 'r', 'n', 'g', 'n', 'n', 3, YL, RL, RL},
            {8,  'n', 'n', 'g', 'n', 'r', 'n', 5, BL, YR, BR, RL, RL},
            {9,  'n', 'n', 'n', 'n', 'g', 'r', 1, RR},
            {10, 'n', 'r', 'g', 'n', 'n', 'n', 5, WR, YL, WL, RL, RL},
            {11, 'n', 'g', 'n', 'n', 'n', 'r', 1, RL},
            {13, 'g', 'n', 'n', 'n', 'r', 'n', 1, GL},
            {15, 'g', 'n', 'r', 'n', 'n', 'n', 2, GL, GL},
            {18, 'g', 'r', 'n', 'n', 'n', 'n', 1, GR},
            
            {1,  'n', 'n', 'n', 'r', 'g', 'n', 5, YR, YR, WR, RL, WL},
            {3,  'n', 'n', 'g', 'r', 'n', 'n', 4, YL, WR, RL, WL},
            {4,  'n', 'n', 'n', 'r', 'n', 'g', 4, YR, WR, RL, WL},
            {6,  'n', 'g', 'n', 'r', 'n', 'n', 3, WR, RL, WL},
            {8,  'n', 'n', 'r', 'n', 'g', 'n', 7, BL, YR, BR, YR, WR, RL, WL},
            {9,  'n', 'n', 'n', 'n', 'r', 'g', 5, RL, YR, WR, RL, WL},
            {10, 'n', 'g', 'r', 'n', 'n', 'n', 7, WR, YL, WL, YR, WR, RL, WL},
            {11, 'n', 'r', 'n', 'n', 'n', 'g', 5, WL, YL, WR, RL, RL},
            {13, 'r', 'n', 'n', 'n', 'g', 'n', 4, BL, OL, GR, GR},
            {15, 'r', 'n', 'g', 'n', 'n', 'n', 3, OL, WL, RL},
            {18, 'r', 'g', 'n', 'n', 'n', 'n', 4, WR, OR, GR, GR},
            
            {16, 'r', 'n', 'n', 'n', 'n', 'g', 6, RR, RR, YR, WR, RL, WL},
            {16, 'g', 'n', 'n', 'n', 'n', 'r', 0},
        }
    };
    
    const char RubikResolver::STAGE1_MOVES[4][24][24] = {
        // MC 12
        {
            {0, 'n', 'n', 'g', 'o', 'b', 'n', 4, YL, BL, YR, BR},
            {0, 'n', 'n', 'o', 'b', 'g', 'n', 3, BL, YL, BR},
            {0, 'n', 'n', 'b', 'g', 'o', 'n', 8, BL, YR, BR, YL, YL, BL, YL, BR},
            
            {2, 'n', 'n', 'n', 'o', 'g', 'b', 5, YL, YL, BL, YR, BR},
            {2, 'n', 'n', 'n', 'b', 'o', 'g', 4, YL, BL, YL, BR},
            {2, 'n', 'n', 'n', 'g', 'b', 'o', 9, YL, BL, YR, BR, YL, YL, BL, YL, BR},
            
            {5, 'n', 'g', 'b', 'o', 'n', 'n', 3, BL, YR, BR},
            {5, 'n', 'o', 'g', 'b', 'n', 'n', 4, YR, BL, YL, BR},
            {5, 'n', 'b', 'o', 'g', 'n', 'n', 9, YR, BL, YR, BR, YL, YL, BL, YL, BR},
            
            {7, 'n', 'b', 'n', 'o', 'n', 'g', 6, YR, YR, YL, BL, YR, BR},
            {7, 'n', 'g', 'n', 'b', 'n', 'o', 5, YR, YR, BL, YL, BR},
            {7, 'n', 'o', 'n', 'g', 'n', 'b', 10, YR, YR, BL, YR, BR, YL, YL, BL, YL, BR},
            
            {14, 'g', 'n', 'n', 'n', 'o', 'b', 7, RL, YL, RR, YL, BL, YR, BR},
            {14, 'o', 'n', 'n', 'n', 'b', 'g', 6, RL, YL, RR, BL, YL, BR},
            {14, 'b', 'n', 'n', 'n', 'g', 'o', 11, RL, YL, RR, BL, YR, BR, YL, YL, BL, YL, BR},
            
            {17, 'g', 'o', 'b', 'n', 'n', 'n', 6, WR, YR, WL, BL, YL, BR},
            {17, 'b', 'g', 'o', 'n', 'n', 'n', 7, WR, YR, WL, YL, BL, YR, BR},
            {17, 'o', 'b', 'g', 'n', 'n', 'n', 11, WR, YR, WL, BL, YR, BR, YL, YL, BL, YL, BR},
            
            {19, 'g', 'b', 'n', 'n', 'n', 'o', 7, RR, YR, YR, RL, BL, YL, BR},
            {19, 'b', 'o', 'n', 'n', 'n', 'g', 8, RR, YR, YR, RL, YL, BL, YR, BR},
            {19, 'o', 'g', 'n', 'n', 'n', 'b', 12, RR, YR, YR, RL, BL, YR, BR, YL, YL, BL, YL, BR},
            
            {12,'b', 'n', 'g', 'n', 'o', 'n', 7, BL, YR, BR, YL, BL, YR, BR},
            {12,'o', 'n', 'b', 'n', 'g', 'n', 7, BL, YL, BR, YR, BL, YL, BR},
            {12,'g', 'n', 'o', 'n', 'b', 'n', 0},
        },
        
        // MC 14
        {
            {2, 'n', 'n', 'n', 'b', 'g', 'r', 3, BR, YR, BL},
            {2, 'n', 'n', 'n', 'r', 'b', 'g', 4, YR, BR, YL, BL},
            {2, 'n', 'n', 'n', 'g', 'r', 'b', 8, BR, YR, YR, BL, YL, BR, YR, BL},
            
            {0, 'n', 'n', 'g', 'b', 'r', 'n', 4, YR, BR, YR, BL},
            {0, 'n', 'n', 'b', 'r', 'g', 'n', 5, YR, YR, BR, YL, BL},
            {0, 'n', 'n', 'r', 'g', 'b', 'n', 9, YR, BR, YR, YR, BL, YL, BR, YR, BL},
            
            {5, 'n', 'g', 'r', 'b', 'n', 'n', 5, YR, YR, BR, YR, BL},
            {5, 'n', 'b', 'g', 'r', 'n', 'n', 4, YL, BR, YL, BL},
            {5, 'n', 'r', 'b', 'g', 'n', 'n', 10, YR, YR, BR, YR, YR, BL, YL, BR, YR, BL},
            
            {7, 'n', 'r', 'n', 'b', 'n', 'g', 4, YL, BR, YR, BL},
            {7, 'n', 'g', 'n', 'r', 'n', 'b', 3, BR, YL, BL},
            {7, 'n', 'b', 'n', 'g', 'n', 'r', 9, YL, BR, YR, YR, BL, YL, BR, YR, BL},
            
            // impossible at 12 if we first resolve 12
            {12, 'g', 'n', 'n', 'n', 'r', 'b', 7, OR, YR, OL, YR, BR, YL, BL},
            {12, 'b', 'n', 'n', 'n', 'g', 'r', 6, OR, YR, OL, BR, YR, BL},
            {12, 'r', 'n', 'n', 'n', 'b', 'g', 11, OR, YR, OL, BR, YR, YR, BL, YL, BR, YR, BL},
            
            {17, 'r', 'g', 'b', 'n', 'n', 'n', 7, WR, YL, YL, WL, BR, YR, BL},
            {17, 'g', 'b', 'r', 'n', 'n', 'n', 8, WR, YL, YL, WL, YR, BR, YL, BL},
            {17, 'b', 'r', 'g', 'n', 'n', 'n', 8, OL, YR, YR, OR, YR, BR, YL, BL},
            
            {19, 'g', 'r', 'n', 'n', 'n', 'b', 6, WL, YL, WR, BR, YR, BL},
            {19, 'b', 'g', 'n', 'n', 'n', 'r', 7, WL, YL, WR, YR, BR, YL, BL},
            {19, 'r', 'b', 'n', 'n', 'n', 'g', 11, WL, YL, WR, BR, YR, YR, BL, YL, BR, YR, BL},
            
            {14,'r', 'n', 'n', 'n', 'g', 'b', 7, BR, YR, BL, YL, BR, YR, BL},
            {14,'b', 'n', 'n', 'n', 'r', 'g', 7, BR, YL, BL, YR, BR, YL, BL},
            {14,'g', 'n', 'n', 'n', 'b', 'r', 0},
        },
        
        // MC 17
        {
            {5, 'n', 'g', 'o', 'w', 'n', 'n', 4, YL, OL, YR, OR}, // G on top
            {5, 'n', 'w', 'g', 'o', 'n', 'n', 3, OL, YL, OR},
            {5, 'n', 'o', 'w', 'g', 'n', 'n', 8, OL, YL, YL, OR, YR, OL, YL, OR},
            
            {0, 'n', 'n', 'g', 'w', 'o', 'n', 5, YL, YL, OL, YR, OR},
            {0, 'n', 'n', 'w', 'o', 'g', 'n', 4, YL, OL, YL, OR},
            {0, 'n', 'n', 'o', 'g', 'w', 'n', 9, YL, OL, YL, YL, OR, YR, OL, YL, OR},
            
            {2, 'n', 'n', 'n', 'w', 'g', 'o', 4, YR, OL, YR, OR},
            {2, 'n', 'n', 'n', 'o', 'w', 'g', 5, YR, YR, OL, YL, OR},
            {2, 'n', 'n', 'n', 'g', 'o', 'w', 10, YR, YR, OL, YL, YL, OR, YR, OL, YL, OR},
            
            {7, 'n', 'o', 'n', 'w', 'n', 'g', 5, YR, YL, OL, YR, OR},
            {7, 'n', 'g', 'n', 'o', 'n', 'w', 4, YR, OL, YL, OR},
            {7, 'n', 'w', 'n', 'g', 'n', 'o', 9, YR, OL, YL, YL, OR, YR, OL, YL, OR},
            
            // 12 and 14 is impossible
            {12,'w', 'n', 'o', 'n', 'g', 'n', 0},
            {12,'g', 'n', 'w', 'n', 'o', 'n', 0},
            {12,'o', 'n', 'g', 'n', 'w', 'n', 0},
            
            {14,'o', 'n', 'n', 'n', 'g', 'w', 0},
            {14,'w', 'n', 'n', 'n', 'o', 'g', 0},
            {14,'g', 'n', 'n', 'n', 'w', 'o', 0},
            
            {17, 'g', 'w', 'o', 'n', 'n', 'n', 0},
            {17, 'w', 'o', 'g', 'n', 'n', 'n', 7, OL, YL, OR, YR, OL, YL, OR},
            {17, 'o', 'g', 'w', 'n', 'n', 'n', 12, OL, YL, OR, YR, OL, YL, YL, OR, YR, OL, YL, OR},
            
            {19, 'o', 'w', 'n', 'n', 'n', 'g', 7, RR, YR, RL, YL, OL, YR, OR},
            {19, 'g', 'o', 'n', 'n', 'n', 'w', 6, RR, YR, RL, OL, YL, OR},
            {19, 'w', 'g', 'n', 'n', 'n', 'o', 11, RR, YR, RL, OL, YL, YL, OR, YR, OL, YL, OR},
        },
        
        // MC 19
        {
            {7, 'n', 'g', 'n', 'w', 'n', 'r', 4, YR, RR, YL, RL}, // G on Top
            {7, 'n', 'w', 'n', 'r', 'n', 'g', 3, RR, YR, RL},
            {7, 'n', 'r', 'n', 'g', 'n', 'w', 8, RR, YL, RL, YR, YR, RR, YR, RL},
            
            {0, 'n', 'n', 'r', 'w', 'g', 'n', 4, YL, RR, YL, RL},
            {0, 'n', 'n', 'g', 'r', 'w', 'n', 5, YR, YR, RR, YR, RL},
            {0, 'n', 'n', 'w', 'g', 'r', 'n', 10, YR, YR, RR, YL, RL, YR, YR, RR, YR, RL},
            
            {2, 'n', 'n', 'n', 'w', 'r', 'g', 5, YR, YR, RR, YL, RL},
            {2, 'n', 'n', 'n', 'r', 'g', 'w', 4, YR, RR, YR, RL},
            {2, 'n', 'n', 'n', 'g', 'w', 'r', 9, YR, RR, YL, RL, YR, YR, RR, YR, RL},
            
            {5, 'n', 'r', 'g', 'w', 'n', 'n', 5, YL, YR, RR, YL, RL},
            {5, 'n', 'g', 'w', 'r', 'n', 'n', 4, YL, RR, YR, RL},
            {5, 'n', 'w', 'r', 'g', 'n', 'n', 9, YL, RR, YL, RL, YR, YR, RR, YR, RL},
            
            {19, 'g', 'w', 'n', 'n', 'n', 'r', 0},
            {19, 'r', 'g', 'n', 'n', 'n', 'w', 7, RR, YL, RL, YR, RR, YL, RL},
            {19, 'w', 'r', 'n', 'n', 'n', 'g', 7, RR, YR, RL, YL, RR, YR, RL},
        }
    };

    const char RubikResolver::STAGE2_MOVES[4][24][32] = {
        // MC10
        {
            {1, 'n', 'n', 'n', 'w', 'o', 'n', 11, YL, OR, YR, OR, YR, OR, YL, OL, YL, OL, YL},
            {3, 'n', 'n', 'o', 'w', 'n', 'n', 10, OR, YR, OR, YR, OR, YL, OL, YL, OL, YL},
            {4, 'n', 'n', 'n', 'w', 'n', 'o', 12, YL, YL, OR, YR, OR, YR, OR, YL, OL, YL, OL, YL},
            {6, 'n', 'o', 'n', 'w', 'n', 'n', 11, YR, OR, YR, OR, YR, OR, YL, OL, YL, OL, YL},
            
            {1, 'n', 'n', 'n', 'o', 'w', 'n', 12, YL, YL, WL, YL, WL, YL, WL, YR, WR, YR, WR, YR},
            {3, 'n', 'n', 'w', 'o', 'n', 'n', 11, YL, WL, YL, WL, YL, WL, YR, WR, YR, WR, YR},
            {4, 'n', 'n', 'n', 'o', 'n', 'w', 11, YR, WL, YL, WL, YL, WL, YR, WR, YR, WR, YR},
            {6, 'n', 'w', 'n', 'o', 'n', 'n', 10, WL, YL, WL, YL, WL, YR, WR, YR, WR, YR},
            
            {8,  'n', 'n', 'w', 'n', 'o', 'n', 20, OL, YL, OL, YL, OL, YR, OR, YR, OR, YR, WL, YL, WL, YL, WL, YR, WR, YR, WR, YR},
            {9,  'n', 'n', 'n', 'n', 'w', 'o', 21, RR, YR, RR, YR, RR, YL, RL, YL, RL, YL, YR, OR, YR, OR, YR, OR, YL, OL, YL, OL, YL},
            {10, 'n', 'w', 'o', 'n', 'n', 'n', 0},
            {11, 'n', 'o', 'n', 'n', 'n', 'w', 20, WR, YR, WR, YR, WR, YL, WL, YL, WL, YL, OR, YR, OR, YR, OR, YL, OL, YL, OL, YL},
            
            {8,  'n', 'n', 'o', 'n', 'w', 'n', 21, OL, YL, OL, YL, OL, YR, OR, YR, OR, YR, YR, OR, YR, OR, YR, OR, YL, OL, YL, OL, YL},
            {9,  'n', 'n', 'n', 'n', 'o', 'w', 20, RR, YR, RR, YR, RR, YL, RL, YL, RL, YL, WL, YL, WL, YL, WL, YR, WR, YR, WR, YR},
            {10, 'n', 'o', 'w', 'n', 'n', 'n', 22, OR, YR, OR, YR, OR, YL, OL, YL, OL, YL, YL, YL, WL, YL, WL, YL, WL, YR, WR, YR, WR, YR},
            {11, 'n', 'w', 'n', 'n', 'n', 'o', 20, WR, YR, WR, YR, WR, YL, WL, YL, WL, YL, YL, WL, YL, WL, YL, WL, YR, WR, YR, WR, YR},
        },
        
        // MC11
        {
            {1, 'n', 'n', 'n', 'r', 'w', 'n', 12, YR, YR, WR, YR, WR, YR, WR, YL, WL, YL, WL, YL},
            {3, 'n', 'n', 'w', 'r', 'n', 'n', 11, YL, WR, YR, WR, YR, WR, YL, WL, YL, WL, YL},
            {4, 'n', 'n', 'n', 'r', 'n', 'w', 11, YR, WR, YR, WR, YR, WR, YL, WL, YL, WL, YL},
            {6, 'n', 'w', 'n', 'r', 'n', 'n', 10, WR, YR, WR, YR, WR, YL, WL, YL, WL, YL},
            
            {1, 'n', 'n', 'n', 'w', 'r', 'n', 11, YR, RL, YL, RL, YL, RL, YR, RR, YR, RR, YR},
            {3, 'n', 'n', 'r', 'w', 'n', 'n', 12, YR, YR, RL, YL, RL, YL, RL, YR, RR, YR, RR, YR},
            {4, 'n', 'n', 'n', 'w', 'n', 'r', 10, RL, YL, RL, YL, RL, YR, RR, YR, RR, YR},
            {6, 'n', 'r', 'n', 'w', 'n', 'n', 11, YL, RL, YL, RL, YL, RL, YR, RR, YR, RR, YR},
            
            {8,  'n', 'n', 'r', 'n', 'w', 'n', 21, BR, YR, BR, YR, BR, YL, BL, YL, BL, YL, YR, WR, YR, WR, YR, WR, YL, WL, YL, WL, YL},
            {9,  'n', 'n', 'n', 'n', 'w', 'r', 21, RR, YR, RR, YR, RR, YL, RL, YL, RL, YL, YL, RL, YL, RL, YL, RL, YR, RR, YR, RR, YR},
            {11, 'n', 'r', 'n', 'n', 'n', 'w', 20, RL, YL, RL, YL, RL, YR, RR, YR, RR, YL, WR, YR, WR, YR, WR, YL, WL, YL, WL, YL},
            
            {8,  'n', 'n', 'w', 'n', 'r', 'n', 20, BR, YR, BR, YR, BR, YL, BL, YL, BL, YL, RL, YL, RL, YL, RL, YR, RR, YR, RR, YR},
            {9,  'n', 'n', 'n', 'n', 'r', 'w', 20, RR, YR, RR, YR, RR, YL, RL, YL, RL, YL, WR, YR, WR, YR, WR, YL, WL, YL, WL, YL},
            {11, 'n', 'w', 'n', 'n', 'n', 'r', 0},
        },

        // MC8
		{
			{1, 'n', 'n', 'n', 'o', 'b', 'n', 10, BR, YR, BR, YR, BR, YL, BL, YL, BL, YL},
			{3, 'n', 'n', 'b', 'o', 'n', 'n', 11, YR, BR, YR, BR, YR, BR, YL, BL, YL, BL, YL},
			{4, 'n', 'n', 'n', 'o', 'n', 'b', 11, YL, BR, YR, BR, YR, BR, YL, BL, YL, BL, YL},
			{6, 'n', 'b', 'n', 'o', 'n', 'n', 12, YR, YR, BR, YR, BR, YR, BR, YL, BL, YL, BL, YL},

			{1, 'n', 'n', 'n', 'b', 'o', 'n', 11, YL, OL, YL, OL, YL, OL, YR, OR, YR, OR, YR},
			{3, 'n', 'n', 'o', 'b', 'n', 'n', 10, OL, YL, OL, YL, OL, YR, OR, YR, OR, YR},
			{4, 'n', 'n', 'n', 'b', 'n', 'o', 12, YL, YL, OL, YL, OL, YL, OL, YR, OR, YR, OR, YR},
			{6, 'n', 'o', 'n', 'b', 'n', 'n', 11, YR, OL, YL, OL, YL, OL, YR, OR, YR, OR, YR},

			{8,  'n', 'n', 'b', 'n', 'o', 'n', 22, BR, YR, BR, YR, BR, YL, BL, YL, BL, YL, YR, YR, OL, YL, OL, YL, OL, YR, OR, YR, OR, YR},
			{9,  'n', 'n', 'n', 'n', 'o', 'b', 22, RR, YR, RR, YR, RR, YL, RL, YL, RL, YL, YR, YR, BR, YR, BR, YR, BR, YL, BL, YL, BL, YL},

			{8,  'n', 'n', 'o', 'n', 'b', 'n', 0},
			{9,  'n', 'n', 'n', 'n', 'b', 'o', 21, RR, YR, RR, YR, RR, YL, RL, YL, RL, YL, YR, OL, YL, OL, YL, OL, YR, OR, YR, OR, YR},
		},

		// MC9
		{
			{1, 'n', 'n', 'n', 'b', 'r', 'n', 11, YR, RR, YR, RR, YR, RR, YL, RL, YL, RL, YL},
			{3, 'n', 'n', 'r', 'b', 'n', 'n', 12, YR, YR, RR, YR, RR, YR, RR, YL, RL, YL, RL, YL},
			{4, 'n', 'n', 'n', 'b', 'n', 'r', 10, RR, YR, RR, YR, RR, YL, RL, YL, RL, YL},
			{6, 'n', 'r', 'n', 'b', 'n', 'n', 11, YL, RR, YR, RR, YR, RR, YL, RL, YL, RL, YL},

			{1, 'n', 'n', 'n', 'r', 'b', 'n', 10, BL, YL, BL, YL, BL, YR, BR, YR, BR, YR},
			{3, 'n', 'n', 'b', 'r', 'n', 'n', 11, YR, BL, YL, BL, YL, BL, YR, BR, YR, BR, YR},
			{4, 'n', 'n', 'n', 'r', 'n', 'b', 11, YL, BL, YL, BL, YL, BL, YR, BR, YR, BR, YR},
			{6, 'n', 'b', 'n', 'r', 'n', 'n', 12, YR, YR, BL, YL, BL, YL, BL, YR, BR, YR, BR, YR},

			{9,  'n', 'n', 'n', 'n', 'b', 'r', 0},
			{9,  'n', 'n', 'n', 'n', 'r', 'b', 22, RR, YR, RR, YR, RR, YL, RL, YL, RL, YL, YR, YR, BL, YL, BL, YL, BL, YR, BR, YR, BR, YR},
		}
    };
	const MCState RubikResolver::MCSS[20][24] = {
        // mc0
        {
            {'n', 'n', 'o', 'y', 'b', 'n', 'a', 0}, // state 0
            {'n', 'n', 'g', 'o', 'b', 'n', 'b', 1, BR}, // state 1
            {'n', 'n', 'y', 'r', 'b', 'n', 'c', 1, BL},
            {'n', 'n', 'o', 'w', 'y', 'n', 'd', 1, OR},
            {'n', 'n', 'o', 'b', 'g', 'n', 'e', 1, OL},
            {'n', 'n', 'b', 'y', 'r', 'n', 'f', 1, YR},
            {'n', 'n', 'w', 'y', 'o', 'n', 'g', 1, YL},
            {'n', 'n', 'r', 'g', 'b', 'n', 'h', 2, BR, BR},
            {'n', 'n', 'o', 'g', 'w', 'n', 'i', 2, OR, OR},
            {'n', 'n', 'r', 'y', 'w', 'n', 'j', 2, YR, YR},
            {'n', 'n', 'b', 'o', 'y', 'n', 'k', 2, BR, OR},
            {'n', 'n', 'w', 'o', 'g', 'n', 'l', 2, BR, OL},
            {'n', 'n', 'b', 'r', 'g', 'n', 'm', 2, BL, RR},
            {'n', 'n', 'w', 'r', 'y', 'n', 'n', 2, BL, RL},
            {'n', 'n', 'o', 'y', 'b', 'n', 'o', 2, OR, WR},
            {'n', 'n', 'y', 'w', 'r', 'n', 'p', 2, OR, WL},
            {'n', 'n', 'g', 'b', 'r', 'n', 'q', 2, OL, BR},
            {'n', 'n', 'y', 'b', 'o', 'n', 'r', 2, OL, BL},
            {'n', 'n', 'g', 'b', 'r', 'n', 's', 2, YR, RR},
            {'n', 'n', 'y', 'w', 'r', 'n', 't', 2, YR, RL},
            {'n', 'n', 'w', 'r', 'y', 'n', 'u', 2, YL, WR},
            {'n', 'n', 'w', 'o', 'g', 'n', 'v', 2, YL, WL},
            {'n', 'n', 'g', 'r', 'w', 'n', 'w', 3, BL, RL, RL},
            {'n', 'n', 'o', 'y', 'b', 'n', 'x', 3, OR, OR, GR},
        },
        // mc1
        {
            {'n', 'n', 'n', 'y', 'b', 'n', 'a', 0}, // state 0
            {'n', 'n', 'n', 'o', 'b', 'n', 'b', 1, BR}, // state 1
            {'n', 'n', 'n', 'r', 'b', 'n', 'c', 1, BL},
            {'n', 'n', 'n', 'y', 'r', 'n', 'd', 1, YR},
            {'n', 'n', 'n', 'y', 'o', 'n', 'e', 1, YL},
            {'n', 'n', 'n', 'g', 'b', 'n', 'd', 2, BR, BR},
            {'n', 'n', 'n', 'y', 'w', 'n', 'f', 2, YR, YR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc2
        {
            {'n', 'n', 'n', 'y', 'b', 'r', 'a', 0}, // state 0
            {'n', 'n', 'n', 'o', 'b', 'y', 'b', 1, BR}, // state 1
            {'n', 'n', 'n', 'r', 'b', 'g', 'c', 1, BL},
            {'n', 'n', 'n', 'y', 'r', 'w', 'd', 1, YR},
            {'n', 'n', 'n', 'y', 'o', 'b', 'e', 1, YL},
            {'n', 'n', 'n', 'b', 'g', 'r', 'f', 1, RR},
            {'n', 'n', 'n', 'y', 'w', 'r', 'g', 1, RL},
            {'n', 'n', 'n', 'g', 'b', 'o', 'h', 2, BR, BR},
            {'n', 'n', 'n', 'y', 'w', 'o', 'i', 2, YR, YR},
            {'n', 'n', 'n', 'g', 'w', 'r', 'j', 2, RR, RR},
            {'n', 'n', 'n', 'y', 'b', 'r', 'k', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc3
        {
            {'n', 'n', 'o', 'y', 'n', 'n', 'a', 0}, // state 0
            {'n', 'n', 'b', 'y', 'n', 'n', 'b', 1, YR}, // state 1
            {'n', 'n', 'w', 'y', 'n', 'n', 'c', 1, YL},
            {'n', 'n', 'o', 'w', 'n', 'n', 'd', 1, OR},
            {'n', 'n', 'o', 'b', 'n', 'n', 'e', 1, OL},
            {'n', 'n', 'r', 'y', 'n', 'n', 'f', 2, YR, YR},
            {'n', 'n', 'o', 'g', 'n', 'n', 'g', 2, OR, OR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc4
        {
            {'n', 'n', 'n', 'y', 'n', 'r', 'a', 0}, // state 0
            {'n', 'n', 'n', 'y', 'n', 'w', 'b', 1, YR}, // state 1
            {'n', 'n', 'n', 'y', 'n', 'b', 'c', 1, YL},
            {'n', 'n', 'n', 'b', 'n', 'r', 'd', 1, RR},
            {'n', 'n', 'n', 'w', 'n', 'r', 'e', 1, RL},
            {'n', 'n', 'n', 'y', 'n', 'o', 'f', 2, YR, YR},
            {'n', 'n', 'n', 'g', 'n', 'r', 'g', 2, RR, RR},
            {'n', 'n', 'n', 'y', 'n', 'r', 'h', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc5
        {
            {'n', 'w', 'o', 'y', 'n', 'n', 'a', 0}, // state 0
            {'n', 'o', 'b', 'y', 'n', 'n', 'b', 1, YR}, // state 1
            {'n', 'r', 'w', 'y', 'n', 'n', 'c', 1, YL},
            {'n', 'g', 'o', 'w', 'n', 'n', 'd', 1, OR},
            {'n', 'y', 'b', 'y', 'n', 'n', 'e', 1, OL},
            {'n', 'w', 'y', 'r', 'n', 'n', 'f', 1, WR},
            {'n', 'w', 'g', 'o', 'n', 'n', 'g', 1, WL},
            {'n', 'b', 'r', 'y', 'n', 'n', 'd', 2, YR, YR},
            {'n', 'b', 'o', 'g', 'n', 'n', 'd', 2, OR, OR},
            {'n', 'w', 'r', 'g', 'n', 'n', 'd', 2, WR, WR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc6
        {
            {'n', 'w', 'n', 'y', 'n', 'n', 'a', 0}, // state 0
            {'n', 'o', 'n', 'y', 'n', 'n', 'b', 1, YR}, // state 1
            {'n', 'r', 'n', 'y', 'n', 'n', 'c', 1, YL},
            {'n', 'w', 'n', 'r', 'n', 'n', 'd', 1, WR},
            {'n', 'w', 'n', 'o', 'n', 'n', 'e', 1, WL},
            {'n', 'b', 'n', 'y', 'n', 'n', 'f', 2, YR, YR},
            {'n', 'w', 'n', 'g', 'n', 'n', 'g', 2, WR, WR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc7
        {
            {'n', 'w', 'n', 'y', 'n', 'r', 'a', 0}, // state 0
            {'n', 'o', 'n', 'y', 'n', 'w', 'b', 1, YR}, // state 1
            {'n', 'r', 'n', 'y', 'n', 'b', 'c', 1, YL},
            {'n', 'y', 'n', 'b', 'n', 'r', 'd', 1, RR},
            {'n', 'g', 'n', 'w', 'n', 'r', 'e', 1, RL},
            {'n', 'w', 'n', 'r', 'n', 'g', 'f', 1, WR},
            {'n', 'w', 'n', 'o', 'n', 'y', 'g', 1, WL},
            {'n', 'b', 'n', 'y', 'n', 'o', 'h', 2, YR, YR},
            {'n', 'b', 'n', 'g', 'n', 'r', 'i', 2, RR, RR},
            {'n', 'w', 'n', 'g', 'n', 'o', 'j', 2, WR, WR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc8
        {
            {'n', 'n', 'o', 'n', 'b', 'n', 'a', 0}, // state 0
            {'n', 'n', 'g', 'n', 'b', 'n', 'b', 1, BR}, // state 1
            {'n', 'n', 'y', 'n', 'b', 'n', 'c', 1, BL},
            {'n', 'n', 'o', 'n', 'y', 'n', 'd', 1, OR},
            {'n', 'n', 'o', 'n', 'g', 'n', 'e', 1, OL},
            {'n', 'n', 'r', 'n', 'b', 'n', 'f', 2, BR, BR},
            {'n', 'n', 'o', 'n', 'w', 'n', 'g', 2, OR, OR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc9
        {
            {'n', 'n', 'n', 'n', 'b', 'r', 'a', 0}, // state 0
            {'n', 'n', 'n', 'n', 'b', 'y', 'b', 1, BR}, // state 1
            {'n', 'n', 'n', 'n', 'b', 'g', 'c', 1, BL},
            {'n', 'n', 'n', 'n', 'g', 'r', 'd', 1, RR},
            {'n', 'n', 'n', 'n', 'y', 'r', 'e', 1, RL},
            {'n', 'n', 'n', 'n', 'b', 'o', 'f', 2, BR, BR},
            {'n', 'n', 'n', 'n', 'w', 'r', 'g', 2, RR, RR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc10
        {
            {'n', 'w', 'o', 'n', 'n', 'n', 'a', 0}, // state 0
            {'n', 'w', 'y', 'n', 'n', 'n', 'b', 1, WR}, // state 1
            {'n', 'w', 'g', 'n', 'n', 'n', 'c', 1, WL},
            {'n', 'g', 'o', 'n', 'n', 'n', 'd', 1, OR},
            {'n', 'y', 'o', 'n', 'n', 'n', 'e', 1, OL},
            {'n', 'w', 'r', 'n', 'n', 'n', 'f', 2, WR, WR},
            {'n', 'b', 'o', 'n', 'n', 'n', 'g', 2, OR, OR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc11
        {
            {'n', 'w', 'n', 'n', 'n', 'r', 'a', 0}, // state 0
            {'n', 'w', 'n', 'n', 'n', 'g', 'b', 1, WR}, // state 1
            {'n', 'w', 'n', 'n', 'n', 'y', 'c', 1, WL},
            {'n', 'y', 'n', 'n', 'n', 'r', 'd', 1, RR},
            {'n', 'g', 'n', 'n', 'n', 'r', 'e', 1, RL},
            {'n', 'w', 'n', 'n', 'n', 'o', 'f', 2, WR, WR},
            {'n', 'b', 'n', 'n', 'n', 'r', 'g', 2, RR, RR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc12
        {
            {'g', 'n', 'o', 'n', 'b', 'n', 'a', 0}, // state 0
            {'r', 'n', 'g', 'n', 'b', 'n', 'b', 1, BR}, // state 1
            {'o', 'n', 'y', 'n', 'b', 'n', 'c', 1, BL},
            {'g', 'n', 'w', 'n', 'o', 'n', 'd', 1, GR},
            {'g', 'n', 'b', 'n', 'r', 'n', 'e', 1, GL},
            {'b', 'n', 'o', 'n', 'y', 'n', 'f', 1, OR},
            {'w', 'n', 'o', 'n', 'g', 'n', 'g', 1, OL},
            {'y', 'n', 'r', 'n', 'b', 'n', 'h', 2, BR, BR},
            {'g', 'n', 'r', 'n', 'w', 'n', 'i', 2, GR, GR},
            {'y', 'n', 'o', 'n', 'w', 'n', 'j', 2, OR, OR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc13
        {
            {'g', 'n', 'n', 'n', 'b', 'n', 'a', 0}, // state 0
            {'r', 'n', 'n', 'n', 'b', 'n', 'b', 1, BR}, // state 1
            {'o', 'n', 'n', 'n', 'b', 'n', 'c', 1, BL},
            {'g', 'n', 'n', 'n', 'o', 'n', 'd', 1, GR},
            {'g', 'n', 'n', 'n', 'r', 'n', 'e', 1, GL},
            {'y', 'n', 'n', 'n', 'b', 'n', 'f', 2, BR, BR},
            {'g', 'n', 'n', 'n', 'w', 'n', 'g', 2, GR, GR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc14
        {
            {'g', 'n', 'n', 'n', 'b', 'r', 'a', 0}, // state 0
            {'r', 'n', 'n', 'n', 'b', 'y', 'b', 1, BR}, // state 1
            {'o', 'n', 'n', 'n', 'b', 'g', 'c', 1, BL},
            {'w', 'n', 'n', 'n', 'g', 'r', 'd', 1, RR},
            {'b', 'n', 'n', 'n', 'o', 'r', 'e', 1, RL},
            {'g', 'n', 'n', 'n', 'o', 'b', 'f', 1, GR},
            {'g', 'n', 'n', 'n', 'r', 'w', 'g', 1, GL},
            {'y', 'n', 'n', 'n', 'b', 'o', 'h', 2, BR, BR},
            {'y', 'n', 'n', 'n', 'w', 'r', 'i', 2, RR, RR},
            {'g', 'n', 'n', 'n', 'w', 'o', 'j', 2, GR, GR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },// mc15
        {
            {'g', 'n', 'o', 'n', 'n', 'n', 'a', 0}, // state 0
            {'g', 'n', 'w', 'n', 'n', 'n', 'b', 1, GR}, // state 1
            {'g', 'n', 'b', 'n', 'n', 'n', 'c', 1, GL},
            {'b', 'n', 'o', 'n', 'n', 'n', 'd', 1, OR},
            {'w', 'n', 'o', 'n', 'n', 'n', 'e', 1, OL},
            {'g', 'n', 'r', 'n', 'n', 'n', 'f', 2, GR, GR},
            {'y', 'n', 'o', 'n', 'n', 'n', 'g', 2, OR, OR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc16
        {
            {'g', 'n', 'n', 'n', 'n', 'r', 'a', 0}, // state 0
            {'g', 'n', 'n', 'n', 'n', 'b', 'b', 1, GR}, // state 1
            {'g', 'n', 'n', 'n', 'n', 'w', 'c', 1, GL},
            {'w', 'n', 'n', 'n', 'n', 'r', 'd', 1, RR},
            {'b', 'n', 'n', 'n', 'n', 'r', 'e', 1, RL},
            {'g', 'n', 'n', 'n', 'n', 'o', 'd', 2, GR, GR},
            {'y', 'n', 'n', 'n', 'n', 'r', 'g', 2, RR, RR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc17
        {
            {'g', 'w', 'o', 'n', 'n', 'n', 'a', 0}, // state 0
            {'g', 'r', 'w', 'n', 'n', 'n', 'b', 1, GR}, // state 1
            {'g', 'o', 'b', 'n', 'n', 'n', 'c', 1, GL},
            {'o', 'w', 'y', 'n', 'n', 'n', 'd', 1, WR},
            {'r', 'w', 'g', 'n', 'n', 'n', 'e', 1, WL},
            {'b', 'g', 'o', 'n', 'n', 'n', 'f', 1, OR},
            {'w', 'y', 'o', 'n', 'n', 'n', 'g', 1, OL},
            {'g', 'b', 'r', 'n', 'n', 'n', 'h', 2, GR, GR},
            {'y', 'w', 'r', 'n', 'n', 'n', 'i', 2, WR, WR},
            {'t', 'b', 'o', 'n', 'n', 'n', 'j', 2, OR, OR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc18
        {
            {'g', 'w', 'n', 'n', 'n', 'n', 'a', 0}, // state 0
            {'g', 'r', 'n', 'n', 'n', 'n', 'b', 1, GR}, // state 1
            {'g', 'o', 'n', 'n', 'n', 'n', 'c', 1, GL},
            {'o', 'w', 'n', 'n', 'n', 'n', 'd', 1, WR},
            {'r', 'w', 'n', 'n', 'n', 'n', 'e', 1, WL},
            {'g', 'b', 'n', 'n', 'n', 'n', 'f', 2, GR, GR},
            {'y', 'w', 'n', 'n', 'n', 'n', 'g', 2, WR, WR},
            {'g', 'w', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
        // mc19
        {
            {'g', 'w', 'n', 'n', 'n', 'r', 'a', 0}, // state 0
            {'g', 'r', 'n', 'n', 'n', 'b', 'b', 1, GR}, // state 1
            {'g', 'o', 'n', 'n', 'n', 'w', 'c', 1, GL},
            {'o', 'w', 'n', 'n', 'n', 'g', 'd', 1, WR},
            {'r', 'w', 'n', 'n', 'n', 'y', 'e', 1, WL},
            {'w', 'y', 'n', 'n', 'n', 'r', 'f', 1, RR},
            {'b', 'g', 'n', 'n', 'n', 'r', 'g', 1, RL},
            {'g', 'b', 'n', 'n', 'n', 'o', 'h', 2, GR, GR},
            {'y', 'w', 'n', 'n', 'n', 'o', 'i', 2, WR, WR},
            {'y', 'b', 'n', 'n', 'n', 'r', 'j', 2, RR, RR},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
            {'n', 'n', 'n', 'n', 'n', 'n', 'd', 0},
        },
	};

	static const MCState NULLSTATE = {'n', 'n', 'n', 'n', 'n', 'n', 'a', 3};

    RubikResolver::RubikResolver(char init[20][6]) : mHead(0), mStageIndex(0), mManualEnabled(true)
    {
    	G.mMCIndex[0] = 12;
    	G.mMCIndex[1] = 13;
    	G.mMCIndex[2] = 14;
    	G.mMCIndex[3] = 15;
    	G.mMCIndex[4] = 16;
    	G.mMCIndex[5] = 17;
    	G.mMCIndex[6] = 18;
    	G.mMCIndex[7] = 19;

    	W.mMCIndex[0] = 5;
		W.mMCIndex[1] = 6;
		W.mMCIndex[2] = 7;
		W.mMCIndex[3] = 10;
		W.mMCIndex[4] = 11;
		W.mMCIndex[5] = 17;
		W.mMCIndex[6] = 18;
		W.mMCIndex[7] = 19;

		O.mMCIndex[0] = 0;
		O.mMCIndex[1] = 3;
		O.mMCIndex[2] = 5;
		O.mMCIndex[3] = 8;
		O.mMCIndex[4] = 10;
		O.mMCIndex[5] = 12;
		O.mMCIndex[6] = 15;
		O.mMCIndex[7] = 17;

		Y.mMCIndex[0] = 0;
		Y.mMCIndex[1] = 1;
		Y.mMCIndex[2] = 2;
		Y.mMCIndex[3] = 3;
		Y.mMCIndex[4] = 4;
		Y.mMCIndex[5] = 5;
		Y.mMCIndex[6] = 6;
		Y.mMCIndex[7] = 7;

		B.mMCIndex[0] = 0;
		B.mMCIndex[1] = 1;
		B.mMCIndex[2] = 2;
		B.mMCIndex[3] = 8;
		B.mMCIndex[4] = 9;
		B.mMCIndex[5] = 12;
		B.mMCIndex[6] = 13;
		B.mMCIndex[7] = 14;

		R.mMCIndex[0] = 2;
		R.mMCIndex[1] = 4;
		R.mMCIndex[2] = 7;
		R.mMCIndex[3] = 9;
		R.mMCIndex[4] = 11;
		R.mMCIndex[5] = 14;
		R.mMCIndex[6] = 16;
		R.mMCIndex[7] = 19;

    	mFaces[0] = G;
    	mFaces[1] = W;
    	mFaces[2] = O;
    	mFaces[3] = Y;
    	mFaces[4] = B;
    	mFaces[5] = R;

        for (int i = 0;i < 20;++i)
        {
            for (int j = 0;j < 6;++j)
            {
                mGoal[i][j] = GOAL[i][j];
            }
        }
        
        // validate state
        for (int i = 0;i < 20;++i)
        {
            set<string> ss;
            for (int j = 0;j < 24;++j)
            {
                bool allNull = true;
                for (int k = 0;k < 6;++k)
                {
                    if (MCSS[i][j].mColor[k] != 'n')
                    {
                        allNull = false;
                        break;
                    }
                }
                
                if (allNull)
                {
                    continue;
                }
                
                pair<set<string>::iterator,bool> ret;
                ret = ss.insert(MCSS[i][j].mColor);
                if (!ret.second)
                {
                    LOGE("Invalid state [%d, %d]", i, j);
                }
            }
        }
        
        mEstimateStrategy = ES_FACE_MOST;
    }
    
    RubikResolver::~RubikResolver()
    {
        HouyiDelete(mHead);
    }

    char RubikResolver::getState(int cubeIndex, int orientationIndex) const
    {
    	if (cubeIndex < 20)
    	{
    		return mState[cubeIndex][orientationIndex];
    	}

    	if (cubeIndex == 20)
    	{
    		if (orientationIndex == FRONT)
    		{
    			return 'g';
    		}
    	}
    	else if (cubeIndex == 21)
    	{
    		if (orientationIndex == TOP)
			{
				return 'w';
			}
    	}
    	else if (cubeIndex == 22)
		{
			if (orientationIndex == LEFT)
			{
				return 'o';
			}
		}
    	else if (cubeIndex == 23)
		{
			if (orientationIndex == BACK)
			{
				return 'y';
			}
		}
    	else if (cubeIndex == 24)
		{
			if (orientationIndex == BOTTOM)
			{
				return 'b';
			}
		}
    	else if (cubeIndex == 25)
		{
			if (orientationIndex == RIGHT)
			{
				return 'r';
			}
		}
    	return 'n';
    }

    void RubikResolver::setState(char (&state)[20][6])
    {
        LOGD("new state:\n");
    	for (int i = 0;i < 20;++i)
		{
			for (int j = 0;j < 6;++j)
			{
				mState[i][j] = state[i][j];
//                LOGD("\'%c\', ", state[i][j]);
			}
//            LOGD("\n");
		}
    }

    RubikResult* RubikResolver::resolve()
    {
    	// no fooling, you gave me a solved cube
    	if (isSolved(mState, mGoal))
    	{
    		LOGW("input already solved!");
    		RubikResult* res = HouyiNew RubikResult(0, 0);
    		return res;
    	}

        mStartingTime = currentTimeMillis();
        
        if (mManualEnabled)
        {
            mManualSteps.clear();
        	findGCross(mState);
            
            firstLayer(mState);
            
            secondLayer(mState);
            
            bottomCross(mState);
            
            bottomFace(mState);
            
            bottomCorner(mState);
            
            finalTouch(mState);

            if (isSolved(mState, mGoal))
            {
                double totalTime = (currentTimeMillis() - mStartingTime) / 1000;
                LOGI("congratulations! solved in: %fs\n", totalTime);
                return generateResult(0, 0);
            }
        }

        if (mHead)
        {
            HouyiDelete(mHead);
        }
    	// add given puzzle to tree as header
    	mHead = HouyiNew RNode(0, INT_MAX, 0, mState);

    	// debug
    	int depth = 0;
    	int activated = 1;

    	// clear set
    	set<string> stateSet;
    	stateSet.insert(toString(mState));
        
        map<string, RNode*> stateMap;
        stateMap.insert(pair<string,RNode*>(toString(mState),mHead));
        
    	while(true)
    	{
    		if (depth >= 100000)
    		{
    			LOGE("failed to solve. not pratical, activated = %d\n", activated);
    			break;
    		}

    		// find lest effort node in tree
    		RNode* cur = lestEffort(mHead);
    		
    		if ((depth >= 1 && cur == mHead) || (cur == 0))
    		{
    			LOGE("failed to solve, activated = %d\n", activated);
    			break;
    		}
            if (cur->mSearchedCount > 1)
            {
                LOGE("failed to solve due to multiple search, activated = %d\n", activated);
                break;
            }

//            LOGI("\nNext Node: ");
//            printNode(cur);
    		//LOGI("%s\n", toString(cur->mState).c_str());
            
    		activated--;

    		// expand this node
    		for (int i = 0;i < OP_COUNT;++i)
    		{
    			char newState[20][6];
    			memcpy(newState, cur->mState, 120);

    			updateState(newState, i);

    			if (isSolved(newState, mGoal))
    			{
                    double totalTime = (currentTimeMillis() - mStartingTime) / 1000;
                    if (totalTime < 60)
                    {
                        LOGD("Very good, solved in: %fs\n", totalTime);
                    }
                    else
                    {
                        LOGD("Are you running on 286?, solved in: %f min\n", totalTime / 60);
                    }
                    LOGI("congratulations! solved with AI help: ");
                    
					return generateResult(cur, i);
    			}
    			else
    			{
                    // calculate distance
                    int distance = calculateDistance(newState, mGoal);
                    if (distance == 0)
                    {
                    	LOGE("Error distance returned 0\n");
                    	LOGE("%s", toString(newState).c_str());
                    	LOGE("%s", toString(mGoal).c_str());
                    	return 0;
                    }
                    
                    // rotate effort
                    float rotateEffort = cur->mRotateEffort + ROTATE_WEIGH;
                    RNode* node = HouyiNew RNode(rotateEffort, distance, i, newState);
                    
                    std::pair<std::map<string,RNode*>::iterator,bool> ret;
                    ret = stateMap.insert(pair<string,RNode*>(toString(newState),node));
                    if (!ret.second)
                    {
                        RNode* nodeInMap = ret.first->second;
                        if (nodeInMap == mHead || node->mTotalEffort >= nodeInMap->mTotalEffort)
                        {
                            HouyiDelete(node);
                            continue;
                        }
                        else
                        {
                        	remove(ret.first->second);
                            ret.first->second = node;
//                            LOGW("Replace\n");
                        }
                    }
                    else
                    {
//                        LOGD("new state inserted setcount = %d\n", (int)stateMap.size());
                    }

                    // add to tree
//                    LOGD("Add Node: ");
//                    printNode(node);
                    addNode(mHead, node);
                    node->mResultParent = cur;
                    activated++;
    			}
    		}

            if ((!cur->mLeft && !cur->mRight)
                || (cur->mLeft && cur->mRight && !cur->mLeft->mOpen && !cur->mRight->mOpen)
                || (cur->mLeft && !cur->mRight && !cur->mLeft->mOpen)
                || ((cur->mRight && !cur->mLeft && !cur->mRight->mOpen)))
            {
                remove(cur);
            }
            cur->mSearchedCount++;
    		depth++;
    	}
        
    	return 0;
    }

    RNode* RubikResolver::lestEffort(RNode* cur)
    {
    	if (cur->mLeft && cur->mLeft->mOpen)
    	{
    		return lestEffort(cur->mLeft);
    	}

    	if (cur->mOpen && cur->mSearchedCount == 0)
    	{
    		return cur;
    	}

    	if (cur->mRight && cur->mRight->mOpen)
		{
			return lestEffort(cur->mRight);
		}
        
    	RNode* res = cur->mParent;
        remove(cur);
        return lestEffort(res);
    }

    bool RubikResolver::isSolved(const char (&state)[20][6], const char (&goal)[20][6])
    {
    	for (int i = 0;i < 20;++i)
		{
			for (int j = 0;j < 6;++j)
			{
				if (goal[i][j] != 'n' && state[i][j] != goal[i][j])
				{
					return false;
				}
			}
		}

    	return true;
    }

    void RubikResolver::disable(RNode* node)
    {
        if (node->mLeft)
        {
            disable(node->mLeft);
        }
        
        if (node->mRight)
        {
            disable(node->mRight);
        }
        
        node->mOpen = false;
    }
    
    void RubikResolver::remove(RNode* node)
    {
    	if (node->mParent)
    	{
			if (node->mTotalEffort < node->mParent->mTotalEffort)
			{
				node->mParent->mLeft = 0;
			}
			else
			{
				node->mParent->mRight = 0;
			}
    	}
        //HouyiDelete(node);
    }

    void RubikResolver::printNode(RNode* node)
    {
    	LOGD("RNode T, O, R, D [%d, %d, %d, %d]\n", (int)node->mTotalEffort, node->mOp, (int)node->mRotateEffort, node->mDistanceEffort);
    }

    void RubikResolver::addNode(RNode* cur, RNode* node)
    {
    	if (node->mTotalEffort < cur->mTotalEffort)
    	{
    		if (cur->mLeft)
    		{
    			addNode(cur->mLeft, node);
    		}
    		else
    		{
    			cur->mLeft = node;
    			node->mParent = cur;

    			// active all parents
    			RNode* p = node->mParent;
    			while(p)
    			{
    				p->mOpen = true;
    				p = p->mParent;
    			}
    		}
    	}
    	else
    	{
    		if (cur->mRight)
    		{
    			addNode(cur->mRight, node);
    		}
    		else
    		{
    			cur->mRight = node;
    			node->mParent = cur;

    			// active all parents
				RNode* p = node->mParent;
				while(p)
				{
					p->mOpen = true;
					p = p->mParent;
				}
    		}
    	}
    }

    string RubikResolver::toString(const char (&state)[20][6])
    {
    	stringstream ss;
    	for (int i = 0;i < 20;++i)
		{
			for (int j = 0;j < 6;++j)
			{
				ss << state[i][j];
			}
		}
    	return ss.str();
    }

    const MCState& RubikResolver::getMCState(char (&state)[6])
    {
    	for (int i = 0;i < 20;++i)
		{
			for (int j = 0;j < 24;++j)
			{
				const MCState& mcs = MCSS[i][j];
				bool equal = true;
				for (int k = 0;k < 6;++k)
				{
					if (mcs.mColor[k] != state[k])
					{
						equal = false;
						break;
					}
				}

				if (equal)
				{
					return mcs;
				}
			}
		}

    	return NULLSTATE;
    }

    int RubikResolver::calculateDistance(char (&state)[20][6], char (&goal)[20][6])
    {
    	int distance = 0;
        
        if (mEstimateStrategy == ES_PLAIN)
        {
            for (int i = 0;i < 20;++i)
            {
                for (int j = 0;j < 6;++j)
                {
                    if (goal[i][j] != 'n' && state[i][j] != goal[i][j])
                    {
                        distance++;
                    }
                }
            }
        }
        else if(mEstimateStrategy == ES_FACE_MOST || mEstimateStrategy == ES_COLOR_MOST)
        {
            int score = 0;
            int sameColor[6][6];
            memset(sameColor, 0, 36*sizeof(int));
            int maxColorCount[6];
            memset(maxColorCount, 0, 6*sizeof(int));
            for (int i = 0;i < 6;++i)
            {
                Face f = mFaces[i];
                for (int j = 0;j < 8;++j)
                {
                    if (state[f.mMCIndex[j]][i] == FACE_COLOR[i])
                    {
                        score++;
                    }

                    int color = color2Int(state[f.mMCIndex[j]][i]);
                    if (color != -1)
                    {
                        sameColor[i][color]++;
                        if (sameColor[i][color] > maxColorCount[i])
                        {
                            maxColorCount[i] = sameColor[i][color];
                        }
                    }
                }
            }
            for (int i = 0;i < 6;++i)
            {
                score += maxColorCount[i]*maxColorCount[i];
            }
            distance = -score;
        }
        else if(mEstimateStrategy == ES_FACE_COVARIANCE)
        {
            int sameColor[6][6];
            memset(sameColor, 0, 36*sizeof(int));
            for (int i = 0;i < 6;++i)
            {
                Face f = mFaces[i];
                for (int j = 0;j < 8;++j)
                {
                    int color = color2Int(state[f.mMCIndex[j]][i]);
                    if (color != -1)
                    {
                        sameColor[i][color]++;
                    }
                }
                
                for (int j = 0;j < 6;++j)
                {
                    distance += -sameColor[i][j] * sameColor[i][j];
                }
            }
        }
        else if (mEstimateStrategy == ES_SINGLE_MC_EFFORT)
        {
            for (int i = 0;i < 20;++i)
            {
                const MCState& s = getMCState(state[i]);
                distance += s.mEffort;
            }
        }
        else if (mEstimateStrategy == ES_GROUP_MC_EFFORT)
        {
            int minIndex = -1;
            int minEfforts[20];
            bool minEffortFlag[20];
            int minEffort = NULLSTATE.mEffort;
            int totalEffort = 0;
            MCState mcs = NULLSTATE;
            for (int i = 0;i < 20;++i)
            {
                minEfforts[i] = NULLSTATE.mEffort;
                minEffortFlag[i] = false;
            }

            // 1. find every MC min effort to solve, add them as sum
            for (int i = 0;i < 20;++i)
            {
                const MCState& s = getMCState(state[i]);
                minEfforts[i] = s.mEffort * s.mEffort * s.mEffort;
                if (minEfforts[i] < minEffort && minEfforts[i] != 0)
                {
                    minEffort = minEfforts[i];
                    mcs = s;
                    minIndex = i;
                }
                totalEffort += minEfforts[i];
            }
            
            while(true)
            {
                if (mcs.mEffort == NULLSTATE.mEffort)
                {
                    LOGW("Missing State, Fix it!\n");
                    return totalEffort;
                }
                
                // 2. find the min effort MCSME (mini cube set minimum effort) of the MC set, as rotate sequence, S
                int* seq = mcs.mStep;

                // 3. perform S and use (1) to see if it indeed decrease the sum. it at least set ME to 0
                char newState[20][6];
                memcpy(newState, state, 120);
                for (int i = 0;i < mcs.mEffort;++i)
                {
                    updateState(newState, seq[i]);
                }

                int newTtotalEffort = 0;
                for (int i = 0;i < 20;++i)
                {
                    const MCState& s = getMCState(newState[i]);
                    newTtotalEffort += s.mEffort;
                }

                // 4. if 3 decrease sum, return sum, otherwise use the 2nd MCSME found by 1
                if (newTtotalEffort < totalEffort)
                {
                    return newTtotalEffort;
                }
                else
                {
                    minEffortFlag[minIndex] = true;
                    mcs = NULLSTATE;
                    minEffort = NULLSTATE.mEffort;
                    totalEffort = 0;
                    for (int i = 0;i < 20;++i)
                    {
                        const MCState& s = getMCState(newState[i]);
                        minEfforts[i] = s.mEffort * s.mEffort * s.mEffort;
                        if (minEfforts[i] < minEffort && !minEffortFlag[i] && minEfforts[i] != 0)
                        {
                            minEffort = minEfforts[i];
                            mcs = s;
                            minIndex = i;
                        }
                        totalEffort += minEfforts[i];
                    }
                }
            }
        }

    	return distance;
    }

    int RubikResolver::color2Int(char c)
    {
    	for (int i = 0;i < 6;++i)
    	{
    		if (c == FACE_COLOR[i])
    		{
    			return i;
    		}
    	}
    	return -1;
    }

    RubikResult* RubikResolver::generateResult(RNode* solved, int lastStep)
    {
    	int acount = solved ? (int)(solved->mRotateEffort/ROTATE_WEIGH) + 1 : 0;
        int mcount = mManualSteps.size();
    	int count = acount + mcount;
//    	LOGD(" solved step count = T, M, A [%d, %d, %d]", count, mcount, acount);
    	int* steps = new int[count];
    	for (int i = 0;i < mManualSteps.size();++i)
    	{
    		steps[i] = mManualSteps[i];
    	}
        
        if (solved)
        {
            steps[count - 1] = lastStep;
            for (int i = count - 2;i >= mcount;--i)
            {
                steps[i] = solved->mOp;
                solved = solved->mResultParent;
            }
        }

//        for (int i = 0;i < count;++i)
//        {
//            LOGD("%d->", steps[i]);
//        }
//        LOGD("\n");
    	RubikResult* res = HouyiNew RubikResult(steps, count);
    	return res;
    }

    void RubikResolver::updateState(char state[20][6], int op)
    {
    	if (op == GR)
    	{
    		char tmp = state[12][BOTTOM];
    		state[12][BOTTOM] = state[14][RIGHT];
    		state[14][RIGHT] = state[19][TOP];
    		state[19][TOP] = state[17][LEFT];
    		state[17][LEFT] = tmp;

    		tmp = state[12][LEFT];
			state[12][LEFT] = state[14][BOTTOM];
			state[14][BOTTOM] = state[19][RIGHT];
			state[19][RIGHT] = state[17][TOP];
			state[17][TOP] = tmp;

    		tmp = state[13][BOTTOM];
    		state[13][BOTTOM] = state[16][RIGHT];
    		state[16][RIGHT] = state[18][TOP];
    		state[18][TOP] = state[15][LEFT];
    		state[15][LEFT] = tmp;

    		tmp = state[12][FRONT];
    		state[12][FRONT] = state[14][FRONT];
    		state[14][FRONT] = state[19][FRONT];
    		state[19][FRONT] = state[17][FRONT];
    		state[17][FRONT] = tmp;

    		tmp = state[13][FRONT];
			state[13][FRONT] = state[16][FRONT];
			state[16][FRONT] = state[18][FRONT];
			state[18][FRONT] = state[15][FRONT];
			state[15][FRONT] = tmp;
    	}
    	else if (op == GL)
    	{
    		char tmp = state[17][LEFT];
    		state[17][LEFT] = state[19][TOP];
    		state[19][TOP] = state[14][RIGHT];
    		state[14][RIGHT] = state[12][BOTTOM];
    		state[12][BOTTOM] = tmp;

			tmp = state[17][TOP];
			state[17][TOP] = state[19][RIGHT];
			state[19][RIGHT] = state[14][BOTTOM];
			state[14][BOTTOM] = state[12][LEFT];
			state[12][LEFT] = tmp;

			tmp = state[15][LEFT];
			state[15][LEFT] = state[18][TOP];
			state[18][TOP] = state[16][RIGHT];
			state[16][RIGHT] = state[13][BOTTOM];
			state[13][BOTTOM] = tmp;

			tmp = state[17][FRONT];
			state[17][FRONT] = state[19][FRONT];
			state[19][FRONT] = state[14][FRONT];
			state[14][FRONT] = state[12][FRONT];
			state[12][FRONT] = tmp;

			tmp = state[15][FRONT];
			state[15][FRONT] = state[18][FRONT];
			state[18][FRONT] = state[16][FRONT];
			state[16][FRONT] = state[13][FRONT];
			state[13][FRONT] = tmp;
    	}
    	else if (op == WR)
    	{
    		char tmp = state[17][FRONT];
    		state[17][FRONT] = state[19][RIGHT];
    		state[19][RIGHT] = state[7][BACK];
    		state[7][BACK] = state[5][LEFT];
    		state[5][LEFT] = tmp;

    		tmp = state[18][FRONT];
    		state[18][FRONT] = state[11][RIGHT];
    		state[11][RIGHT] = state[6][BACK];
    		state[6][BACK] = state[10][LEFT];
    		state[10][LEFT] = tmp;

    		tmp = state[19][FRONT];
			state[19][FRONT] = state[7][RIGHT];
			state[7][RIGHT] = state[5][BACK];
			state[5][BACK] = state[17][LEFT];
			state[17][LEFT] = tmp;

			tmp = state[17][TOP];
			state[17][TOP] = state[19][TOP];
			state[19][TOP] = state[7][TOP];
			state[7][TOP] = state[5][TOP];
			state[5][TOP] = tmp;

			tmp = state[18][TOP];
			state[18][TOP] = state[11][TOP];
			state[11][TOP] = state[6][TOP];
			state[6][TOP] = state[10][TOP];
			state[10][TOP] = tmp;
    	}
    	else if (op == WL)
		{
			char tmp = state[5][LEFT];
			state[5][LEFT] = state[7][BACK];
			state[7][BACK] = state[19][RIGHT];
			state[19][RIGHT] = state[17][FRONT];
			state[17][FRONT] = tmp;

			tmp = state[10][LEFT];
			state[10][LEFT] = state[6][BACK];
			state[6][BACK] = state[11][RIGHT];
			state[11][RIGHT] = state[18][FRONT];
			state[18][FRONT] = tmp;

			tmp = state[17][LEFT];
			state[17][LEFT] = state[5][BACK];
			state[5][BACK] = state[7][RIGHT];
			state[7][RIGHT] = state[19][FRONT];
			state[19][FRONT] = tmp;

			tmp = state[5][TOP];
			state[5][TOP] = state[7][TOP];
			state[7][TOP] = state[19][TOP];
			state[19][TOP] = state[17][TOP];
			state[17][TOP] = tmp;

			tmp = state[10][TOP];
			state[10][TOP] = state[6][TOP];
			state[6][TOP] = state[11][TOP];
			state[11][TOP] = state[18][TOP];
			state[18][TOP] = tmp;
		}
    	else if (op == OR)
		{
			char tmp = state[12][FRONT];
			state[12][FRONT] = state[17][TOP];
			state[17][TOP] = state[5][BACK];
			state[5][BACK] = state[0][BOTTOM];
			state[0][BOTTOM] = tmp;

			tmp = state[15][FRONT];
			state[15][FRONT] = state[10][TOP];
			state[10][TOP] = state[3][BACK];
			state[3][BACK] = state[8][BOTTOM];
			state[8][BOTTOM] = tmp;

			tmp = state[17][FRONT];
			state[17][FRONT] = state[5][TOP];
			state[5][TOP] = state[0][BACK];
			state[0][BACK] = state[12][BOTTOM];
			state[12][BOTTOM] = tmp;

			tmp = state[0][LEFT];
			state[0][LEFT] = state[12][LEFT];
			state[12][LEFT] = state[17][LEFT];
			state[17][LEFT] = state[5][LEFT];
			state[5][LEFT] = tmp;

			tmp = state[3][LEFT];
			state[3][LEFT] = state[8][LEFT];
			state[8][LEFT] = state[15][LEFT];
			state[15][LEFT] = state[10][LEFT];
			state[10][LEFT] = tmp;
		}
		else if (op == OL)
		{
			char tmp = state[0][BOTTOM];
			state[0][BOTTOM] = state[5][BACK];
			state[5][BACK] = state[17][TOP];
			state[17][TOP] = state[12][FRONT];
			state[12][FRONT] = tmp;

			tmp = state[8][BOTTOM];
			state[8][BOTTOM] = state[3][BACK];
			state[3][BACK] = state[10][TOP];
			state[10][TOP] = state[15][FRONT];
			state[15][FRONT] = tmp;

			tmp = state[12][BOTTOM];
			state[12][BOTTOM] = state[0][BACK];
			state[0][BACK] = state[5][TOP];
			state[5][TOP] = state[17][FRONT];
			state[17][FRONT] = tmp;

			tmp = state[5][LEFT];
			state[5][LEFT] = state[17][LEFT];
			state[17][LEFT] = state[12][LEFT];
			state[12][LEFT] = state[0][LEFT];
			state[0][LEFT] = tmp;

			tmp = state[10][LEFT];
			state[10][LEFT] = state[15][LEFT];
			state[15][LEFT] = state[8][LEFT];
			state[8][LEFT] = state[3][LEFT];
			state[3][LEFT] = tmp;
		}
		else if (op == YR)
		{
			char tmp = state[5][TOP];
			state[5][TOP] = state[7][RIGHT];
			state[7][RIGHT] = state[2][BOTTOM];
			state[2][BOTTOM] = state[0][LEFT];
			state[0][LEFT] = tmp;

			tmp = state[6][TOP];
			state[6][TOP] = state[4][RIGHT];
			state[4][RIGHT] = state[1][BOTTOM];
			state[1][BOTTOM] = state[3][LEFT];
			state[3][LEFT] = tmp;

			tmp = state[7][TOP];
			state[7][TOP] = state[2][RIGHT];
			state[2][RIGHT] = state[0][BOTTOM];
			state[0][BOTTOM] = state[5][LEFT];
			state[5][LEFT] = tmp;

			tmp = state[1][BACK];
			state[1][BACK] = state[3][BACK];
			state[3][BACK] = state[6][BACK];
			state[6][BACK] = state[4][BACK];
			state[4][BACK] = tmp;

			tmp = state[0][BACK];
			state[0][BACK] = state[5][BACK];
			state[5][BACK] = state[7][BACK];
			state[7][BACK] = state[2][BACK];
			state[2][BACK] = tmp;
		}
		else if (op == YL)
		{
			char tmp = state[0][LEFT];
			state[0][LEFT] = state[2][BOTTOM];
			state[2][BOTTOM] = state[7][RIGHT];
			state[7][RIGHT] = state[5][TOP];
			state[5][TOP] = tmp;

			tmp = state[3][LEFT];
			state[3][LEFT] = state[1][BOTTOM];
			state[1][BOTTOM] = state[4][RIGHT];
			state[4][RIGHT] = state[6][TOP];
			state[6][TOP] = tmp;

			tmp = state[5][LEFT];
			state[5][LEFT] = state[0][BOTTOM];
			state[0][BOTTOM] = state[2][RIGHT];
			state[2][RIGHT] = state[7][TOP];
			state[7][TOP] = tmp;

			tmp = state[4][BACK];
			state[4][BACK] = state[6][BACK];
			state[6][BACK] = state[3][BACK];
			state[3][BACK] = state[1][BACK];
			state[1][BACK] = tmp;

			tmp = state[2][BACK];
			state[2][BACK] = state[7][BACK];
			state[7][BACK] = state[5][BACK];
			state[5][BACK] = state[0][BACK];
			state[0][BACK] = tmp;
		}
        else if (op == BR)
		{
			char tmp = state[0][LEFT];
			state[0][LEFT] = state[2][BACK];
			state[2][BACK] = state[14][RIGHT];
			state[14][RIGHT] = state[12][FRONT];
			state[12][FRONT] = tmp;
            
			tmp = state[8][LEFT];
			state[8][LEFT] = state[1][BACK];
			state[1][BACK] = state[9][RIGHT];
			state[9][RIGHT] = state[13][FRONT];
			state[13][FRONT] = tmp;
            
			tmp = state[12][LEFT];
			state[12][LEFT] = state[0][BACK];
			state[0][BACK] = state[2][RIGHT];
			state[2][RIGHT] = state[14][FRONT];
			state[14][FRONT] = tmp;
            
			tmp = state[0][BOTTOM];
			state[0][BOTTOM] = state[2][BOTTOM];
			state[2][BOTTOM] = state[14][BOTTOM];
			state[14][BOTTOM] = state[12][BOTTOM];
			state[12][BOTTOM] = tmp;
            
			tmp = state[1][BOTTOM];
			state[1][BOTTOM] = state[9][BOTTOM];
			state[9][BOTTOM] = state[13][BOTTOM];
			state[13][BOTTOM] = state[8][BOTTOM];
			state[8][BOTTOM] = tmp;
		}
		else if (op == BL)
		{
			char tmp = state[12][FRONT];
            state[12][FRONT] = state[14][RIGHT];
            state[14][RIGHT] = state[2][BACK];
            state[2][BACK] = state[0][LEFT];
            state[0][LEFT] = tmp;
            
			tmp = state[13][FRONT];
            state[13][FRONT] = state[9][RIGHT];
            state[9][RIGHT] = state[1][BACK];
            state[1][BACK] = state[8][LEFT];
            state[8][LEFT] = tmp;
            
			tmp = state[14][FRONT];
            state[14][FRONT] = state[2][RIGHT];
            state[2][RIGHT] = state[0][BACK];
            state[0][BACK] = state[12][LEFT];
            state[12][LEFT] = tmp;
            
			tmp = state[12][BOTTOM];
			state[12][BOTTOM] = state[14][BOTTOM];
			state[14][BOTTOM] = state[2][BOTTOM];
			state[2][BOTTOM] = state[0][BOTTOM];
			state[0][BOTTOM] = tmp;
            
			tmp = state[8][BOTTOM];
			state[8][BOTTOM] = state[13][BOTTOM];
			state[13][BOTTOM] = state[9][BOTTOM];
			state[9][BOTTOM] = state[1][BOTTOM];
			state[1][BOTTOM] = tmp;
		}
        else if (op == RR)
		{
			char tmp = state[7][TOP];
			state[7][TOP] = state[19][FRONT];
			state[19][FRONT] = state[14][BOTTOM];
			state[14][BOTTOM] = state[2][BACK];
			state[2][BACK] = tmp;
            
			tmp = state[11][TOP];
			state[11][TOP] = state[16][FRONT];
			state[16][FRONT] = state[9][BOTTOM];
			state[9][BOTTOM] = state[4][BACK];
			state[4][BACK] = tmp;
            
			tmp = state[19][TOP];
			state[19][TOP] = state[14][FRONT];
			state[14][FRONT] = state[2][BOTTOM];
			state[2][BOTTOM] = state[7][BACK];
			state[7][BACK] = tmp;
            
			tmp = state[7][RIGHT];
			state[7][RIGHT] = state[19][RIGHT];
			state[19][RIGHT] = state[14][RIGHT];
			state[14][RIGHT] = state[2][RIGHT];
			state[2][RIGHT] = tmp;
            
			tmp = state[4][RIGHT];
			state[4][RIGHT] = state[11][RIGHT];
			state[11][RIGHT] = state[16][RIGHT];
			state[16][RIGHT] = state[9][RIGHT];
			state[9][RIGHT] = tmp;
		}
		else if (op == RL)
		{
			char tmp = state[2][BACK];
            state[2][BACK] = state[14][BOTTOM];
            state[14][BOTTOM] = state[19][FRONT];
            state[19][FRONT] = state[7][TOP];
            state[7][TOP] = tmp;
            
			tmp = state[4][BACK];
            state[4][BACK] = state[9][BOTTOM];
            state[9][BOTTOM] = state[16][FRONT];
            state[16][FRONT] = state[11][TOP];
            state[11][TOP] = tmp;
            
			tmp = state[7][BACK];
            state[7][BACK] = state[2][BOTTOM];
            state[2][BOTTOM] = state[14][FRONT];
            state[14][FRONT] = state[19][TOP];
            state[19][TOP] = tmp;
            
			tmp = state[2][RIGHT];
			state[2][RIGHT] = state[14][RIGHT];
			state[14][RIGHT] = state[19][RIGHT];
			state[19][RIGHT] = state[7][RIGHT];
			state[7][RIGHT] = tmp;
            
			tmp = state[9][RIGHT];
			state[9][RIGHT] = state[16][RIGHT];
			state[16][RIGHT] = state[11][RIGHT];
			state[11][RIGHT] = state[4][RIGHT];
			state[4][RIGHT] = tmp;
		}
    }

    bool RubikResolver::stateEqual(const char* s1, const char* s2)
    {
    	for (int i = 0;i < 6;++i)
		{
			if (s1[i] != s2[i])
			{
				return false;
			}
		}

		return true;
    }
    
    void RubikResolver::insertStep(char (&state)[20][6], char step)
    {
    	updateState(state, step);
    	mManualSteps.push_back(step);
    }
    void RubikResolver::insertSteps(char (&state)[20][6], int count, char* steps)
    {
    	for (int i = 0;i < count;++i)
		{
			int step = steps[i];
			insertStep(state, step);
		}
    }
    
    void RubikResolver::findGCross(char (&state)[20][6])
    {
    	for (int i = 0;i < 4;++i)
    	{
    		for (int j = 0;j < 24;++j)
    		{
    			int mcidx = STAGE0_MOVES[i][j][0];
    			if (stateEqual(&state[mcidx][0], &STAGE0_MOVES[i][j][1]))
    			{
    				int stepCount = STAGE0_MOVES[i][j][7];
    				for (int k = 0;k < stepCount;++k)
    				{
    					int step = STAGE0_MOVES[i][j][8 + k];
    					updateState(state, step);
    					mManualSteps.push_back(step);
    				}
                    break;
    			}
    		}
    	}
    }
    
    void RubikResolver::firstLayer(char (&state)[20][6])
    {
        for (int i = 0;i < 4;++i)
    	{
    		for (int j = 0;j < 24;++j)
    		{
                int mcidx = STAGE1_MOVES[i][j][0];
    			if (stateEqual(&state[mcidx][0], &STAGE1_MOVES[i][j][1]))
    			{
    				int stepCount = STAGE1_MOVES[i][j][7];
    				for (int k = 0;k < stepCount;++k)
    				{
    					int step = STAGE1_MOVES[i][j][8 + k];
    					updateState(state, step);
    					mManualSteps.push_back(step);
    				}
                    break;
    			}
    		}
    	}
    }
    
    void RubikResolver::secondLayer(char (&state)[20][6])
    {
        for (int i = 0;i < 4;++i)
    	{
    		for (int j = 0;j < 24;++j)
    		{
                int mcidx = STAGE2_MOVES[i][j][0];
    			if (stateEqual(&state[mcidx][0], &STAGE2_MOVES[i][j][1]))
    			{
//                    LOGD("STAGE[2] located [%d, %d, %d]\n", i, mcidx, j);
    				int stepCount = STAGE2_MOVES[i][j][7];
    				for (int k = 0;k < stepCount;++k)
    				{
    					int step = STAGE2_MOVES[i][j][8 + k];
    					updateState(state, step);
    					mManualSteps.push_back(step);
    				}
                    break;
    			}
    		}
    	}
    }

    void RubikResolver::bottomCross(char (&state)[20][6])
    {
        if (state[1][CY] == 'y' && state[3][CY] == 'y' && state[4][CY] == 'y' && state[6][CY] == 'y')
        {
//            LOGD("Stage 3, already in cross state");
            return;
        }
        
    	char f1[6] = {OR, WR, YR, WL, YL, OL};
    	if (state[3][CY] == 'y' && state[4][CY] == 'y' && state[6][CW] == 'y')
    	{
    		insertStep(state, YR);
    	}
    	if (state[1][CY] == 'y' && state[6][CY] == 'y' && state[3][CO] == 'y')
    	{
    		insertSteps(state, 6, &f1[0]);
    	}

    	char f2[6] = {RR, YR, BR, YL, BL, RL};
    	if (state[1][CY] == 'y' && state[3][CY] == 'y' && state[6][CW] == 'y')
        {
    		insertStep(state, YR);
    	}
        else if (state[4][CY] == 'y' && state[6][CY] == 'y' && state[1][CB] == 'y')
        {
    		insertStep(state, YL);
    	}
        else if (state[3][CY] == 'y' && state[6][CY] == 'y' && state[4][CR] == 'y')
        {
            insertStep(state, YL);
    		insertStep(state, YL);
    	}
        
    	if (state[1][CY] == 'y' && state[4][CY] == 'y' && state[3][CO] == 'y')
		{
			insertSteps(state, 6, &f2[0]);
		}

    	if (state[1][CB] == 'y' && state[3][CO] == 'y' && state[4][CR] == 'y' && state[6][CW] == 'y')
		{
			insertSteps(state, 6, &f1[0]);
            insertSteps(state, 6, &f2[0]);
		}
    }
    
    void RubikResolver::bottomFace(char (&state)[20][6])
    {
        char f[8] = {RL, YR, YR, RR, YR, RL, YR, RR};
        for (int i = 0;i < 4;++i)
        {
            if (state[0][CB] == 'y' && state[2][CR] == 'y' && state[5][CO] == 'y' && state[7][CY] == 'y')
            {
                insertSteps(state, 8, &f[0]);
                break;
            }
            
            if (state[0][CO] == 'y' && state[2][CB] == 'y' && state[5][CW] == 'y' && state[7][CY] == 'y')
            {
                insertSteps(state, 8, &f[0]);
                insertStep(state, YR);
                insertStep(state, YR);
                insertSteps(state, 8, &f[0]);
                break;
            }
            
            if (state[0][CB] == 'y' && state[2][CB] == 'y' && state[5][CW] == 'y' && state[7][CW] == 'y')
            {
                insertSteps(state, 8, &f[0]);
                insertSteps(state, 8, &f[0]);
                break;
            }
            
            if (state[0][CY] == 'y' && state[2][CR] == 'y' && state[5][CY] == 'y' && state[7][CR] == 'y')
            {
                insertSteps(state, 8, &f[0]);
                insertStep(state, YL); // ???
                insertSteps(state, 8, &f[0]);
                insertStep(state, YR);
                insertStep(state, YR);
                insertSteps(state, 8, &f[0]);
                break;
            }
            
            if (state[0][CY] == 'y' && state[2][CY] == 'y' && state[5][CO] == 'y' && state[7][CR] == 'y')
            {
                insertSteps(state, 8, &f[0]);
                insertStep(state, YR); // ???
                insertSteps(state, 8, &f[0]);
                insertStep(state, YR);
                insertStep(state, YR);
                insertSteps(state, 8, &f[0]);
                break;
            }
            
            if (state[0][CO] == 'y' && state[2][CB] == 'y' && state[5][CO] == 'y' && state[7][CW] == 'y')
            {
                insertSteps(state, 8, &f[0]);
                insertStep(state, YR);
                insertSteps(state, 8, &f[0]);
                break;
            }
            
            if (state[0][CB] == 'y' && state[2][CY] == 'y' && state[5][CY] == 'y' && state[7][CR] == 'y')
            {
                insertSteps(state, 8, &f[0]);
                insertSteps(state, 8, &f[0]);
                insertStep(state, YR);
                insertStep(state, YR);
                insertSteps(state, 8, &f[0]);
                break;
            }
            
            insertStep(state, YR);
        }
    }
    
    void RubikResolver::bottomCorner(char (&state)[20][6])
    {
        while (true)
        {
            int inPlaceCount = 0;
            if (state[0][CO] == 'o' && state[0][CB] == 'b')
            {
                inPlaceCount++;
            }
            if(state[2][CB] == 'b' && state[2][CR] == 'r')
            {
                inPlaceCount++;
            }
            if(state[5][CO] == 'o' && state[5][CW] == 'w')
            {
                inPlaceCount++;
            }
            if(state[7][CW] == 'w' && state[7][CR] == 'r')
            {
                inPlaceCount++;
            }
            
            if (inPlaceCount == 4)
            {
//                LOGD("bottomCorner solved\n");
                return;
            }
            
            bool found = false;
            while (!found)
            {
                if (state[0][CB] == state[2][CB])
                {
                    insertStep(state, YL);
                    found = true;
                }
                if (state[2][CR] == state[7][CR])
                {
                    insertStep(state, YL);
                    insertStep(state, YL);
                    found = true;
                }
                if (state[5][CW] == state[7][CW])
                {
                    insertStep(state, YR);
                    found = true;
                }
                if (state[0][CO] == state[7][CO])
                {
                    found = true;
                }
            
                char f[] = {WR, RL, WR, OR, OR, WL, RR, WR, OR, OR, WR, WR};
                insertSteps(state, sizeof(f), &f[0]);
            }
            
            while (state[0][CO] != 'o' || state[0][CB] != 'b')
            {
                insertStep(state, YR);
            }
        }
    }
    
    void RubikResolver::finalTouch(char (&state)[20][6])
    {
        int caseNo = 0;
        char fw[] = {WR, YL, WR, YR, WR, YR, WR, YL, WL, YL, WR, WR};
        char fr[] = {RR, YL, RR, YR, RR, YR, RR, YL, RL, YL, RR, RR};
        char fb[] = {BR, YL, BR, YR, BR, YR, BR, YL, BL, YL, BR, BR};
        char fo[] = {OR, YL, OR, YR, OR, YR, OR, YL, OL, YL, OR, OR};
        if (state[3][CO] == 'w' && state[6][CW] == 'b' && state[1][CB] == 'o' && state[4][CR] == 'r')
        {
            insertSteps(state, sizeof(fw), &fw[0]);
            caseNo = 11;
        }
        
        if (state[3][CO] == 'w' && state[6][CW] == 'r' && state[4][CR] == 'o')
        {
            insertSteps(state, sizeof(fr), &fr[0]);
            caseNo = 12;
        }
        
        if (state[3][CO] == 'w' && state[1][CB] == 'r' && state[4][CR] == 'o' && state[6][CW] == 'b')
        {
            insertSteps(state, sizeof(fw), &fw[0]);
            insertStep(state, YR);
            insertSteps(state, sizeof(fw), &fw[0]);
            insertStep(state, YL);
            caseNo = 13;
        }
        
        if (state[3][CO] == 'w' && state[1][CB] == 'r' && state[4][CR] == 'b' && state[6][CW] == 'o')
        {
            insertSteps(state, sizeof(fw), &fw[0]);
            insertStep(state, YL);
            insertSteps(state, sizeof(fb), &fb[0]);
            insertStep(state, YR);
            caseNo = 14;
        }
        
        if (state[3][CO] == 'r' && state[6][CW] == 'o' && state[4][CR] == 'w')
        {
            insertSteps(state, sizeof(fr), &fr[0]);
            insertSteps(state, sizeof(fr), &fr[0]);
            caseNo = 21;
        }
        
        if (state[3][CO] == 'r' && state[4][CR] == 'o' && state[1][CB] == 'w' && state[6][CW] == 'b')
        {
            insertSteps(state, sizeof(fw), &fw[0]);
            insertStep(state, YL);
            insertSteps(state, sizeof(fw), &fw[0]);
            insertStep(state, YR);
            caseNo = 22;
        }
        
        if (state[3][CO] == 'r' && state[6][CW] == 'w' && state[4][CR] == 'b')
        {
            insertSteps(state, sizeof(fo), &fo[0]);
            caseNo = 23;
        }
    
        if (state[3][CO] == 'o' && state[6][CW] == 'r' && state[4][CR] == 'b')
        {
            insertSteps(state, sizeof(fb), &fb[0]);
            caseNo = 31;
        }
        
        if (state[3][CO] == 'o' && state[6][CW] == 'b' && state[4][CR] == 'w')
        {
            insertSteps(state, sizeof(fb), &fb[0]);
            insertSteps(state, sizeof(fb), &fb[0]);
            caseNo = 32;
        }
        
        if (state[3][CO] == 'b' && state[6][CW] == 'o' && state[4][CR] == 'r')
        {
            insertSteps(state, sizeof(fw), &fw[0]);
            insertSteps(state, sizeof(fw), &fw[0]);
            caseNo = 41;
        }
        
        if (state[3][CO] == 'b' && state[1][CB] == 'r' && state[4][CR] == 'o')
        {
            insertSteps(state, sizeof(fo), &fo[0]);
            insertSteps(state, sizeof(fo), &fo[0]);
            caseNo = 42;
        }
        
        if (state[3][CO] == 'b' && state[6][CW] == 'r' && state[4][CR] == 'w')
        {
            insertSteps(state, sizeof(fr), &fr[0]);
            insertStep(state, YL);
            insertSteps(state, sizeof(fr), &fr[0]);
            insertStep(state, YR);
            caseNo = 44;
        }

        LOGD("final touch caseNo = %d\n", caseNo);
    }
}
