0.1.0 PUCT and random rollouts
```
Results of Artemis vs Random (10+0.1, NULL, 64MB, noob_4moves.epd):
Elo: inf +/- -nan, nElo: inf +/- -nan
LOS: 100.00 %, DrawRatio: 0.00 %, PairsRatio: inf
Games: 148, Wins: 148, Losses: 0, Draws: 0, Points: 148.0 (100.00 %)
Ptnml(0-2): [0, 0, 0, 0, 74], WL/DD Ratio: -nan
LLR: 2.95 (100.2%) (-2.94, 2.94) [0.00, 10.00]
```

0.2.0 Material eval
```
--------------------------------------------------
Results of Artemis-dev vs Artemis-master (8+0.08, NULL, 128MB, noob_4moves.epd):
Elo: 42.05 +/- 10.91, nElo: 126.40 +/- 32.46
LOS: 100.00 %, DrawRatio: 75.00 %, PairsRatio: 26.50
Games: 440, Wins: 55, Losses: 2, Draws: 383, Points: 246.5 (56.02 %)
Ptnml(0-2): [0, 2, 165, 51, 2], WL/DD Ratio: 0.00
LLR: 2.95 (100.2%) (-2.94, 2.94) [0.00, 10.00]
--------------------------------------------------
```

0.3.0 NNUE, 32hl 47m positions
```
--------------------------------------------------
Results of Artemis-dev vs Artemis-master (8+0.08, NULL, 32MB, noob_4moves.epd):
Elo: 100.99 +/- 23.94, nElo: 178.72 +/- 39.99
LOS: 100.00 %, DrawRatio: 38.62 %, PairsRatio: 7.90
Games: 290, Wins: 102, Losses: 20, Draws: 168, Points: 186.0 (64.14 %)
Ptnml(0-2): [1, 9, 56, 65, 14], WL/DD Ratio: 0.19
LLR: 2.95 (100.3%) (-2.94, 2.94) [0.00, 10.00]
--------------------------------------------------
```

0.3.1 Gameover detection bugfix
```
--------------------------------------------------
Results of Artemis-dev vs Artemis-master (8+0.08, NULL, 64MB, noob_4moves.epd):
Elo: 129.80 +/- 27.03, nElo: 226.37 +/- 42.90
LOS: 100.00 %, DrawRatio: 26.98 %, PairsRatio: 9.22
Games: 252, Wins: 106, Losses: 16, Draws: 130, Points: 171.0 (67.86 %)
Ptnml(0-2): [0, 9, 34, 67, 16], WL/DD Ratio: 0.26
LLR: 2.97 (100.9%) (-2.94, 2.94) [0.00, 10.00]
--------------------------------------------------
```

0.4.0 Internal Mate Proving
```
--------------------------------------------------
Results of Artemis-dev vs Artemis-master (8+0.08, NULL, 64MB, noob_4moves.epd):
Elo: 44.78 +/- 14.80, nElo: 56.90 +/- 18.59
LOS: 100.00 %, DrawRatio: 35.32 %, PairsRatio: 1.70
Games: 1342, Wins: 526, Losses: 354, Draws: 462, Points: 757.0 (56.41 %)
Ptnml(0-2): [42, 119, 237, 171, 102], WL/DD Ratio: 1.76
LLR: 2.95 (100.2%) (-2.94, 2.94) [0.00, 5.00]
--------------------------------------------------
```

0.6.0 Increased internal mate value and new net
```
--------------------------------------------------
Results of Artemis-dev vs Artemis-master (8+0.08, NULL, 64MB, noob_4moves.epd):
Elo: 41.78 +/- 21.18, nElo: 45.42 +/- 22.77
LOS: 100.00 %, DrawRatio: 37.36 %, PairsRatio: 1.59
Games: 894, Wins: 435, Losses: 328, Draws: 131, Points: 500.5 (55.98 %)
Ptnml(0-2): [61, 47, 167, 68, 104], WL/DD Ratio: 19.88
LLR: 2.94 (101.6%) (-2.25, 2.89) [0.00, 10.00]
--------------------------------------------------
```