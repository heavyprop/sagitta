```
brew install heavyprop/sagitta/sagitta
brew test sagitta

// to run
sagitta

// to run file
sagitta [file_name].sgt 
```
## Example to run:
``` cpp
name = "Sagitta";
[]: demo;
demo->append("Hello from " + name);
demo->append(2 + 3 * 4);
demo->append(7 / 2);
demo->append(-(-5));
print(demo);
```
### Output:
`[[1], [1, 1], [1, 2, 1], [1, 3, 3, 1], [1, 4, 6, 4, 1], [1, 5, 10, 10, 5, 1]]`

