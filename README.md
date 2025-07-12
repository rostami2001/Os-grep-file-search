# Os-grep-file-search
---

### Os-grep  
**Description**  
This program is a multi-threaded file search tool written in C. It recursively scans directories to search for occurrences of a specific word (e.g., "ipsum") within files. It uses pthreads for parallel processing to improve performance.  

**Functionality**  
- The program takes a directory path as a command-line argument.  
- It recursively traverses the directory structure.  
- For each file encountered, it searches for the word "ipsum".  
- It calculates the total number of files processed and the total number of matches found.  
- It outputs the file paths, line numbers, and character positions where matches are found.  

**Compilation and Execution**  
To compile the program, use the following command:  
```bash
gcc -o os-grep os-grep.c -lpthread
```  
To run the program, execute the compiled binary with the directory path as an argument:  
```bash
./os-grep <directory_path>
```  

**Example**  
For instance, if you want to search for the word "ipsum" in the files within the directory `/home/user/documents`, run the following command:  
```bash
./os-grep /home/user/documents
```  

**Notes**  
- The program uses pthreads for concurrent file processing, improving performance.  
- Mutexes are used to synchronize access to shared variables (`counter` and `match_count`) between threads.  
- Error handling is implemented for file operations and pthread functions.  
- The program limits file paths to a maximum length defined by `MAX_PATH_LENGTH`.  
- The output includes file paths, line numbers, and character positions where matches are found.  
- This program is designed for POSIX-compatible systems.  
- Ensure you compile with the `-lpthread` flag to link the pthread library.  
- The search term ("ipsum") is currently hardcoded in the program. To search for a different term, modify the `word` variable in the `processFile` function.  

--- 
