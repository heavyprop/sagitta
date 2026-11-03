```
brew install heavyprop/sagitta/sagitta
brew test sagitta
sagitta test.sgt #naming convention.sgt
```
## Example to run:
``` cpp
print("LeetCode 118: Pascals Triangle, numRows = 6");

[]: triangle;

c1_1 = 1;
[]: row;
row->append(c1_1);
triangle->append(row);

c2_1 = 1;
c2_2 = 1;
[]: row;
row->append(c2_1);
row->append(c2_2);
triangle->append(row);

c3_1 = 1;
c3_2 = c2_1 + c2_2;
c3_3 = 1;
[]: row;
row->append(c3_1);
row->append(c3_2);
row->append(c3_3);
triangle->append(row);

c4_1 = 1;
c4_2 = c3_1 + c3_2;
c4_3 = c3_2 + c3_3;
c4_4 = 1;
[]: row;
row->append(c4_1);
row->append(c4_2);
row->append(c4_3);
row->append(c4_4);
triangle->append(row);

c5_1 = 1;
c5_2 = c4_1 + c4_2;
c5_3 = c4_2 + c4_3;
c5_4 = c4_3 + c4_4;
c5_5 = 1;
[]: row;
row->append(c5_1);
row->append(c5_2);
row->append(c5_3);
row->append(c5_4);
row->append(c5_5);
triangle->append(row);

c6_1 = 1;
c6_2 = c5_1 + c5_2;
c6_3 = c5_2 + c5_3;
c6_4 = c5_3 + c5_4;
c6_5 = c5_4 + c5_5;
c6_6 = 1;
[]: row;
row->append(c6_1);
row->append(c6_2);
row->append(c6_3);
row->append(c6_4);
row->append(c6_5);
row->append(c6_6);
triangle->append(row);

print(triangle);
```
### Output:
`[[1], [1, 1], [1, 2, 1], [1, 3, 3, 1], [1, 4, 6, 4, 1], [1, 5, 10, 10, 5, 1]]`

