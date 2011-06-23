--------------------------------------------------------------------------------
I1 cache:         32768 B, 64 B, 8-way associative
D1 cache:         32768 B, 64 B, 8-way associative
LL cache:         3145728 B, 64 B, 12-way associative
Command:          ./effectuate -o n-mode vector100.in tests/compressed-row-tensor100.out compressed-row-tensor100.out
Data file:        cache.compressed.dat
Events recorded:  Ir I1mr ILmr Dr D1mr DLmr Dw D1mw DLmw
Events shown:     Ir I1mr ILmr Dr D1mr DLmr Dw D1mw DLmw
Event sort order: Ir I1mr ILmr Dr D1mr DLmr Dw D1mw DLmw
Thresholds:       20 100 100 100 100 100 100 100 100
Include dirs:     
User annotated:   
Auto-annotation:  on

--------------------------------------------------------------------------------
         Ir  I1mr  ILmr         Dr   D1mr  DLmr         Dw   D1mw   DLmw 
--------------------------------------------------------------------------------
333,617,782 3,052 2,116 81,095,914 16,091 2,052 42,673,662 11,414 11,023  PROGRAM TOTALS

--------------------------------------------------------------------------------
        Ir I1mr ILmr         Dr   D1mr DLmr        Dw  D1mw  DLmw  file:function
--------------------------------------------------------------------------------
84,177,264  120   70 19,909,772     91   79 7,499,328    13    13  ???:__svfscanf_l

