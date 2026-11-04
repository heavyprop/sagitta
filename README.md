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
# this file contains a demo
name = "Sagitta";
[]: demo;
demo->append("Hello from " + name);
demo->append(2 + 3 * 4);
demo->append(7 / 2);
demo->append(-(-5));
print(demo);
```


